/* SPDX-License-Identifier: GPL-2.0 */
/*
 * kernel/lockdep_internals.h
 *
 * Runtime locking correctness validator
 *
 * lockdep subsystem internal functions and variables.
 */

/*
 * Lock-class usage-state bits:
 */
enum lock_usage_bit {
#define LOCKDEP_STATE(__STATE)		\
	LOCK_USED_IN_##__STATE,		\
	LOCK_USED_IN_##__STATE##_READ,	\
	LOCK_ENABLED_##__STATE,		\
	LOCK_ENABLED_##__STATE##_READ,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
	LOCK_USED,
	LOCK_USED_READ,
	LOCK_USAGE_STATES,
};

/* states after LOCK_USED_READ are not traced and printed */
static_assert(LOCK_TRACE_STATES == LOCK_USAGE_STATES