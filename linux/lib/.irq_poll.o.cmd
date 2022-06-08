.h \
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
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
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
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
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
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
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
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/kobject_ns.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
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
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/linux/fwnode.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
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
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
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
  include/linux/property.h \
  include/linux/resource_ext.h \
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
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
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
  include/sound/core.h \
    $(wildcard include/config/SND_DYNAMIC_MINORS) \
    $(wildcard include/config/SND_MAX_CARDS) \
    $(wildcard include/config/SND_MAJOR) \
    $(wildcard include/config/SND_DEBUG) \
    $(wildcard include/config/SND_MIXER_OSS) \
    $(wildcard include/config/SND_OSSEMUL) \
    $(wildcard include/config/ISA_DMA_API) \
    $(wildcard include/config/SND_VERBOSE_PRINTK) \
    $(wildcard include/config/SND_DEBUG_VERBOSE) \
    $(wildcard include/config/GAMEPORT) \
  include/sound/pcm.h \
    $(wildcard include/config/SND_PCM_OSS) \
    $(wildcard include/config/SND_VERBOSE_PROCFS) \
    $(wildcard include/config/SND_PCM_XRUN_DEBUG) \
  include/sound/asound.h \
  include/uapi/sound/asound.h \
  include/sound/memalloc.h \
    $(wildcard include/config/GENERIC_ALLOCATOR) \
    $(wildcard include/config/SND_DMA_SGBUF) \
    $(wildcard include/config/HAS_DMA) \
  include/linux/dma-direction.h \
  include/sound/minors.h \
  include/linux/pm_qos.h \
    $(wildcard include/config/CPU_IDLE) \
  include/sound/pcm_oss.h \
    $(wildcard include/config/SND_PCM_OSS_PLUGINS) \
  drivers/media/pci/cx18/cx18-driver.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/pci.h \
    $(wildcard include/config/PCI_IOV) \
    $(wildcard include/config/PCIEAER) \
    $(wildcard include/config/PCIEPORTBUS) \
    $(wildcard include/config/PCIEASPM) \
    $(wildcard include/config/HOTPLUG_PCI_PCIE) \
    $(wildcard include/config/PCIE_PTM) \
    $(wildcard include/config/PCIE_DPC) \
    $(wildcard include/config/PCI_ATS) \
    $(wildcard include/config/PCI_PRI) \
    $(wildcard include/config/PCI_PASID) \
    $(wildcard include/config/PCI_P2PDMA) \
    $(wildcard include/config/PCI_DOMAINS_GENERIC) \
    $(wildcard include/config/PCI_DOMAINS) \
    $(wildcard include/config/PCI_QUIRKS) \
    $(wildcard include/config/ACPI_MCFG) \
    $(wildcard include/config/HOTPLUG_PCI) \
    $(wildcard include/config/EEH) \
  include/uapi/linux/pci.h \
  include/uapi/linux/pci_regs.h \
  include/linux/pci_ids.h \
  include/linux/dmapool.h \
  arch/x86/include/asm/pci.h \
    $(wildcard include/config/PCI_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/VMD) \
  arch/x86/include/asm/memtype.h \
  include/asm-generic/pci.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/i2c-algo-bit.h \
  include/media/v4l2-ioctl.h \
  include/media/tuner.h \
  include/media/v4l2-mc.h \
  include/media/i2c/ir-kbd-i2c.h \
  include/media/rc-core.h \
    $(wildcard include/config/LIRC) \
  include/linux/kfifo.h \
  include/media/rc-map.h \
    $(wildcard include/config/IR_RC5_DECODER) \
    $(wildcard include/config/IR_JVC_DECODER) \
    $(wildcard include/config/IR_SONY_DECODER) \
    $(wildcard include/config/IR_NEC_DECODER) \
    $(wildcard include/config/IR_SANYO_DECODER) \
    $(wildcard include/config/IR_MCE_KBD_DECODER) \
    $(wildcard include/config/IR_RC6_DECODER) \
    $(wildcard include/config/IR_SHARP_DECODER) \
    $(wildcard include/config/IR_XMP_DECODER) \
    $(wildcard include/config/IR_IMON_DECODER) \
    $(wildcard include/config/IR_RCMM_DECODER) \
    $(wildcard include/config/MEDIA_CEC_RC) \
  include/linux/input.h \
  include/uapi/linux/input.h \
  include/uapi/linux/input-event-codes.h \
  include/uapi/linux/lirc.h \
  drivers/media/pci/cx18/cx18-mailbox.h \
  drivers/media/pci/cx18/cx18-av-core.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  drivers/media/pci/cx18/cx23418.h \
  include/media/drv-intf/cx2341x.h \
  include/media/demux.h \
  include/uapi/linux/dvb/dmx.h \
  include/media/dmxdev.h \
  include/media/dvbdev.h \
    $(wildcard include/config/DVB_MAX_ADAPTERS) \
    $(wildcard include/config/MEDIA_CONTROLLER_DVB) \
    $(wildcard include/config/MEDIA_ATTACH) \
  include/media/dvb_ringbuffer.h \
  include/media/dvb_vb2.h \
    $(wildcard include/config/DVB_MMAP) \
  include/media/videobuf2-core.h \
  include/linux/dma-buf.h \
    $(wildcard include/config/DMABUF_SYSFS_STATS) \
  include/linux/iosys-map.h \
  include/linux/file.h \
  include/linux/dma-fence.h \
  include/media/frame_vector.h \
  include/media/videobuf2-dma-contig.h \
  include/media/videobuf2-v4l2.h \
  include/media/videobuf2-vmalloc.h \
  include/media/dvb_demux.h \
  include/media/dvb_frontend.h \
  include/uapi/linux/dvb/frontend.h \
  include/media/dvb_net.h \
    $(wildcard include/config/DVB_NET) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/XDP_SOCKETS) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/IPV6) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/XFRM) \
    $(wildcard include/config/IP_VS) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/flow.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/uapi/linux/socket.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/net/flow_dissector.h \
  include/linux/siphash.h G�A���p]�Zs��*䞔/A�P3�n��*v��
+�;�X����ol4�ʁ:V�*���(�wz��Q�)�X9<��
[:2�I���W�nrVK���R�	��Λ�ۘ�+�P�Iޫ���|��?�� d��������(�,7�SC�L7hԧ�w�;��%��#4����y��]�d�T9\)�+.���nK����M�O�4��$�2TɤyBK�6�@����;�[�����~.��m��5ݮ��p����ߺ��!�F���sfʄ;X8�gA�[�~��7��v��+� ����� 9�Ё����f���]kOY0��V�	~�������i�~�_�5闍��*����Mз��,v*f
�R����ݸ�B
k���V�(����=��]��
��pPl�}�і5ex���f���|�\Z\?_�JV`u4��c�q+I��)�8��s8�7y�w��^�nC�Q�rx*%���[���� �?E.#��~r�9hOQg�!fM�����(�	����ޚ�/�A���x��_��+xP�v�o꟝l���R�����ZPկY�t���ۖ~��!������kx2�:1_��5�Ө�|��[{2��Z#���+M88��������-��$ w�ߓIb-$pj�ɡ����yjJo�#p�I,���}�g��<�ͭ�\YfY#Hӽ�&UuDhI��d56��� oi(}OZ	hŦ�Dv��A~� eLM!^6Ca.��V�J)�^D��mق:����=�k.#2׸'h��m/+k:��'yeu���� B葉A�"�)Mb���ݗ|^�T*�˼���} ��SeDF�}�/#�Q�y��Ǵ�+�.�����R5g�����uD�R���f��G�2Ӝ��o���M����d~�'Ϸ�L���4څ>�zܑ�Bj���~9Og�.W�4��e��]w��ϚL�Ȅ���(�>>���0+��d�E���0ަp;��!�w^}E)M5"��-�1Rɏ�w",~E��1��A)��\)Km3�x֓�|�>hf�d�%��ɂfo*�+����y��е��UdNLDn'Q������*��.�n���C��H����7�i&7N��ֆ�X�hV��χF�������N� �+GhE~�7�*E� ԮA�l+3s2�}���n/"Ս��ݦsk=
���Q��@�1�U�UG${��@a�81	<̵)�ݭ9���H7P���y~$��O����0;��a���])�@���g�IA	1Nw_�}?f��:~n�͵br�|!e�M��S�=��Sr-1�)]#@���&%vz;�3o����Z�5��u��ڰ��4�|�"����r���biz9�"T�*�b%m(���s�z wo��]� ��]�j�J�jK�UA����ZL����sA�Ao%�c�n%��L+�"��l	{�?�!�gGg���6o�܆�Ί�O�����Φz����P��_�'�Sߥ��L�e>gb��B�p��N���ZO�CA�sz	�C�S��qb����Ѷ>Z8xpJ��U�h���ɘb^�'3˝�(�SG/ss�)�$�S����P�G��4c�]ʧ+�oi@6��J;�z��g)��y̷oON��67���q��uݩ�>�r�l�;X�Dc!�z����^�
���R��3CYP�����M�0$��-)�%��T_.�������ךP�DY�{���C�V�����8|��P92���A(���1��]ΉbՇ,��|�;>�В�g�sa��0�D'��4�CX��k�\�t�UW��Y�o�_��Y�"'a�V�~�Z�7��0]�>Ə�H�j���,,p����G<C�w��N�����7Z���%�)�0�
��3���H��և�/�(.̷	�Rt8J/Ĭ��)i��?7Gq5���v8��L��p�v��A���qs.�A����s��*�*���چ��R�.���u�&�lN�Ek�{5w�߆|����c!�i���8�,���`�UNv���M�-��̴�>�/_!�E��&�؊�|������_h�`�$�6Y{�@2�E,��+[�;/�x�zUm��dʝ��B�(�j�dcV�kyq�wH\@ꦿ�� q!��S�٧�����D�Hw<iZ���s�b�&�A�5��������Aj���n'�G�{q G��/L�����"���JC5���;'�g��=�\~q$ۜx,gbɟ��~�1��:N/��T6��Z�W�<��\�,�6۲O/�"��\K�������F�|wE)�������Ev4aD�\�����:\d�2L� �mH���o4B�֕�Ub��g]@T^��j�$4�J����H �����J}σ(��ܖ��.��>�HQ4ε�~��q�,��$��$Vv�(PO-�.�Q��5S ��^�.�B�3�t�N�|�X�D`���L�&@�8ϷW������[�c��"(x����<|[5k-����� (Ye�GJT�B�~,�ݔ怪��nN:�k���������Vi������_	h��.=f�W�����5��ƙ��h�<d�k����V�I�}����?>������(R�Uؓs�s����kNX�r�k��Q��慣猷흣��j>�r�I�d���֧`��/��ļg͕�,cyv�EH���l������`�Q�K'-c{����h���:�abOvݶx��uX��@w��a�f�(P5[J I�=�B��J#��O#����G�z�2��5{aocxv���qk�>J3��=T��y��_�t�
�������bS^���H�}i�uKN�_�q}�$��&��[����WG�E�v��K���pV���l/`'�X���W!dR�^k[�ԜR5@���h{�z�9Y�(�(������Z���S�-��Cv,s]xͪ�)}Ȇ��e<1-�ׁ��\O�H�ȉG�%1@{�ێU�r5W��v6+tҘ����6K+p��o��id�/q�DG� ��Ȯkޣ�qY7����4���"�܏&�ĝB��[��DG*��KY��6A������3���O�?��0�ȑu!/о��I_�Ŵ �@k�M�G�Jc��F�;F�(_��oF�$�߼y���`�)����m�y��+��i�X��_�Fǋ�W�r��{�T���JI�O���2�:)��
oq�|�9H,I���Cz�l�h����u��jV��k6^H��1�-d�{�h{����M�af�Wn{��or�*�v�����G�lxo���*�?{p�	�^%��$-"�j(�ɂt�o�����^;�ꤰ,n.���Ъ���#�&M���y{cu�s�`���X�٣\`���� ��<����ɶ�0�]�Pj.�Y�Uo �k}�v��:�DT�2c���r��s�	`�]ʤo�����U8�M�����1T�a��~'�8I�Q7����j�����V:��i������kP��H�=��Uy��	�d�����1<L����iݲ�y$�g����ۼg@I_��5�������:��������2K�	ݚ��j�L���K���������kV/�슾־���tے��Y1��BYP�E�^Z�Ml����)�WШƩL�vp�G'(I�IZ��T�1|
��l�W�$I&��,
�K�@�|),�@=��-��i�Z�F(��C{A��r����zC��A��Nl����p�~q���\?N�~��d��NO�1�,a�ữ��``�K8T�}f�i�E�4���V9�Ls��e����Sm%ɶ?xE_�7�6S_%��P��p��?.]FP�VV�l�O��q��L*Pq�;�b���n�Ҿ�-ڄ�d�A ���<������qo��Z	0h�w0#�ʣ^���Ңُ�㹯xw4��SzP0��f��ƀ������*.޼�l�`�= ��<�u[�|]�i�����!)��{mr�y�+@푎��:V��,wX�J���M�3V&Y��;�ye�)�y�!~q��Gy�˭���d.7� ��F��s�~�1s���>���� de/linux/ptr_ring.h \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/inetdevice.h \
    $(wildcard include/config/INET) \
  include/linux/ip.h \
  include/uapi/linux/ip.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/media/videobuf-core.h \
  include/media/videobuf-vmalloc.h \
  drivers/media/pci/cx18/cx18-queue.h \
  drivers/media/pci/cx18/cx18-streams.h \
  drivers/media/pci/cx18/cx18-fileops.h \
  drivers/media/pci/cx18/cx18-alsa.h \
  drivers/media/pci/cx18/cx18-alsa-pcm.h \

drivers/media/pci/cx18/cx18-alsa-pcm.o: $(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o)

$(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   �����\�)M_1~+j:p�Ċ��uJ�q%݉U �`�M�Uj��F�UY�#�41�q�u���o�Ň�a���I�9:���֧Of��P>0�<̢�j�'S�>�7%��䁶x(��b%�}���W�p!�!H�Eb��܉L*�.���S֮�Q^��a���&+�*�@{S���hP�{|�Ye��|&�WF\����ϫ�<b$�y���cxl>�<�ȅ�|�^%ӕ~iO1Cx՞����_Ɉ�Ǻ>���׺_�ڃG�g]���rw�����2\f�ԥ���Ls�?*�',�m(KL���5d:��E�n�����5��	F�+G=t0y����jT�W��d���]�*=wqL*Y�[/mN�|Ě[���!aXd<�ִPr�T�oذ�����|?���0[o=Y�K6�>��}�4n�9�9ڭ�����v����͕�������1+%�]�Μ�(�Pu_7�9�ET4�G���|�A���` ���
B����9>�Ū�CM�O�$(9	�>7��9��[��$1�P�<�Q-�{���2TO�W�7�Z�3�*�3'�@�TZH��wFI����tx�s�t��+J������+��6抎^�-[��c]������@;�a�ljg��>�f�?]���=����h�������
��h~�[BD"�i��uzvb�X>d�B��Z)�mT�b�\�')��X�e'�J{��U��I�%�SZ�����M�e�L��_��s�q��;Ij��2�$����eRQ+��_Z��� #�h�p�����L��8?3�o�����`U@����!����Ho�[�n������F�{RWcrsؔ��(�Z�a�z�#�w�2�8���zeB�� : ��i놓9�")�ɤE�*��<���֮���a��4�^��x�.�9D�{3k���������2�K8�7F�t����1M��T�+�8��oǒ�#�(r:��fLvHЛ&G�gg�=����6�&zn�%+�TiE��ɤQ�L�}�%�@ |l�z�^�FN�0l�"T�?��p	J�G7�3�Dc�p�`?T�w��繪��`�7�m��|,����^��%��<��:N?��sޠO�O+Ȭ52��ͿH�yE�Zj�{4ѭJ����W�-�$b��3ăK���3�a� ��GD7V�C� �ex�+��Qն���V\�K��O.0�r��=�74)�(ٗ�+��#prV^0gN��gd�$_A�N�	�08&�Bk3 ��[U�(>"��dީ,bb��4�f����KkY�q���̛^��VÁz���xP�����<;a�k���;+�	HM�����֑i���X�4��}�����7��d�x�h�5ټHK���د���2� O(�ߢ�(xHi �岦k��N��\7c9[UU�%�/���<��~p�	�\ɋ/�ϙ&q["���Ro�{wkCHShy�� �eYDn��!�{Uk���I��%�~8��)#�$6z�,k�l�
i$�U��e?�Q[��"5��saߏ��&*I��<�k'�J*>���:bB>a�)���\�'�W��~z��NAjf@2� �S���Q^��Ǜj�A��n��V��mj�Y#�;_Ͻ3��>6K�}�=mkk.�j�{<ܲ��7�������Fm��*�O.Q����T�VegP�b���f�Z,EFN����e����瑤�Z�u��y5�6vn+���͓���^��;�їAJ?Q�B	����g8}!���_�>�f���R����s2�PBX{��W��p4�,�A�8�^!*��(����8�z�%5�����P���6��hA�p�9�*�r�q�a+y&a�юF�/�6bw��J6�mL�GL�O�b�s?�3�9אH���G��P01�'�'0J�d�������R��\�%���G�M�����ܠfNZW�a�+nd]��ZY�4�i�e 8H��f�NՋ����J��f�z��_6�~,�t���F?����%��E��SI�5�R�Ŋ���\�E����0�b(S����z|�*�$)��)}�h��\d��� �ߩ�q�Q���#}g���PGGvG9��S��vU��8���}�+k�)��PŦ�W�ؠ�|�V�n<����[�U�2���������\ݽ��Nt��(j*�L��M8�$�����bI}B�7'&F�Z��W��;�
5J�G���3��?�ɇ����Aa�Z�`��ES܁2{ߧ(Y�ݙ�1Ρ�<�
B��SԟA���[�J۩�h��hGB
�\���B��7�"�T�&��A���Ȋ����X���Β$��lbY3ޥ�	��5�5��^.�'e%i+�� S�夫��:Y�}ۖL=z��(+����5����p>�����,J� ct�x]s��X����j���nET���b���ʢ]E�X�D��aR�Z�-���4��ۣ���̭#�� �e�ը�gu��u���Vx�.���@��q�Ʃ��S��Q�ۭ���s����=Vk�e��g̓'O���?5Ĳ��^c�9�����3<&�S���y %X��%1�RFļC���ŷ�MN������	%M�D�ć��1��`�T��Gb~�$@�V�Z��_:��;���F>&�U�Ccę��"'�¯���Y��!��OͶ^
���B��	�����H����qqU|���q!����iU��k���F#}eLn��$q�9�;�oz*�@S�|w.a7�|�<�ƀ�-�đ�E��'�S^��^=!�f������>*�ajѪպ��О�y԰Њ��{:W��z߂*��-"7���|���ό����RE;ބ`֍��4z�m���g\~9	��
�%�G��=hQE����n��^�����;G0�3X�I��L�;r_��;4Z�����I������pr�n�l,�y�?o|�i,��� h�md�����:�YC��O�L=k'�}k0b�=x@u$��^O!E4C̛��Vg 	�Y,���I�]�0�
ݶ�4F�jƦ��d�G�6��Xg;����	(�x@�i��y�:�C-�����0�E�K�ı����Ār
ϔ!��B	{��Jf>4�P_j�Ư���魮�(S	�u(�Y����EX�lH��9��H�%SEa��nZ ���0�wF����Z�7ٜ���y ̷�Y���F&��=�g��� ��.��G�O�S�� �ˠ�e��Ƞ��Vz�}%�����ԧ:�����*y��:S�D�z��X��E�9m9�,%�X)޹�@�
�_�;�AK���K�-�U�ᝫ�|3�%ϸ�;gjAc��c�������,=�
h�p�l�G+�C�As�iD�x��(��&�hY �~5�"]�w�ܹ��׵ԁ�Xާch�g����&�y��kܰZ��>��*�DNZ��d��q=Ɩ�yK��S��$�Rd�A��n�c�{=2���:F*ţ�O���,w�
QЛ�Г�l�,��&;��21I�v��֭�#]�%��OAi)1�\�Fǘ�T��ӿ0��Z�4����K&�@"AnFZ9�@�<�#��EB�w���jZ?-�{�4��m�F�C����}|�b~
^���u"$<����{^@���LZ�%�.+�T�C�>�1�d��H"ؼx_��;_5���4�����2̵��O&u�a���G�)5������)�3RAM�OEE��I
��)��v�����M.�|�q�F%[�AS9_�6��.�}���\��:�&��X�F�S�ߖH N]�D�<_�p�O{3�lr�K�&��UOUf���d����	��	}JZ���E���{��_��}*l��)V�L.�Y?�LC[z��||��W�ް~�^���e��$xe=��<|�A�N��_����x�N�(n@�4��!�u��h2��4jR.��*(��Oc$�ޘ<ڒ��i!Q��b��m}�
�@#���z����{B�7B��嬨=g�"l�❈�.֝)]�}V�%�ЧxFA�C���de~�a��'�'�;��<z��ed���0D���?�Õr�׼h�	�Ҫh��>���v_;�X!�bIKH�^8|/ �{��K<r��v�WP�j��B� ���4���>��y��O�e�%����!��`��r�]`X�L�[�2֜_W<qY	��bk�&��F�cSPO.�	&���a���V���o�F}�`����+p���"�֮Ч��R�I!�
MJ��Nu �ް(p����Ѽ���o,�q��%{����@�ׇ)�ko���� �s`�\Ԣ2�oz�$&�0vRo���ʝ�� AI&㍸�@V�C?����T�y��^�pbCR�$�[f@\�gz-bG��Kۧ�+�2�T�H�b��KI����4�����WJM�u���$q�g#N(6��}���Ÿ*s�B�p��:�d���h����!�=)?�t�����m�ud	�Z�%|*�C����Ư.�����-�D��Łs�i�mM��bi��ڽ��3�"��/���%����.����Sy8
��0z�8�B�=r��ڄ |��K��eM��C��C9���K^�OVD�Y��5Bb�=͐�/���y�7�)aU3T�@�,UXXg�d2.����*�K�����A��C�8�2�*��Tp��R޻Re=��CK��P��o�Q'�l������5
�33���ÙI���
���h#E�'/���
q����DL�J"���@c��ε�pv�	��)U���������ZDk�.�U1!�I�([N�ܻ�"�N|��G�nSu�Dn�n�V1��s���2��4�����(�{����嘸�4�m=�2�L���u��Y5��D�#A�n�]�[?�ul� m��,�Rlmȵ�,�3J��|
���,z/G��;V�6�=l�c�����U� �Z�3�n&sE)���֭UD�Gߔ��?RR�(���z �H<bj�5o+��Yj�7B8�*�]�%.���9����CХ�<E��q�B"�x�Oo���L�0�!����|%���.u���Y�*��.���[�
.����Y()p���I���~��PQ�[�_�cMU�9�9I�o,������Q彿Sƭ�c!{�	���M���N��3_ٍ6���zp3�ª����+�H�Qh�G����,�5�L���E��)F�ok����%�J���:Ʋ{����Bih�&x��N$�St)�t���ɓ]�G��Ӑ0��t�������ܥt�{�)�L���k�(�C��D��zz��J^���k�t�2Ϝ��]��#xLv8�e����Tie2���5�Yˀp��$wUI�&�zX���筘���.�Pf��m�个�>i|E)�������Om�ۂ�j��x�2y#����I�i�`��q=�m���e���@�U)����:Rv�VԦ��i;��Bđ��.��:���Ecc�L��v�T-�Db2�c�G_�M'[&���"��c"\w�bP��R�����q['�\,>�oq+:�Ϭ���]��������l��$���V���~DE���;�v5b��Rj���;c`[�������r�d\��і�資�/t��ybnי��Z)F���UN�A8b#�<i�;��q��%&�ʋ�h�cav-!��M��1/�HAc��nB�)�F�8�����i��>ٵS"�R$��L��U���FkY�\�!ҏJǂ���U֬��٬J�2ߓ"�"�?�:;Mm�R��Ջ�b�}�Qp}������c�UD%��S8�|��׊��}�r[IV��V�3L��C���9T�F�:*��9W�ɞ�lIK%�!d�m)8ǲ�Q���ʯ�Si|�6\�!�ʆi���u�W�$�!	��2`\�Q��O���d'�z.8�H�=7��%�@�}����l2��� ���W���[�[���pd�[`�#�p�!~���h���Na�>8=\��`G�u�%�DWӤ���G�KJy�)����{�Vœ#N&֦�[����<E'耉��|��0�]?6G�w`��B�U��0���`I�`�Zm�� ���M�������¯���і��7���N �y�V�]��J#k9�Ǡ}�W��#��Q�4&�*���`�e|f'Ixt�}�f^�<�F�v�p�N4���a������E��wU]&��׾9����9��V�v�1\��=n�*��?���q��t�p�D��rkzP1����!W�/�k"��J��~���>�M��9pf��I�'��j��ȡuw�*o	�ç���'AA��˸�G�	/"ۃ9�*D�8H�UN��fC1�ɿ�M1�5�Af�Vw��̍���6����hb�X�|5<΃Q�����`>MD �?D�Pig���r�.�q~��I���/��9ras"�2�W���	�x�C$0��P$J�+�3S�Y�V���A��J1��ï엀�Drdĝ 3�}f
H�:e��%������.�ni �����BX������h�sī�ثɉm��|��?�� Ra��ۡz�ŋ����dV�6st��[�[q�II�5<��Los,�<�T!�H�0Gz��ς�"SI�M��Z�'�~��>7�"��F�a�7��?ؘ����`�Z��?؂\	����PQ-֗�%ޚ�%�|O�NLk��`��^�D���eOw�"%qC8��z�9��1��[�ם$��y�D��>=�|^�F�� �n~B�cꢦ���@�M�Ҥ}lA�0g>��^��K�I�rO������\�jQ)����bX<��w��Q�G�\�9"u�6PSC?�����Xq��n|���(�y�¡J��l��/��{RI��TKVyӕiX
�ރ:��=p���N��۪C	\
��@[�ߟ�oʥ�1ٻ�O:I�Y"{2���m)����r��,�*������Qm40`g�$�#�vq��%�|����)5�}����d��T^�8$�kX�Ĳ?�vKI�rǺs4�Us��n<l���}�q��F���>�R�#��0'�L��I����sXh�Ʃ��ND��C�;��2o�����0�]�ՒI;7�
ό�GQy�F�Yd�y�[k"��hg��F�ڲ�t�Fɣ%M���f*�-�3���Cw	��-6����/y �������7m��q�!�)x�uT�����]�t�|D��6Y��3���b�z�|h�P��:�X�Wg+٭2�"�p��kR:�RW�2/�M�E.�OۃVr3�<����l���7�{�����Pj��9�&<3�r[<Yz_�r��_mS/��	m���$*.Q�ë��X�M�����lB�j�C{>������C�r�x���1Ra����v$ =�S�!h�k��v����j����!�&	D��k��C�MٟEa���kVDx���θ��8�杕��8k2���P~V��/���a�N��k���EN��ah������-��, M3�#�oXN2�ʇA���޴;��G>� ���%..9WaQ��80�H��8/t�S���Z��nR3��E�C���8W�"̄��q��g?�����'�0��)��J ?q�]{!�9�o������؏|���K�P��ƌX��؆_��+��c���[�����a�D���}������0-Y��l3|��ԟ���e�qU�{
�өQ-A����Z���E�1f�ؕ5��̓R�>D��epş��7�t5hS����c��$J	�����ج�ߪG`,���N[�?ze�M�D)N(��k�aq���1/�!^Dm-�)=�ejUx������!��Y�%�vf��=x}uL�5J�m�5�y��� ��.�Wy0}�`aͲ(ZQ�F![�t�6�u��"WW�/�#F��?iO����p��\�I�/>K��4��& *����L�']r�Yf|�j&V��h�;��g�i��(L"*� ���J�\��rb���aW�9��V�����B�H�*A�tPgXM�����d��� 0OF`V`�@��r���0$�߼7����p�j�<7�w�ޫo��|#`3o����^�F��-Sܘzи�s�8�P&�K�"�|E��gp��	��m�u�-TD�O��$��=�Œ*��I�-�(�ga	$!�W��yǊh�cg�x��k`2��>��'�M+��)=��*n"�>�_��]DJ8xe�88:�jn~\�T�� s�����Pa�jr�W1o]^�'�����k3cחU��k<:�d�̑M�j��0}��<���g��ٝ6V����Ĕ�T6�%��bb�XA-���c;��<��|��?�U5��]����t[WK#t��9S����2�t(�}D��� �V��ǝ�U:ı$��U#���ʄ @Cɬ�]^qt�b�~�}��LqWXa>R��Y���kBe�澇j���>�c�ϣ�c��Ğ�<Z�TI�ň�A�Bt�"w���W���D�f��x�j�37�*u��]a%�%(QmR��Q$���JsF8x�ӡR���U�����h� �2P��\$L�^7��p�;��I�.�R���j�_����w؋%�;c�sY�x2m���!� ���]p�Ĳg�b��#�XH������S�U!�c��A�H;�~�sU@[�>����g 8��
 Ye���ZU1�g$�"6=B�ޤJ�u+ɇ-N��aO���ѣ~�����T�+J��eRCK�Jқe�@�W��d��H�U����|�c֖�=��}����lw�l� _s��^��?j��<@L�7e~d�|!�m�a���űf��Tx�cȟ�B����	�gtc�&�G����=�I'�I9�D�M�q����̽4�a��Mc�my'��"�%yK�ظI: �2J9�?�D��)���g;$2��H��ޮOw[U]�hD����i�Y�0���IjǊ��u��K�&�"��u�:�63n�44�u���ϚS���ci�K�{�<����y��O���*��(ܭ��'�8�����C���.����F/�3:B��!�1!�� �FfTt��75Rd�?�(�G

�����A�����u���nM!i�y��~CO���"|�I�D���.E�cF�[�c�|.S�:�b��PB ��ÞwI5o��]O���