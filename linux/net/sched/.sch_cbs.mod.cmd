* use task_rq_lock() here and obtain the other rq->lock.
	 *
	 * Silence PROVE_RCU
	 */
	rc