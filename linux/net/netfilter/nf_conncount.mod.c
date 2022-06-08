p_next_lockchain(long i);
unsigned long lock_chain_count(void);
extern unsigned long nr_stack_trace_entries;

extern unsigned int nr_hardirq_chains;
extern unsigned int nr_softirq_chains;
extern unsigned int nr_process_chains;
extern unsigned int nr_free_chain_hlocks;
extern unsigned int nr_lost_chain_hlocks;
extern unsigned int nr_large_chain_blocks;

extern unsigned int max_lockdep_depth;
extern unsigned int max_bfs_queue_depth;
extern unsigned long max_lock_class_idx;

extern struct lock_class lock_classes[MAX_LOCKDEP_KEYS];
extern unsigned long lock_classes_in_use[];

#ifdef CONFIG_PROVE_LOCKING
extern unsign