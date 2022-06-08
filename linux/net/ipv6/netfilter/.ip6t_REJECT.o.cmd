odule.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
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
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
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
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
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
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/linux/of_graph.h \
  include/uapi/linux/v4l2-dv-timings.h \
  include/media/i2c/tvp7002.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
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
  include/media/v4l2-fwnode.h \
  drivers/media/i2c/tvp7002_reg.h \

drivers/media/i2c/tvp7002.o: $(deps_drivers/media/i2c/tvp7002.o)

$(deps_drivers/media/i2c/tvp7002.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_kernel/bpf/cgroup.o := gcc -Wp,-MMD,kernel/bpf/.cgroup.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/bpf/cgroup"' -DKBUILD_BASENAME='"cgroup"' -DKBUILD_MODNAME='"cgroup"' -D__KBUILD_MODNAME=kmod_cgroup -c -o kernel/bpf/cgroup.o kernel/bpf/cgroup.c 

source_kernel/bpf/cgroup.o := kernel/bpf/cgroup.c

deps_kernel/bpf/cgroup.o := \
    $(wildcard include/config/NET) \
    $(wildcard include/config/INET) \
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
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
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
  arch¤$9_q-¥rz‹Âíºäâ$Xñzb!úø^ÆF»ğ Q\Éyñ”n80ú¹Ã‚()bÚ¬$~^¹ª¼Ãyú\@W ¨27ó–Qí j†¹;&ó”J¨x&:ÌŒÍ‹¤¿ˆñÈq71x`»hæøŞUKp;Ş¤°Igh d­ğcñÍı½áú2¹ÔÜ:\Kœ¡Y»¸;‡…­Úá9UÕÍQeIri“QPÇEJxjëZ•´J+¶^ïaµ¥©GôAÛ¡U`úR!¤d4­Õnw…U¡Î%™û?›c´òªïİjxïkSnß÷dÅ«=¥©úSMs¥D¢¾‰ÉV|kåÔO6uÊ¼ÿº0áÚğËO…NZ§™áÃú‹N…q \ãİÎësÃ…¤Ó§ÌEërDò~Û¤¡Ş›Şì£İ+ÁY„—H´œªœ^¸Ó8“íû› şçU•«`©˜Ü*õğ¼Mè¤!è„ü;‚½ƒ„vi‰,”Äœw‡×9{İ’ÕµJEGpÑ
wLK;™eÒ–h9C…Ş£,ewPÌ•ËÖp°Î+¬Ş;Íå)z­) £T;=uTòiv:(p–qâ$ç”£­/à/ÑÓI’ıo?÷ ¹Á*Ş`X/İ]ßéó£Ü7š–5ş–¯\¶Š®Au»Øà©Xñq9†[œch)}˜„O¤å0¿
Z&äf0iúæZ|â;¯Pj®æø¶¤ğŒ¥Î\Á	 b†èê«5D}èá€r±æÀ³üÜH¾f‹‡€Øšpİ	uÄ$¶,	z"Z†;ÖCt¹*wDä©3^0äĞ\J;ğGW«Ô5¦u¥~JÈúSô§5··çcÁ{lò…—ªÎ¾©m‡¥ƒ?¿üÆƒ,[æ‹_”™1“D’çşÈOëJ§?z §UÒ;0\ó™‘ãíZ>ÛCNƒ§‘ü:Ú°•tm0`µ) Np9+awk­aÏY3~NÀüe8¨Iç[SÙ+^§¼C@áÇ|ˆÁ…eb‘'²‡«†Æ¯·9˜|áËÎz¢.jŸ’>é¹L 3Y¼¿Nløå¶ìŒa½ ?Ôu	Ô![„‘1ÅíVèT-½Ï|ê¤lyw4MÒÑ$â6ß_DÔ&‡Ê±“‰pWØKo•èÒ_õÅıÈÿ¿";İã¾ı-J †á;:n5ä·k÷æ~ÃGøå°>à°‚ÄçàÄÍ¹Cêm”Fğ¸g–‘ÿÔ·µoéÍtĞYjäâ<ãJ_sB)SÌ€×ë§é:Şôâé+ŠWàa÷P{¹…æmşº›ºw&Í’»Ìˆ˜èZêùuõà-ÂÆ3°G,ÿ=VZeÎIğÏ›”x•'ÈÌ®©A“—!¦ö«¾:<K$ŸÚq,W¯¼ Gö´šœ¦–.†å¯°#Â”­njOŸy$b.˜ÂK^`ı9ú=1r¼_WàöóïÄTšlbÑáÍ?¼•@Õ˜Ÿ \·ÙÆ“æ´Ç©lâ¿é¦0FıIä/mrî»"ìà;4¶•l3;z`š‘JÉ£ÃŠ¨¸Î‰ Ÿl§l¤oS™«­¢„ÁœOÃî¹µî¹¸UÓaÜ¢Ëâ²CòJLø_½M4ÛßŒ‡qJœ`ƒ•ãÓ•‰YëÈ©Ñ_
)ÓéˆæS­·®ÌÕ‹áÛç±­Ékì„¿9œ$VÄå„fŠ•§è´Ö/9ı+É’öÙ‘pñĞô âmIí{ªßŸ*CßJKYÑK¿Xûƒˆ‰ ÷íXWz×lqÃßÒ:eGõ¹.Æwóü>’sE 8(ç·Œè¹G‡K‹tã>ÇŒ8x9uõFšì‹xZÀî,›@køG'W<ŞÿXŒGšş^KGYO†³Ü;nã8ÔÌÀN†Ì29àˆŞ£tŸ‘Qá‡‹ ó\MóïŞEQPî6ºPÆKğ!IÃº"&Ó1kÖvÆå„ÉHv"9Zy—>Á>ŞÍğwäNòe0Uƒ¢D®^î¾÷Ş]Üóé Óa*š¨´ç‘hCí‹{Í`°¨µ®)ÑWƒL»Dîc·tÒû¼0l€ñ,ZoB…ìjgzÃ•×«z+.<õœ@#rxD_;‡6Ë`ePäg%Ì€.3m`µd›¢§œƒ2¦ì8›²	ÂB±“bÍx¶Ii3ÉÍß]]â(ïs·=(z>+#´æWPFHüz‰ŞŸì„rV<Ø®2 uq0Îõ~ONŸ¢¤+ã¶¼z¨P†/|Zƒ±¸±h¾¡¸áQ‹ËA±lB¤}H\ZóŸË‹Pî¢?gN,{¨èF¦-	-“„€1­L ~Xh üaO)FW{B2y²êí°&e….SSÛjŞUX„Ğ6lö:·ì£#.ü%xf7Íh³ä>$™¾zª¬€c5×$	»*ã”“ñ¤f|§=<¿e‡øØ”ÎNı6¢‚oo±‘…z¦oDå¥&ÁAO+İèñ	Hü³\Z¥xÎZ;pW€Ñ"ğ+Ê®;—ø·UÇïŒÛÚÙª•*å9n€dÕnU0\.#Ïä²t’=A7V!¾&K¦³KM®(d2Z×Ş„Oœy€P#Y©dm(„ôSô,‡ï;Ššrï4[:|·)m{`(tYû]r…Ì}8V
&ã¥_–úLvÎÒ¨§ŠBl9-b_îzY’^-ie\GL7†…ÙŸ*,’!•³Ùc‹Õ‰tşˆ§«>CuSmŠ¬Wm³ÅØg™›A„Š—u#3=jÔf¡ê(kı¾·¸Åä(G„æ›Äh5øüfRdYÕXRè[óšT	nOù¶–¤¼*I¯•áüB˜Õ4úúõF1l’ÕCñgšàsRO¯…U¼òWê¸;/ô@Éó
Jy;ƒ2ß„£ô/Á‚‹“a8lj¾Ëè:ÓÃ‚™†öj 0Ñ”˜ÔãrFYÜ¯»ıÊ®ğÀ<1SÈ±F¸<™Ü"¨YæjÄ ªV9ĞaH	TYâxİ ø&wWªÒE¡ñSyü¯Ğ¶å« Æ(>FÖ]O¿×pâ}¥[g”É¹3¤t1ÁÎnŒÖ<ñD‡>ÇšÎ¡aç@lœNÓ¨@Dc‡ìÊçÌòû4Ø¤‡
NaY-7@Z(ùôÃsº÷HÄy¢%”52¿¹µ–0òzîGŒc#ñ{Ø®y ¶[²‡ÛY±©ì<mšŞïkãĞ~¹šÓ¶†¸_®–‰rÊ¶µâIØkñ~nÂ~ñSÿõöÆÖÈÉi3¦íŠ­ùï(7t”4îT¬ÆDİ "RÃ¨4CˆwÛ-£„“ëom=Î‘ìúıÆ™êï‰igaMgz©u>UEÆ¸RqHºyP]k«Dô¡æ¥‰—Ÿ›7Ÿ;,3²À÷Ú”J¾%é+`§òsÌÈ3µéXtn¥Ø}_s®;Ï'á]Ì%H> ÓŸ–D±iW¸`:}XG–™LîMœ/:Ç|œe5
ï¤åC#î³¢ê-”İ›Fo„FqDDiÜá¥c\$h»1®öÔw~U"ÑÄA¿Æ5–ç³ærÎE¯ñÙ$Ò»]#¦eõîÕÅùGŞ•®ù÷äÒšû+Õ®ıÖ-zÎ°ÛÇâaJDûHXÀ&ó(‹	ŠÈÓ´Ìh!æ[ºŠ¼0µL‰ùóx09ı1Ÿ­':·3…·K¹ˆÃ»°t$Ã†Ÿ\àÂÉ¶WŠ£š%¦ÈkÍ×]-øeAÖC/¸g¬"v¦İvZ«sı¡O
iH-şß]•½úU£Û„ÅVCT Hr:"a÷ğ)j×v™ˆ›¦[Ë=q—JH¼qµ!:¶ùRkDdaçòøãCÈëO'Â^M¾½jÃ°é…å“C¯|%ÿœïA„óR}´ü¬7‡çšË hì2#,åÏ€Ìã˜»’Æ,µrĞY¡·Ôìıê÷Âgf9QGÌ9—à.Rˆcå°«8ŞÖƒª=İkÃ]§@)¼·”S"1äüF©å	Â‚]¾
œÇyş¬Ğ"UçÂDñ€ÆüH‡8ëú åAÆ²}$Œ½%¤%ñ‘_Q @äº;3ªpë—.ú=ö‘9ÚºîïoY‘FÓØOßbõı°à·®·wœç£‘¸ú´Û>d_º·ºb©‰®]}PZa œáÛ§­3FY³Cé’:Ë]kÙ*²«ï¬CêÅØùşÓç:yIQsO¶m	ûµ(Elà¦n.,ß‚P7K—ór,½{i“út—!CMSS¢Q­Öy]ê€¡|Ëğ@gÖè$9n¯ò+œì¥üà„°ÙÚô¿x„
£¼’¢ìUÄ8+?V¥eIŸ5E_‚]£¹óˆ+)ÛLÊÎ“/˜$(k–«ÔbàÊÒèÅ¶)Ã¡ÌüqÒ¨?Ùš»³Óé3óŒ-qµ2Ië>ëú3r3Şæ
#8¢$¼kÜZ@ÅæšIˆ/°û	1¡G+€î­Óx?¾C÷•Så¶üY&qV€S6q©e´%C‹nØZ{:¦´¬'ú •ª"èàIÊ°_l„åó€&‚>	x^*º7iÜ
TÂ¸à³×óÌjhtÇØGˆñ.°–9Äx H§w¬*ÖÊ\A–Ñ™¾N-<o0`+3ÿÍÅ—Ë!.¾}UŠ^jtJBéI[¢1¿ƒä­IA":$¨ÇôP4±YxØ2å¢Lîo]iI¨t8pÁÈwï+È$_'83¢jØÀÑÂpV=€ô§ë›Jô™\Ôheä‰óâ8	wø¦f'ú£; å§ËğÈ>Îr |DOÀaE#’H»ıàvAo°GÀu°ó8ÂaÚ¤Íş…¶c8ÆE{5pœJœX  ğ"³È[˜ı\ˆŒ9£|µØ¸'Ã-r€ëdR†ïÆí0‡¯Ê0³Ëœ;Ãr9Ÿ¦/,JxçûAÔ¹ÖRşµWpŸÎ±{ÈqõcŸÊ
ØØüxÀ~ä¶¬wÚ¶\Ô×A
)÷ûVá*Jv°üV­Ş?l¢,|i]ùŒn5öD“¿Opî¾g"…¹­ òd’ï¤Tıˆ8;é)eŒMjÉÕºR®U:Ä)™X±5ÎIÈSfı÷PGJ7äscDÂf”ÚÍR¦3Ñ5F_öÎdUÇ[%Â¨*û‰(šØÉûÙ
Øw¿dX`Âò8¬W8}OqÌƒ¦Úå—İú°^OÁ¥Ö5mÆÑ4+F£´-Ë†%™»ôeoÙ\¢Ñ*ß,ÏDƒµŞŞZú…½#×É×êjÂV9ª ä=ıPèŞCéàB·>}|­©k¨9¾RH]OT)ª¤õBî[s‹ƒu3EÅçq\pš³ÛûåŠñ€¢unuÌ·‚¨–à"¡æ°c„DtVº;GüŒåW[ïúAŒé›
±tNd³FXL[µUhå÷ÍÅãb„,’e\–ûE›À™•ä3&ôôİãlÜß;·IÎ9º[ÅZ,İJB`öBª Í%—ØVQ¤æ«î{²¸÷·ö†ÓG‹MtxÌ‘7á-\³¼ÌÀdú*ËT±º>8\è{BôŠ’c¯úKÿW(bVÚ‡GJš˜U½åÂÙ°ß6Ù-S“l_JÚÛ¢ÕfÆÏHâÕ¡LFrqÍ"9ŒÖ÷@vÆ¨·"[†ÎŠŠ¯ˆ®#7(O›§v<U#‹^mÁ2-ß'ô©º(XÍ_›êŞfšË9ì2äÑØ¸!qyÁ¦ó¬¢Dc6>XË!o€ú}“ TØÕ q3™(kW6wkÀ¬@|Ø8]îs†ƒË%ıœîk¿Gste‚^æ`ÅX·MtÉ¿ëHMÒï§ü1 kIØg½¨9ÁÍ$«SbÀOĞs‚AÄm.!9Æ
e^Å¥ĞÆ
9f¨ù¹<ıèL ŒkºÔû›P¼€Ë 2ø8°I*áåYıëVËóäT…‘añ‚Jõ²ıåùq¤mì†rõSïç£tÆ½lÿ“±ˆ…ş¥D55C”ş^/$‹•¨&)Èç#	’õPMyÃ…4ÆUgØY”’ÌÒ&s¾;¥]ñÏµ|×[rGŠ×¿6GtaDãØklĞ\ nƒ?:Y™ËB·Êee²{üöh.èùÖÒ9_ˆ]Š»'„Å²ñIJ˜úáWš$]Œ¼pSO<ä½qï—åİ{+š ±:xúIÎfoZ•*ğ¨‰ú×y^ùá`î¹Ïeù?©å{ÙøÜQ:™ÿñfMbûìcr©(i-ñ>™X»ĞjõJe Ÿ|İGW&Ô¯{i¶}skÑûa=bí¯¨óûOÆôyË`oGlÊAìÖûõ~.,çP¯YşF–»œ÷È4>®1ìîÌğ³[±Í ë|·¯fáÉŒëÀó<\ËªÃ»‡—aO‰ëí*Ã¬Œß8([ÖUmgòÂj·Ÿ¬^³Óãîù¯gOéïõ×ğäÊÜwæ%Õòp˜·ºz9?F‘ş$·ŒãVß'<†ÓÆ·ËÅ´ÍròX¸
V&ÁHCø(ÃÍELK4|Ä2Ñ4á*¥¿›s8æöàÖÁÜâ,9Qš-+1Š…¹Ø©èà“:Ûú#ñLİÁ»N–}\ãgdÇ :‹Â­ÎKÅ’j Xs7ŞšJê}³_;²B)O[w…"8àÂù\s_0À&d9ØCüÇfiJrO7b)K£àßõ’el,‚ùGÑy²”ç({[ĞÚ…’ŸçáÑ5çå×í+c{°‡-	?ä29Ôîg™¿ŒãªŞ7ÚãBb<ì‰ğô4¡lv‰”„ãNb’íËĞqğkÿ=n Ü&<ğ5‹¸3â,.Nxeın·6•Ğªpm~iŒ.¼T¬ÔÚÛx²ËáEÏ ‹ºóÖN=AÈP‚eØù“«_“W\‘~SònÔ(Û¤ññê»(ó75RiWù8gâ³•ïâe>s'BG\a”¦šlÛ€"™å`V8‚Dƒ`¼¡W]i¿:wíåÏ=´Šk2ú³Ìº±Ò>Œ²°W@5İüÔZ‡‚ÑtºîJÄ”ïà‚Û›n's¥Foà*»İíg?¨§€lIè>ÕQ«£¢À÷İ<+©öì×Ÿ	Cã²,½Ş92÷‡	_‘Vş1åçÕ`’r<»TÙxÍÍ†‹Í+H$Wœ= Ş–P2û_Ú e‚ÅZøù*¾ab‡ö.éûD7¼¹¾™<ùïÎ¹Ô^~gM)jBzJ°ÇI²lµ~nÔã•^ÔÛíğ6èh~†ûoO­GşÛbJ6#
Ï'—cİƒ–lÅ¡ ¸:>X¯™ÑÄïÜÊ=Ş)xİÿ µrêÏmCfl>Të÷‘Á	fM½gëºQ¡÷J¡š13|"’§I˜@4÷B¼¯‰º©ˆ‡KcBcÄéÛ“†|ğFZ€/W”Ô( 8V«â\^l‚Æq]k·æ#nÉîîë—R÷Kqåä—Cü£éÚ­${>²ôhş¯^,ÿ$ìphñ¿+à,¨´ñ(€yÕ=vÒÑ”å'cnã1J0ìSbºÚ¤cÍU(Ë’¡½áè=(BÅ&ğœÅ‘u[Ò†ÑqÄñ˜)ùà_ú	Ws_,'ÍB¬RN±…Ó¥©f™¿óAğ½[iéÕ'øÌ¦ì|_M‚W*7”&{'éŒ6š‡nËš‹	XŸ¹Ş’ I¨y¢@ªŒË{€¼Âs9HijQ«4§/”/«ß×¿™©§çw.¼Ğß·$vîµ‘û“CŒ²
Îùı‹6Q¦tfˆ©èKÈ²n°Q†…KŠÔâ¸¶Qõ´0PØh2ù]BÎ=aÚ>õªx¨Ùæ´œtÒtÅQ'UøÅ§{ZË©téeVoß±:Pvú%B©Ã¬û´G¢şÑÚ\ú¸³%>:ŸŠ¯. Ğ hgi!ª¯=Õ·«3X2ùWÓüZ…°)-_vYË­"t+ò~ŠÉ=Ğ£l¢Ù4×o$‹{°½o²ŒO®rUíqTæğâ•GôÀ‚İğ,EÕŒd4ø±J†ˆÏŞ{Wê ø¿Lµ´ë¸ìÁÈo¨¹‚ï˜r:ÿšıîEŞ‹lÎz¡¢j™ãÕ¤Í

¯Õâ}¿/ËÅ^¢P´dØ×Se`}èsk×uùá‡½E7÷NhK«Ò¹Û–?¿ŒºökûJ­Q‰¶Ñ4;ßCû°œèìuR.üYycˆ¯$¹>¸#paGM"ƒ¯“ã/A~H¸oz“Z /ÆOpƒTë-ËŒ+Ÿ'Æå‘=ğñ´¸¸ÚÒË¼&Ìã²ÊZš58H®\Âi)7^Öi]lĞ(®¼Q1:(¼°¹ˆjGÉ›¸ÙÓÔ›Œ(ÿÈáºãl›´¿¼ùI\º—Ñ¿[ àV'çDŠ­†yh×ó²±P_@8æ*T«ĞoFè’„¢¹ZÃdW¤{e©[‰%Ùq	!ZğßtĞ&©r	&T9jKÚ\€§Qÿd|î·k=Çv’ğ-L±pŞl›ÚELé®£|ãtÑâ@’?Á&po¤yøy»©ó:Ÿjt8Jj¿?È×:Û-´('Èt•'W´ğU ¹{U©ãkÀ= íˆ0>Åµãt2 œ<¾Íû ÁÑVÕx`$Cñ•†*BÑ²ƒy^ìØjÔ{Ñæ”ìgÜlAU€Ù:»=Uš~8EŠ­ıÍ÷!ß‰ÕOÇÄ>±"ÃáKw£î¥?1ıÖÌ°M°XãAl}r3VÓlmñ;õ{Á¾¤Ğ0^ÆÓNDbõ~ÖlÖ2‡ß¢5w}”w¹ûßaF[<8o¡Cqwû#bÙÿ|ë5‹¢Wÿq­Pï0F?>¡¬@ŸA_Â>Æ÷ñ*- ÅÈg®TC€;¼ºr¹]«˜’“÷óŠÑM6”xìtwîœ7;£–X’ëfÀ¡Ùîw„ÊÏĞ\[SåŞÍ~	õ$5° pI
i¦æ3ù,s´îJÀÕİSHgÊ±ÆL	1V÷óĞŸ%]ÕÊœ$Ø@ÓëWÀf'Ed,Yç0¼rÌ%'Y±Áûş4ÿ¼ÿCz×ÆƒÃç0‹ëX@øRKµ‰ƒ~ælF$e®M”C==Mo¯78İÍ“FÆ|Õé'Æ¨*óƒr;m hvMØ-Èÿ½SäÇzÀ%›@ì0œW
‚ËŞ=pkÏ1|J‡]Ìë"²ô¡È…ác¨òøú°›>åáOêräU€”oß~ëmóµÉ’jnÉFÎ€È-µG}aqİ’}ê$¢YhƒFi:‚J…9_,Æ<ÁôYÍ®%°~Û'C[s7?êVÖşÎO`ñ”œíkìÎ)@Ëº«Ï_ô>;mØ‰´W¶U}nº¹h{ŠoÙZÍ&ƒÇ5ü¦¸bz»¶ç‚#›ÙÖSjÖÔ(Z0_¸‘«3£bœ<¾NµDì2¬ª›Îw ş’Âu7Tp@mRÎ’ìôcÀşÁLÊTâ‚ñ5l„^3<8›ToÜ{j¯\î³<ïg¼4 ?VÔìş\s}G/ş\^_Y¥Ü÷Ş„îK»6ÙLºÈ Pt—•ÁuGs¸§=Û`áñ×ó&¦xUtÍ³ìPÌ'™«~ª#ğãoN­?¡H"s<álµPtÁ¿çVe¶xü:Œ¬ûŸ\H†J€GÉUà5e0>:QµT¸{ĞÁİˆX–5éj4E_ÏhûØ&Œµ¤!ä£X¨³'ßTĞï#ıÃì—bã<ƒˆ–twâV¶Hz›OK`ÔÍ%:=:D ®ïJ¬¼L»Ö%ø–ŒL´w¬Sì&(A58’SÎ®¿Ûq¹wu®#Ş¶od»ƒ»wü0Ãa¦˜Ú”j¦u46Áó^UZ°qÜ(‰²ê™±N+§š€-¾v\¿µ|hZä®4˜Öİ-¤.ê	!GŸv…P”«®vóÜ9ò‰ñÁ˜‹jdµ‘~ºFÜµ¡¥ZıåšÔ­ŞÕ._W¹3B’Le>R§¢Jóf¦áN#ƒY„iêAjœïLmë¼N°ÆŞV©ÇŞs¾Lªµ³ŸÄH:hReã-0éÇ2""®.¸lL¿ƒÅZg2¿ëdI0	h”i¢Dún O+g g\±K^Ÿ›Ñ3Ø) ¨ª!/»²ù#~ôéÆ =a~@C,öhú0[xY«S˜üƒ9şäf÷…I¼İøAÿe"Œ¸Rş?|?Ş±Ñ‡Eé'LÉ{r‡’=iÔ_R[©¯e²…Çå÷*«X]ögÇmŞÆU¸¤¾l€h$L _xLª+§›/>Ÿ	›8»j;w?3ZŸl¦ï-xEXÒ¬×ö²…œÀ‹ÜäWyU¹"µ5ø+£yKvµ®¨ıËI]W}#
™Vk±g$ß“÷İÛ Àì(ÒÉÜôQ³Ó-pñ6Ùp@ÿ× ·£èÍÈ&;‡g•úSæY0ÆZ÷{{’ŒQû¦CÀo†êEtIX,bO Å|Ïù]‹ğ{P[3"
¹¡- ÂèÒ¦ñöèÇ"^Q
râ< Ü«GÓ‚ıSoå~ó§$§Ö&¾Æ{’ÜŒhÀ¬+Kö1Fû¹†³S¥u<’5‰­â¿[ÿï1ZUï#ÿ±Å?"5‰aÕJÏ·	pøıq•Æd	faƒñ?ø°7”\á´VNùyĞèZŞ;"WæH“ÿãöt¬f=É#·9"ôú’„×øFÇù-gcZXã£	ãÄÎúlbÄ®8¾‡--ú ¢%Î9È®ò«|Ô†1ˆmÍz¼MKÈ¶R XÚsâ´Ã`|9÷eÂˆæc	'føwÿiÏ‹•)á±ï¬Èb™Àz… ºæ¼h›H0$¿ƒù`ÆŒ‚Á›¿J[;%G­ZCXo‘³ yÑÉ'R5Zë£:ìJ5ıø[õÚ×FãíO÷M´=äÈÄ¹FâDº´ÒŒé2}wÄ‰ ÛÃEÆ!ŒŒ÷òô4:yù0 õ u„»Ëû c”¿¬SBiÿ°*ñdğ_Vô¡•Nl…¹'„î¿°Ô_;öª)bRz`W´Íª‹}á	1±*©v-ßEN$€ŒŸÃˆÀÿ&€Å7ã€¨Ïj‰P3×qÂ¾wwD$Rœ$«?Që…Ì´;·^·ˆù%è.lìŞJaâ²ı–LF}ãÎ€6A· u³×—ŸÆç^=™öª?QgÙ’LÊ7ÁjÒ9ÁF´n£50®k½«%åŸRÌ¬ì÷õ\[Ï/'«:`ÆµÑ—.±ËØ*¦ÒA€MÕÀ­o*YsíJß’£j{ìJ\“ƒbªˆ¼øTdæ`WÏìÌw›fÿ»µSú_š"à¦P¾4ë`ˆÚu §YÜæ˜Jäi‰ œƒNŞ6p7"ıÃ"xå®•‘g‚÷‘pù­kÇ´ªO-´ßâ¾ıYâŸ3ê¶@<¢Mù¸†3öA›çì:ĞçR/ôTI–o]û¥ì‘âñ:ÔW8#“CÌTÇ¾ê„¿qŠĞeCœSÖ|c›ÇĞ£œ)¼~¢%yÆğŒÁÓsŠho¼€¯oäi£ßä(’‡$‹‡jÑ>,–³ÅXòHL.è}øùŠäWy"äx GO‚úÊ5ì`èé,Mi‹¥öD>Ï¹3¸²Ö¦È2¹ø;üÜXºzrèo²8Õ81ÊÊdÑ¢36äGH^²K3£³¶¯öt¹_TeäJ;&½][•ˆË¾G¦éFŸ-I™Â›8FğŠ0qÊ­,‡8¸Œ„ºUB©u|ÔCßò!^m±TŞ­4üÚû”Ós&äöİÁ‡+CÒƒÌ lİ‰·àİx?Ä–$vJ}µ¿µA8¸T6ÁÎçÚlóæDŠRöÁKnÍp…R—V¾\e"ÙıB)¹û__wÏ‡#³MuŸBà/‹oÔ­6³¬Ô=3oÙÑpSêÌâ­*3ÚéÌ˜S‹@óX¤O¡ë–¼hEqPrü]HøÿV—¨¾;Ä¨nüèBç*nÎˆ5gd}¤#¼O§Ù¹¥Òn»¢ƒş¨,â»:â?Ñ¨v«îÌœr}1¥agC^¤S$£œ+öl)påS[?”!}<ÄTÄĞ¤©É’ñ–‰µ2ƒ…‚o¢3r;¬æ7ú“à)¨’yÏ»“²\+Y(ï-,{Æ'@‘í±~:	,Æ—k TË]ëD@¥DØ¢vØ‘¥B63Èm×¿W'’2PÏš)^‰8H+Xá²¨N›ÊC2ÚÓ§ã»Ú¯e?åQm¬€,ü!¬$pI,‘JËô)Í–égò`²ÛËûò¿¥è¹x*4wXau>-0€üH±Bì•¥7Ôv<hë®e2ÃÉR¯bÀÊÔ®‹şk-?ÒÅëKR ê±%Ğß×¡ªÊ #­º±º)dõºy š‚S•±é	ÊÕÆˆ©{˜Œ‰®›è8^/Tk›_á3½/A‡–«¯q2šxi›¨¯R•Ğ±äÁRf¶ÖøW¼´óÃw^Ãİ÷,-`ÜCHk.æÛ}û$cÅ5½8…[`“Ì¸¦!7uÿ‹„iG¢7âô£ùÂ—€[ÓÕçBˆqëBéG„=©ÀÌk¨;Éuo&Ú½Í=j>¹'QŸŠáL³áRhu Şş"ôGe&3³ô0NÍÚ–€¸\jÅc°Ê¹ÔšĞMd–ÇŒ&™ĞÕµÀß’©ÜÑöœ¿¬O)Ü`Ğd¥ÌÊ§$¬@”còv54U·ÁzõÀ£;*
ùá{ca—
ƒ{`IUu9ÖšğÃ©ıÄUo×7ÏcŸ¬şSœÖo®L+–üZ¡÷¹¶Ezú¨f¬›,ï©ÿ21eäÜËşTŒ5eK‚Ğµ;×0cá7İÉL}EôÂ¶PÛÙö×J:NÂFQt‹KRşè„…«¯ß€€ËŠÔµÍˆœóbÿ&yVcƒÑy1¶W´ƒXoøæ¿([¢ÌC]‘¸¿b‡rõyp]mÆ(pPµ3ê&dÕ¾ßİ¯=Ñ#Ï®œí—ìè¥)öUA_‹¸’ADL(‹ö;óa3?è¥Ü†§ s‚çû?†Ãü5Jfgïø\ª•gú¸>ícÓŞ¡0äï
rFp-“¾Wä&Ï”éG>,=®cq¯œ¶ïŒ“®¯‹š4bK×Î3!$à
‡wéF„vÑP[Cx>ü»Ò£_np+W¢_0±Ù~ıÂ<l¬€;hÔr(-ÇRÎÈşĞhZ0g0suÅÌêZüµ:W¿ÛñĞ>ãbûM»Cå"«¼Céò’ÕÒÄ"•ìÜ«{m1ÔÖEñ)×©aÀmÓ%İ QáÉ_+ »±ğ0`ë`s-¬+Eû´ràÀu±ø^É²S’,·¼™Æi¦ÄC%„p.»À@\@„·¶Xç÷
ùP—oürô ™Aö»+4`‘\Hƒµ
k²®ŒáÍ›å$×˜ëŞ37W ‚¡7‰+pEÉ}ş¾ºÜE–Õı6{ŠRr&
}[×l²íşß=¡+4²‚E‹YBDú“FöX©ax¾¨qÎÒë£ô™~#$jí5a”fİlÄRlßt…áA9ì¥ ˜Sò¹(q‹
m¾jœ”À3Ğ.)Š²œ†"ñõâZ¡>R ‡‘áãg<óÓ¶˜7°({œ5>$êmDy’Ğ·È«„aåo?JÉ–jôş”=@„ÍĞæ<Ü«Şg_V7vpÁ‚˜ëÜÀM2øùæ‚ŞDT·©Q™àô1 KE¤ï¦21«™1,ıónİæt/Öaú¢çĞÜŞ°Ç:ÑY±\ÀÃİöG™KzB* =0ÕÌ–\O²ß;ë›D|™ ûïæöP;.U­l±À´tÍuÆcìÍ !-ùW(ªúu’ñÓÂóehÀ™SGg@¬;ÿ€í>V@½$êå4wÙe
œ>Œ„¿ØK¢l“™U¶ãzi:fJ£@XKp]f??q‚²®¦s(ÎÛÇlOªÿãÌÚMŸÿÇïñ	ÜÈ;“|İbßªÒç‚îo­¸ÊŞ_9üÚŠ‹”×õ‡?ß/GıòŒĞ¨˜ptMYb15nÏß.Qò1áº³(R«!ÿËì8#·¢)Ñ|á£•GÚÊz / ˜¾3í†á¡ü „l…Ä
Â9aeĞC‚O§}8]w[%cwŠçh½]àÊå`!ºèª-À<3L¨ävÒ½Á1@$	½cõÙıF	’r™è:7B
°
	x
ÚE)ªÓŠôøg	¶éÑ³K#Ç"`÷‚¿TƒhêŠMÄr’ªU|?¯.Iñì†ƒæ‘çÇ±ÓL‚eÊJî8\YÁi=yÁÌ¬|Åïx8næ
Îo¸æ³’v\S>¡fèR ®ğ©îÎÏÁìó¤ï¾û«¾íK˜Ep»” Âü¢?%gtİG]$…uƒ¥Üö‘‘x"Z€„ÔRË¸}ON×ÆÅTí¦Q¬yƒ1áN«¦§ì"¦(0lËnüô†'^eT–Œhªë|	şùóö”õ®×üÖ°D[Ì,¿C±*ßF.Ÿò2cu`(ØÅÛ)Ït\ae·Ê|¨ØG?&Ä—ã®	èŒBÈè0H*t@È‚h%bO¤"ïş»™ÍŞş•æZÄÈçÂÁKbÏ"j¤+B;j—u5ŸüÙ÷oëØTY¡¾í§2r
j¨÷wh i8lrî”¥ê‰¤g@vQ‡#´ÚštAñÇ£È˜ÍíZfu©mSªÓ_qT‘ş«oh@)<Ç•7^ÜÅ$ğïw.sÌÃ6}¤zW¢_î¦ÔğvD†§)ìÍ›bçä´U–ËÂ«È0Äà@ZÏ’qaúnÍ…¢l'™àÓW$”8¹ÇË:£ÆJÿítÒÈÉ¼×Š`ö×L#
}¢rQÇ/º
ó|Ùœm@ß‘.;N>xášZINLÌéÔM€>Z‘*ƒpYqÓ o'ŞZ(ºôCã`IFµÕ	`•iÖ±‚¾Ã5ñOhW#³IDßµ¸iOÔÇ­ÖOêzäÓœêÿõ9ˆõŸû›+Š,Ğ…İûd|Z2]V	£µ~É©eñãÏ©(ÒæÂã­O¨íd¼¯
42
¹Â¾æÑ<Ñ9!°•[Í[í"ëšC6ÃN6Ôë-*ğŒ:4óC|ùHPe ÈÓs²rP‹tWè¥RA´—Ï‚3”§Üá<AÅ‰MZv	It’:ì‚˜3HãÖæH	 F¢û''µ'Æáx€ßííº7Ş#3§³Ô««$;–/Cd<nÒo˜ÑÅc2Æ>ÓÔe^l§Wd©yGwø~ÃÄãPÄŞUòñ¨—2“hpõ	¦ï]Ê€ò#À¡~[2Om=…­—wÃ„ë¶ëNuşş@qéGË¡#ı”yHÁåÜA”b¼Ù^ŞÚP†PW´/}a(È}¾Ñ,›ÅNÊ‡ÌQjh?üZI_:Çá^=:Ÿÿ¼ı“éòW•[@vzX€°”ÇÖ‹%:J´0ĞZUˆ„Ë’“T|·†NË /<¹3e¾ÛÓæ¶ëÉ0ØJ§­ÇµQ>@î¤…t¾Í×½¹›ŠàäÎÁZÃo‰Õo¶|–-PQâçÍın„iÔFN
Öh í‡!·Nì‚gÙµH¬ãŸ=+yuú^ÓêJL‰åĞŠ×¹¥Ş1´ÅT–<	¡áàßäyí¨ˆ9Ú¹wÁNö!v‘ƒ[Õlo_¸…óGhºòwggå¹—y¢Æó„Sıø¶6|Ë¸º/Òù¾ÑêGè¢ov¬!Ïyƒ|€J³³Är¹¦ÚrÕÊÖ^²ÁİMåónQ ……Ë	ØU“‡hU‘èk`bó…Ñ­cs¹½Ê8Æ!êùÙÙjÑ—>¿!Z4{UƒrÎx¥õp(¶tº/WÒL ‹ ØŞ·Ã´o(i€Ü¿¸×İGlİÛ¢Ù¼È3è}Ì.,;¶ûhÚı1¿ìw©eá=‰r¾¶ìz% Üú>vÛJa¦1î=S,¡<+Ìg?iX
oû€ˆJ´êáSo+,V†¡’ú'By'üæ±0»ÅŞª¾ëÇÃ_j’ğ×²=¤ù3Ö1Ú6\7ËR6Ç®
6]şWA¹.š‡IAåÎqGÃµªı~Ë ®¯‚Ôs)K
[%™¬Â¸ì^@ËíS3f”FfV[ùbc”¿ä6îR<hÏMlo3ÒÊ?AÿŠy3Í»k:"#÷—?âĞñ5xgÓqÏŒ.*õŸµ²5€¨p-ÅÑDp£Y•Ï—ÑUïº¾eÜ]r7ÕS	0û4Î¨9@[‡öÆoQ‰³è3t›#á‘¥D¡C=#ƒo]éºØ‹Wó1¥¼«<.óôqøV Õ:³Zâv÷\8Rp‰À»Ãæº¢é²Ç)p:šŸŒXj<&ãÚdÊ2Uœo¨Ó½Áu:(ÛfÔ„ò€±ôÖ³K;5¶Ê-„ÊãLÚyÿâm8ó[D|"»Ïò—í…ÙO×#¹÷öúøÆ)êŞíf°‚	¢B95ı}P±æm^ˆxãs"‘\ĞêPESzSJ¸™å‹mĞä¤º7é¨.B:ÏlÚ=[­­FÃ´ç§IK?ñP9 (#A…q~'§ñØÙÜ¸|3pî\Ó¦&ÛL%`z(bÙ3(h« ãİ*—Õs&K—xETÊ<˜Í~åôT bŸşdGr…šík¼€ñl¤‡~î8h½:z!¾)äM”(²Kf@Ü¦
[<ó¾vÛsZ¹ê=I²Vë)¯1Ö ï/HÜZšØÕ8Ôè"–Xzn­_òşØõ)×ÔEOuÂ¸LÆ {İ«‚æ ^±Q8¥ô|í$ÓNV­]ğô/¬@®éI[ŒËWfaw'İú_DÌõñ>ˆ-ÂÆÚhñ@(-y/¶üKÈgiÿn²ıø ƒÚÌñhÆNÈù^ZÒ‡—_Çk¢‹;»_Ã%ğl<·%Ğù²Ö3^tRM:pp}Öq„÷j™ÖïwFgÓxû²/)¤ó¶jú—ŸŞáàA¶åÖ"K¼4¶¾>]÷1Ô¶n"àC¶–Ÿà¼dˆxHQ®b<½„÷;¡ª;m:slÎ«¤Jiù2Cv#Š8ü€>ÉˆŠPNâXúàŸKÕÖ¦]Qq™¼&¨<O¾€tOú¢?c°% [=üM`m3Ùíşq467A~ôÆb57‘#Ñ,¡µôÂ;êiÇ‰!0ë+!¾Û,ÌM_¿¯ƒßjïuëY„KDVUR¹ÿãzßİFü@
ÿ,	G[æÇ…:MÕ‚íĞ½³÷­˜§³'Ÿö‹3x|ÆÁ¹Páª +»–oÒÓœÍw_¸ğd|‘Mú
Ä.Ş¦¥Åˆ~7Õ•éÔ°Ùõó<¢ á5mf¦ıånßkx(š:7 ,áFÄîç 4oÁ¼;rUÁÃõ\Nø¨O³˜4˜TÄƒÑæ€1Ôºz®
1¤‰<`„§¹€xRJyÏQ™µºT‡‹]ûeCÂGT1•r=a¸âê-[.~I¬z—ñÜô†Åäz³Â%r CİÑ^,îg3àZ-4^Áî.>qR²9š·hè„zšNäˆ¥mÄ µí6f!cÆüìeúCYƒÑÛ§…DPtºÍ6€ŞğÔ~F‘ %]hÛ!Ù´šC Ñ Ø­Géj‡¤èÎàæ!K¶.>¹›à¬ÅÓ@ì,²âŒí`†Â<V¯0Ì{r/Sµ}ÇÌRW=,lRúr7»•ö6rT]HP±;Ô-J"êè)½œaÌ$O#cÂÌq>ñœ~ÎmÍ<‰>à„&OŒé¾’ğË¹6ìÔÛH|NsYgş"_ö‰ÿş`[ZûÿÂ¶…+D‰É=cC	¿‹iÁ ÷–fhöÕöĞ®—¦³uoÁ¼ñÕKR)uóøM`ŒdD ˜ÕgRòÖOÜ„« ÙDAª‡å|>p¸MP¸`ñWü'—vğÕšç–Š	cR?eù£Ş' H è¿URíôñT%ğ|je-˜ŒÁ»º`ß€Â!úÏ±¹;óuÆLç‘ŠPÍóe–ÃNŠßı–(f¡¼\hdé@u;×Íµğêñˆ€®*íšÖôè8£Zv÷Áu€;ˆ¡‚à]Ö_âìµ\ÓïYÏ1®ıÜ–~šjG8/î3æ¯ú½VÎ-#®€<zk©ÜCç¢Wûc­jl âDâÄap<Out gu%Pw_Htÿ¡‡Ÿß	¬$ağì²¢0Š“G¥µ,S~ñò7šRE4v Y£3=È’RlØc®}Z«xõ~¢Ymz`¦&Ø¢²™…pˆiâ'©« eÑŞCó5ËGLtké¾‡ÏbPç*®BJÌË©Y3¢U¢Ä^’kShh½…â?pG|*bJ>qøŸJ¨ÆĞkO:/Qÿ¦^©BIŒjmb€­ßŸôœ½,3Í¹Ù`ù# ,Ñî+]6
0ãÓ$°«ËÂ^b_3.+u"Æ›M:KÓCâİ´×Şyˆ|’éë ££ËmZŠSÇõ4rÖ¯_¤¨<~@Îb›X:  ÌÃ„·M«~Í¾äu;™ûm;³¦¢Ã5ZU&“PÁ:î@˜_ÑÚş†kgQ!†,¨3Óò5„ Mıìx‹‰{FÁİ‡O
$´1¨â‘=ñ§¿bÄHç÷²>+¤hßˆë¨ş7!¤¡
°ÿÕãš•ŠCK½n9pTWB8$’m‘ùÓ>l|×}E>jÉWDÚ÷kàNAüK¾Am
 [Ú- N'JE¿O	âZuÆÁ¢ÄÃ¡Æƒ×¡À,R—üò¹Šµz´è/0.ÔèöÓŠå†e%²7/=ÓÒŠwe 7LK_”aSÊ‡®ìS§LOËÇ¾ƒYV›ˆNGªlR´l[{-D^ı9rm™Ã½#İ{ tí-âÇ¥¯µù9€½ØG}à*Ëèİ„VçDŞZáÉG§c·ø‘ÚïÚ·ëšÃwXAœ\áP·Î5êò!juıÓ0Á›¥µ¾šh3ÁÈ}Šf/sÒ.·Ä{ø…èHƒU”P uœb|†Ë£ÅHó¼s´¾c}ûúZ“è¬mæ«İ¾==}>Y”Äî¥‹NUìN˜@ô¦f¦Zlœ+Úâ·vÒ…QB’î'Õ¿Jgmj}JşŸ·Œh"%¿ÜœµPî	°ŞWh’Ä•5V[úCóXúä¼×O†K©šü7@Å½?øBºŠà²y€+ƒ1ÓÌğZ“BVoË{åÚ*ÑP†ºöÜNÅa<WÚi¬%îÂŸ–h3ş'$ñHäNy·¯Ö‡.Á5=vq\>Û¦ˆX«Õï÷€=y£¨A~¢m0;8¢Uñ"æş¬>´¿Úâ™à}»¹?§ˆµaŠÅĞ™šDZaexOZw~¾mG\ØcŒA´v¼äz†K~ Štpõ¸Óğş7¶W´Õb¨Î’ÊÖ<õ‘òRr´üô:ĞmĞôm[A`°•õ¬¡h4şŠ1-ahp^¼×)®”S‰‘*HÏ–#âÃv‚Ú1-ödfß©Â…ğvÓ›ÇÕUjz…¾c´$ÔåâÌq¹qôü¢prµÃs='èk£‚ËÊGf/´­ÊK{G…—ä„Ùh¡íuİ¤TÕç‚zÃ½ÁAÃ?6İ÷¡¡DVƒ²¬Zù…SÀÑí½x8Z m¤’¾FG&}ï½œ9|ÔV:®~ı—Ò\òUÊió¬´ß¥>yHu5(JVj|Ğ4Ç9IÚ‹pÓbF¹{y;1k9õaa¯píH0H–hÄâZ²XFªiêú8È‡zl¾Üı2‰³ê¨ÀÏv4OÑéš‚eŞğ]“Dy3ƒO{¾ğ}€òLu×
C¨ƒ<}dÅbÊxÛ<pH–à[A*’‡NÍ/İ¯\…SkG$I³ é2Òq­BŸŸÔYFà²,ØÅüh‚¤PÎ„&Z;¤¤Tk?Où.§h¸	¬ËØ“Zw´ÙÜŒ%(ZÂëÑÔ*ª7¹Õ
âø¼¨}·Óë,íÙÜÔt“2Bèomæîc†Š†f´¯­BX—o<(ó]öw `*|Ô1)FXy3'ğ¯AÓ‚Û’–6°>É¥ùdÕX(!:Z±©#EEÌí*š;£\ı¾E¨XrŠD³$=l,Ô¼Ü¹Œ	è÷ví`°‰‡&÷ËÑÌ‰GÉÀ©[öa“‘]LäN(V:‚ÿ
İúÕ6qöXÈÅ&j¬=µü Ë ƒí5Ì?ØF<¡öoh`$ï„çœjÿg&o¥¼—Ö(Èq€ÄÿYú4é8£d[X9Öµ¶ùò>[½Ö ÿí‡¯¹;ŒĞ¶0'şfîXºÔy w¾Zˆ î©ÕÈ6†gz1¿üÖŞÂµWDéqÇ¿;<R2@J)¡! .…†‡-ÈÆÖı	¨HÂıÍZNAC•)–ç¼¢¿õCcUUë¾ô;m1	³aHˆà2ª–xDÈ^µ%^ƒÅª58Ö”XŸ¯I½íıõIèƒÅë ™"Aæèj÷«™¿2±0æk+àÂ<.5}|ÜÿuÑ<LİÇJ0Áİ©gÕŒ¤)€I#süSq>î\Cs¯ã6q€ój‚àHWòY@¤RáD°Ä[{F‡„ äıF¨Ç,#n>¿JŸ—œ&Šÿëº§lèŒ¯E‹`¨/pŒ¡nŒ’!ÇÅçĞÀ$0p”GÆÜS`•ÿäÚŒ_á˜Õ‘ i1e^÷XÂãdaşÎ."ÄZî2|ïÈL‡$9‡gÇ6UÑİS1@·íàœ³Ë–®ã*÷N/ÈİâÛwsÇH#‚á#obıÇÒN½kÖZn/5´cÇãş šÊàh_ÓîöUA:	Ş<ˆsğL‰
¿g™P¹‰¢|¥­ÁÈ£ªBÅ¨d{ÖÃëœÔVÙÇ PöÚ—aßh¯+‘‘3'È
±ˆ‹HÈ»¬¦V()Ù¦²GÓ¸£sÒ=ƒbßÊèÇáV8Ö+£_è´]-!X¾û±¸Bıñ6JÇ€»¨QíÁÄ€Ç‹×‚WÃÅıº²œ ˜Ç°ê(üñ-ğ_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return cookie;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			/*
			 * Grab 16bits of randomness; this is sufficient to not
			 * be guessable and still allows some pin nesting in
			 * our u32 pin_count.
			 */
			cookie.val = 1 + (prandom_u32() >> 16);
			hlock->pin_count += cookie.val;
			return cookie;
		}
	}

	WARN(1, "pinning an unheld lock\n");
	return cookie;
}

static void __lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			hlock->pin_count += cookie.val;
			return;
		}
	}

	WARN(1, "pinning an unheld lock\n");
}

static void __lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (WARN(!hlock->pin_count, "unpinning an unpinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 0, "pin count corrupted\n"))
				hlock->pin_count = 0;

			return;
		}
	}

	WARN(1, "unpinning an unheld lock\n");
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static noinstr void check_flags(unsigned long flags)
{
#if defined(CONFIG_PROVE_LOCKING) && defined(CONFIG_DEBUG_LOCKDEP)
	if (!debug_locks)
		return;

	/* Get the warning out..  */
	instrumentation_begin();

	if (irqs_disabled_flags(flags)) {
		if (DEBUG_LOCKS_WARN_ON(lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-off.\n");
		}
	} else {
		if (DEBUG_LOCKS_WARN_ON(!lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-on.\n");
		}
	}

#ifndef CONFIG_PREEMPT_RT
	/*
	 * We dont accurately track softirq state in e.g.
	 * hardirq contexts (such as on 4KSTACKS), so only
	 * check if not in hardirq contexts:
	 */
	if (!hardirq_count()) {
		if (softirq_count()) {
			/* like the above, but with softirqs */
			DEBUG_LOCKS_WARN_ON(current->softirqs_enabled);
		} else {
			/* lick the above, does it taste good? */
			DEBUG_LOCKS_WARN_ON(!current->softirqs_enabled);
		}
	}
#endif

	if (!debug_locks)
		print_irqtrace_events(current);

	instrumentation_end();
#endif
}

void lock_set_class(struct lockdep_map *lock, const char *name,
		    struct lock_class_key *key, unsigned int subclass,
		    unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_set_class(lock, name, key, subclass, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_set_class);

void lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_downgrade(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_downgrade);

/* NMI context !!! */
static void verify_lock_unused(struct lockdep_map *lock, struct held_lock *hlock, int subclass)
{
#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class = look_up_lock_class(lock, subclass);
	unsigned long mask = LOCKF_USED;

	/* if it doesn't have a class (yet), it certainly hasn't been used yet */
	if (!class)
		return;

	/*
	 * READ locks only conflict with USED, such that if we only ever use
	 * READ locks, there is no deadlock possible -- RCU.
	 */
	if (!hlock->read)
		mask |= LOCKF_USED_READ;

	if (!(class->usage_mask & mask))
		return;

	hlock->class_idx = class - lock_classes;

	print_usage_bug(current, hlock, LOCK_USED, LOCK_USAGE_STATES);
#endif
}

static bool lockdep_nmi(void)
{
	if (raw_cpu_read(lockdep_recursion))
		return false;

	if (!in_nmi())
		return false;

	return true;
}

/*
 * read_lock() is recursive if:
 * 1. We force lockdep think this way in selftests or
 * 2. The implementation is not queued read/write lock or
 * 3. The locker is at an in_interrupt() context.
 */
bool read_lock_is_recursive(void)
{
	return force_read_lock_recursive ||
	       !IS_ENABLED(CONFIG_QUEUED_RWLOCKS) ||
	       in_interrupt();
}
EXPORT_SYMBOL_GPL(read_lock_is_recursive);

/*
 * We are not always called with irqs disabled - do that here,
 * and also avoid lockdep recursion:
 */
void lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check,
			  struct lockdep_map *nest_lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);

	if (!debug_locks)
		return;

	if (unlikely(!lockdep_enabled())) {
		/* XXX allow trylock from NMI ?!? */
		if (lockdep_nmi() && !trylock) {
			struct held_lock hlock;

			hlock.acquire_ip = ip;
			hlock.instance = lock;
			hlock.nest_lock = nest_lock;
			hlock.irq_context = 2; // XXX
			hlock.trylock = trylock;
			hlock.read = read;
			hlock.check = check;
			hlock.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != 