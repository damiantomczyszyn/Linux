# SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT		:= n

obj-y += mutex.o semaphore.o rwsem.o percpu-rwsem.o

# Avoid recursion lockdep -> KCSAN -> ... -> lockdep.
KCSAN_SANITIZE_lockdep.o := n

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_lockdep.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_lockdep_proc.o = $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_mutex-debug.o = $(CC_FLAGS_FTRACE)
endif

obj-$(CONFIG_DEBUG_IRQFLAGS) += irqflag-debug.o
obj-$(CONFIG_DEBUG_MUTEXES) += mutex-debug.o
obj-$(CONFIG_LOCKDEP) += lockde