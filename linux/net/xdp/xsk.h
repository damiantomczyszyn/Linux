/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef LOCK_EVENT
#define LOCK_EVENT(name)	LOCKEVENT_ ## name,
#endif

#ifdef CONFIG_QUEUED_SPINLOCKS
#ifdef CONFIG_PARAVIRT_SPINLOCKS
/*
 * Locking events for PV qspinlock.
 */
LOCK_EVENT(pv_hash_hops)	/* Average # of hops per hashing operation */
LOCK_EVENT(pv_kick_unlock)	/* # of vCPU kicks issued at unlock time   */
LOCK_EVENT(pv_kick_wake)	/* # of vCPU kicks for pv_latency_wake	   */
LOCK_EVENT(pv_latency_kick)	/* Average latency (ns) of vCPU kick	   */
LOCK_EVENT(pv_latency_wake)	/* Average latency (ns) of kick-to-wakeup  */
LOCK_EVENT(pv_lock_stealing)	/* # of lock stealing operations	   */
LOCK_EVENT(pv