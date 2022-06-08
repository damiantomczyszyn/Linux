st_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 