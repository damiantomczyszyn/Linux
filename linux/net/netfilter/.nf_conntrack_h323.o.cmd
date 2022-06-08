e if RCU thinks that the current CPU is not idle
 *
 * Return true if RCU is watching the running CPU, which means that this
 * CPU can safely enter RCU read-side critical sect