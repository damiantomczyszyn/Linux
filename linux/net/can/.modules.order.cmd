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
#define