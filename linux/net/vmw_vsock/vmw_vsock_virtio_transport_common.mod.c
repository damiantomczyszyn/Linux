reempt_dynamic_undefined = -1,
	preempt_dynamic_none,
	preempt_dynamic_voluntary,
	preempt_dynamic_full,
};

int preempt_dynamic_mode = preempt_dynamic_undefined;

int sched_dynamic_mode(const char *str)
{
	if (!strcmp(str, "none"))
		return preempt_dynamic_none;

	if (!strcmp(str, "voluntary"))
		return preempt_dynamic_voluntary;

	if (!strcmp(str, "full"))
		return preempt_dynamic_full;

	return -EINVAL;
}

#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define preempt_dynamic_enable(f)	static_call_update(f, f##_dynamic_enab