	cpu_util_update_eff(css);
	rcu_read_unlock();
	mutex_unlock(&uclamp_mutex);
#end