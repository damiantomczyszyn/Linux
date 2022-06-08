ority = p->rt_priority;
	rcu_read_unlock();

	/*
	 * This one might sleep, we cannot do it 