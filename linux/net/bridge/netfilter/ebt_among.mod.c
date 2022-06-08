// SPDX-License-Identifier: GPL-2.0
/*
 * kernel/lockdep_proc.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * Code for /proc/lockdep and /proc/lockdep_stats:
 *
 */
#include <linux/export.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/debug_locks.h>
#include <linux/vmalloc.h>
#include <linux/sort.h>
#include <linux/uaccess.h>
#include <asm/div64.h>

#include "lockdep_internals.h"

/*
 * Since i