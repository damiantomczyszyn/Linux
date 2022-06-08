ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track loc