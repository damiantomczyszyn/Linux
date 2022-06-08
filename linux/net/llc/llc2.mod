move lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allo