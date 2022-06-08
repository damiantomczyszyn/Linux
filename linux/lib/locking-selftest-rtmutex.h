s);

/**
 * struct pending_free - information about data structures about to be freed
 * @zapped: Head of a list with struct lock_class elements.
 * @lock_chains_