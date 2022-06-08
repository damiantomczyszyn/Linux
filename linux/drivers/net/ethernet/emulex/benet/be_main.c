\
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/media/i2c/tvaudio.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tvaudio.o: $(deps_drivers/media/i2c/tvaudio.o)

$(deps_drivers/media/i2c/tvaudio.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       cmd_drivers/media/i2c/tvaudio.o := gcc -Wp,-MMD,drivers/media/i2c/.tvaudio.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tvaudio"' -DKBUILD_MODNAME='"tvaudio"' -D__KBUILD_MODNAME=kmod_tvaudio -c -o drivers/media/i2c/tvaudio.o drivers/media/i2c/tvaudio.c 

source_drivers/media/i2c/tvaudio.o := drivers/media/i2c/tvaudio.c

deps_drivers/media/i2c/tvaudio.o := \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/ioport.h \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/ratelimit.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
    $(wildcard include/config/CGROUP_WRITEBACK) \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/media/i2c/tvaudio.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tvaudio.o: $(deps_drivers/media/i2c/tvaudio.o)

$(deps_drivers/media/i2c/tvaudio.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ELF                       �      4     (            GNU  �       �          ��������    ��������� � t2�� @ uǀx     1���Í�&    �v �������Í�&    ǀx      1���������W�JV�2S#�d  #�h  ���   �2�=    �J[1�^_Ë�d  ��h  ����T  �����WVPh    Sh   ��������ȍ�&    �t& �������L  ��u�R89��  t���  ���   �������1�������UWVS�Ã�8���   d�    �D$41���P  �.   ��������.   ��&    ������z$u=V�ƃ���L  ��u��P  t,���  �B4���  �J(p�B8��1�^Í�&    1�Ít& ����   �$������$����&    ��    ��������   �@\���  ��t'���  �   �  ������P�   j �����X1�Í�&    ��&    ��������    ����  �@@���  ��t'���  �   �  ������P�   j �����X1���������WVS��L  ���   ��u��`  ��\  1�1�	�tJ��\  ��`  ǀL      �C\���  ��t'���  �   �  ������P�   j �����X[1�^_Ít& �����S�ù   ��8d�    �D$41��C�D$    �=    �D$    f�$f�D$f�D$f�D$$�D$2�D$(    �D$  �D$   �D$  �D$   f�L$�D$     �D$&  �D$,��  �C�   �����������  �C�   �T$���������  �C�   �T$���������  1��T$4d+    u��8[��������    �����U1�W�Ϲ   V��S�Ã�(d�    �D$$1��C�D$    �T$!�D$    f�D$f�D$�D$�D$�F�D$!��f���D$    f�D$"�Cf�L$�T$�D$  �$�C�   �T$��������  �D$f���=    ����  �T$$d+    u��([^_]�������v �����UW��V��S�Ã�d�    �D$1��ȈT$f��f�D$�D$$f���=    f�D$
��  �C1�$j �   �؍T$�����Y���R  1��T$d+    u��[^_]��������&    ������UWVS�p�X|�@(�~�=	� �P  vN=	� ��  =
	� �  �۸   �   �   E؉�S�����~~ Xuu�t& �1�[^_]Í�&    f�=	� ti=	� ��  �ۍ��   �   �   H؉����À����S������~~ ]t�S�0   �   ������Y뙍�&    f�S�3   ����&    ����   ���   �-    �@|��	��  �ٺ� ���)ى���
����)������2  S1ɺ   ���<���X�݃�U�   �   ���$������    X��   �~~ �����S�   �I�����[�� �����  0 ������
���   ��)˹   ��S������~~ X�����S�1   ������t& ��[�� �����  0 ������
���   ��)˹   ��S�{����~~ X�`���S�2   �����t& ���>S�   1ɉ��   �H���Z����f�U�@   �   ���.���X��������������  �  �t& ������    S�X���]  �������1�[Í�&    ��&    �����V��S�X\����������  ��t���  ���������������   �����1�[^������U��WVS�� ��u�@�   �+  ����������������t  �E��  ��  �D$������Å��f  �   ��������C   ���  ��ǃ�     �   ǃ�     ǃ�     ǃ�     ������ƅ���   ǃ\   �  ���  �   �F  ǃ`      ǃd  ��� ǃh      ǃ�     ǃ�  ����ǃx      ������   �   ��ǃ�      ǃ�  B   �����D$��0  ���u&�    ����  ��&    f�������� ��[^_]Ð�   �   ���_�����0  ��4  �|$�=    ���}  �|$���t��D$	�t��t$�������N@���$���D$�D$�ԉ׺���̉��|$�����T$�������)Ǎ�N�  �t$i�'  ȉ�8  �D$�������<  �<$F�D$
���D$
������=  �����D$�t$��>  	�	ȃ<$C��A  ��@  ��!΃|$�   ��  ��B  1Ƀ<$C��C  N��T$�L$
��D  ����F  !у<$G��G  ������E  ��!�����H  ��!��|$��I  !Ѓ|$��!Ј�J  ��   ��v �<$F~ǃP     ��t& �1��<$C����P  j ���   1ɺ   ���������G   ^��   j �	� �0  ��h �  j h�  j h��  j j ������	� �0  �� ���  ��j h �  j h�  j h��  j j ������		� �0  ���� j j j jj jj j ��������   �{l���  �� ����   ����������� [^_]Í�&    f������   1��n�����&    j �	� �0  ��h �  j h�  j h��  j j ������	� �0  ���� j h �  j h�  j h��  j j ������
	� �0  ���� j j j jj jj j ������� �����t& �ƃB  ��C  �������    �    ���������  ������������  �   ��������������=   �u  �W  �}  ��&    ��&    ������Ѻ   ������&    ��&    �������Ѻ   ������&    ��&    ������Q�Ѻ   �N���ZÍ�&    �t& ������Q�Ѻ   �.���ZÍ�&    �t& ������WVS�ÉЉʋs\��t  ��w��v?�`  �=    ��p  ���*  Q�   �   ���������E   Zu:[^_Í�    �ɍ|�<�`  ���t��`  ��#�p  	�렍�&    �v ��x  �ع@   �   �y���X[^_Ít& �����UW��V��S�Éȃ����   �<$�׃��|$���������|$�L$9��  �  ���  �   1����  �   �����Dǃ���u���	   ǃ�     ET$1ɉ�X  ��������T$�   ��������T$�   ����������������   �   ��P  �0   E؉���������$��%��  ����   	�R�   �n����F\Z���  ��t'���  �   �  ������P�   j �����X��1�[^_]Í�&    9��  ������፴&    �v �����UWV��S�Í��  d�-    ��d�    �D$1�����$    �D$    �D$    �l$�D$    ������������u��xN��������������������������    d�    ��u4���  ���T$d+    u1��[^_]Ít& ��   �E�����믐���������t�1��������������&    �����S�Ë��  �@�@��������  ���   �P|���  ���   �P|���  �~������  [�@�@�������&    �t& ������VS��L  ���   ��ug�    ��ǀL     ���T  ��P  ���  ���tF����w4�F\���  ��t'���  �   �  ������P�   j �����X1�[^Ít& ���   �������r�� �r�� ������������1�[^�f�f�f�f�f�f�f��������=     WV��S�Ëx\tW�   �   ������   �	   ���������
   ���������   ���������   ���������C   u$[^_É�   �������	   �������뭍t& ��غA   [^_�������&    ��&    ������WVS�Éȃ��{\���  ��w;�4�4  ��   ��u7���� ��5    �҅��o  ����[^_������v �    �   ��tɃ�
~��I   t�p��	��f�����	�뮍�&    �����U�   WVS�h\�Ë��  ���������H������   ���:�����	   �������&�����
   ������������C   u���غ   [��^_��]��������A   ������������э�&    �v �����S��  ��1ɸ�  ���&    �v �B��t�
��9�u�[Í�    ��  [Ð�����W��V�Ή���S���  �ú�   ���������   ���������   �؁��  ���������   ���������1�[�V   ^_�������&    f������U��  WV��S���x\���  ��w��H  ��P  �    ty��X  ��	�:  ���  ��to���  ���   F݃�u]���B  ���U  ��  ����  ��GZ �GZ ������    ���<  1��}�t& ����   ��[^_]�1ۃ�t���   ���:  ����   ����  ���   ��  ����  �    1҅���  ����   ����   ����   ��X  ��  ���  �0   EС    ����  ����[^_]������������U����v ����   ~����  ��[^_]Ít& �t���^  ��l  1҅������W������u��� 늍�    ��t�늍�&    f��� �a � �f ������    ����  1҃��L�����&    �����  1�����������  1���������u� �a �  d ��������    ���  1��������    ��us�� �a � �f ������    ����  1��������    ����  �   �|�����    �C����Z���� 0  �   ��������    1��D�����    ���T����`�����&    �v �    ���z  �   1ɉ�������   �>�����&    �v �0  �   ��������    ���[  1�������&    �t& �����U��WVS�Ã��@\�$���  ���D$�    ������  �$�|� ����   ����   �   ��X  ��  ǀ�     ��������t& ���`  �   ���������s�   �   �ؾ   ������@   �   �������1ɺ   ��������v ���x  �   ���������s�D� ��   �4�`  �؋N<������N4�V0��������V@���&����   �   ��������ND�   ��������$��<   u
��[^_]�f����ع Z  �   [^_]�������&    ������S�P\�Ë�P  ��~��u��T   t,��[������t& ��y[Í�&    �v ��[������&    ����  t���  �����t�    �    �������몍�    �!   �    �������둍�&    ��    �����UWV��S���x\��P  ����   ��u�   ������ŉ���1���   ��@��X  	tK�   ��D؋��  ���  ��t��T   ��  Ǉ�     �   �=     �L  9�������[^_]Ít& ��y��1�[^_]�f���l  ��X  �˃���   �P�1���wV�#   ��������=    �#  �Ë��  �   ��uT�t& 9���   �    ���  ���  ��l  �   9��e����    ���/  ��l  ���   [^_]Ð������t6�   ��t����   E�뎍�    ��
�   E��x�����&    ��   �f�����    �   �������=�  ��  ��O=    ��  ���  1�=    �= ����   �   M������v �����  ����f���l  1������x  ��&    ��&    �����U��WVS���X\�    ���.	  ǃd  ��� ǃh      ������E�$�v �=    �	  �������������    ����	  ���  ���������E  ��X  	�(  ǃ�     �ؾ@   �������L  ��u��\  % �  ������� ���  ���   ��ǃl      ������ǡ    ����  �=    ��  ���   1�1��"��&    �v ���@�լ  ��t�ՠ  9�u�    ����  �@��T  �=     �Š  ǃ�     �P�@��|  ���  �]  ��L  ���O  ��\    � �?  ��	�6  �    ����  �	   �    ��ǃT  	   ������t& ����  ǃX     ǃl     �   �   �������� 0  �   ���������<   ��  ��E   ��  ���z�����ǃ�      ��������  ������   �C�t& ���  ���������uD����}\�������uO�=     t���  ����  �������u���  ��������ƅ�t��    ��������d	  ��&    f��������먍�&    1�� �t& ����Ŭ  ����   �Š  9�u���Ŵ  �Ű  ��d  ��h  ��F��	��  ���D  ��	�����������������  ǃX     ������&    f��� �/  ��@�q���ǃX     ��   ǃ�     ������r�� �r�� ��������;�����&    ��    ���	  ǃ�      ��������m����t& ����� 1��%����t& ��x  �@   �������� ����� Z  �   ��������������&    �v �   �    ��������%�t& ��~   �������=�  ~e�=    ��  �d   ���������t��7����v �   ���  ��X  ǃl     �R�����&    ǃX     �<�����&    f����I�����&    f��    ���I	  ��1�[^_]�1���
����둸�  ��t& ���;1tF�P����u��	  �v �D$��  ��  1����    �J����  ���9�u�D$��  �	  ��&    ��&    �����UWV��S���=    �X\����
  ǃd  ��� �~ǃh      ������    �<$�t& ����u
  �������������    ���U
  ���:
  ���  ���������  ��X  	�  �n\��ǅT     ���  ���������E   �r  �F\��L  ���!  ��\  ��`  �р� 	���  �р��	��  �с�  @ 	��{  �p  ����  ���0   ����   	����������������   �   �������� 0  �   ���������<   ��  �    �"   ���������L  �@   ��u��J   �    t���    Dȉ�T  �    ���������T  ��ub�    ����	  �
   �t& �d   �����������  �~   �������=�  �5  �=    ��	  ��uË�T  ���%  �-    ����  1�1�����@�<լ  ����   �ՠ  9�u��@�Ŵ  �Ű  ��d  ��h  ��	�b  ���������p  �   ����������  ��T   ��   �    ����f��    ����  ǃ�      ��������    �o�����&    ���� 1��u����t& �    ���h  �   ������&    �� Z  �   ��������<�����&    �v ��x  �@   ��������w�����&    f����  ���~\���������   �=     t.���  ��%�t& ��=     t���  ���  ���������u(���  ��������~\�������tŉ������뼍t& �    ������    � |  �   �����������f��    ���H  �   ������&    f��    ���(  �   �������&    f�����T  ��������    ���������
  �t& �������������t& �    ���  �@  �����    ����
  �`  �|����    ����
  1���[^_]Ã������[^_]ø�  1���  ���&    �P���
  �8��9�u��
  ��&    �����UW��VS���p\�    ���0  ǆd  ��� ǆh      ������G�D$f��=    �  �������������    ����  ���  ����������  ��L  ��u[��X  	tRǆ�     �������1҉���������  ���   ��ǆl      ������$��t<�    ��~��  �t& ��-    ����  ǆ�      ��������<����t& �=     ��   ��\  �  � tv�    ���l  �  ��|  �  � �  ��<   ��  ��   �   ���  ����������  ǆl     ǆd  � 2 ǆh      ��  �v �D$����1�1��D��&    �v �   ���������  ��=�  O�9�}�\$�ŋ    ���.  ����t.��   ����������d   ���������t��    �����t& �D$����   �D$   ��   �=     ��  �D$����1��F�t& ��   ���������  ��=�  O�;$~�$�\$�    ���  ��;\$��  �T� ����������d   ���������t��    ������&    ����  �=     ��  ��   �D$������|  �|$~E�|$�  �D$������  �|$ u?��\  ������    ���Q  ��1�[^_]ËD$��uǆd   �  ǆh      ���  ���   �������|  ���  ���������ǆ�      �������������    ���  ���  ������   �I��&    ��  ����������>��������o\�������uE�=     t���  ����  �F�����u���  ��������Å���������o\�������t��������벍t& ��D$��   ��|  �������ǆd    �D$ǆh      ����  �|$�������<   �������   �   ���  ����������  ǆl     ������_\��|  ;��  ��   ���K  ��X  �P���ve����   �    ���������  ��&    f�ǆd     �   ��ǆh      ǆ�    [ ��������  ǆl     �%����-    ���O����  �t& ����m�����  �v ���,������  9�|  �����    �������  ��&    �v �D$�   ���   ���  ����������  ǆd  � 2 ǆh      ������\  �  � �Q����L$��   ��|  ����������+������&������1���ǆd    ǆh      ��   �   ���  ����������  ������   �   ���  ����������  ǆd    � ǆh      �������\  �  � �M����D$��   ��|  ��u4ǆd    ǆh      �����=     u�D$   ��   �T����D$�����������\  �  � t������R�T$h    ���   ���D$Qh   ������D$�T$���   �~����4  ��0  Vh    W������CpPh8   j4�t$ V��������   ����������X  ��	vL�   ��P  ����   ���  �+   �$   ��D΀��3   �,   D�QRPhU   W����������    u�PhG   W����������  h   W��������  ���  h@   W�������p  h�   W�������(�D$4d+    uc��81�[^_]Ã�uPhG   W����������  �+   �$   �,   �D��3   D���T  �����UVPhj   W��������T��������h�   P�D$h   ������D$���  ��h�   S�����X���Z�  �Ch�   Ph   ��������a  P���D$WVh�   Sh   ������D$���  WVU��hl   �t$������
   ���������uh�   �t$������������X���Z�  �L  WVU��h�   �t$������
   ���������u@h�   �t$������������X���Z�
  �C�t$$QRh�   Ph   ���������  ��  ���   �I|����   ���  �  �  D�Q�   RP�Gh�   Ph   �����1ɺ   ��S�  ���f  ���   �p|�Gh  h  h�   Ph   �����1ɺ   ��S�  ���a  �  �  �h  Ph   ��������  �+   ��<   �  D׋�P  �4��   WRP�E  P�D$(��P�D$$�ă�@P�t$�t$<�t$(h(  �t$D�������P  ��0���Q	  ��    ���Q	  hk  j�UP������ǃ�= �����   ��������  �E\���   �Q	  ���  �   �  ������P�   j �����[�Q	  �+   �  �  ��<   D�D��%  ������4  �t$hQ  �t$$h   �������0  �D$ ����t��4  �|$	ǉD$��	  �=     �L	  h�   ������t$h   ��������Q	  �Eh3  �����Ph   ��������Q	  ���  hs  �t$�����ZY�����QR�4�@  �Chd  Ph   �������p  ���  �Fh�  Ph   ���������  R�T$Q�L$P�Ch�  Ph�  ������L$�T$���  �FRh�  Ph�  ��������  �Fjh(  Ph�  �����1҃��  R�F�T$h  Ph�  ������T$���m  �F�$Sh(  Ph�  ������T$�����  ����  ���D  ����  �`  �FRh�  Ph�  ��������  �FRh�  Ph�  ������    ���  ��h  Vh�  ���������  �FRh�  Ph�  ������    1҃��"  �FRh�  Ph�  ������    1҃��"  �Fjh(  Ph�  �����1҃��`  �FSh(  Ph�  �����1҃�������Sh�  Vh�  ���������  �Fj h(  Ph�  �����1҃��D  �Fjh(  Ph�  �����1҃���  �Fh8  Ph�  ��������
  �FRh�  Ph�  ������    �   ���"  ���`  ��  �C�L$RhN  Ph�  ������L$���  P�V�D$h\  Rh�  ������D$���#  �FURhD  Ph�  ��������r  ��SQh�  Vh�  ��������  ���$SUQRhd  Vh�  ��������  �D$����  ������R���D$R�Vhx  Rh�  ������D$���F  Vh:  h�  �t$h�  ��������$  h  �t$h�  ���������  �D$��  �D$VQh�  �t$h�  ������D$���  h�  �t$h�  ���������  �Eh�  Ph�  ��������  ��hQ  Uh�  ��������I   h�  �t$h�  ���������  h  �t$h�  ��������9  h�  �t$h�  ������    ���%  ��  VRh�  �t$h�  ��������s  h  �t$h�  ���������"  h   �t$h�  ��������"  ��  QRh�  �t$h�  �������T  ����"  h�  �t$h�  ��������?!  h�  �t$h�  ������    ���7!  hx  �t$h�  ��������!  �@h_  Ph�  ���������   ��hQ  Vh�  �����1����r%  h�  �t$h�  ������`  ����!  hH  �t$h�  ������    ���!  h�  �t$h�  ������@  ����!  h�  �t$h�  ������   ����!  h  �t$h�  ������   ����!  h�  �t$h�  ������   ����!  h  �t$h�  ��������#  �Ȼ�  �����  ����Ȼ�� R���Ph0  �t$h�  ��������Y)  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  h9  �t$h�  ��������	&  �Gh!  Ph�  ���������%  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  ���  ��  �ș����  ����Ƚ�� R���Ph�  �t$h�  �������X  ���*  ���  ��  �ș����  ����Ƚ�� R���Ph  �t$h�  �������X  ���$+  �t�Ph�  �t$h�  ��������@(  �4�  Ph{  �t$h�  ��������'  �GhQ  Ph�  ���������(  hh  ��   �t$h�  ��������D$   �D$   ��'  h  �t$h�  ��������&  h�  �t$h�  ��������#&  hP  �t$h�  ������    ���&  ������    �    ������    �����                                  �     �  �  �  �         %                          msp3400                                                                 �     �     �                                                                 4  ;  F                             `  @  �                          0                          �  �          @  �                                                   P          `                                                                                                                              0      `  d  h  l  p  w  3   ~                   ��������      @  @         @     @      @  �0     �  �   �0      �        0  dolby   amsound standard    stereo_threshold    debug   once    opmode              0                                  �  �  �  �  �      K      $   #   '   (   K      $   #   '   (   �cL �cL �           0  ������������   ;   ~   ������������   ;   ~   �cL �cL �           0  ��������      N   k   ��������      N   k   r�� r�� �   �       0           0   B   H            0   B   H   �cL �cL �   �  0    0     	         !   %            0   B   H   GZ GZ �   �       0  ������������
   2   V            0   B   H   �cL �cL �   @       0        ��������(   ^            0   B   H   UUS UUS �   @       0  ������������
   2   V   ������������   O   ~   GZ GZ �   @      |              �  ���                �  ���         �> 9�A �   �         �cL ��O            GZ  �V #  � 2        GZ 9�] A  � 2        GZ GZ _  � 2        GZ ��O x  � 2        �cL  @Q :        	   GZ  @Q �    �     
   UUS ��Z �            GZ  @Q �  � 2        GZ  @Q T  � 2        GZ  @Q t  � 2          �>  �> �         !    �>  �> �         0    �>  �>           @   r�� r��   ���     P   GZ GZ 1  ���     Q    �a   d ?  ���     `     d   d U  ���     ����                    Setting I2S speed to %d
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   �      @  �     P  �  �  �  0  �  �           @  `  �  0  �  p  �      �  @  �  p  �     `  �  `  �  �   �%    detected standard: %s(0x%08Lx)
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor �          ��     �          ��     �          ��     �          ��     �          ��               ��               $� �             �                                  �                  +                                                              �           `                       �     ����                     @Q �   �V �  9�]    �a     d 9   �f S  ��O m   @Q �   �> �  �cL �  UUS �  GZ �   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                           ��       `     '       2     A   `   e     N   �   0     Z      9     i   2   a    }       (    
 �   @  c     �   �  B     �      P     �   �        �   P  |     �           �           �            �   �  3     �   �  �     �   �  p       �  �       V  k     &  0  \    1  �  �     A  �  "     M  a       ^  �  @     i     �    s          
 {           �  �        �  0      
 �  y  �    �  �       
 �          
 �  `  l    
 �  .  *     �  @       
 �  0  V    �  �  �     �  X                        �            
     0  �   \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o  �  P    
 }      &     �  &   B     �  h   Q     �  �   ?     �  �   '     �    *     
  I  Y        �       9           G  �      
 Y  �       t          �  �      
 �  �       �  (        �  �  	    
 �  �       �  <          �      
 4          M  P        [  �      
 m         �  d        �        
 �  &       �  x        �        
 �  :       �  F         X  9     '           ��:  �  �     I  @  �     ]  4      
 g         
 q  s  .     �  �  �     �  �     
 �     5    �  H      
 �  �  -    �  `  @   
 �  �  $       �  �     %  �  3    :  �	  �    O           l  �   0     �  �        �  �  O    �             �          �             �             �                                         �  B    /             L      
     [  �        m  `       {             �           �             �          �             �  �  g     �             �                            `      ,             7           C             S             b  p  ?     x  �  �     �             �             �             �             �  �  �     �             �             �             	             &	  �        3	             >	  �  �     H	             _	             l	             	             �	             �	  �  �     �	             �	             �	  �%  
    �	  `  X    �	  @        
             
  p  T     )
             1
             >
             M
             _
             v
             �
             �
  `   0    
 �
             �
          �
          �
             �
             �
               �   �                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     �  
   u       a   �  �   u  �   �  �   	  �     �   �  �   �  �   �    �    z  )    0  �  5    A  �  �  �  �  �  �  �    �  
  u      E  �  Q  �  �  �  �  �  �  z  �  u  2    B    O    a    p  �  y    �  �  �    �  �  �    �  z  �  �  �  �  �  z  ]  �  f    u  u      �  z  �  �  �  �  �  z  �  u  �    �  �        z  $  �  1  �    u  �    �    �  �  �  u  �    �  �  �  �  �  �  �  �  �  �  �  �    �         �  '  �  F  v  U    \  t  �  �  �    �    	  �  ;	  u  �	  u  �	    �
    )  |  C    a  �  k    �  �  �    �  �  �  �      .  �  8    [  �  e      �  �  u  �    �  �  �  �  �    �    �      �  !  �  A  �  a  �  �  �  �  u  �    �    �    1  �  �  �  �  �  �  �  �  �  b  �  �  �  �  �  �  z  �  �  �  �  �  �  �  w  �  �    �    �    �    �  -  z  I  �  S  �  ^  �  e  �  q  �  �  �  �  �  �  �  �  u  �    I  �  `  �  q  �  x  �  �  �  �  �  �    �    �    �    �            -    A  �  `    g    y  u  �    �    �  �  q  �  w    �  	  �    �  �  �  �  �  �  �  �    �    �  !  �  '    D    Q  u  �    �  �  �  u  �    �           u  *    e  u  m    �    �    �  �    u  
    $    4    R  �  W  u  _    �  �  �  u  �    �    �    �  u    u  
      �  =    B  u  J    a  �  �  u  �    �    �  �  �    �  �  �  �    �    �  #    /  �  D    N  �  [  �  w    �    �    �  �  4  �  M  �  a  �  �  �  �  u  �    9  �  ?  u  j  u  r    �  u  �      �    u  #    n    �  �  �  u  �    �  �  �  u  �    �  �  �  u  �    �  �    �  W  �  ^  u  l  u  �    �    �  u  �  �  �      u  $    ?  �  p    �    �  �  �  �  �  �  �  �    �  %  u  3    C  �  a    p    ~    �      �    �  1  u  9    J  �  v  �  �    �  �  �  �  �  u  �    �  �  A   u  I     e     �     �     �   	  �     �     �     �   �  �   u  �     !  �  !  u  !    )!  �  .!  u  7!    ?!    K!  �  }!  �  �!  �  "    !"    4"  ~  @"  �  \"  �  {"  �  �"  u  �"    �"  �  �"  �  �"  u  �"    �"  u  	#    #    &#    -#    I#  �  [#    u#  u  �#  u  �#    �#  �  �#  u  �#  u  �#    �#    $  �  -$  �  ;$  �  R$  �  h$  �  �$  �  �$  �  �$  u  �$    �$  u  �$    �$  u  �$    %  u  %    +%  �  :%  u  B%    Q%  u  Y%    h%  u  p%    �%    �%  	  �%    �%    �%  �  �%  u  �%    �%  �  &  u  	&    &  �  &  u  #&    /&  �  [&  �  d&  �  �&  �  �&  u  �&    �&  u  �&    �&  �  �&  �  �&  u  �&    �&    '    ''  �  x'  �  �'  u  �'    �'    �'  �  �'  �  �'  u  �'    �'  �  (  �  8(  u  @(    Z(  �  f(  �  o(  u  �(  �  �(    �(  u  �(    !)  �  4)  �  L)  �  Q)  u  �)  �  �)  �  �)  �  �)  �  �)  �  �)  �  �)    C*    U*  �  �*    �*  u  �*    �*  �  +  u  +    !+    E+  u  R+    l+    y+  �  �+    �+    	,  �  ,    ,,  �  h,    �,  �  �,                 �  .     F   	  L   �  U     a   y  p   �  �     �     �     �     �     �     �   �  �     �     �   �  �     �   �     	    �        �  %  z  <    B  �  P    U    Z    f    t  �  |    �  �  �  �  �    �    �  �  �    �    �  �  �    �    �    �  �  �    �    �    �  �        	    �  &  �  3  	  <  �  C  �  M    R    ]  	  f  �  p  �  }  	  �  �  �  �  �    �    �    �  �  �    �    �    �    �    �  	  �    �  �          %    *    /  	  5    :  �  I    Q    V    [    b    h    m  �  u    z    �    �    �  	  �  �  �    �    �    �       �    �  +    Q  �  W    \    a    f    x    �    �    �  �  �    �  u  �    �  	  �    �  �  �    �          �          #  �  3    ;  	  A    F  �  T    \    b    g  �  o    �  	  �    �  �  �    �    �    �  �  �    �    �    �  �  �    �  	  �    �  �  �              �  #    4    =    B    K    Q    V  �  ^    g    m    r  �  x  u  �    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �  �  �  u  �    �    �    �  �                �  )  	  /    4  �  <    F    L    Q  �  [    e    k    p  �  z    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �    �    �  �  �    �          �        	  "    '  �  /    9    ?    D  �  L    [  	  a    f  �  x    �    �    �  �  �    �    �  	  �    �  �  �    �    �    �  �  �    �    �  	  	    	  �  	    	    !	    &	  �  .	    6	    <	    A	  �  I	    Q	    W	    \	  �  d	    i	    r	    w	  �  	    �	    �	    �	  �  �	    �	    �	    �	  �  �	  u  �	    �	    �	  	  �	    �	  �  �	    �	    �	    �	  �  �	    �	  	  
    

  �  
    
    
  	  '
    ,
  �  :
    ?
    H
    M
  �  U
    Z
    c
    h
  �  m
  u  u
    z
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
  	  �
    �
  �     u                �  (    -    6    ;  �  H    M    V    [  �  h    m    v    {  �  �    �    �    �  �  �    �  	  �    �  �  �    �  	        �          #    (  �  0    8    >    C  �  K    n  	  w    |  �  �    �  	  �    �  �  �    �  	  �       �          !    &  �  .    5    ;    D    I  �  Q    Y    _    d  �  l    q    v        �  �  �    �    �    �  �  �    �    �    �  �  �    �    �    �  �  �  u  �       �          x     �          �      �     {     �        $     (     ,     0     4     8     <     @     D     �     �     �     �     �     �     �                   $    (    ,    D    H    d    h    t    x    �    �         8    @    D    H    L    P    T    X    \    H    L    P    T    X    �  	  �    �    �        $    <    T    l    �    �    �    �  	  �  	  �        ,    D    \    t    �                                                   $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �     �              x     �     �          x     �  $   �  (     ,   x  0   �  8   �  <     @   x  D   �  L   ~  P     T   x  X   �  `   u  d     h   x  l   �  t   �  x     |   x  �   �  �     $     (     <     |     �     �     �     �     �     �     �     �     �                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .rodata.str1.1 .rel__mcount_loc .rodata.str1.4 .modinfo .rel__param .rel.data .bss .comment .note.GNU-stack                                                         4   (                  2             `   �,                 .   	   @       �k  �              <             *-  �                 8   	   @       �y  �              O              ;                    K   	   @       L�                  ^             4;  
                  Z   	   @       l�                 m             @;  �                  i   	   @       |�  0     
         u      2       �B  b                �             RK  �                  �   	   @       ��  (              �      2       �K  �                �             {P  �                 �             S  �                  �   	   @       ԉ  �               �             �S                     �   	   @       ��  �               �             �T                    �      0       �T  &                 �              �T                                   �T  �     t         	              �`  K                               <�  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                       �      4     (            GNU  �       �          ��������    ��������� � t2�� @ uǀx     1���Í�&    �v �������Í�&    ǀx      1���������W�JV�2S#�d  #�h  ���   �2�=    �J[1�^_Ë�d  ��h  ����T  �����WVPh    Sh   ��������ȍ�&    �t& �������L  ��u�R89��  t���  ���   �������1�������UWVS�Ã�8���   d�    �D$41���P  �.   ��������.   ��&    ������z$u=V�ƃ���L  ��u��P  t,���  �B4���  �J(p�B8��1�^Í�&    1�Ít& ����   �$������$����&    ��    ��������   �@\���  ��t'���  �   �  ������P�   j �����X1�Í�&    ��&    ��������    ����  �@@���  ��t'���  �   �  ������P�   j �����X1���������WVS��L  ���   ��u��`  ��\  1�1�	�tJ��\  ��`  ǀL      �C\���  ��t'���  �   �  ������P�   j �����X[1�^_Ít& �����S�ù   ��8d�    �D$41��C�D$    �=    �D$    f�$f�D$f�D$f�D$$�D$2�D$(    �D$  �D$   �D$  �D$   f�L$�D$     �D$&  �D$,��  �C�   �����������  �C�   �T$���������  �C�   �T$���������  1��T$4d+    u��8[��������    �����U1�W�Ϲ   V��S�Ã�(d�    �D$$1��C�D$    �T$!�D$    f�D$f�D$�D$�D$�F�D$!��f���D$    f�D$"�Cf�L$�T$�D$  �$�C�   �T$��������  �D$f���=    ����  �T$$d+    u��([^_]�������v �����UW��V��S�Ã�d�    �D$1��ȈT$f��f�D$�D$$f���=    f�D$
��  �C1�$j �   �؍T$�����Y���R  1��T$d+    u��[^_]��������&    ������UWVS�p�X|�@(�~�=	� �P  vN=	� ��  =
	� �  �۸   �   �   E؉�S�����~~ Xuu�t& �1�[^_]Í�&    f�=	� ti=	� ��  �ۍ��   �   �   H؉����À����S������~~ ]t�S�0   �   ������Y뙍�&    f�S�3   ����&    ����   ���   �-    �@|��	��  �ٺ� ���)ى���
����)������2  S1ɺ   ���<���X�݃�U�   �   ���$������    X��   �~~ �����S�   �I�����[�� �����  0 ������
���   ��)˹   ��S������~~ X�����S�1   ������t& ��[�� �����  0 ������
���   ��)˹   ��S�{����~~ X�`���S�2   �����t& ���>S�   1ɉ��   �H���Z����f�U�@   �   ���.���X��������������  �  �t& ������    S�X���]  �������1�[Í�&    ��&    �����V��S�X\����������  ��t���  ���������������   �����1�[^������U��WVS�� ��u�@�   �+  ����������������t  �E��  ��  �D$������Å��f  �   ��������C   ���  ��ǃ�     �   ǃ�     ǃ�     ǃ�     ������ƅ���   ǃ\   �  ���  �   �F  ǃ`      ǃd  ��� ǃh      ǃ�     ǃ�  ����ǃx      ������   �   ��ǃ�      ǃ�  B   �����D$��0  ���u&�    ����  ��&    f�������� ��[^_]Ð�   �   ���_�����0  ��4  �|$�=    ���}  �|$���t��D$	�t��t$�������N@���$���D$�D$�ԉ׺���̉��|$�����T$�������)Ǎ�N�  �t$i�'  ȉ�8  �D$�������<  �<$F�D$
���D$
������=  �����D$�t$��>  	�	ȃ<$C��A  ��@  ��!΃|$�   ��  ��B  1Ƀ<$C��C  N��T$�L$
��D  ����F  !у<$G��G  ������E  ��!�����H  ��!��|$��I  !Ѓ|$��!Ј�J  ��   ��v �<$F~ǃP     ��t& �1��<$C����P  j ���   1ɺ   ���������G   ^��   j �	� �0  ��h �  j h�  j h��  j j ������	� �0  �� ���  ��j h �  j h�  j h��  j j ������		� �0  ���� j j j jj jj j ��������   �{l���  �� ����   ����������� [^_]Í�&    f������   1��n�����&    j �	� �0  ��h �  j h�  j h��  j j ������	� �0  ���� j h �  j h�  j h��  j j ������
	� �0  ���� j j j jj jj j ������� �����t& �ƃB  ��C  �������    �    ���������  ������������  �   ��������������=   �u  �W  �}  ��&    ��&    ������Ѻ   ������&    ��&    �������Ѻ   ������&    ��&    ������Q�Ѻ   �N���ZÍ�&    �t& ������Q�Ѻ   �.���ZÍ�&    �t& ������WVS�ÉЉʋs\��t  ��w��v?�`  �=    ��p  ���*  Q�   �   ���������E   Zu:[^_Í�    �ɍ|�<�`  ���t��`  ��#�p  	�렍�&    �v ��x  �ع@   �   �y���X[^_Ít& �����UW��V��S�Éȃ����   �<$�׃��|$���������|$�L$9��  �  ���  �   1����  �   �����Dǃ���u���	   ǃ�     ET$1ɉ�X  ��������T$�   ��������T$�   ����������������   �   ��P  �0   E؉���������$��%��  ����   	�R�   �n����F\Z���  ��t'���  �   �  ������P�   j �����X��1�[^_]Í�&    9��  ������፴&    �v �����UWV��S�Í��  d�-    ��d�    �D$1�����$    �D$    �D$    �l$�D$    ������������u��xN��������������������������    d�    ��u4���  ���T$d+    u1��[^_]Ít& ��   �E�����믐���������t�1��������������&    �����S�Ë��  �@�@��������  ���   �P|���  ���   �P|���  �~������  [�@�@�������&    �t& ������VS��L  ���   ��ug�    ��ǀL     ���T  ��P  ���  ���tF����w4�F\���  ��t'���  �   �  ������P�   j �����X1�[^Ít& ���   �������r�� �r�� ������������1�[^�f�f�f�f�f�f�f��������=     WV��S�Ëx\tW�   �   ������   �	   ���������
   ���������   ���������   ���������C   u$[^_É�   �������	   �������뭍t& ��غA   [^_�������&    ��&    ������WVS�Éȃ��{\���  ��w;�4�4  ��   ��u7���� ��5    �҅��o  ����[^_������v �    �   ��tɃ�
~��I   t�p��	��f�����	�뮍�&    �����U�   WVS�h\�Ë��  ���������H������   ���:�����	   �������&�����
   ������������C   u���غ   [��^_��]��������A   ������������э�&    �v �����S��  ��1ɸ�  ���&    �v �B��t�
��9�u�[Í�    ��  [Ð�����W��V�Ή���S���  �ú�   ���������   ���������   �؁��  ���������   ���������1�[�V   ^_�������&    f������U��  WV��S���x\���  ��w��H  ��P  �    ty��X  ��	�:  ���  ��to���  ���   F݃�u]���B  ���U  ��  ����  ��GZ �GZ ������    ���<  1��}�t& ����   ��[^_]�1ۃ�t���   ���:  ����   ����  ���   ��  ����  �    1҅���  ����   ����   ����   ��X  ��  ���  �0   EС    ����  ����[^_]������������U����v ����   ~����  ��[^_]Ít& �t���^  ��l  1҅������W������u��� 늍�    ��t�늍�&    f��� �a � �f ������    ����  1҃��L�����&    �����  1�����������  1���������u� �a �  d ��������    ���  1��������    ��us�� �a � �f ������    ����  1��������    ����  �   �|�����    �C����Z���� 0  �   ��������    1��D�����    ���T����`�����&    �v �    ���z  �   1ɉ�������   �>�����&    �v �0  �   ��������    ���[  1�������&    �t& �����U��WVS�Ã��@\�$���  ���D$�    ������  �$�|� ����   ����   �   ��X  ��  ǀ�     ��������t& ���`  �   ���������s�   �   �ؾ   ������@   �   �������1ɺ   ��������v ���x  �   ���������s�D� ��   �4�`  �؋N<������N4�V0��������V@���&����   �   ��������ND�   ��������$��<   u
��[^_]�f����ع Z  �   [^_]�������&    ������S�P\�Ë�P  ��~��u��T   t,��[������t& ��y[Í�&    �v ��[������&    ����  t���  �����t�    �    �������몍�    �!   �    �������둍�&    ��    �����UWV��S���x\��P  ����   ��u�   ������ŉ���1���   ��@��X  	tK�   ��D؋��  ���  ��t��T   ��  Ǉ�     �   �=     �L  9�������[^_]Ít& ��y��1�[^_]�f���l  ��X  �˃���   �P�1���wV�#   ��������=    �#  �Ë��  �   ��uT�t& 9���   �    ���  ���  ��l  �   9��e����    ���/  ��l  ���   [^_]Ð������t6�   ��t����   E�뎍�    ��
�   E��x�����&    ��   �f�����    �   �������=�  ��  ��O=    ��  ���  1�=    �= ����   �   M������v �����  ����f���l  1������x  ��&    ��&    �����U��WVS���X\�    ���.	  ǃd  ��� ǃh      ������E�$�v �=    �	  �������������    ����	  ���  ���������E  ��X  	�(  ǃ�     �ؾ@   �������L  ��u��\  % �  ������� ���  ���   ��ǃl      ������ǡ    ����  �=    ��  ���   1�1��"��&    �v ���@�լ  ��t�ՠ  9�u�    ����  �@��T  �=     �Š  ǃ�     �P�@��|  ���  �]  ��L  ���O  ��\    � �?  ��	�6  �    ����  �	   �    ��ǃT  	   ������t& ����  ǃX     ǃl     �   �   �������� 0  �   ���������<   ��  ��E   ��  ���z�����ǃ�      ��������  ������   �C�t& ���  ���������uD����}\�������uO�=     t���  ����  �������u���  ��������ƅ�t��    ��������d	  ��&    f��������먍�&    1�� �t& ����Ŭ  ����   �Š  9�u���Ŵ  �Ű  ��d  ��h  ��F��	��  ���D  ��	�����������������  ǃX     ������&    f��� �/  ��@�q���ǃX     ��   ǃ�     ������r�� �r�� ��������;�����&    ��    ���	  ǃ�      ��������m����t& ����� 1��%����t& ��x  �@   �������� ����� Z  �   ��������������&    �v �   �    ��������%�t& ��~   �������=�  ~e�=    ��  �d   ���������t��7����v �   ���  ��X  ǃl     �R�����&    ǃX     �<�����&    f����I�����&    f��    ���I	  ��1�[^_]�1���
����둸�  ��t& ���;1tF�P����u��	  �v �D$��  ��  1����    �J����  ���9�u�D$��  �	  ��&    ��&    �����UWV��S���=    �X\����
  ǃd  ��� �~ǃh      ������    �<$�t& ����u
  �������������    ���U
  ���:
  ���  ���������  ��X  	�  �n\��ǅT     ���  ���������E   �r  �F\��L  ���!  ��\  ��`  �р� 	���  �р��	��  �с�  @ 	��{  �p  ����  ���0   ����   	����������������   �   �������� 0  �   ���������<   ��  �    �"   ���������L  �@   ��u��J   �    t���    Dȉ�T  �    ���������T  ��ub�    ����	  �
   �t& �d   �����������  �~   �������=�  �5  �=    ��	  ��uË�T  ���%  �-    ����  1�1�����@�<լ  ����   �ՠ  9�u��@�Ŵ  �Ű  ��d  ��h  ��	�b  ���������p  �   ����������  ��T   ��   �    ����f��    ����  ǃ�      ��������    �o�����&    ���� 1��u����t& �    ���h  �   ������&    �� Z  �   ��������<�����&    �v ��x  �@   ��������w�����&    f����  ���~\���������   �=     t.���  ��%�t& ��=     t���  ���  ���������u(���  ��������~\�������tŉ������뼍t& �    ������    � |  �   �����������f��    ���H  �   ������&    f��    ���(  �   �������&    f�����T  ��������    ���������
  �t& �������������t& �    ���  �@  �����    ����
  �`  �|����    ����
  1���[^_]Ã������[^_]ø�  1���  ���&    �P���
  �8��9�u��
  ��&    �����UW��VS���p\�    ���0  ǆd  ��� ǆh      ������G�D$f��=    �  �������������    ����  ���  ����������  ��L  ��u[��X  	tRǆ�     �������1҉���������  ���   ��ǆl      ������$��t<�    ��~��  �t& ��-    ����  ǆ�      ��������<����t& �=     ��   ��\  �  � tv�    ���l  �  ��|  �  � �  ��<   ��  ��   �   ���  ����������  ǆl     ǆd  � 2 ǆh      ��  �v �D$����1�1��D��&    �v �   ���������  ��=�  O�9�}�\$�ŋ    ���.  ����t.��   ����������d   ���������t��    �����t& �D$����   �D$   ��   �=     ��  �D$����1��F�t& ��   ���������  ��=�  O�;$~�$�\$�    ���  ��;\$��  �T� ����������d   ���������t��    ������&    ����  �=     ��  ��   �D$������|  �|$~E�|$�  �D$������  �|$ u?��\  ������    ���Q  ��1�[^_]ËD$��uǆd   �  ǆh      ���  ���   �������|  ���  ���������ǆ�      �������������    ���  ���  ������   �I��&    ��  ����������>��������o\�������uE�=     t���  ����  �F�����u���  ��������Å���������o\�������t��������벍t& ��D$��   ��|  �������ǆd    �D$ǆh      ����  �|$�������<   �������   �   ���  ����������  ǆl     ������_\��|  ;��  ��   ���K  ��X  �P���ve����   �    ���������  ��&    f�ǆd     �   ��ǆh      ǆ�    [ ��������  ǆl     �%����-    ���O����  �t& ����m�����  �v ���,������  9�|  �����    �������  ��&    �v �D$�   ���   ���  ����������  ǆd  � 2 ǆh      ������\  �  � �Q����L$��   ��|  ����������+������&������1���ǆd    ǆh      ��   �   ���  ����������  ������   �   ���  ����������  ǆd    � ǆh      �������\  �  � �M����D$��   ��|  ��u4ǆd    ǆh      �����=     u�D$   ��   �T����D$�����������\  �  � t������R�T$h    ���   ���D$Qh   ������D$�T$���   �~����4  ��0  Vh    W������CpPh8   j4�t$ V��������   ����������X  ��	vL�   ��P  ����   ���  �+   �$   ��D΀��3   �,   D�QRPhU   W����������    u�PhG   W����������  h   W��������  ���  h@   W�������p  h�   W�������(�D$4d+    uc��81�[^_]Ã�uPhG   W����������  �+   �$   �,   �D��3   D���T  �����UVPhj   W��������T��������h�   P�D$h   ������D$���  ��h�   S�����X���Z�  �Ch�   Ph   ��������a  P���D$WVh�   Sh   ������D$���  WVU��hl   �t$������
   ���������uh�   �t$������������X���Z�  �L  WVU��h�   �t$������
   ���������u@h�   �t$������������X���Z�
  �C�t$$QRh�   Ph   ���������  ��  ���   �I|����   ���  �  �  D�Q�   RP�Gh�   Ph   �����1ɺ   ��S�  ���f  ���   �p|�Gh  h  h�   Ph   �����1ɺ   ��S�  ���a  �  �  �h  Ph   ��������  �+   ��<   �  D׋�P  �4��   WRP�E  P�D$(��P�D$$�ă�@P�t$�t$<�t$(h(  �t$D�������P  ��0���Q	  ��    ���Q	  hk  j�UP������ǃ�= �����   ��������  �E\���   �Q	  ���  �   �  ������P�   j �����[�Q	  �+   �  �  ��<   D�D��%  ������4  �t$hQ  �t$$h   �������0  �D$ ����t��4  �|$	ǉD$��	  �=     �L	  h�   ������t$h   ��������Q	  �Eh3  �����Ph   ��������Q	  ���  hs  �t$�����ZY�����QR�4�@  �Chd  Ph   �������p  ���  �Fh�  Ph   ���������  R�T$Q�L$P�Ch�  Ph�  ������L$�T$���  �FRh�  Ph�  ��������  �Fjh(  Ph�  �����1҃��  R�F�T$h  Ph�  ������T$���m  �F�$Sh(  Ph�  ������T$�����  ����  ���D  ����  �`  �FRh�  Ph�  ��������  �FRh�  Ph�  ������    ���  ��h  Vh�  ���������  �FRh�  Ph�  ������    1҃��"  �FRh�  Ph�  ������    1҃��"  �Fjh(  Ph�  �����1҃��`  �FSh(  Ph�  �����1҃�������Sh�  Vh�  ���������  �Fj h(  Ph�  �����1҃��D  �Fjh(  Ph�  �����1҃���  �Fh8  Ph�  ��������
  �FRh�  Ph�  ������    �   ���"  ���`  ��  �C�L$RhN  Ph�  ������L$���  P�V�D$h\  Rh�  ������D$���#  �FURhD  Ph�  ��������r  ��SQh�  Vh�  ��������  ���$SUQRhd  Vh�  ��������  �D$����  ������R���D$R�Vhx  Rh�  ������D$���F  Vh:  h�  �t$h�  ��������$  h  �t$h�  ���������  �D$��  �D$VQh�  �t$h�  ������D$���  h�  �t$h�  ���������  �Eh�  Ph�  ��������  ��hQ  Uh�  ��������I   h�  �t$h�  ���������  h  �t$h�  ��������9  h�  �t$h�  ������    ���%  ��  VRh�  �t$h�  ��������s  h  �t$h�  ���������"  h   �t$h�  ��������"  ��  QRh�  �t$h�  �������T  ����"  h�  �t$h�  ��������?!  h�  �t$h�  ������    ���7!  hx  �t$h�  ��������!  �@h_  Ph�  ���������   ��hQ  Vh�  �����1����r%  h�  �t$h�  ������`  ����!  hH  �t$h�  ������    ���!  h�  �t$h�  ������@  ����!  h�  �t$h�  ������   ����!  h  �t$h�  ������   ����!  h�  �t$h�  ������   ����!  h  �t$h�  ��������#  �Ȼ�  �����  ����Ȼ�� R���Ph0  �t$h�  ��������Y)  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  h9  �t$h�  ��������	&  �Gh!  Ph�  ���������%  �Ƚ�  �����  ����Ƚ�� R���Ph�  �t$h�  ��������*  ���  ��  �ș����  ����Ƚ�� R���Ph�  �t$h�  �������X  ���*  ���  ��  �ș����  ����Ƚ�� R���Ph  �t$h�  �������X  ���$+  �t�Ph�  �t$h�  ��������@(  �4�  Ph{  �t$h�  ��������'  �GhQ  Ph�  ���������(  hh  ��   �t$h�  ��������D$   �D$   ��'  h  �t$h�  ��������&  h�  �t$h�  ��������#&  hP  �t$h�  ������    ���&  ������    �    ������    �����                                  �     �  �  �  �         %                          msp3400                                                                 �     �     �                                                                 4  ;  F                             `  @  �                          0                          �  �          @  �                                                   P          `                                                                                                                              0      `  d  h  l  p  w  3   ~                   ��������      @  @         @     @      @  �0     �  �   �0      �        0  dolby   amsound standard    stereo_threshold    debug   once    opmode              0                                  �  �  �  �  �      K      $   #   '   (   K      $   #   '   (   �cL �cL �           0  ������������   ;   ~   ������������   ;   ~   �cL �cL �           0  ��������      N   k   ��������      N   k   r�� r�� �   �       0           0   B   H            0   B   H   �cL �cL �   �  0    0     	         !   %            0   B   H   GZ GZ �   �       0  ������������
   2   V            0   B   H   �cL �cL �   @       0        ��������(   ^            0   B   H   UUS UUS �   @       0  ������������
   2   V   ������������   O   ~   GZ GZ �   @      |              �  ���                �  ���         �> 9�A �   �         �cL ��O            GZ  �V #  � 2        GZ 9�] A  � 2        GZ GZ _  � 2        GZ ��O x  � 2        �cL  @Q :        	   GZ  @Q �    �     
   UUS ��Z �            GZ  @Q �  � 2        GZ  @Q T  � 2        GZ  @Q t  � 2          �>  �> �         !    �>  �> �         0    �>  �>           @   r�� r��   ���     P   GZ GZ 1  ���     Q    �a   d ?  ���     `     d   d U  ���     ����                    Setting I2S speed to %d
 7 unknown , dual  stereo mono %s: Audio:     Mode:     %s
 Mode:     %s (%s%s)
 Standard: %s (%s%s)
 Audmode:  0x%04x
 ACB:      0x%04x
 resume
 msp_reset
 chip reset failed
 msp_read(0x%x, 0x%x): 0x%x
 msp_write(0x%x, 0x%x, 0x%x)
 no on yes off suspend
 nicam  and  radio msp3400 msp3400 not found
 &state->wq rev1=0x%04x, rev2=0x%04x
 msp34xx kernel_thread() failed
 switching to radio mode
 AM (for carrier detect) FM Radio Terrestrial FM-mono/stereo Satellite FM-mono NICAM/FM (B/G, D/K) NICAM/FM (I) NICAM/AM (L) BTSC External input manual autodetect autodetect and autoselect in1 in2 in3 in4 in1 da in2 da mute 7 unknown FM set_audmode: %s
 SAT set_audmode: %s
 NICAM set_audmode: %s
 BTSC set_audmode: %s
 extern set_audmode: %s
 FM-Radio set_audmode: %s
 mono set_audmode
 set audmode %d
 switching to AM mono
 set_mode: %d
 stereo detect register: %d
 nicam sync=%d, mode=%d
 watch: nicam %d => %d
 msp3410 daemon started
 msp3410 thread: sleep
 msp3410 thread: wakeup
 thread: restart scan
 thread: no carrier scan
 detection still in progress
 5.5/5.85  B/G NICAM FM thread: exit
 msp34xxg daemon started
 msp34xxg thread: sleep
 msp34xxg thread: wakeup
 selected radio modus
 selected M (EIA-J) modus
 selected M (A2) modus
 selected SECAM-L modus
 selected M (BTSC) modus
 msp3400 daemon started
 msp3400 thread: sleep
 msp3400 thread: wakeup
 AM sound override
 carrier1 val: %5d / %s
 carrier2 val: %5d / %s
 mono stereo lang2 lang1 lang1+lang2 5.85  PAL D/K + SECAM NICAM 6.25  PAL D/K1 FM-stereo 6.74  PAL D/K2 FM-stereo 7.02  PAL SAT FM-stereo s/b 7.20  PAL SAT FM-stereo s 7.38  PAL SAT FM-stereo b 5.742 PAL B/G FM-stereo 5.85  PAL B/G NICAM 4.5   NTSC 5.5   PAL B/G 6.0   PAL I 6.5   PAL D/K + SAT + SECAM autodetect start 4.5/4.72  M Dual FM-Stereo 5.5/5.74  B/G Dual FM-Stereo 6.5/6.25  D/K1 Dual FM-Stereo 6.5/6.74  D/K2 Dual FM-Stereo 6.5  D/K FM-Mono (HDEV3) 6.5/5.74  D/K3 Dual FM-Stereo 6.5/5.85  L NICAM AM 6.0/6.55  I NICAM FM 6.5/5.85  D/K NICAM FM 4.5  M BTSC-Stereo 4.5  M BTSC-Mono + SAP 4.5  M EIA-J Japan Stereo 10.7  FM-Stereo Radio 6.5  SAT-Mono 7.02/7.20  SAT-Stereo 7.2  SAT ADR     `   �      @  �     P  �  �  �  0  �  �           @  `  �  0  �  p  �      �  @  �  p  �     `  �  `  �  �   �%    detected standard: %s(0x%08Lx)
 %s rev1 = 0x%04x rev2 = 0x%04x
 Routing:  0x%08x (input) 0x%08x (output)
   I/O error #%d (read 0x%02x/0x%02x)
 resetting chip, sound will go off.
 I/O error #%d (write 0x%02x/0x%02x)
    mute=%s scanning=%s volume=%d
  not an msp3400 (cannot read chip version)
  MSP%d4%02d%c-%c%d found on %s: supports %s%s%s, mode is %s
 scart switch: %s => %d (ACB=0x%04x)
    set source to %d (0x%x) for output %02x
    could not detect sound standard set_audmode called with mode=%d instead of set_source (ignored)
    set_audmode final source/matrix = 0x%x
 watch: rxsubchans %02x => %02x
 status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d
  setting standard: %s (0x%04x)
  current standard: %s (0x%04x)
  autodetection failed, switching to backup standard: %s (0x%04x)
    started autodetect, waiting for result
 detection still in progress after 10 tries. giving up.
 detected standard: %s (0x%04x)
 mono sound carrier: %d.%03d MHz
    main sound carrier: %d.%03d MHz
    NICAM/FM carrier  : %d.%03d MHz
    NICAM/AM carrier  : %d.%03d MHz
    FM-stereo carrier : %d.%03d MHz
    6.5/5.85  D/K NICAM FM (HDEV2)  6.5/5.85  D/K NICAM FM (HDEV3) parm=dolby:Activates Dolby processing parm=amsound:Hardwire AM sound at 6.5Hz (France), FM can autoscan parm=standard:Specify audio standard: 32 = NTSC, 64 = radio, Default: Autodetect parm=stereo_threshold:Sets signal threshold to activate stereo parm=debug:Enable debug messages [0-3] parm=once:No continuous stereo monitoring parm=opmode:Forces a MSP3400 opmode. 0=Manual, 1=Autodetect, 2=Autodetect and autoselect parmtype=dolby:bool parmtype=amsound:bool parmtype=standard:int parmtype=stereo_threshold:int parmtype=debug:int parmtype=once:bool parmtype=opmode:int license=GPL author=Gerd Knorr description=device driver for msp34xx TV sound processor �          ��     �          ��     �          ��     �          ��     �          ��               ��               $� �             �                                  �                  +                                                              �           `                       �     ����                     @Q �   �V �  9�]    �a     d 9   �f S  ��O m   @Q �   �> �  �cL �  UUS �  GZ �   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                           ��       `     '       2     A   `   e     N   �   0     Z      9     i   2   a    }       (    
 �   @  c     �   �  B     �      P     �   �        �   P  |     �           �           �            �   �  3     �   �  �     �   �  p       �  �       V  k     &  0  \    1  �  �     A  �  "     M  a       ^  �  @     i     �    s          
 {           �  �        �  0      
 �  y  �    �  �       
 �          
 �  `  l    
 �  .  *     �  @       
 �  0  V    �  �  �     �  X                        �            
     0  �   \    
 ?  `   0    
 F     0    
 S  `  ,    
 a  @      
 o  �  P    
 }      &     �  &   B     �  h   Q     �  �   ?     �  �   '     �    *     
  I  Y        �       9           G  �      
 Y  �       t          �  �      
 �  �       �  (        �  �  	    
 �  �       �  <          �      
 4          M  P        [  �      
 m         �  d        �        
 �  &       �  x        �        
 �  :       �  F         X  9     '           ��:  �  �     I  @  �     ]  4      
 g         
 q  s  .     �  �  �     �  �     
 �     5    �  H      
 �  �  -    �  `  @   
 �  �  $       �  �     %  �  3    :  �	  �    O           l  �   0     �  �        �  �  O    �             �          �             �             �                                         �  B    /             L      
     [  �        m  `       {             �           �             �          �             �  �  g     �             �                            `      ,             7           C             S             b  p  ?     x  �  �     �             �             �             �             �  �  �     �             �             �             	             &	  �        3	             >	  �  �     H	             _	             l	             	             �	             �	  �  �     �	             �	             �	  �%  
    �	  `  X    �	  @        
             
  p  T     )
             1
             >
             M
             _
             v
             �
             �
  `   0    
 �
             �
          �
          �
             �
             �
               �   �                 -          6              msp3400-driver.c msp_s_i2s_clock_freq msp_s_i2s_clock_freq.cold msp_querystd msp_s_tuner msp_log_status msp_log_status.cold CSWTCH.74 msp_g_tuner msp_s_frequency msp_resume msp_resume.cold msp_s_std reset_off.1 reset_on.2 write.3 msp_reset.cold msp_read msp_read.cold msp_write msp_write.cold msp_s_ctrl msp_s_ctrl.cold msp_suspend msp_suspend.cold msp_remove msp_probe msp_ops __key.4 msp_ctrl_ops msp_probe.cold opmode_str CSWTCH.84 scarts msp_set_scart.cold scart_names msp_s_routing msp_s_radio msp_s_radio.cold msp_driver_init msp_driver msp_driver_exit msp3400_pm_ops msp_id msp_core_ops msp_tuner_ops msp_audio_ops msp_video_ops __UNIQUE_ID_dolby282 __UNIQUE_ID_amsound281 __UNIQUE_ID_standard280 __UNIQUE_ID_stereo_threshold279 __UNIQUE_ID_debug278 __UNIQUE_ID_once277 __UNIQUE_ID_opmode276 __UNIQUE_ID_dolbytype275 __param_dolby __param_str_dolby __UNIQUE_ID_amsoundtype274 __param_amsound __param_str_amsound __UNIQUE_ID_standardtype273 __param_standard __param_str_standard __UNIQUE_ID_stereo_thresholdtype272 __param_stereo_threshold __param_str_stereo_threshold __UNIQUE_ID_debugtype271 __param_debug __param_str_debug __UNIQUE_ID_oncetype270 __param_once __param_str_once __UNIQUE_ID_opmodetype269 __param_opmode __param_str_opmode __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 msp3400-kthreads.c msp_set_source msp34xxg_set_source CSWTCH.91 CSWTCH.92 msp34xxg_set_source.cold msp34xxg_set_sources msp_stdlist msp3400c_set_audmode strmode.0 msp3400c_set_audmode.cold msp3400c_init_data msp3400c_set_mode.cold msp_detect_stereo.cold msp3410d_thread.cold msp34xxg_thread.cold msp3400c_carrier_detect_main msp3400c_carrier_detect_65 msp3400c_carrier_detect_55 msp3400c_thread.cold v4l2_i2c_subdev_init msp_debug devm_kmalloc __msecs_to_jiffies __this_module snprintf __stack_chk_guard msp3410d_thread v4l2_ctrl_handler_init_class cleanup_module msp_stereo_thresh msp_write_dsp set_freezable msp_dolby schedule_timeout_interruptible msp_amsound add_wait_queue msp3400c_set_carrier i2c_transfer_buffer_flags kthread_should_stop __wake_up msp_detect_stereo __fentry__ init_module wake_up_process __refrigerator msp_standard_std_name msp_reset v4l2_ctrl_handler_log_status v4l2_ctrl_handler_setup schedule __stack_chk_fail msp_set_audmode _dev_info i2c_register_driver v4l2_device_unregister_subdev _dev_err msp_standard mutex_lock msp_sleep media_entity_pads_init kthread_stop freezing_slow_path current_task _dev_warn msp_set_scart default_wake_function __init_waitqueue_head msp3400c_thread msp3400c_set_mode msp_write_dem kthread_create_on_node msp_update_volume strscpy mutex_unlock param_ops_bool v4l2_ctrl_new_std v4l2_ctrl_handler_free system_freezing_cnt i2c_transfer __mod_i2c__msp_id_device_table remove_wait_queue msp_read_dsp msp_read_dem i2c_del_driver param_ops_int _dev_printk msp34xxg_thread v4l2_ctrl_cluster msp_once __SCT__might_resched     �  
   u       a   �  �   u  �   �  �   	  �     �   �  �   �  �   �    �    z  )    0  �  5    A  �  �  �  �  �  �  �    �  
  u      E  �  Q  �  �  �  �  �  �  z  �  u  2    B    O    a    p  �  y    �  �  �    �  �  �    �  z  �  �  �  �  �  z  ]  �  f    u  u      �  z  �  �  �  �  �  z  �  u  �    �  �        z  $  �  1  �    u  �    �    �  �  �  u  �    �  �  �  �  �  �  �  �  �  �  �  �    �         �  '  �  F  v  U    \  t  �  �  �    �    	  �  ;	  u  �	  u  �	    �
    )  |  C    a  �  k    �  �  �    �  �  �  �      .  �  8    [  �  e      �  �  u  �    �  �  �  �  �    �    �      �  !  �  A  �  a  �  �  �  �  u  �    �    �    1  �  �  �  �  �  �  �  �  �  b  �  �  �  �  �  �  z  �  �  �  �  �  �  �  w  �  �    �    �    �    �  -  z  I  �  S  �  ^  �  e  �  q  �  �  �  �  �  �  �  �  u  �    I  �  `  �  q  �  x  �  �  �  �  �  �    �    �    �    �            -    A  �  `    g    y  u  �    �    �  �  q  �  w    �  	  �    �  �  �  �  �  �  �  �    �    �  !  �  '    D    Q  u  �    �  �  �  u  �    �           u  *    e  u  m    �    �    �  �    u  
    $    4    R  �  W  u  _    �  �  �  u  �    �    �    �  u    u  
      �  =    B  u  J    a  �  �  u  �    �    �  �  �    �  �  �  �    �    �  #    /  �  D    N  �  [  �  w    �    �    �  �  4  �  M  �  a  �  �  �  �  u  �    9  �  ?  u  j  u  r    �  u  �      �    u  #    n    �  �  �  u  �    �  �  �  u  �    �  �  �  u  �    �  �    �  W  �  ^  u  l  u  �    �    �  u  �  �  �      u  $    ?  �  p    �    �  �  �  �  �  �  �  �    �  %  u  3    C  �  a    p    ~    �      �    �  1  u  9    J  �  v  �  �    �  �  �  �  �  u  �    �  �  A   u  I     e     �     �     �   	  �     �     �     �   �  �   u  �     !  �  !  u  !    )!  �  .!  u  7!    ?!    K!  �  }!  �  �!  �  "    !"    4"  ~  @"  �  \"  �  {"  �  �"  u  �"    �"  �  �"  �  �"  u  �"    �"  u  	#    #    &#    -#    I#  �  [#    u#  u  �#  u  �#    �#  �  �#  u  �#  u  �#    �#    $  �  -$  �  ;$  �  R$  �  h$  �  �$  �  �$  �  �$  u  �$    �$  u  �$    �$  u  �$    %  u  %    +%  �  :%  u  B%    Q%  u  Y%    h%  u  p%    �%    �%  	  �%    �%    �%  �  �%  u  �%    �%  �  &  u  	&    &  �  &  u  #&    /&  �  [&  �  d&  �  �&  �  �&  u  �&    �&  u  �&    �&  �  �&  �  �&  u  �&    �&    '    ''  �  x'  �  �'  u  �'    �'    �'  �  �'  �  �'  u  �'    �'  �  (  �  8(  u  @(    Z(  �  f(  �  o(  u  �(  �  �(    �(  u  �(    !)  �  4)  �  L)  �  Q)  u  �)  �  �)  �  �)  �  �)  �  �)  �  �)  �  �)    C*    U*  �  �*    �*  u  �*    �*  �  +  u  +    !+    E+  u  R+    l+    y+  �  �+    �+    	,  �  ,    ,,  �  h,    �,  �  �,                 �  .     F   	  L   �  U     a   y  p   �  �     �     �     �     �     �     �   �  �     �     �   �  �     �   �     	    �        �  %  z  <    B  �  P    U    Z    f    t  �  |    �  �  �  �  �    �    �  �  �    �    �  �  �    �    �    �  �  �    �    �    �  �        	    �  &  �  3  	  <  �  C  �  M    R    ]  	  f  �  p  �  }  	  �  �  �  �  �    �    �    �  �  �    �    �    �    �    �  	  �    �  �          %    *    /  	  5    :  �  I    Q    V    [    b    h    m  �  u    z    �    �    �  	  �  �  �    �    �    �       �    �  +    Q  �  W    \    a    f    x    �    �    �  �  �    �  u  �    �  	  �    �  �  �    �          �          #  �  3    ;  	  A    F  �  T    \    b    g  �  o    �  	  �    �  �  �    �    �    �  �  �    �    �    �  �  �    �  	  �    �  �  �              �  #    4    =    B    K    Q    V  �  ^    g    m    r  �  x  u  �    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �  �  �  u  �    �    �    �  �                �  )  	  /    4  �  <    F    L    Q  �  [    e    k    p  �  z    �    �    �  �  �    �    �    �  �  �  u  �    �    �    �    �    �  �  �    �          �        	  "    '  �  /    9    ?    D  �  L    [  	  a    f  �  x    �    �    �  �  �    �    �  	  �    �  �  �    �    �    �  �  �    �    �  	  	    	  �  	    	    !	    &	  �  .	    6	    <	    A	  �  I	    Q	    W	    \	  �  d	    i	    r	    w	  �  	    �	    �	    �	  �  �	    �	    �	    �	  �  �	  u  �	    �	    �	  	  �	    �	  �  �	    �	    �	    �	  �  �	    �	  	  
    

  �  
    
    
  	  '
    ,
  �  :
    ?
    H
    M
  �  U
    Z
    c
    h
  �  m
  u  u
    z
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
    �
    �
  �  �
    �
  	  �
    �
  �     u                �  (    -    6    ;  �  H    M    V    [  �  h    m    v    {  �  �    �    �    �  �  �    �  	  �    �  �  �    �  	        �          #    (  �  0    8    >    C  �  K    n  	  w    |  �  �    �  	  �    �  �  �    �  	  �       �          !    &  �  .    5    ;    D    I  �  Q    Y    _    d  �  l    q    v        �  �  �    �    �    �  �  �    �    �    �  �  �    �    �    �  �  �  u  �       �          x     �          �      �     {     �        $     (     ,     0     4     8     <     @     D     �     �     �     �     �     �     �                   $    (    ,    D    H    d    h    t    x    �    �         8    @    D    H    L    P    T    X    \    H    L    P    T    X    �  	  �    �    �        $    <    T    l    �    �    �    �  	  �  	  �        ,    D    \    t    �                                                   $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     �     �     �     �     �              x     �     �          x     �  $   �  (     ,   x  0   �  8   �  <     @   x  D   �  L   ~  P     T   x  X   �  `   u  d     h   x  l   �  t   �  x     |   x  �   �  �     $     (     <     |     �     �     �     �     �     �     �     �     �                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .rodata.str1.1 .rel__mcount_loc .rodata.str1.4 .modinfo .rel__param .rel.data .bss .comment .note.GNU-stack                                                         4   (                  2             `   �,                 .   	   @       �k  �              <             *-  �                 8   	   @       �y  �              O              ;                    K   	   @       L�                  ^             4;  
                  Z   	   @       l�                 m             @;  �                  i   	   @       |�  0     
         u      2       �B  b                �             RK  �                  �   	   @       ��  (              �      2       �K  �                �             {P  �                 �             S  �                  �   	   @       ԉ  �               �             �S                     �   	   @       ��  �               �             �T                    �      0       �T  &                 �              �T                                   �T  �     t         	              �`  K                               <�  �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      �2      4     (               �������Sk�t  L�|  �X4�H01��J�Z[Í�&    �t& �������Sk�t  L�|  �X4�H01��J�Z[Í�&    �t& ������y��   VS�y   �É���   �y��   k�t  L�I;�`  �~   ��d  9pus���  �P���  �P���  �P���  �P���  �P���  �P ���  �P$���  �P(���  �P,���  �P0���  �@4���  1�[^Ít& ��������&    f������Í�&    �v �����S�X\����������   �����1�[Ð�������W��1��zS�Y�����B    )��B,    ��(����u�B   [_Ív �������&    f������UWVS�Ã��@X�T$�$���   �C�P�R�����%   =   �z  �C��  ��  ������Ņ��e  1���   �ﾀ   󫍽4  ��0  ǅ<  �  ǅ@  �  ���ǅD  UYVY)�ǅH     )΁�   ǅL  �  ��ǅQ  �
 ǅl     ǅp     ǅx     ǅ|  `  ��   ��0  �|  ��,  󥹠  �D$�<$�@��0  ���  ��	��  �G�WǅD      ����	�	��  �ډ�������M\�   ��ǅ�     ���  �E  ������Å������j ���   1ɺ   �������� 	� �P  ��j h�   j jj h�   j j ������	� �P  ����$j h�   j jj h�   j j ������	� �P  ���� j h�   j jj h�   j j ������	� �P  ���� j j j h�   j h�   j�hL���������	� �P  ���� j jj jj jj j ��������   �ul�� ��t��x,����[^_]Í�    ���������������Å��-   ��yԉ�������؃�[^_]Ív �����빻����벍�&    �v ������ȋ	�x ��   ��t>�@   k�t  L��`  �Hk�t  L�@   �@   ��d  �P1�Í�    ���  �H���  �H���  �H���  �H���  �H���  �H���  �H ���  �H$���  �H(���  �H,���  �H0���  �P41�Í�&    f������Í�&    �v �����U��WVS�Ӄ��D$��Qp�T$<�}   ��&    �v <t\<t`�D$�k�$    �{���   ������������t8�<$�D   P�D$�t$h-   ������T$������[^_]�f��C������C��<u���1�[��^_]Ð�����U�hpWV1�S���   �ډډ��������uPSUhd   �������[^_]Í�    ���l   �ۍ�    �����UWVS����x  �$����   ��|  �։�1҉D$�����    ����L9���   ��H!�!�	�t����   k�L�D$����h�$�D$    ���   ��p�D$�   ����������t�|$��   ����[^_]Ít& �<$�|  ��t  �C�=     ��D  ��   ����[^_]Ã������[��^_]�t�$1�1���|  �d�����&    ��    �����UWVS�Ã����  ������ƍCp�D$����   ���   1�   ���������y	���  �D$���   �$    ��   ��������<$t����  �$���   �p   ���������t����  �=     ���  ��   �t& ���������  UWV��S�Ã���8  ����  �Cp���   1��$�   �����������   �ǃ���   ��<�!  <��   ���  �   ���  �w  �   ���  �Cp���   �D$    �D$�����$�ǉ˺:   ���������y�|$��  �����$��!���8�u6k�L��|  ��F#T)#D)��=     �F�f  ��1�[^_]Ít& �    1��F    ��[^_]Í�&    f�����  �ǃ������Cp���   �$�?   �����������������B  ��<�������&    f����  1���������D��w
���r�������[^_]ÿ   �������    ��������   ������P��������Ít& �����9�8  ��   UWV��S�Ӄ���t8�������u ��[^_]�:�����&    �v ǆ8      ����[^_]Ít& ���   �hp�   �   ���������tȃ���  �$UhX  �����XZ�$봍�&    �v 1�Í�&    ��    �����UWVS���$��^��   �Ã���   �Cp���   ��1��D$1҉���������t����  ����[^_]Í�&    ����   1��{p�D$�D$�3   �������t���  ���Cp�\$���   �D$��$1��˺3   �ى��������t���;  ����[^_]�f��ً$�\$�=     ��4  ���  ���  ���  �P����_  �t& �������<�����&    ��&    �����UWVS�Ã��p�x|�@(=	� ��   v\=	� �I  =	� �  ���F��$    �n�ɉD$����|$���Ϻ   ������������z  �<$t^�  �= 	� ��   =	� ��  ���$    �n��ȍF��|$�D$�Ϻ
   ������������  �<$�@  �t& �=     �  ����[^_]Ð���$    �n��ȍF��|$�D$�Ϻ   �������������   �<$t��}  ����$    �n��ȍF��|$�D$�Ϻ	   �������������   �<$�j�����  �t& ����   ��   ��L�����   ���ȍF��$    �nԉD$�|$�Ϻ   �����������tI�<$������  ��|$���   ������|$���   ������|$�~|������|$���   ������|$���   �������&    ��   �   �f������   ��   �V������������           �  @                  �                                                                                                                                ?                              	  �    
  �      �                                �      �      �            �           U           %                      @                          !      "     #      $     %      &      '      (  �   )      *      +      ,      -      .      /      0      1       2       3       4      5  �    6  �    7  �    8  �    9                                                        0   `   @  `  �  �  `    `  p      
  �
  �      No platform data
 4%s: Write: retry ... %d
 3%s: Write failed. Err[%d]
 4%s: Read: retry ... %d
 6%s: Reg(0x%.2X): 0x%.2X
 7%s: Standard set to: %s
 7%s: Current STD: %s
 7%s: Input set to: %d
 tvp514x    3%s: %s decoder driver failed to register !!
  6%s: %s decoder driver registered !!
  3%s: Unable to turn on decoder
    7%s: chip id detected msb:0x%x lsb:0x%x rom version:0x%x
  3%s: chip id mismatch msb:0x%x lsb:0x%x
   3%s: Unable to detect decoder
 6%s: %s (Version - 0x%.2x) found at 0x%x (%s)
 3%s: Unable to configure decoder
  3%s: Unable to turn off decoder
   7%s: Set Control: ID - %d - %d
 ��pUUh    ��������$  ��h    S����������XZ�$  ��pUUh0   ��������$  �4$�t$h   ������
   �D$��������  V��UhJ   ������
   ��������#  �t$�t$h   ������
   �D$���������  ���T$S�D$��pPh   ������T$����  �t$hX   �����Y[����[^_]����T$P�D$P�D$P�t$h|   ������T$ ���|$Qt/�$P�D$P�t$Vh�   �����V�����h�   ���������$��F<w�����4  �O��  R�W��RPW�t$h  �������0  ���\  �ƃ�����   �=     ��   ǃ8     �)���V���t$hJ   ������
   ���������  �4$�t$hJ   ������
   �D$���������  U���t$hJ   ������
   ��������  �t$h4  �����XZ����1҉��  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ	   �  �غ
   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ   �  �غ    �  �غ!   �  �غ"   �  �غ#   �  �غ$   �  �غ%   �  �غ2   �  �غ3   �  �غ4   �  �غ5   �  �غ6   �  �غ7   �  �غ8   �  �9   ���  �r���W���t$hJ   ������
   ��������k	  �|  ��p�EPSh�   �����1����;	  W���t$hJ   ������
   ��������P  �t$�t$hJ   ������
   �D$���������  S��Uh   ������
   ��������]
  W���t$h   ������
   ���������
  V��WhJ   ������
   ��������  W���t$h   ������
   ��������S  ��p�$USh�   ������T$����
  �4$�t$h   ������
   �D$���������  �4$�t$h   ������
   �D$��������(  �4$�t$h   ������
   �D$��������  �4$�t$h   ������
   �D$��������x  ��P�$�s|�s(Vh|  ������T$���  �4$�t$h   ������
   �D$��������~  ������    �    ������    �����     tvp5146             @  tvp5146m2           x   tvp5147             �   tvp5147m1           x                                                                �      �       �  �    �      �  `    �       �  �    �      �      �       �  �    �      �  `    �       �  �    �                                   �      �       �  �    �      �  `    �       �  �    �      �                                                     �      `          �  `                                           �
          `                          
          0                                   �      �  �          �      NTSC                    �  0u                    �  @        �       PAL                           q                                     8-bit UYVY 4:2:2 Format         UYVY                debug license=GPL description=TVP514X linux decoder driver author=Texas Instruments parm=debug:Debug level (0-1) parmtype=debug:bool    @          ��      GCC: (GNU) 11.2.0           GNU  �        �                                  ��                                                 %     $   0   %     =   `   �     T   `  �     e   @       t   `  G                                	              
 �   �  �    �   �        �      @     �   �        �   P       �       H    
 �   �  �     
  `  �       H   (    
 5    J     >  p   !    
 L  `      ^           Z  �   O    
 m  p  �     �  �   f   
 �     �    �  F  �    
 �   
  �     �  �  !    
 �  �
        �  �    
   �  I    (  �  �    
               <           P      �     _      
     s      x                   ~  @  `     �  x        �  �   �     �     P     �  �  @     �           �     )       5        (  N      