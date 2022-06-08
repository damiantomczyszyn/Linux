inline bool usage_accumulate(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		*(unsigned long *)mask |= ent