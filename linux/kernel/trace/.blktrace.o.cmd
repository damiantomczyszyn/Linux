
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);

		tda18271_attach(&dev->ts1.analog_fe,
			0x60, &dev->i2c_bus[1].i2c_adap,
			&hauppauge_tda18271_config);

		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
		i2c_bus = &dev->i2c_bus[0];
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
			&hcw_s5h1411_config,
			&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(tda18271_attach, fe0->dvb.frontend,
			   0x60, &dev->i2c_bus[0].i2c_adap,
			   &hauppauge_tda18271_config);
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		switch (port->nr) {
		/* port B */
		case 1:
			i2c_bus = &dev->i2c_bus[0];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg1,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg1);
			break;
		/* port C */
		case 2:
			i2c_bus = &dev->i2c_bus[1];
			fe0->dvb.frontend = dvb_attach(atbm8830_attach,
				&mygica_x8558pro_atbm8830_cfg2,
				&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			dvb_attach(max2165_attach, fe0->dvb.frontend,
				   &i2c_bus->i2c_adap,
				   &mygic_x8558pro_max2165_cfg2);
		}
		break;
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
		if (port->nr > 2)
			return 0;

		i2c_bus = &dev->i2c_bus[0];
		mfe_shared = 1;/* MFE */
		port->frontends.gate = 0;/* not clear for me yet */
		/* ports B, C */
		/* MFE frontend 1 DVB-T */
		fe0->dvb.frontend = dvb_attach(stv0367ter_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		if (NULL == dvb_attach(xc5000_attach, fe0->dvb.frontend,
					&i2c_bus->i2c_adap,
					&netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		/* load xc5000 firmware */
		fe0->dvb.frontend->ops.tuner_ops.init(fe0->dvb.frontend);

		/* MFE frontend 2 */
		fe1 = vb2_dvb_get_frontend(&port->frontends, 2);
		if (fe1 == NULL)
			goto frontend_detach;
		/* DVB-C init */
		fe1->dvb.frontend = dvb_attach(stv0367cab_attach,
					&netup_stv0367_config[port->nr - 1],
					&i2c_bus->i2c_adap);
		if (fe1->dvb.frontend == NULL)
			break;

		fe1->dvb.frontend->id = 1;
		if (NULL == dvb_attach(xc5000_attach,
				       fe1->dvb.frontend,
				       &i2c_bus->i2c_adap,
				       &netup_xc5000_config[port->nr - 1]))
			goto frontend_detach;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		switch (port->nr) {
		/* port b */
		case 1:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[0],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[0],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		/* port c */
		case 2:
			fe0->dvb.frontend = dvb_attach(drxk_attach,
					&terratec_drxk_config[1],
					&i2c_bus->i2c_adap);
			if (fe0->dvb.frontend == NULL)
				break;
			if (!dvb_attach(mt2063_attach,
					fe0->dvb.frontend,
					&terratec_mt2063_config[1],
					&i2c_bus2->i2c_adap))
				goto frontend_detach;
			break;
		}
		break;
	case CX23885_BOARD_TEVII_S471:
		i2c_bus = &dev->i2c_bus[1];

		fe0->dvb.frontend = dvb_attach(ds3000_attach,
					&tevii_ds3000_config,
					&i2c_bus->i2c_adap);
		if (fe0->dvb.frontend == NULL)
			break;
		dvb_attach(ts2020_attach, fe0->dvb.frontend,
			   &tevii_ts2020_config, &i2c_bus->i2c_adap);
		break;
	case CX23885_BOARD_PROF_8000:
		i2c_bus = &dev->i2c_bus[0];

		fe0->dvb.frontend = dvb_attach(stv090x_attach,
						&prof_8000_stv090x_config,
						&i2c_bus->i2c_adap,
						STV090x_DEMODULATOR_0);
		if (fe0->dvb.frontend == NULL)
			break;
		if (!dvb_attach(stb6100_attach,
				fe0->dvb.frontend,
				&prof_8000_stb6100_config,
				&i2c_bus->i2c_adap))
			goto frontend_detach;

		fe0->dvb.frontend->ops.set_voltage = p8000_set_voltage;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR4400: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b */
		case 1:
			/* attach demod + tuner combo */
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
			info.addr = 0x05;
			info.platform_data = &tda10071_pdata;
			request_module("tda10071");
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
			port->i2c_client_demod = client_demod;

			/* attach SEC */
			a8293_pdata.dvb_frontend = fe0->dvb.frontend;
			memset(&info, 0, sizeof(info));
			strscpy(info.type, "a8293", I2C_NAME_SIZE);
			info.addr = 0x0b;
			info.platform_data = &a8293_pdata;
			request_module("a8293");
			client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_sec))
				goto frontend_detach;
			if (!try_module_get(client_sec->dev.driver->owner)) {
				i2c_unregister_device(client_sec);
				goto frontend_detach;
			}
			port->i2c_client_sec = client_sec;
			break;
		/* port c */
		case 2:
			/* attach frontend */
			memset(&si2165_pdata, 0, sizeof(si2165_pdata));
			si2165_pdata.fe = &fe0->dvb.frontend;
			si2165_pdata.chip_mode = SI2165_MODE_PLL_XTAL;
			si2165_pdata.ref_freq_hz = 16000000;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2165", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2165_pdata;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			if (fe0->dvb.frontend == NULL)
				break;
			fe0->dvb.frontend->ops.i2c_gate_ctrl = NULL;
			if (!dvb_attach(tda18271_attach,
					fe0->dvb.frontend,
					0x60, &i2c_bus2->i2c_adap,
				  &hauppauge_hvr4400_tuner_config))
				goto frontend_detach;
			break;
		}
		break;
	}
	case CX23885_BOARD_HAUPPAUGE_STARBURST: {
		struct tda10071_platform_data tda10071_pdata = hauppauge_tda10071_pdata;
		struct a8293_platform_data a8293_pdata = {};

		i2c_bus = &dev->i2c_bus[0];

		/* attach demod + tuner combo */
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "tda10071_cx24118", I2C_NAME_SIZE);
		info.addr = 0x05;
		info.platform_data = &tda10071_pdata;
		request_module("tda10071");
		client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		fe0->dvb.frontend = tda10071_pdata.get_dvb_frontend(client_demod);
		port->i2c_client_demod = client_demod;

		/* attach SEC */
		a8293_pdata.dvb_frontend = fe0->dvb.frontend;
		memset(&info, 0, sizeof(info));
		strscpy(info.type, "a8293", I2C_NAME_SIZE);
		info.addr = 0x0b;
		info.platform_data = &a8293_pdata;
		request_module("a8293");
		client_sec = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
		if (!i2c_client_has_driver(client_sec))
			goto frontend_detach;
		if (!try_module_get(client_sec->dev.driver->owner)) {
			i2c_unregister_device(client_sec);
			goto frontend_detach;
		}
		port->i2c_client_sec = client_sec;
		break;
	}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];
		switch (port->nr) {
		/* port b - satellite */
		case 1:
			/* attach frontend */
			fe0->dvb.frontend = dvb_attach(m88ds3103_attach,
					&dvbsky_t9580_m88ds3103_config,
					&i2c_bus2->i2c_adap, &adapter);
			if (fe0->dvb.frontend == NULL)
				break;

			/* attach tuner */
			memset(&ts2020_config, 0, sizeof(ts2020_config));
			ts2020_config.fe = fe0->dvb.frontend;
			ts2020_config.get_agc_pwm = m88ds3103_get_agc_pwm;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "ts2020", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &ts2020_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;
			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}

			/* delegate signal strength measurement to tuner */
			fe0->dvb.frontend->ops.read_signal_strength =
				fe0->dvb.frontend->ops.tuner_ops.get_rf_strength;

			/*
			 * for setting the voltage we need to set GPIOs on
			 * the card.
			 */
			port->fe_set_voltage =
				fe0->dvb.frontend->ops.set_voltage;
			fe0->dvb.frontend->ops.set_voltage =
				dvbsky_t9580_set_voltage;

			port->i2c_client_tuner = client_tuner;

			break;
		/* port c - terrestrial/cable */
		case 2:
			/* attach frontend */
			memset(&si2168_config, 0, sizeof(si2168_config));
			si2168_config.i2c_adapter = &adapter;
			si2168_config.fe = &fe0->dvb.frontend;
			si2168_config.ts_mode = SI2168_TS_SERIAL;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2168", I2C_NAME_SIZE);
			info.addr = 0x64;
			info.platform_data = &si2168_config;
			request_module(info.type);
			client_demod = i2c_new_client_device(&i2c_bus->i2c_adap, &info);
			if (!i2c_client_has_driver(client_demod))
				goto frontend_detach;
			if (!try_module_get(client_demod->dev.driver->owner)) {
				i2c_unregister_device(client_demod);
				goto frontend_detach;
			}
			port->i2c_client_demod = client_demod;

			/* attach tuner */
			memset(&si2157_config, 0, sizeof(si2157_config));
			si2157_config.fe = fe0->dvb.frontend;
			si2157_config.if_port = 1;
			memset(&info, 0, sizeof(struct i2c_board_info));
			strscpy(info.type, "si2157", I2C_NAME_SIZE);
			info.addr = 0x60;
			info.platform_data = &si2157_config;
			request_module(info.type);
			client_tuner = i2c_new_client_device(adapter, &info);
			if (!i2c_client_has_driver(client_tuner))
				goto frontend_detach;

			if (!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				goto frontend_detach;
			}
			port->i2c_client_tuner = client_tuner;
			break;
		}
		break;
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		memset(&si2168_config, 0, sizeof(si2168_config));
		si2168_config.i2c_adapter = &adapter;
		si2168_config.fe = &fe0->dvb.frontend;
		si2168_config.ts_mode = SI2168_TS_PARALLEL;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2168", I2C_NAME_SIZE);
		info.addr = 0x64;
		info.platform_data = &si2168_config;
		request_module(info.type);
		client_demod = i2c_new_client_device(&i2c_bus2->i2c_adap, &info);
		if (!i2c_client_has_driver(client_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner)) {
			i2c_unregister_device(client_demod);
			goto frontend_detach;
		}
		port->i2c_client_demod = client_demod;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = fe0->dvb.frontend;
		si2157_config.if_port = 1;
		memset(&info, 0, sizeof(struct i2c_board_info));
		strscpy(info.type, "si2157", I2C_NAME_SIZE);
		info.addr = 0x60;
		info.platform_data = &si2157_config;
		request_module(info.type);
		client_tuner = i2c_new_client_device(adapter, &info);
		if (!i2c_client_has_driver(client_tuner))
			goto frontend_detach;
		if (!try_module_get(client_tuner->dev.driver->owner)) {
			i2c_unregister_device(client_tuner);
			goto frontend_detach;
		}
		port->i2c_client_tuner = client_tuner;
		break;
	case CX23885_BOARD_DVBSKY_S950C:
		i2c_bus = &dev->i2c_bus[0];
		i2c_bus2 = &dev->i2c_bus[1];

		/* attach frontend */
		fe0->dvb.frontend = dvb_attach(m88ds3103_attachif
}

DEFINE_STATIC_KEY_FALSE(sched_numa_balancing);

#ifdef CONFIG_NUMA_BALANCING

int sysctl_numa_balancing_mode;

static void __set_numabalancing_state(bool enabled)
{
	if (enabled)
		static_branch_enable(&sched_numa_balancing);
	else
		static_branch_disable(&sched_numa_balancing);
}

void set_numabalancing_state(bool enabled)
{
	if (enabled)
		sysctl_numa_balancing_mode = NUMA_BALANCING_NORMAL;
	else
		sysctl_numa_balancing_mode = NUMA_BALANCING_DISABLED;
	__set_numabalancing_state(enabled);
}

#ifdef CONFIG_PROC_SYSCTL
int sysctl_numa_balancing(struct ctl_table *table, int write,
			  void *buffer, size_t *lenp, loff_t *ppos)
{
	struct ctl_table t;
	int err;
	int state = sysctl_numa_balancing_mode;

	if (write && !capable(CAP_SYS_ADMIN))
		return -EPERM;

	t = *table;
	t.data = &state;
	err = proc_dointvec_minmax(&t, write, buffer, lenp, ppos);
	if (err < 0)
		return err;
	if (write) {
		sysctl_numa_balancing_mode = state;
		__set_numabalancing_state(state);
	}
	return err;
}
#endif
#endif

#ifdef CONFIG_SCHEDSTATS

DEFINE_STATIC_KEY_FALSE(sched_schedstats);

static void set_schedstats(bool enabled)
{
	if (enabled)
		static_branch_enable(&sched_schedstats);
	else
		static_branch_disable(&sched_schedstats);
}

void force_schedstat_enabled(void)
{
	if (!schedstat_enabled()) {
		pr_info("kernel profiling enabled schedstats, disable via kernel.sched_schedstats.\n");
		static_branch_enable(&sched_schedstats);
	}
}

static int __init setup_schedstats(char *str)
{
	int ret = 0;
	if (!str)
		goto out;

	if (!strcmp(str, "enable")) {
		set_schedstats(true);
		ret = 1;
	} else if (!strcmp(str, "disable")) {
		set_schedstats(false);
		ret = 1;
	}
out:
	if (!ret)
		pr_warn("Unable to parse schedstats=\n");

	return ret;
}
__setup("schedstats=", setup_schedstats);

#ifdef CONFIG_PROC_SYSCTL
int sysctl_schedstats(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos)
{
	struct ctl_table t;
	int err;
	int state = static_branch_likely(&sched_schedstats);

	if (write && !capable(CAP_SYS_ADMIN))
		return -EPERM;

	t = *table;
	t.data = &state;
	err = proc_dointvec_minmax(&t, write, buffer, lenp, ppos);
	if (err < 0)
		return err;
	if (write)
		set_schedstats(state);
	return err;
}
#endif /* CONFIG_PROC_SYSCTL */
#endif /* CONFIG_SCHEDSTATS */

/*
 * fork()/clone()-time setup:
 */
int sched_fork(unsigned long clone_flags, struct task_struct *p)
{
	__sched_fork(clone_flags, p);
	/*
	 * We mark the process as NEW here. This guarantees that
	 * nobody will actually run it, and a signal or other external
	 * event cannot wake it up and insert it on the runqueue either.
	 */
	p->__state = TASK_NEW;

	/*
	 * Make sure we do not leak PI boosting priority to the child.
	 */
	p->prio = current->normal_prio;

	uclamp_fork(p);

	/*
	 * Revert to default priority/policy on fork if requested.
	 */
	if (unlikely(p->sched_reset_on_fork)) {
		if (task_has_dl_policy(p) || task_has_rt_policy(p)) {
			p->policy = SCHED_NORMAL;
			p->static_prio = NICE_TO_PRIO(0);
			p->rt_priority = 0;
		} else if (PRIO_TO_NICE(p->static_prio) < 0)
			p->static_prio = NICE_TO_PRIO(0);

		p->prio = p->normal_prio = p->static_prio;
		set_load_weight(p, false);

		/*
		 * We don't need the reset flag anymore after the fork. It has
		 * fulfilled its duty:
		 */
		p->sched_reset_on_fork = 0;
	}

	if (dl_prio(p->prio))
		return -EAGAIN;
	else if (rt_prio(p->prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;

	init_entity_runnable_average(&p->se);


#ifdef CONFIG_SCHED_INFO
	if (likely(sched_info_on()))
		memset(&p->sched_info, 0, sizeof(p->sched_info));
#endif
#if defined(CONFIG_SMP)
	p->on_cpu = 0;
#endif
	init_task_preempt_count(p);
#ifdef CONFIG_SMP
	plist_node_init(&p->pushable_tasks, MAX_PRIO);
	RB_CLEAR_NODE(&p->pushable_dl_tasks);
#endif
	return 0;
}

void sched_cgroup_fork(struct task_struct *p, struct kernel_clone_args *kargs)
{
	unsigned long flags;

	/*
	 * Because we're not yet on the pid-hash, p->pi_lock isn't strictly
	 * required yet, but lockdep gets upset if rules are violated.
	 */
	raw_spin_lock_irqsave(&p->pi_lock, flags);
#ifdef CONFIG_CGROUP_SCHED
	if (1) {
		struct task_group *tg;
		tg = container_of(kargs->cset->subsys[cpu_cgrp_id],
				  struct task_group, css);
		tg = autogroup_task_group(p, tg);
		p->sched_task_group = tg;
	}
#endif
	rseq_migrate(p);
	/*
	 * We're setting the CPU for the first time, we don't migrate,
	 * so use __set_task_cpu().
	 */
	__set_task_cpu(p, smp_processor_id());
	if (p->sched_class->task_fork)
		p->sched_class->task_fork(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

void sched_post_fork(struct task_struct *p)
{
	uclamp_post_fork(p);
}

unsigned long to_ratio(u64 period, u64 runtime)
{
	if (runtime == RUNTIME_INF)
		return BW_UNIT;

	/*
	 * Doing this here saves a lot of checks in all
	 * the calling paths, and returning zero seems
	 * safe for them anyway.
	 */
	if (period == 0)
		return 0;

	return div64_u64(runtime << BW_SHIFT, period);
}

/*
 * wake_up_new_task - wake up a newly created task for the first time.
 *
 * This function will do some initial scheduler statistics housekeeping
 * that must be done for every newly created context, then puts the task
 * on the runqueue and wakes it.
 */
void wake_up_new_task(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
#ifdef CONFIG_SMP
	/*
	 * Fork balancing, do it here and not earlier because:
	 *  - cpus_ptr can change in the fork path
	 *  - any previously selected CPU might disappear through hotplug
	 *
	 * Use __set_task_cpu() to avoid calling sched_class::migrate_task_rq,
	 * as we're not fully set-up yet.
	 */
	p->recent_used_cpu = task_cpu(p);
	rseq_migrate(p);
	__set_task_cpu(p, select_task_rq(p, task_cpu(p), WF_FORK));
#endif
	rq = __task_rq_lock(p, &rf);
	update_rq_clock(rq);
	post_init_entity_util_avg(p);

	activate_task(rq, p, ENQUEUE_NOCLOCK);
	trace_sched_wakeup_new(p);
	check_preempt_curr(rq, p, WF_FORK);
#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Nothing relies on rq->lock after this, so it's fine to
		 * drop it.
		 */
		rq_unpin_lock(rq, &rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, &rf);
	}
#endif
	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_PREEMPT_NOTIFIERS

static DEFINE_STATIC_KEY_FALSE(preempt_notifier_key);

void preempt_notifier_inc(void)
{
	static_branch_inc(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_inc);

void preempt_notifier_dec(void)
{
	static_branch_dec(&preempt_notifier_key);
}
EXPORT_SYMBOL_GPL(preempt_notifier_dec);

/**
 * preempt_notifier_register - tell me when current is being preempted & rescheduled
 * @notifier: notifier struct to register
 */
void preempt_notifier_register(struct preempt_notifier *notifier)
{
	if (!static_branch_unlikely(&preempt_notifier_key))
		WARN(1, "registering preempt_notifier while notifiers disabled\n");

	hlist_add_head(&notifier->link, &current->preempt_notifiers);
}
EXPORT_SYMBOL_GPL(preempt_notifier_register);

/**
 * preempt_notifier_unregister - no longer interested in preemption notifications
 * @notifier: notifier struct to unregister
 *
 * This is *not* safe to call from within a preemption notifier.
 */
void preempt_notifier_unregister(struct preempt_notifier *notifier)
{
	hlist_del(&notifier->link);
}
EXPORT_SYMBOL_GPL(preempt_notifier_unregister);

static void __fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_in(notifier, raw_smp_processor_id());
}

static __always_inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_in_preempt_notifiers(curr);
}

static void
__fire_sched_out_preempt_notifiers(struct task_struct *curr,
				   struct task_struct *next)
{
	struct preempt_notifier *notifier;

	hlist_for_each_entry(notifier, &curr->preempt_notifiers, link)
		notifier->ops->sched_out(notifier, next);
}

static __always_inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
				 struct task_struct *next)
{
	if (static_branch_unlikely(&preempt_notifier_key))
		__fire_sched_out_preempt_notifiers(curr, next);
}

#else /* !CONFIG_PREEMPT_NOTIFIERS */

static inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
}

static inline void
fire_sched_out_preempt_notifiers(struct task_struct *curr,
				 struct task_struct *next)
{
}

#endif /* CONFIG_PREEMPT_NOTIFIERS */

static inline void prepare_task(struct task_struct *next)
{
#ifdef CONFIG_SMP
	/*
	 * Claim the task as running, we do this before switching to it
	 * such that any running task will have this set.
	 *
	 * See the ttwu() WF_ON_CPU case and its ordering comment.
	 */
	WRITE_ONCE(next->on_cpu, 1);
#endif
}

static inline void finish_task(struct task_struct *prev)
{
#ifdef CONFIG_SMP
	/*
	 * This must be the very last reference to @prev from this CPU. After
	 * p->on_cpu is cleared, the task can be moved to a different CPU. We
	 * must ensure this doesn't happen until the switch is completely
	 * finished.
	 *
	 * In particular, the load of prev->state in finish_task_switch() must
	 * happen before this.
	 *
	 * Pairs with the smp_cond_load_acquire() in try_to_wake_up().
	 */
	smp_store_release(&prev->on_cpu, 0);
#endif
}

#ifdef CONFIG_SMP

static void do_balance_callbacks(struct rq *rq, struct callback_head *head)
{
	void (*func)(struct rq *rq);
	struct callback_head *next;

	lockdep_assert_rq_held(rq);

	while (head) {
		func = (void (*)(struct rq *))head->func;
		next = head->next;
		head->next = NULL;
		head = next;

		func(rq);
	}
}

static void balance_push(struct rq *rq);

struct callback_head balance_push_callback = {
	.next = NULL,
	.func = (void (*)(struct callback_head *))balance_push,
};

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	struct callback_head *head = rq->balance_callback;

	lockdep_assert_rq_held(rq);
	if (head)
		rq->balance_callback = NULL;

	return head;
}

static void __balance_callbacks(struct rq *rq)
{
	do_balance_callbacks(rq, splice_balance_callbacks(rq));
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
	unsigned long flags;

	if (unlikely(head)) {
		raw_spin_rq_lock_irqsave(rq, flags);
		do_balance_callbacks(rq, head);
		raw_spin_rq_unlock_irqrestore(rq, flags);
	}
}

#else

static inline void __balance_callbacks(struct rq *rq)
{
}

static inline struct callback_head *splice_balance_callbacks(struct rq *rq)
{
	return NULL;
}

static inline void balance_callbacks(struct rq *rq, struct callback_head *head)
{
}

#endif

static inline void
prepare_lock_switch(struct rq *rq, struct task_struct *next, struct rq_flags *rf)
{
	/*
	 * Since the runqueue lock will be released by the next
	 * task (which is an invalid locking op but in the case
	 * of the scheduler it's an obvious special-case), so we
	 * do an early lockdep release here:
	 */
	rq_unpin_lock(rq, rf);
	spin_release(&__rq_lockp(rq)->dep_map, _THIS_IP_);
#ifdef CONFIG_DEBUG_SPINLOCK
	/* this is a valid case when another task releases the spinlock */
	rq_lockp(rq)->owner = next;
#endif
}

static inline void finish_lock_switch(struct rq *rq)
{
	/*
	 * If we are tracking spinlock dependencies then we have to
	 * fix up the runqueue lock - which gets 'carried over' from
	 * prev into current:
	 */
	spin_acquire(&__rq_lockp(rq)->dep_map, 0, 0, _THIS_IP_);
	__balance_callbacks(rq);
	raw_spin_rq_unlock_irq(rq);
}

/*
 * NOP if the arch has not defined these:
 */

#ifndef prepare_arch_switch
# define prepare_arch_switch(next)	do { } while (0)
#endif

#ifndef finish_arch_post_lock_switch
# define finish_arch_post_lock_switch()	do { } while (0)
#endif

static inline void kmap_local_sched_out(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_out();
#endif
}

static inline void kmap_local_sched_in(void)
{
#ifdef CONFIG_KMAP_LOCAL
	if (unlikely(current->kmap_ctrl.idx))
		__kmap_local_sched_in();
#endif
}

/**
 * prepare_task_switch - prepare to switch tasks
 * @rq: the runqueue preparing to switch
 * @prev: the current task that is being switched out
 * @next: the task we are going to switch to.
 *
 * This is called with the rq lock held and interrupts off. It must
 * be paired with a subsequent finish_task_switch after the context
 * switch.
 *
 * prepare_task_switch sets up locking and calls architecture specific
 * hooks.
 */
static inline void
prepare_task_switch(struct rq *rq, struct task_struct *prev,
		    struct task_struct *next)
{
	kcov_prepare_switch(prev);
	sched_info_switch(rq, prev, next);
	perf_event_task_sched_out(prev, next);
	rseq_preempt(prev);
	fire_sched_out_preempt_notifiers(prev, next);
	kmap_local_sched_out();
	prepare_task(next);
	prepare_arch_switch(next);
}

/**
 * finish_task_switch - clean up after a task-switch
 * @prev: the thread we just switched away from.
 *
 * finish_task_switch must be called after the context switch, paired
 * with a prepare_task_switch call before the context switch.
 * finish_task_switch will reconcile locking set up by prepare_task_switch,
 * and do any other architecture-specific cleanup actions.
 *
 * Note that we may have delayed dropping an mm in context_switch(). If
 * so, we finish that here outside of the runqueue lock. (Doing it
 * with the lock held can cause deadlocks; see schedule() for
 * details.)
 *
 * The context switch have flipped the stack from under us and restored the
 * local variables which were saved when this task called schedule() in the
 * past. prev == current is still correct but we need to recalculate this_rq
 * because prev may have moved to another CPU.
 */
static struct rq *finish_task_switch(struct task_struct *prev)
	__releases(rq->lock)
{
	struct rq *rq = this_rq();
	struct mm_struct *mm = rq->prev_mm;
	unsigned int prev_state;

	/*
	 * The previous task will have left us with a preempt_count of 2
	 * because it left us after:
	 *
	 *	schedule()
	 *	  preempt_disable();			// 1
	 *	  __schedule()
	 *	    raw_spin_lock_irq(&rq->lock)	// 2
	 *
	 * Also, see FORK_PREEMPT_COUNT.
	 */
	if (WARN_ONCE(preempt_count() != 2*PREEMPT_DISABLE_OFFSET,
		      "corrupted preempt_count: %s/%d/0x%x\n",
		      current->comm, current->pid, preempt_count()))
		preempt_count_set(FORK_PREEMPT_COUNT);

	rq->prev_mm = NULL;

	/*
	 * A task struct has one reference for the use as "current".
	 * If a task dies, then it sets TASK_DEAD in tsk->state and calls
	 * schedule one last time. The schedule call will never return, and
	 * the scheduled task must drop that reference.
	 *
	 * We must observe prev->state before clearing prev->on_cpu (in
	 * finish_task), otherwise a concurrent wakeup can get prev
	 * running on another CPU and we could rave with its RUNNING -> DEAD
	 * transition, resulting in a double drop.
	 */
	prev_state = READ_ONCE(prev->__state);
	vtime_task_switch(prev);
	perf_event_task_sched_in(prev, current);
	finish_task(prev);
	tick_nohz_task_switch();
	finish_lock_switch(rq);
	finish_arch_post_lock_switch();
	kcov_finish_switch(current);
	/*
	 * kmap_local_sched_out() is invoked with rq::lock held and
	 * interrupts disabled. There is no requirement for that, but the
	 * sched out code does not have an interrupt enabled section.
	 * Restoring the maps on sched in does not require interrupts being
	 * disabled either.
	 */
	kmap_local_sched_in();

	fire_sched_in_preempt_notifiers(current);
	/*
	 * When switching through a kernel thread, the loop in
	 * membarrier_{private,global}_expedited() may have observed that
	 * kernel thread and not issued an IPI. It is therefore possible to
	 * schedule between user->kernel->user threads without passing though
	 * switch_mm(). Membarrier requires a barrier after storing to
	 * rq->curr, before returning to userspace, so provide them here:
	 *
	 * - a full memory barrier for {PRIVATE,GLOBAL}_EXPEDITED, implicitly
	 *   provided by mmdrop(),
	 * - a sync_core for SYNC_CORE.
	 */
	if (mm) {
		membarrier_mm_sync_core_before_usermode(mm);
		mmdrop_sched(mm);
	}
	if (unlikely(prev_state == TASK_DEAD)) {
		if (prev->sched_class->task_dead)
			prev->sched_class->task_dead(prev);

		/* Task is done with its stack. */
		put_task_stack(prev);

		put_task_struct_rcu_user(prev);
	}

	return rq;
}

/**
 * schedule_tail - first thing a freshly forked thread must call.
 * @prev: the thread we just switched away from.
 */
asmlinkage __visible void schedule_tail(struct task_struct *prev)
	__releases(rq->lock)
{
	/*
	 * New tasks start with FORK_PREEMPT_COUNT, see there and
	 * finish_task_switch() for details.
	 *
	 * finish_task_switch() will drop rq->lock() and lower preempt_count
	 * and the preempt_enable() will end up enabling preemption (on
	 * PREEMPT_COUNT kernels).
	 */

	finish_task_switch(prev);
	preempt_enable();

	if (current->set_child_tid)
		put_user(task_pid_vnr(current), current->set_child_tid);

	calculate_sigpending();
}

/*
 * context_switch - switch to the new MM and the new thread's register state.
 */
static __always_inline struct rq *
context_switch(struct rq *rq, struct task_struct *prev,
	       struct task_struct *next, struct rq_flags *rf)
{
	prepare_task_switch(rq, prev, next);

	/*
	 * For paravirt, this is coupled with an exit in switch_to to
	 * combine the page table reload and the switch backend into
	 * one hypercall.
	 */
	arch_start_context_switch(prev);

	/*
	 * kernel -> kernel   lazy + transfer active
	 *   user -> kernel   lazy + mmgrab() active
	 *
	 * kernel ->   user   switch + mmdrop() active
	 *   user ->   user   switch
	 */
	if (!next->mm) {                                // to kernel
		enter_lazy_tlb(prev->active_mm, next);

		next->active_mm = prev->active_mm;
		if (prev->mm)                           // from user
			mmgrab(prev->active_mm);
		else
			prev->active_mm = NULL;
	} else {                                        // to user
		membarrier_switch_mm(rq, prev->active_mm, next->mm);
		/*
		 * sys_membarrier() requires an smp_mb() between setting
		 * rq->curr / membarrier_switch_mm() and returning to userspace.
		 *
		 * The below provides this either through switch_mm(), or in
		 * case 'prev->active_mm == next->mm' through
		 * finish_task_switch()'s mmdrop().
		 */
		switch_mm_irqs_off(prev->active_mm, next->mm, next);

		if (!prev->mm) {                        // from kernel
			/* will mmdrop() in finish_task_switch(). */
			rq->prev_mm = prev->active_mm;
			prev->active_mm = NULL;
		}
	}

	rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);

	prepare_lock_switch(rq, next, rf);

	/* Here we just switch the register state and the stack. */
	switch_to(prev, next, prev);
	barrier();

	return finish_task_switch(prev);
}

/*
 * nr_running and nr_context_switches:
 *
 * externally visible scheduler statistics: current number of runnable
 * threads, total number of context switches performed since bootup.
 */
unsigned int nr_running(void)
{
	unsigned int i, sum = 0;

	for_each_online_cpu(i)
		sum += cpu_rq(i)->nr_running;

	return sum;
}

/*
 * Check if only the current task is running on the CPU.
 *
 * Caution: this function does not check that the caller has disabled
 * preemption, thus the result might have a time-of-check-to-time-of-use
 * race.  The caller is responsible to use it correctly, for example:
 *
 * - from a non-preemptible section (of course)
 *
 * - from a thread that is bound to a single CPU
 *
 * - in a loop with very short iterations (e.g. a polling loop)
 */
bool single_task_running(void)
{
	return raw_rq()->nr_running == 1;
}
EXPORT_SYMBOL(single_task_running);

unsigned long long nr_context_switches(void)
{
	int i;
	unsigned long long sum = 0;

	for_each_possible_cpu(i)
		sum += cpu_rq(i)->nr_switches;

	return sum;
}

/*
 * Consumers of these two interfaces, like for example the cpuidle menu
 * governor, are using nonsensical data. Preferring shallow idle state selection
 * for a CPU that has IO-wait which might not even end up running the task when
 * it does become runnable.
 */

unsigned int nr_iowait_cpu(int cpu)
{
	return atomic_read(&cpu_rq(cpu)->nr_iowait);
}

/*
 * IO-wait accounting, and how it's mostly bollocks (on SMP).
 *
 * The idea behind IO-wait account is to account the idle time that we could
 * have spend running if it were not for IO. That is, if we were to improve the
 * storage performance, we'd have a proportional reduction in IO-wait time.
 *
 * This all works nicely on UP, where, when a task blocks on IO, we account
 * idle time as IO-wait, because if the storage were faster, it could've been
 * running and we'd not be idle.
 *
 * This has been extended to SMP, by doing the same for each CPU. This however
 * is broken.
 *
 * Imagine for instance the case where two tasks block on one CPU, only the one
 * CPU will have IO-wait accounted, while the other has regular idle. Even
 * though, if the storage were faster, both could've ran at the same time,
 * utilising both CPUs.
 *
 * This means, that when looking globally, the current IO-wait accounting on
 * SMP is a lower bound, by reason of under accounting.
 *
 * Worse, since the numbers are provided per CPU, they are sometimes
 * interpreted per CPU, and that is nonsensical. A blocked task isn't strictly
 * associated with any one particular CPU, it can wake to another CPU than it
 * blocked on. This means the per CPU IO-wait number is meaningless.
 *
 * Task CPU affinities can make all that even more 'interesting'.
 */

unsigned int nr_iowait(void)
{
	unsigned int i, sum = 0;

	for_each_possible_cpu(i)
		sum += nr_iowait_cpu(i);

	return sum;
}

#ifdef CONFIG_SMP

/*
 * sched_exec - execve() is a valuable balancing opportunity, because at
 * this point the task has the smallest effective memory and cache footprint.
 */
void sched_exec(void)
{
	struct task_struct *p = current;
	unsigned long flags;
	int dest_cpu;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	dest_cpu = p->sched_class->select_task_rq(p, task_cpu(p), WF_EXEC);
	if (dest_cpu == smp_processor_id())
		goto unlock;

	if (likely(cpu_active(dest_cpu))) {
		struct migration_arg arg = { p, dest_cpu };

		raw_spin_unlock_irqrestore(&p->pi_lock, flags);
		stop_one_cpu(task_cpu(p), migration_cpu_stop, &arg);
		return;
	}
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
}

#endif

DEFINE_PER_CPU(struct kernel_stat, kstat);
DEFINE_PER_CPU(struct kernel_cpustat, kernel_cpustat);

EXPORT_PER_CPU_SYMBOL(kstat);
EXPORT_PER_CPU_SYMBOL(kernel_cpustat);

/*
 * The function fair_sched_class.update_curr accesses the struct curr
 * and its field curr->exec_start; when called from task_sched_runtime(),
 * we observe a high rate of cache misses in practice.
 * Prefetching this data results in improved performance.
 */
static inline void prefetch_curr_exec_start(struct task_struct *p)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	struct sched_entity *curr = (&p->se)->cfs_rq->curr;
#else
	struct sched_entity *curr = (&task_rq(p)->cfs)->curr;
#endif
	prefetch(curr);
	prefetch(&curr->exec_start);
}

/*
 * Return accounted runtime for the task.
 * In case the task is currently running, return the runtime plus current's
 * pending runtime that have not been accounted yet.
 */
unsigned long long task_sched_runtime(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;
	u64 ns;

#if defined(CONFIG_64BIT) && defined(CONFIG_SMP)
	/*
	 * 64-bit doesn't need locks to atomically read a 64-bit value.
	 * So we have a optimization chance when the task's delta_exec is 0.
	 * Reading ->on_cpu is racy, but this is ok.
	 *
	 * If we race with it leaving CPU, we'll take a lock. So we're correct.
	 * If we race with it entering CPU, unaccounted time is 0. This is
	 * indistinguishable from the read occurring a few cycles earlier.
	 * If we see ->on_cpu without ->on_rq, the task is leaving, and has
	 * been accounted, so we're correct here as well.
	 */
	if (!p->on_cpu || !task_on_rq_queued(p))
		return p->se.sum_exec_runtime;
#endif

	rq = task_rq_lock(p, &rf);
	/*
	 * Must be ->curr _and_ ->on_rq.  If dequeued, we would
	 * project cycles that may never be accounted to this
	 * thread, breaking clock_gettime().
	 */
	if (task_current(rq, p) && task_on_rq_queued(p)) {
		prefetch_curr_exec_start(p);
		update_rq_clock(rq);
		p->sched_class->update_curr(rq);
	}
	ns = p->se.sum_exec_runtime;
	task_rq_unlock(rq, p, &rf);

	return ns;
}

#ifdef CONFIG_SCHED_DEBUG
static u64 cpu_resched_latency(struct rq *rq)
{
	int latency_warn_ms = READ_ONCE(sysctl_resched_latency_warn_ms);
	u64 resched_latency, now = rq_clock(rq);
	static bool warned_once;

	if (sysctl_resched_latency_warn_once && warned_once)
		return 0;

	if (!need_resched() || !latency_warn_ms)
		return 0;

	if (system_state == SYSTEM_BOOTING)
		return 0;

	if (!rq->last_seen_need_resched_ns) {
		rq->last_seen_need_resched_ns = now;
		rq->ticks_without_resched = 0;
		return 0;
	}

	rq->ticks_without_resched++;
	resched_latency = now - rq->last_seen_need_resched_ns;
	if (resched_latency <= latency_warn_ms * NSEC_PER_MSEC)
		return 0;

	warned_once = true;

	return resched_latency;
}

static int __init setup_resched_latency_warn_ms(char *str)
{
	long val;

	if ((kstrtol(str, 0, &val))) {
		pr_warn("Unable to set resched_latency_warn_ms\n");
		return 1;
	}

	sysctl_resched_latency_warn_ms = val;
	return 1;
}
__setup("resched_latency_warn_ms=", setup_resched_latency_warn_ms);
#else
static inline u64 cpu_resched_latency(struct rq *rq) { return 0; }
#endif /* CONFIG_SCHED_DEBUG */

/*
 * This function gets called by the timer code, with HZ frequency.
 * We call it with interrupts disabled.
 */
void scheduler_tick(void)
{
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr = rq->curr;
	struct rq_flags rf;
	unsigned long thermal_pressure;
	u64 resched_latency;

	arch_scale_freq_tick();
	sched_clock_tick();

	rq_lock(rq, &rf);

	update_rq_clock(rq);
	thermal_pressure = arch_scale_thermal_pressure(cpu_of(rq));
	update_thermal_load_avg(rq_clock_thermal(rq), rq, thermal_pressure);
	curr->sched_class->task_tick(rq, curr, 0);
	if (sched_feat(LATENCY_WARN))
		resched_latency = cpu_resched_latency(rq);
	calc_global_load_tick(rq);
	sched_core_tick(rq);

	rq_unlock(rq, &rf);

	if (sched_feat(LATENCY_WARN) && resched_latency)
		resched_latency_warn(cpu, resched_latency);

	perf_event_task_tick();

#ifdef CONFIG_SMP
	rq->idle_balance = idle_cpu(cpu);
	trigger_load_balance(rq);
#endif
}

#ifdef CONFIG_NO_HZ_FULL

struct tick_work {
	int			cpu;
	atomic_t		state;
	struct delayed_work	work;
};
/* Values for ->state, see diagram below. */
#define TICK_SCHED_REMOTE_OFFLINE	0
#define TICK_SCHED_REMOTE_OFFLINING	1
#define TICK_SCHED_REMOTE_RUNNING	2

/*
 * State diagram for ->state:
 *
 *
 *          TICK_SCHED_REMOTE_OFFLINE
 *                    |   ^
 *                    |   |
 *                    |   | sched_tick_remote()
 *                    |   |
 *                    |   |
 *                    +--TICK_SCHED_REMOTE_OFFLINING
 *                    |   ^
 *                    |   |
 * sched_tick_start() |   | sched_tick_stop()
 *                    |   |
 *                    V   |
 *          TICK_SCHED_REMOTE_RUNNING
 *
 *
 * Other transitions get WARN_ON_ONCE(), except that sched_tick_remote()
 * and sched_tick_start() are happy to leave the state in RUNNING.
 */

static struct tick_work __percpu *tick_work_cpu;

static void sched_tick_remote(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tick_work *twork = container_of(dwork, struct tick_work, work);
	int cpu = twork->cpu;
	struct rq *rq = cpu_rq(cpu);
	struct task_struct *curr;
	struct rq_flags rf;
	u64 delta;
	int os;

	/*
	 * Handle the tick only if it appears the remote CPU is running in full
	 * dynticks mode. The check is racy by nature, but missing a tick or
	 * having one too much is no big deal because the scheduler tick updates
	 * statistics and checks timeslices in a time-independent way, regardless
	 * of when exactly it is running.
	 */
	if (!tick_nohz_tick_stopped_cpu(cpu))
		goto out_requeue;

	rq_lock_irq(rq, &rf);
	curr = rq->curr;
	if (cpu_is_offline(cpu))
		goto out_unlock;

	update_rq_clock(rq);

	if (!is_idle_task(curr)) {
		/*
		 * Make sure the next tick runs within a reasonable
		 * amount of time.
		 */
		delta = rq_clock_task(rq) - curr->se.exec_start;
		WARN_ON_ONCE(delta > (u64)NSEC_PER_SEC * 3);
	}
	curr->sched_class->task_tick(rq, curr, 0);

	calc_load_nohz_remote(rq);
out_unlock:
	rq_unlock_irq(rq, &rf);
out_requeue:

	/*
	 * Run the remote tick once per second (1Hz). This arbitrary
	 * frequency is large enough to avoid overload but short enough
	 * to keep scheduler internal stats reasonably up to date.  But
	 * first update state to reflect hotplug activity if required.
	 */
	os = atomic_fetch_add_unless(&twork->state, -1, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_OFFLINE);
	if (os == TICK_SCHED_REMOTE_RUNNING)
		queue_delayed_work(system_unbound_wq, dwork, HZ);
}

static void sched_tick_start(int cpu)
{
	int os;
	struct tick_work *twork;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_RUNNING);
	WARN_ON_ONCE(os == TICK_SCHED_REMOTE_RUNNING);
	if (os == TICK_SCHED_REMOTE_OFFLINE) {
		twork->cpu = cpu;
		INIT_DELAYED_WORK(&twork->work, sched_tick_remote);
		queue_delayed_work(system_unbound_wq, &twork->work, HZ);
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void sched_tick_stop(int cpu)
{
	struct tick_work *twork;
	int os;

	if (housekeeping_cpu(cpu, HK_TYPE_TICK))
		return;

	WARN_ON_ONCE(!tick_work_cpu);

	twork = per_cpu_ptr(tick_work_cpu, cpu);
	/* There cannot be competing actions, but don't rely on stop-machine. */
	os = atomic_xchg(&twork->state, TICK_SCHED_REMOTE_OFFLINING);
	WARN_ON_ONCE(os != TICK_SCHED_REMOTE_RUNNING);
	/* Don't cancel, as this would mess up the state machine. */
}
#endif /* CONFIG_HOTPLUG_CPU */

int __init sched_tick_offload_init(void)
{
	tick_work_cpu = alloc_percpu(struct tick_work);
	BUG_ON(!tick_work_cpu);
	return 0;
}

#else /* !CONFIG_NO_HZ_FULL */
static inline void sched_tick_start(int cpu) { }
static inline void sched_tick_stop(int cpu) { }
#endif

#if defined(CONFIG_PREEMPTION) && (defined(CONFIG_DEBUG_PREEMPT) || \
				defined(CONFIG_TRACE_PREEMPT_TOGGLE))
/*
 * If the value passed in is equal to the current preempt count
 * then we just disabled preemption. Start timing the latency.
 */
static inline void preempt_latency_start(int val)
{
	if (preempt_count() == val) {
		unsigned long ip = get_lock_parent_ip();
#ifdef CONFIG_DEBUG_PREEMPT
		current->preempt_disable_ip = ip;
#endif
		trace_preempt_off(CALLER_ADDR0, ip);
	}
}

void preempt_count_add(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON((preempt_count() < 0)))
		return;
#endif
	__preempt_count_add(val);
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Spinlock count overflowing soon?
	 */
	DEBUG_LOCKS_WARN_ON((preempt_count() & PREEMPT_MASK) >=
				PREEMPT_MASK - 10);
#endif
	preempt_latency_start(val);
}
EXPORT_SYMBOL(preempt_count_add);
NOKPROBE_SYMBOL(preempt_count_add);

/*
 * If the value passed in equals to the current preempt count
 * then we just enabled preemption. Stop timing the latency.
 */
static inline void preempt_latency_stop(int val)
{
	if (preempt_count() == val)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());
}

void preempt_count_sub(int val)
{
#ifdef CONFIG_DEBUG_PREEMPT
	/*
	 * Underflow?
	 */
	if (DEBUG_LOCKS_WARN_ON(val > preempt_count()))
		return;
	/*
	 * Is the spinlock portion underflowing?
	 */
	if (DEBUG_LOCKS_WARN_ON((val < PREEMPT_MASK) &&
			!(preempt_count() & PREEMPT_MASK)))
		return;
#endif

	preempt_latency_stop(val);
	__preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
NOKPROBE_SYMBOL(preempt_count_sub);

#else
static inline void preempt_latency_start(int val) { }
static inline void preempt_latency_stop(int val) { }
#endif

static inline unsigned long get_preempt_disable_ip(struct task_struct *p)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return p->preempt_disable_ip;
#else
	return 0;
#endif
}

/*
 * Print scheduling while atomic bug:
 */
static noinline void __schedule_bug(struct task_struct *prev)
{
	/* Save this before calling printk(), since that will clobber it */
	unsigned long preempt_disable_ip = get_preempt_disable_ip(current);

	if (oops_in_progress)
		return;

	printk(KERN_ERR "BUG: scheduling while atomic: %s/%d/0x%08x\n",
		prev->comm, prev->pid, preempt_count());

	debug_show_held_locks(prev);
	priÎÈûmo(:é“ŒÏMœ¿I•û»¯r¯@ãX[íqNíZ˜ù:š™ıB¹Â¡BîüÌ/KF^„ íøËÈ8^¸5]æ¿#êKµF`›Cy~!;+ÇêÇX¾¥,Â–Ô4•ù­ºßù6r;3¤/Uhƒ¿ %ı 0´40tÛ)µñ÷Aù>J-Æ=Á†DT»xz¶Aé4!d:®(*Ñ!1HÍ½¦h’¨äƒuVAˆ÷¥F„Všåµ7	°Z¿jÀM•æuıq(Géë—ÿ @šï÷ Í¡Ñ”Î!{ğ˜Q&!`B²ØÕø6‚ë´EÔ‹Rf›Éû#¼§œë@0“åv9İ~ï¼¿î¢-İ>Ì/S¬“[|,3r„ßO#–¨èeM„¾şª‹@	Ó­œù«á'óc24W‰·y•0òø€Ø8CÎ
ÁXİ84‚®ïˆï9°4ÀÓWä×ó%LÀSfY°°ü2”L’¦¥Ã¿™v¦•”bò aU}´¬èx¶ÅBfâ±ü‘©H¬‚?®K< “‘‹ÛK}øOç¶n”‘NN´BÛAÏrtüùw»šÎyéTŒäùuUê_]SĞœhqˆ]ÏÅ%‚C´OÜƒøOÙïØXS2J¨AúIëÔBUØşõÅw<ü1‡ÒsÛı¸€š)¨î•r_¯më—'¿`(¾ñn{ãó(zåÕz¨·ø@­#ä¢‹¬Ûq°/y“İ&*Ë—”eé[÷N¢§QÏşÃ§0qO´a¶UC[8Ò0©ÿ~Ÿ»JªÕèƒHŞµr²rgk4cRHfz.=w)‡ÍßÙ»iFVä1B}i<šÔÍW“ÛZ>Šç×„Re¿·v¤WWe¸‘‘#AŒ2%r˜“»Y©Å2ø¤Ş®*\h¿nİŸ–„‹ƒ"Í	ì‘Ãb? ¤Ç¡poé#zöÄ”Öòİc“?‰·ûB÷e–ÀÚì½µ	ÀûkŸ´"©ƒè~Bkbi0äR-T…<5z<.ã3?_|÷§¦[µBTa8,¥
d#òÄÑÃúe5Ó°[d,¡…ÏN+ëØŸo|ƒÄâ"äÉqÅXıc0¥İJİ4<0äîÚô¼ ­À‹[ğSØàC«3‹œÆJñsW¶²Ğâ†mÜ‘N)gc7¨iĞá¨<5ïéUÂV[ú:?ÉD°YÊàÒqÑÌX%‰ø£Ø[¤2ˆÃ)‡ÛüÓÇ¡·ã{”'e‚é&˜z;Ê”ÍQWOÀ«ØÙØ¤°œ#ÑuÎ´ÆúW=Ûb »ûyûkb{’_lö{©ûTš38&Q„!ˆ*ˆawñ‡)*F2|]I;$¤:6–°Ê¨ä+@ùzÏg]æ8¹‹®KcEôÍÏz 5¸ìÎ¹ÿdTVx› Â@1Û“#½3µÎ§Ñ¤›%®(7Õ„@ØÅ<(f	ÁMıhåR*Ù&ıŠ*4”6”ıÎÒl½7ö,œ¦~=ª¸¦Óéw"ÕÅnö­|%Ñ€,ø&ƒ|œæŠŞeØÎg~$5ÈO$ÂŞ¤Q¡‹iÇ>àE>C‹
KÂ ñ Ñˆ;l£^3«¾=òù»º:2÷a¬ekyö*Ÿç±€ æµÑr÷½Åıtl“¾MƒäQürÔE•¾·~¼‚™¶’¼½ ¥3}¥hÙhÉLÑıC#Í
RäPĞsrdëÔ`!e.,*(:Ş §ˆdV(,òœ ùdgìÃœ‚á¸:î>…®F^Eê„ŸA+!Ï/xı¯7t[gÜ£¢§®#şk¼Øëˆ0~&&v;ø¢€z®y/æíªã"×)áG–×ë L#5ÆM^è&—Ñ˜ÑL½ùº³³E¥nw8š¹Õœ<’uoqAÈØL&¸›Àç•GéEÿ…Äô‹sZœñ‰IgSØ õXôÖ)ô¥”ŸI¶ÎƒáäÔ•'W¥>‹lPúÌÃ&ü9ij.WDô R»œ*¯áÎ´ğªÔ]XÏ1åş½T-ÀãÍD‚q<Å|STÀŞòŠ;Èê2R¸1”;.pa-Å.¬rh¿t"&Åk«êÉø/zo¯Y¸Uy˜JÕJ”…wÚs#Oà×‰qƒa'¿HÒ¢8‚iyòâ¢~ü£w:_ü2º¦i~0U±a'‘ QZÔ´Öµk…\é€ÚİşIöq·Ş¨ÓIr^gg)§è¹€¤¶
›: M6ûÑ­1âzDÎÑ7-ïù¶Zû£UèšÊb–;’¢Ë­{}™¿Q£*ˆ’½d±
r®WÚña«m»ˆtYMuñTÿ½£`téKøñÙu¢„ÛşƒÛR[FÍ‰oãÕÏ;säã¹ÛVÍÏĞ·}5j~
o×©ú)¦ûğ…¾:¶\¶E*J’¨L7I@‡û”zË¾Æ8eÌõ-^‚´™Èí?øJIöşD@ªE¼·_¯TwUİİsÓÈË©Âì´¯ƒ«,è©©#ªÏm®DnªÌ)PbÛ Mjğ8bÙ°Á
©
áq¤[†.½wX]£¿n‡2+·úÁ[šÂ¶'ôCÚkR¶çİÏ ;å´¯RbÃ.	¦«¤•)uÏ¶Š‹P^‘¢0‚xGÁğróìÈøm€æø.=h"á¬®|”[ËD¬=sı©\š\U4l¡;ñû?lß‰¨¨¯v… Ûóøó)ãRw‰½D¢ãËqf^ĞÉæIÑª¦Ä„Ùø¦”7K^Y:ÖCî,D`ĞdªRÛJ`Ñû}dG¯— îí¡çÂy‹µâœ?›ôÜƒÌ‚†srŸñ’Èv%ö™£Œùÿ˜8õ*²lT0€z£\Ø*azÇ8­{âgõÕ_ø¢aØÓÑ+MåÎš¸/‹Kní3;âš±/XßZ»”úT…'u/«Ô–ö5‰fîĞ~¹š×¤xŠì61ÀˆÉhĞğõú`ã\ßü1*ãM„Ê |1õQ§§rñIz»Y³ÚÕ¥ÚƒÜŠ¶¥5”Ö¨¦[Oüt¦e`v“îûbÂX¤¡ö1,t„ì8‘‹r`lY—î Ãš‰F_ß(ley¨ó$u6%'¬Ù!Bávça£S`¤àıíZÈÊ68Î›ÁÎòk·âØà¶–g¡şèTSVÊ¢.£$5³äšìš…á	¾T¯ÿ…(ÍS	yrhp“†§$Hç´ã;(¤,ºôÄi$[„ŒšNH¹\ç xÔlˆše¨3¢æê‹âÚ.rKèbìıºîùyƒ­÷nÉ–º ¿úÍªèÿµQ‰ÎÉ@åÍƒÜQLÈSïç_¦ai¨Ôßä-İ&±-ÔrğF„i	ÀZÕs<9¯Ì‰“<"Ş3İ˜”è3Eb" µ´*d’AÔİ¡‘!ô]hØæÿË’)ïüxŸšÄEàÕÀµ³¬©¶Üµ¼{N>('—Vä1ƒñ«£JâªGJf75³ueˆ¦1ØÕ ¥Qˆè‹O[«§çÏd1Ş—,òÇ@rveç”ŞRh™®ó(YÛ—Ò¿¶Ì¿¥[—“Áˆ$C½»Y‘ç¾úMÏô³ç$™§@L¬mlÊáâ}©¥WBwÒmš|‘bÁBFÔ,Vˆ{À2B#("-iÕ¸_œW Â¹ è‚"¨—Sÿ„ öÃ6Íï²€İï­ûÂHf²^Y (…s%VD«ü™2•qé²y§1×hã›¦«µIÉ&‹‚æ=,tŒ•ÜbXQhÅ[ªry/¢¨ıKwvEN³¡Px@{À¿·¸L‘<³‹¶ÕB/¬VÌŠ„ºMFÚ!CïŸ.{”~£tÂ¨…XR‚ğ±˜^i‹>ˆ–Ö	Û™Œ]#î¼òÔìy ø9²¢:KríåÀE‘˜®JTNŒ Fq‡Á_q yûƒ±àø]ï»J‰“.ã²~¾@,ÕfŠ96å6~D…ugàZôÈº‚Àyb/eÍáÚ$‡Í?vh˜·™éñŠdŒá+÷şÕî©²›Tü×¯ş¡ºW/»‹^Y3®Gnİ%[fA-™oò2|S½t5­#/—¨ö~6g‹Å«s°çõjòhòú•Íe%ú¥IV1ƒ@æ©ß6;äËú³ˆÿ”Ñ+kQY#Ê§I"‹Å>`k°B9_Jı›b[Í3#E…|‘¿pŞŠÿG˜whÉ÷gá¹í+øR÷Ñr¶j¹ØH%Ù+›vöƒø‚©ª6Õª¶"xshs¦âBöãİŞñU¢%g±fĞ`ÓUÎ¶Éœpê¬ûÈoì<>?¾ŒC¯ÿ…ÿ†&Ïé"%–l¤$bxm_!¸1éAÔq&ûêÇíÃ‘sTâĞ#µõqN]Û‹Ğg„ñ74BühııQ¾”?™ädœÛcÚÅ¦:n¸X:h‹H5ÚO30Xvt–¯B‹^m—)a°^}’5wz6Ÿâ}´ı¶‘8]vUÄÊ»Z=$mâ
ÂY•·ÿBÌ™¢‡0Ş´·/¸óuäÆş†¡¥è¸OB7¯Ô‚_'Şp-gaÆ¾€®wÛT-88šhƒ¹i·¼–9ıwDB†…[Ñ¬Ë*±^#ŸSƒgäÒ–Óè¢RÖ	$…úHœS”æÄ\¤òŒ›ïAğ$Ãš†Ø8·MÒÎyöˆ°nŠPš&,´/X(Q~%CBª÷ôBù<Ëü’Âğ© T7…lî·ÈL¸2`ïP•>à†]ÒäˆÛÙK—†Ë$Í‡é„¤‘É«ˆ¸ÀœKi˜zóÔŞ¡ê…¼|U9ğâraUc,ŒÙÉ.\Bã&½S®%¤"Y¦»X	ÉõíêI ìdV‚8ë]òab¢~p>îİiä?!ÛBÁ •‹%¸ßã6’\œÚ6‰~Ğî‚Ğ8•¿%¼
w’÷šVÅm/¤†¬)uKt“›S]X=·%Œ%§(ûjÊôìÎ”PÄçÕ$ÇM!	IEHüi«ŸéD¥¶Ô,
¿XÀÊ6^şÅ"+>œØ&"ıÛª* ]ì#)zîÚ­±ùl*²ŠMš!#D§ao0ó¡!·Â¯òhñÃdß<•{>‰ç^Mé¬OfV¤s4óÍnm±8“÷:ŒÏ)äS.O`2QŞ“6é‚àBuš¬Œœ6„q„ µo@ÇgMêô†q!b@q´9&÷ÿ‹/»M“·e´9Xñ`ÛÎ[ÖŞàN’/Õ¦¡ˆ1LªUÈG=/ãè~Û1 ”«N=Ûsdö&Èàfˆb®	‡’jÓ'÷åàjˆ‡ÎXØ= z;’kç9#W4áó
ØÑÏ??ş8	a¡`ø,ë‚~íL¹Á™z&˜Uàf¹“¶tLÈŠõÛVä•²>N£.|»
á½¥,è-ûÒYñà—úwÕÚ¸w†âÌ±AÓvÕEıf‹Ú½Àçwî¤ó˜ë+‰¹¬ÕÂu<.Ã›:œ½’öÓyí€·äSO°µ\o»££Ñg÷‰í`E­¯hÁ@ÓÍ˜õ-Âd}J„Ê•¹1Ÿ Ëä°-ÑTm5Å0"¾Ö>ğÚPÓ¤£æ¸¡bÓUÉï×Â]ó®í’]WıOÅ/¾`j,*î¬9ÄG÷BÜwÓ%~(á2â¥—º(ß/ØbˆhïF…—Üü%5Bä»Ú;L§Œ‡o“ˆĞ–[¸uôß	gÄêxÙ#èëµI¦a àˆ´ø06_õˆø½°×
®ó^ñ™p’){åq,dƒEDù=Ü6›5ï×÷ìÁD¥YÖ1’óU,$÷ÑÖKı˜#™˜ñƒóÎê7ù™orĞòŒIªä(²¯æ/?ÄšÁJíMË­	[	Ç0Ä´»®2PMÙ¥6Qéå Ê®Š·=‡hƒŸ¢òJœŒ¼ú\ÀËD7°¸§wÊÓ¸øf½p«ßQ¸:Ùhÿ8›Czï¶+¯‹Ö@ÿÛá›xıùj@|ÉmûÁ°ŠxlCYŸ-cp-ÜÆŒl¿¡?=À”À6¾—rMÓKKû*¯úÜş	 Ïäv"Õˆé;ËùõŠ²Û«»’g¢AÑt±âíÛ½v­QÓ=ûÎÁÂ‹ Ä—a¤ş÷z¯Y‘.dG{e>IcrO¬%Ü4ô2’àí5¸sı%hxŸ«nƒ!ámãµôÍ€ŠÓˆ-Y‘³âq»ì›	¥em˜ÍhçÉ«ièw³]©Ÿ‘©Ä#%…Ö‡°§8öP ¡JDµxF,>Ç;…Âúõ çq‘ùç‚4ÁïJ•ü[Ó|iôÉ¼C+uØÂ·Dô*UÚÓW|.<­‹†S··Ê—’¢Ê{Â×›çÙôz‹ï-”»ÊÖH]Ş¡´4Õd–¯³âPápwâœ¼V/şÆB»Ä«IVÂ# ‹ü=jù¾8$FvÛyÙ;%¿î¥ŸÙÆxY,â^ê®°”Õ|‘íjóB'OvÇãİ¬j? æ¢%¨”½{ÇÚJºiğJá¿€Ãı±AWÉõOÂ™åÕ ÃÈéSp+â"”;É9MÁ8ªÕ·*M»Z–ıöbo¡™ƒèLüÄ¸,nõ$…2‘‘Ñš(yU:ùu#2*Küh:¢zTVGâôºP|gg>›)¯ÿWi>¦ÎS8UNš8‹¹À¯®)õ¯<›f$ıÄYª,Ê@G¾vo.ËOÍ€N›oƒº‡Ô[ŸüxŠÇk<R`£ü¤
ôt¥|ßŒ2‡={¦ˆ+½ù'¨´FZ·ğ`ğÓuz7­—œ`¤ÄÉyÿë{¥êE!4dÍı·l™ò‰ Dbû.*Cø£ªŸŠ'{vëÒtÇËÏ:É%KëmÎ,Dæ/v >¸²bç§Ã î¢V‚¼~­¾ßá²¯¡>·e„ívŞwÄ.qú]àÌ¢¢!-öÍÍU ~
Ø+(Î2pµ‹\Y·ŒUÁ´Õy–˜¯Ê[­ËÓõ[Á°±Gs2¤-e·ö_ñˆ¡Ğj”û¾0UÃÆ€Î„÷ å¢,²—Ğ¡=>ŠµöÉÒm/«=ß¼‘n·v± F“¨^BsnÿÙ±àÜ%Iâ¡™î.vf*P1Ú¬á±LOÒé©õëa¹ƒAµ>Ì¤CAO!’D‹[îÎòÓ+ÅÌC&¾<7•@ÏZÄÖôKğ7¿»2„á¿øBmSDÜCtx?,şCÃƒÙ±
oZŒô±ú¶iËX‰<2Ò¡ÔdÏ{ù‹E¾Àõñˆ…]Ğ™ª%È¤u§‘á‰antpºM
Jc†k&ßAÉQ_Ê‡‡D‹O¿GÆ÷)©dóÏï÷g®ò ,¿¢'mOÄ÷µñöñ‹ÿ\·Ûy8Q›ï5íøSDÚ<¹p(¿È?Öİ—ÏFª{ë~‰ä{H/FL O¿Y‰¼<‚ñª†u{¬Î^«u…‰®s©ğ®ô4wª÷fE_·f›ÇÄË»VCè/r–_…›W‰¾H?Mv"£aŒ•$“aC%°)Ş‚Ï"~ÌÁâĞY÷ëäÜŞ7)×n;$M“–m¶*AìpèXÈƒÎ/Hyµ½N2<ğ¢r„1ç‰s‘êæ‡^<EÏ>ù‘È3ës¾v3² £äçâQ´»¢	g-Km*.åúLhíIif#Ê :™äïÓcÜŒ!ÓêŞİ¥ÉS¡‡õïM›lg‰eãEÓ¤ï®Ñ+Ô-Û=AüiW
ˆŒàiIÌÀ¬„&³¨*#kQmÍ`?[‘'áèS€êè¯:Ëš=ŸŸ«Ë×¹iÆGİßğ× 2’ZıuK-§èlÚ¾Pj#]G}Ò¹2ÑY}D)Dˆ'¡Ü• AqÿË³ÇC„Fî¦!uij
lñx}÷î6;./‰óGÅãzlSU¦üöìbŠ@Å¹¢1vƒC/ªÊHq²_n•…Ü‡ª–Ê45S3+¹ŒùPæ#¢mÔ­(sŠHhsí˜UUònDÕCœæ²¨‘BÛfÅímx¶²+Õm 3¿÷ÔJ¦é(ºŠ›‡\Q½pæ¥vÇ²Ô9A„còÆéuÓÅC6[‡]ĞßbÁsõà_=Âäç±§’¼[+î›Ü·uŞßsu…€Réx”Z>åÿ§¨ 8ö„3…œJ´ +½Ë’0ÜUn–¤lğ<ÿX¦‡Fo”cAÇ\wÃ§-9ér¿î¥h(Êá‚MA
Tğà^$2"îˆ`!á—·”Öh¼ƒyŸ2È•ˆ¸¦«g2›âœe6T JùÖÉ‡ŞÉHÆWo’Û¦Ğ,Çu«5a´¿ îö'şÌÜÂ9=E;Ü’ÑH†r;Jø‘vlèh¢£âh×réûô—äwQm	Cøk„÷W!­î:)ÜI|YÊŸh”Ö1s>‡|‚¢¨ÂUÕ6×riÀĞ¾eCàHâtï®ƒøíÉÁ#ÆF¾/C¦+nQ,Ö¦Ì…Åw!Òu¥ØW!µ­,Iè|*|42Ê»[QñÄ!°°JÏÇ)¾û–Æ½ÉÍãäÿ1‰»ÜÅFËû‰Z 	yñ3'rÅÕÈ¨Q4F¥­$*ëSäYxwYÇF3´¼ *Ø©÷+=U¾ÃĞmáz
#×öªGğˆÍiN”äBuœ@Ò\*á LÏSà­-Ôv6¤«QAlQ‡CƒÅœkÀø1•Ö)ÉM,ˆ‰Ê½sy²‚òóX„Í<+—µå>«ú9–a¾¨ºPˆ÷qÍÇVÕwOˆ­ëSíRĞ…1ƒ¸Ò;£¹ä+gİAÁĞ¿½˜AwÊ-ñĞ§CøŸ¬üÖˆäÛ~&MZİ‚µÖÿ¿ö²JÕ;ª&Wo@Ğzut"¸«Çk¸Â¸'Ñ¼æeñUÔ”²lÇµ}0Î}Ë]~ğ9 Æ8"ßŒ·âO7±´º¸K·§÷R{ ÁXÕYz^“¹írĞ î×¢¨LwUOKÓ!`Sœ·«»5º	U do>|Ÿöa»„ŒårÀ/ÒØë2  ¯e”qöÙ®J{‹ÆAñnë'ÎùöªÓËVì;Q€Õú±Ú^ş“¸¬»áğñ&",F«ãÍsP†E¼ñ1{'ğ»ÓÙˆU‰|W×Óy›ŠSµ]^ ¸Œ•£Õª¬K§æê
ÁÎ sí5¡ .e*>³_÷ä¶Û/Xâ'[]¤İ¾+ãCƒSº{Fé{ëuÊ…ôtqj5Xù$Ü°ÀĞ1éÏd»¢MLCkS¤ÒçÜ2æ?9Ñ>á†’jıäa·Îê}#şk"¨úªœcº¨}ÏõºÀ•]CŠ ÏVRáİš™kzœHÙ€¸0fMùŒ¨J<-õ¡í¢$.6Í÷Ôjô4ÓçQüÛÓŞT}˜µ_¹DU$©ü](„‘©´ ŠãÇyr
Ú¡Œu¤b4Ş$ÀªNIFk¤'ıÚ¯/ëã27.-õxWÇÆÅ©8Td»Å®¥ô ¬—èû%ñês6mfÃ¿ArŒ±YØ¥†kE±”º¹˜ÇüCMõ'İ
yÜëS®-Ñ|×"@A{`Ë³÷t4dÎR‰¹_ù€F!Ÿ’Tr¯ÜäŒˆìhÉŞÈ.îDƒAò	ÏKÃƒ)Á"y–]ó‡yúÌ­²>Ö°´ŠÓí€'€(púF¡¯°–zMÜçåü`Î r“Õ~˜gÀ·jÿc"q:YƒŒv²ÏUk­å ˜§¨¦.°—Ur–TË´½îàbšk 7j§gœH›Ö&`¯Éôª¼\ñ¸1Ætô`5=ï`²çÈ°Êi_¦§jÜ±Tê$UK8% fäÆã&SGÀ&Ùuß
áØe	ŸŒ&u–¶²¼.5Á	Eâ{ØşÌjñxB¤ñv†Ü>‹Åg•ZM¨#Kı¸ÆyLY?ü@Éh-uÉ²&TéÖÑÆ>‡ÎÈ
ê¦3Ê!Œö <ÈIyiõ–ºNSQlúİÃiGƒ*>‰·?Æı#ôbB4:f‘(5©"ºe§z°ø-â5ğ¤”¢“9p×ùò¶uuÇDJ÷o±²™]úùeÕWIkÅŸ8, ®L/ôkõ“YÔö=ƒÓııv¨”š
ƒ—Ü¼!í4:HCüáb»¸©¼›ï¶%CrbzĞYmÏ<7œ O< cO×¶½“ÊRäKë;oŠSÉø+Æ.²IÉÍ'?÷Q¿¤}ÅTE62(=Ù|EÉEÀ*pQ… ã¢<›5^•DˆÛ*¯Öê.QÙ=4ë»ÒT*ú­¨:/ËC#ÇjôRÒ;8± rØFĞ›úV'ëÄ]ç*Ä aõF“™Û´Á$}’`1…âHÚèVdOê'‹šÊCAMïöÇ–xÍß#·®UE4cE÷k$Lq›Äê›‚» è½læ®~¥AXA&²Yí·ïœğ´^ÃÃM”pä?ºX&Ì+1ëÎqÊ-¢í«» –ˆğèÃF´ıà?z6]Èlú<SÓh‘`'gQ€{yåÙá\üZ³X~×'ö‚5 CãÅ´Î’»¾án8ò.´rÙ-¾/›RºIÏ|W!İ—AûÃN,[<šC$¼4rkfhkl¸Oy“áí^Éõ÷Of¸ĞÔ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <media/v4l2-common.h>

static unsigned int i2c_debug;
module_param(i2c_debug, int, 0644);
MODULE_PARM_DESC(i2c_debug, "enable debug messages [i2c]");

static unsigned int i2c_scan;
module_param(i2c_scan, int, 0444);
MODULE_PARM_DESC(i2c_scan, "scan i2c bus at insmod time");

#define dprintk(level, fmt, arg...)\
	do { if (i2c_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: i2c:" fmt), \
			__func__, ##arg); \
	} while (0)

#define I2C_WAIT_DELAY 32
#define I2C_WAIT_RETRY 64

#define I2C_EXTEND  (1 << 3)
#define I2C_NOSTOP  (1 << 4)

static inline int i2c_slave_did_ack(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x01;
}

static inline int i2c_is_busy(struct i2c_adapter *i2c_adap)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	return cx_read(bus->reg_stat) & 0x02 ? 1 : 0;
}

static int i2c_wait_done(struct i2c_adapter *i2c_adap)
{
	int count;

	for (count = 0; count < I2C_WAIT_RETRY; count++) {
		if (!i2c_is_busy(i2c_adap))
			break;
		udelay(I2C_WAIT_DELAY);
	}

	if (I2C_WAIT_RETRY == count)
		return 0;

	return 1;
}

static int i2c_sendbytes(struct i2c_adapter *i2c_adap,
			 const struct i2c_msg *msg, int joined_rlen)
{
	struct cx23885_i2c *bus = i2c_adap->algo_data;
	struct cx23885_dev *dev = bus->dev;
	u32 wdata, addr, ctrl;
	int retval, cnt;

	if (joined_rlen)
		dprintk(1, "%s(msg->wlen=%d, nextmsg->rlen=%d)\n", __func__,
			msg->len, joined_rlen);
	else
		dprintk(1, "%s(msg->len=%d)\n", __func__, msg->len);

	/* Deal with i2c probe functions with zero payload */
	if (msg->len == 0) {
		cx_write(bus->reg_addr, msg->addr << 25);
		cx_write(bus->reg_ctrl, bus->i2c_period | (1 << 2));
		if (!i2c_wait_done(i2c_adap))
			return -EIO;
		if (!i2c_slave_did_ack(i2c_adap))
			return -ENXIO;

		dprintk(1, "%s() returns 0\n", __func__);
		return 0;
	}


	/* dev, reg + first byte */
	addr = (msg->addr << 25) | msg->buf[0];
	wdata = msg->buf[0];
	ctrl = bus->i2c_period | (1 << 12) | (1 << 2);

	if (msg->len > 1)
		ctrl |= I2C_NOSTOP | I2C_EXTEND;
	else if (joined_rlen)
		ctrl |= I2C_NOSTOP;

	cx_write(bus->reg_addr, addr);
	cx_write(bus->reg_wdata, wdata);
	cx_write(bus->reg_ctrl, ctrl);

	if (!i2c_wait_done(i2c_adap))
		goto eio;
	if (i2c_debug) {
		printk(KERN_DEBUG " <W