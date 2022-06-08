I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
    $(wildcard include/config/ACPI) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/PM_SLEEP) \
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
    $(wildcard include/config/MEMORY_HOTREMOVE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
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
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
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
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/resource_ext.h \
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
  arch�$9_q-�rz������$X�zb!��^�F���Q\�y�n80��Â�()bڬ$~^����y��\@W �27��Q�j���;&�J�x&:̌͋�����q71x`�h���UKp;ޤ�Igh d��c������2���:\K��Y��;�����9U��Q�eIri�QP�EJxj�Z��J+�^�a���G�AۡU`�R!�d4��nw�U���%��?�c����jx�kSnߐ�d�ū=���SMs�D����V|k��O6uʼ��0����O�NZ������N�q� \����sÅ�ӧ�E�rD�~ۤ�ޛ����+�Y��H����^���8���� ��U��`���*��M�!��;����vi�,�Ĝw��9{ݒյJEGp�
wLK;�eҖh9C�ޣ,�ewP̕�֍p��+��;��)z�) �T;=uT��iv:��(p�q�$産�/��/��I��o?� ��*�`X/�]����7��5���\���Au���X�q9�[�ch)}��O��0�
Z&�f0i��Z|�;�Pj���������\�	�b����5D}���r�����ܐH�f���ؚp�	u�$�,	z"Z�;�Ct�*wD�3^0��\J;�GW��5�u�~J��S��5���c�{l����ξ�m���?��ƃ,[�_��1�D����O�J�?z ��U�;0\���Z>�C�N����:ڰ�tm0`�)�Np9+awk�a�Y3~N���e8�I�[S�+^��C@��|����eb�'�������9��|���z�.j��>�L 3Y��Nl���a�� ?�u	�![��1��V�T-��|�lyw4M��$�6�_D�&�ʱ��p�W�Ko���_������";���-J ��;�:n5�k��~�G�尞>ం�����͹C�m�F�g���Է�o����t�Yj��<�J_sB)S̀���:����+�W�a�P{���m�����w&͒�̈���Z��u��-��3�G,�=VZ�e΁I�ϛ�x�'����A��!�����:<K$��q,W�� G�������.����#���njO�y$b.��K^`�9�=1r�_W�����T�lb���?��@՘��\�����ǩl�馐0�F�I�/mr�"��;4���l3;z`��JɣÊ��Ή���l��l�oS��������O�����U�aܢ����C�JL�_�M4�ߌ�qJ�`���ӕ�Y����_
)���S��������籭�k섿9�$V���f�����/9�+ɒ���p���� �mI�{�ߟ*C�JKY�K�X���� ��X�Wz�lq���:eG��.�w��>�sE 8(��聹G�K���t�>ǌ8x9u�F��xZ��,��@k�G'W<��X�G��^KGYO���;n�8���N��29��ޣt��Qᇋ �\M���EQP�6�P�K�!Iú"&�1k�v���H�v"9Zy�>�>���w�N�e0U��D�^���]��� �a*����hC�{�`����)�W�L�D�c�t���0l��,ZoB��jgzÕ׫z+.<��@#rxD_;�6�`e�P�g%��.3m�`�d�����2��8��	�B��b�x�Ii3���]]�(�s�=(z>+#��WPFH�z�ޟ�rV<�خ2�uq0��~ON���+㶼z�P�/|Z����h����Q�ˁA�lB�}H\Z�ˋP�?gN,{��F�-	-���1�L ~Xh �aO)FW{B2y���&e�.SS�j�UX���6l�:���#.�%�xf7�h��>$��z����c5�$	��*�����f|�=<�e��ؔ��N�6��oo���z�oD�&�AO+���	H�����\Z�x�Z;pW��"�+ʮ;���U������٪�*�9n�d�nU0\.#��t�=A7V!�&K��KM�(d2Z��ބO�y�P#Y�dm(��S�,��;��r�4[:|�)m{`(t�Y�]r��}8V
&�_��Lv�����Bl9-b_�zY��^-ie\GL7��ٟ*,�!���c�Չt����>CuSm��Wm���g��A���u#3=j�f��(k������(G����h5��f�R�d�Y�XR�[�T	nO�����*I�����B��4���F1l��C�g��sRO��U��W�;/�@��
Jy;�2�߄��/����a8lj���:�Â����j 0�����rFYܯ��ʝ���<1SȱF�<��"�Y�j� ��V9�aH	TY�x� �&wW��E��Sy��ж� �(�>F�]O��p�}�[g�ɹ3�t1��n�֍<�D�>��Ρa�@l�NӨ@Dc��������4ؤ�
NaY-7@Z(���s��H�y�%�52����0�z�G�c#�{خy��[���Y����<m���k��~�������_���rʶ���I�k�~n�~�S�����Ȏ�i3�튭��(7t�4�T��Dݠ"Rè4C�w�-����om=Α���ƙ���igaMgz�u>UE��RqH�yP]k�D��楉���7�;,3����ڔJ�%�+`��s��3��Xtn��}_s�;�'�]�%�H> ӟ�D�iW�`:}XG���L�M�/:�|�e5
��C#�-�ݛFo�FqDDi���c\$h�1���w~U"��A��5���r�E����$һ]#�e������Gޝ�����Ҟ��+ծ��-zΐ����aJD�HX�&�(�	��Ӵ�h!�[���0�L����x09�1��':�3��K��û�t$Æ��\����W���%��k��]-�eA�C/�g�"v��vZ�s��O
iH-��]����U�ۄ��V��CT �Hr:�"a��)j�v����[�=�q��JH�q�!�:��RkDda����C��O'�^M��jð��C�|%���A��R}����7��� h�2#,�����㘻��,�r�Y��������gf9QG�9��.R�c尫8�փ�=�k�]�@)���S"1��F��	]�
��y���"U��D���H�8����AƲ}$��%�%��_Q @�;3�p�.�=��9����oY�F���O�b����෮�w�磑����>d_����b���]}PZa ��ۧ�3FY��C�:�]k�*���C�������:y�IQsO�m	��(El�n.,߂P7K���r,�{i��t�!CMSS�Q��y]ꀡ|��@g��$�9n��+����������x�
������U�8+?�V�eI�5E_�]����+)�L�Ώ�/�$(k���b����Ŷ)á��qҨ?ٝ�����3��-q�2I�>��3r3��
#8�$�k�Z@��I�/��	1�G+���x?�C��S��Y&qV�S6q�e�%C�n�Z{:���'����"��Iʰ_l���&�>	x^*�7i�
T¸ೝ���jht��G��.��9�x�H�w�*��\A����N-<o0`+3��ŗ�!.�}U�^jtJB�I[�1����IA":$���P4�Yx�2�L�o]iI�t8p��w�+�$_'83�j����pV=����J��\��he���8	w��f'��;�姁���>�r�|DO�aE#�H���vAo�G�u��8�aڤ����c8�E{5p�J�X ��"��[��\��9�|�ظ'�-r��dR����0��ʏ0�˜;�r9��/,Jx��AԹ�R��Wp�α{�q�c��
���x�~䶬wڶ\��A
)��V�*Jv��V��?l�,|i]��n5�D��O�p�g"��� �d��T��8;�)e�Mj���R��U:�)��X�5�I�Sf��PGJ7�scD�f��́R�3�5F_��dU�[%¨*��(�����
�w�dX`��8�W8}Oq̃������^O����5m��4+F��-ˆ%���eo�\��*�,�D����Z���#����j�V9� �=�P��C��B�>}|��k�9�RH]OT)���B�[s��u3E��q\p������unu̷����"�氎c�D�tV�;G�����W[��A��
�tNd�FXL[�Uh�����b�,�e\��E�����3&����l��;�I�9�[�Z,�JB`�B���%��VQ���{�������G�Mtx̑7�-\����d�*ˎT��>8\�{B�c���K�W(bVڇGJ��U���ٰ�6�-S��l_J�ۢ�f��H�աLFrq�"�9���@vƨ��"[�Ί����#7(O��v<U#�^m�2-�'���(X�_���f��9�2��ظ!qy���Dc6>X�!o��}� T�� q3�(kW6wk��@|�8]�s���%���k�Gste�^�`�X�Mtɿ��HM���1�kI�g��9��$�Sb�O�s�A�m.!9�
e�^ť��
9f���<��L��k����P���� 2��8�I*��Y���V���T��a�J�����q�m�r�S��tƽl��������D55C��^/$���&)��#	��PMyÎ�4�Ug�Y����&s�;�]�ϵ|�[rG�׿6GtaD��kl�\ n�?:Y��B��ee�{��h.����9_��]��'�Ų�IJ���W�$]��pSO<��q����{+� �:x�I�foZ�*�����y^���`��e�?��{���Q�:���fMb��cr�(i-��>�X��j�Je ��|�GW&��{i��}sk��a=b�����O��y�`oGl�A����~.,�P�Y�F����Ȟ4>�1����[�� �|��f�Ɍ���<\˪����aO���*ì��8([�Umg�j���^�������gO��������w�%��p���z9?F��$���V�'<��ƞ��Ŵ��r�X�
V&�HC�(��ELK4|Ď2�4�*����s8�������,9Q�-+1���ة���:��#�L���N�}\�gd� :����KŒj�Xs7ޞ�J�}�_;��B)O[w�"8���\s_0�&d9�C��fiJrO7b)K�����el,��G�y���({[�څ������5����+c{��-	?�29��g�����7��Bb<쉞��4�lv����Nb����q��k�=n� �&<��5��3�,.Nxe�n�6��Ъpm~i�.�T����x���E� �����N=A�P�e����_�W\�~S�nԁ(ۤ���(�75RiW�8g�����e>s'BG\a���lۀ"��`V8�D�`��W]i�:w���=��k2��̺��>���W@5���Z���t��JĔ���ۛn's�Fo�*���g?���lI�>�Q������<+���ן	C�,��92��	_�V�1���`�r<�T�x�͆��+H$W�= ޖP2�_� e��Z��*�ab���.��D7����<��ιԎ^~gM)j�BzJ��I�l�~n���^����6�h~��oO�G��b�J6#
�'�c݃�l�� �:>X�������=�)x�� �r��mCfl>T����	fM�g�Q��J��13|"��I�@4�B�������KcBc��ۓ��|�FZ�/W���( �8V��\^l��q]k��#n����R�Kq��C����ڭ�${>��h��^,�$�ph�+�,���(�y�=v�є�'�cn�1J0�Sb����c�U(˒����=(B�&�őu[�҆�q��)��_�	Ws_,'�B�RN��ӥ�f���A�[i��'�̦�|_M�W*7�&{'�6��n˚�	X���ޒ I�y�@���{����s9HijQ�4�/�/��׿����w.��߷$v�����C��
����6Q�tf���KȲn�Q��K��⸶Q��0P�h2�]B�=a�>��x�ُ洜t�t�Q'U�ŧ{Z˩t�eVo߱:Pv��%B��Þ���G����\���%>:���. Рhgi!��=�շ�3X2�W��Z��)-_vY˭"t+�~��=��l���4�o$�{��o��O�rU�qT����G�����,EՌd4��J����{W� ��L�������o����r:����Eދl�z��j��դ�

���}�/��^��P�d��S�e`}�sk�u�ᇽE7��Nh�K�ҹۖ?����k���J�Q���4;�C�����uR.�Yyc��$�>�#paGM"����/A~H�oz�Z /�Op�T�-ˌ+�'��=�񴸸��˼&���Z�58H�\�i�)7^֞i]l�(��Q�1:(����jGɛ�����ԛ�(����l�����I\��ѿ[��V'�D����yh��P_@8�*T��oF蒄���Z�dW�{e�[�%�q	!Z��t�&��r	&T9jKڝ\��Q�d|�k=�v��-L�p�l��EL�鮣|�t��@�?�&po�y�y���:�jt8Jj�?��:�-�(�'�t�'W��U �{U��k�=��0>ŵ�t2 �<��� ��V�x`$C�*B���y^��j�{���g�lAU��:�=U�~8E�����!߉�O��>�"��Kw��?1��̰M�X�Al}r3V�lm�;�{����0^��NDb�~֏l�2���5w}�w���aF[<8o�Cqw��#b���|�5��W�q�P�0F?>��@�A_�>���*- ��g�TC�;��r�]�������M6�x�tw�7;��X��f����w����\[S���~	�$5� pI
i��3�,s��J���SHgʱ�L	1V��П%]�ʜ$�@��W�f'�Ed,Y�0�r��%'Y����4���Cz�ƃ��0��X@�RK���~�lF$e�M�C==Mo�78�͓F�|��'��*�r;m hvM�-���S��z�%�@��0�W
���=pk�1|�J��]��"���ȅ�c������>��O�r�U���o�~�m�ɒj�n�F΀�-�G}aqݒ}�$�Yh�Fi�:�J�9_,�<��Yͮ%�~�'C[s�7?�V���O`��k�΍)@˺��_�>;m؉�W�U}n��h{��o�Z�&��5���b�z���#���Sj��(Z0_����3�b��<�N��D��2����w ���u7Tp@mRΒ��c���L�T��5l�^3<8�To�{j�\�<�g�4 ?V���\s}G/�\�^_�Y���ބ�K��6�L�� Pt���uGs��=�`����&�xUtͳ�P�'��~�#��o�N�?�H"s<�l�Pt���Ve�x�:����\H�J�G�U�5e0>:Q�T�{Ё�݈X�5�j4E_�h��&���!�X��'��T��#���b�<���tw�V�Hz�OK`��%:=:D����J��L��%���L�w�S�&(A58�Sή��q�wu�#޶od���w�0�a��ڔj�u46��^UZ�q�(��꙱N+���-�v\��|hZ�4���-�.�	!G�v�P���v��9������jd��~�Fܵ��Z��ԭ��._W�3B���Le>R��J�f��N#�Y�i�Aj��Lm�N���V���s�L������H:hRe�-�0��2""�.�lL��ŐZg2��dI0	h�i�D�n O+g g\�K^���3�) ��!/���#~��� �=a~@C,�h�0[xY�S���9��f��I���A�e"��R�?|?ޱ�чE�'L�{r���=i�_R[��e�����*�X]�g�m��U���l�h$L�_xL�+��/>�	�8�j;w?3Z�l��-xEXҬ����������WyU�"�5�+�yKv�����I]W}#
�Vk�g$ߓ��� ��(����Q��-p�6�p@�� �����&;�g��S�Y0�Z�{{��Q��C�o��EtIX,bO��|��]��{P[3"
���- ��Ҧ����"^Q
r�< ���Gӂ�So�~��$��&��{��܌h��+K�1F����S�u<�5���[��1ZU�#���?"5�a�JϷ	p��q��d	fa���?��7�\�VN�y��Z�;"W�H����t�f=�#�9�"������F��-gcZX���	����lbĮ8��--� �%Ύ9Ȯ��|Ԇ1�m�z�MKȶR X�s��`|9�e�c	'f�w�iϋ�)����b��z� ��h�H0$���`ƌ����J[;%G�ZC�Xo���y��'R5Z�:�J5��[���F��O�M�=��ĹF�D��Ҍ�2}wĉ ��E�!�����4:y�0 � u���� c���SBi��*�d�_V���Nl���'��_;��)bRz`W�ͪ�}�	1�*�v-�EN$���È��&��7㎀��j�P�3��q��wwD$R��$�?Q��̴;�^���%��.l��Ja���LF}�΀6�A� u�ח���^=���?QgْL�7�j�9�F�n�50�k��%�R̬���\[�/'��:`Ƶї.���*��A�M���o*Ys�Jߒ�j{�J\���b����Td�`W���w�f���S�_�"��P�4�`��u �Y��J�i� ���N�6p7"��"x���g���p���kǴ�O-�����Y�3�@<�M���3�A���:��R/�TI�o]�����:�W8#�C�TǾ���q��eC�S�|c���У��)�~�%y�����s�ho���o�i���(��$��j�>,���X�HL.�}����Wy"�x GO���5�`��,Mi���D>Ϲ3��֦�2��;��X�zr�o�8�81��dѢ36��G�H^��K3�����t��_Te�J;&�][��˾G��F�-I�8F��0qʭ,�8�����UB�u|�C��!^m�Tޭ4�����s&������+C҃� l�����x?Ė$vJ}���A8�T6����l��D��R��Kn�p�R�V�\e"���B)��__wχ#�Mu�B�/�oԭ6���=3o��pS���*�3��̘S�@�X�O�떼hEqPr�]H���V���;Ĩn���B�*nΈ5gd}�#�O�ٹ��n�����,���:�?Ѩv���̜r}1�agC^�S$��+�l)p�S�[?�!�}<��T�Ф������2���o�3r;��7���)��yϻ��\+Y(�-,{�'@��~:	,Ɨk T�]�D@�D؞��vؑ�B63�m׿W'�2PϚ)^�8H+XᲨN��C2����گe?�Qm��,�!�$pI,�J��)͖�g�`������x*4�wXa�u>-0��H�B앥7�v<h�e2��R�b��Ԯ��k-?���KR��%Ѝ�ס�� #�����)d��y���S���	��ƈ�{������8^/Tk�_�3��/A����q2�xi���R�б��Rf���W����w^���,-`�CHk.��}�$c�5��8�[`�̸�!7u���iG�7�����[���B�q�B�G�=���k�;�uo&ڽ�=j>�'Q���L��Rhu���"��Ge&3��0N�ږ��\j�c�ʹԚ��Md�ǌ&��յ�ߒ�������O)�`��d��ʝ�$�@�c�v54U��z���;*
��{ca�
�{`IUu9֚�é���Uo�7�c���S��o�L+��Z����Ez��f��,��21e����T�5eK�Ў�;�0c�7��L}E�¶P����J:N�FQt�KR�脅��߀�ˊԵ͈��b�&�yVc��y1�W��Xo��([��C]���b�r���yp]m�(pP�3�&dվ��ݯ=�#���������)�UA_���ADL(��;�a3?�܆� s���?�Î�5Jfg��\��g��>�c�ޡ0��
rFp-��W�&ϔ�G>,=�cq�����������4bK��3!$�
�w�F�v�P[Cx>��ң_np+W�_0��~��<l��;h�r(-�R����hZ0g0su���Z��:W����>�b�M�C�"��C�����"��ܫ{m1��E�)שa�m�%� Q��_+ ���0`�`s-�+E��r��u��^ɲS�,����i��C%�p.��@\@���X���
�P�o�r� ��A��+4`�\H��
k����͛�$׍���37W���7�+pEɏ}�����E���6{�Rr&
}[�l����=�+4��E�YBD��F�X�ax��q�����~#$j�5a�f�l�Rl���t��A9쥞 �S�(q�
m�j���3�.)����"���Z�>R ����g<�Ӷ�7�({�5>$�mDy�зȫ�a�o?Jɖj���=@����<ܫ�g_V7vp������M2����DT��Q���1�KE��21��1,��n��t/�a�����ް�:�Y�\����G�KzB* =0���\O���;�D|������P;.U�l���t�u�c�� !-�W(��u�����eh��SGg@�;����>V@��$��4w�e
�>����K�l��U��z�i:fJ�@XKp]f??q����s(���lO������M�����	��;�|�bߪ���o����_9�ڊ�����?�/G��Ш�ptMYb15n��.Q�1ẳ(R�!���8#��)�|���G��z /���3�����l��
�9a�e�C��O�}8]w�[%�cw��h�]���`!��-�<3L��vҽ�1@$	�c���F	�r��:7B
�
	x
�E)�ӊ��g	��ѳK#�"`���T�h�M�r��U|?�.I�솃摁�Ǳ�L�e�J�8\Y�i=y�̬|��x8n�
�o�泒v\S>�f�R �������������K�Ep�� ����?%gt�G]$�u������x"Z���R˸}ON���T�Q�y�1�N�����"��(�0�l�n����'^eT��h��|	�������׍�ְD[�,�C�*�F.��2cu`(���)�t\ae��|��G?&ė�	�B��0H*t@ȝ�h%bO�"���������Z�����Kb�"j�+B;j�u5����o��TY���2r
j��wh i8lr��ꉤg@vQ�#�ښtA�ǣȘ��Zfu�mS��_qT���oh@)<Ǖ7^��$��w.s�Í6}�zW�_���vD��)�͛b��U��«�0��@Zϒqa�nͅ�l'����W$�8�ǝ���:��J��t��ɼ׊`��L#
}�rQ�/�
�|��m@ߑ.;N>x�ZINL���M�>Z�*�p�Yqӏ o'�Z(��C�`IF���	`�iֱ���5�OhW#�IDߵ�iO�ǭ�O�z�Ӝ���9�����+�,Ѕ��d|Z2]V	��~ɩe��ϩ(����O��d��
42
�¾��<�9!���[�[�"�C6�N6ԏ�-*��:4�C|�HPe� ��s�rP�tW�RA��ς3����<A��MZv	It�:삘3H���H	�F��''�'��x����7�#3��ԫ�$;�/Cd<n�o���c2�>��e^l�Wd��yGw�~���P��U��2�hp�	��]ʀ�#��~[2Om=���w�Ä��Nu��@q�Gˡ#��yH���A�b��^��P���PW�/}a(�}��,��Nʇ�Qjh?�Z�I_:��^=:�������W�[�@vzX����֋%:J�0�ZU��˒�T|��N� /<�3e�ۏ����0�J��ǵQ>@t���׽�������Z�o��o�|�-PQ����n�i�F�N
�h���!�N�g�ٵH��=+yu�^Ӎ�JL��Њ׹��1��T�<	�����y���9ڹw�N�!v��[�lo_���Gh��wgg幗y���S���6|˸�/�����G�ov�!�y�|�J���r���r����^���M��nQ ���	�U���hU��k`b�ѭcs���8�!����jї>�!Z4{U�r�x��p(�t�/W�L�� �޷ôo(i�ܿ���Gl�ۢټ�3�}�.,;��h��1��w�e�=�r���z%���>v�Ja�1�=S,��<�+�g?�iX
o���J���So+,V����'By'��0��ު���Ý_j��ײ=��3�1�6\7�R6Ǯ
6]�WA��.��IA��qGõ��~� �����s)K
[%��¸�^@��S3f�FfV[�bc���6�R<h�Mlo3��?A��y3͍�k:"#��?���5xg�qό.*����5��p-��Dp�Y�ϗ�U���e�]r7�S	0�4Ψ9@[���oQ���3t�#���D�C=#�o]�؋W��1���<.��q�V �:�Z�v�\8Rp����溢���)p:���Xj<&��d�2U��o�ӽ�u:(�f����ֳK;5��-���L�y���m8�[D|"������O�#������)���f��	�B95�}P��m^�x�s"�\��PESzSJ���m�䤺7�.B:�l�=[��F���IK?�P9 (#A�q~'���ٍܸ|3p��\Ӧ&�L%`z(b�3(h����*��s&K�x�ET�<��~��T b���dGr���k���l��~�8h�:z!�)�M�(�Kf@ܦ
[<�v�sZ��=I�V�)��1֠�/H�Z���8��"�Xzn���_����)א�EOu¸LƠ{ݫ�� ^�Q8��|�$�NV�]��/�@��I[��Wfaw'��_D���>�-���h�@(-y/��K�gi�n��� ����h�N��^Z҇�_�k��;��_Ý%�l<�%����3^tRM:pp}�q��j���w�Fg�x��/)��j������A���"K�4��>]�1�Զn"�C����d�xHQ�b<���;���;m:slΫ�Ji�2Cv#�8��>Ɉ�PN�X����K�֦]Qq��&�<O���tO��?c�% [=�M`m3���q467A~��b57�#�,����;�iǉ!0�+!��,�M_����ߍj�u�Y�KDVUR����z��F�@
��,	G[�ǅ:MՂ�н������'���3x|���P� +��o�ӎ��w_��d|�M�
�.ަ�ň~7Օ��԰���<� �5�mf��坍n�kx(�:7 ,�Fč��� �4o��;rU�����\N��O��4�Tă��1Ժz�
1��<`����xRJy�Q���T��]�e�C�GT1�r=a���-[.~I�z������z��%r�C��^,�g3�Z-4^��.>qR�9��h�z�N䈥m� ��6f!c���e�CY��ۧ�DPt��6����~F��%]h�!ٴ�C�ѠحG�j������!K�.>�����@�,���`��<V�0�{r�/S�}��RW=,lR�r7���6rT�]HP�;�-J"��)���a�$O#c��q>�~�m�<�>��&O����˹6���H|NsYg�"_����`[Z�����+D��=cC	��i����fh���Ю����uo������KR)u��M`�d�D ��gR��O܄� �DA���|>p�MP�`�W�'�v�՚疊	cR?e���' H �UR���T%�|je-�����`߀�!�ϱ��;�u�L���P��e��N����(f��\h�d�@u;�͵��񈀮*����8�Zv��u�;����]�_��\��Y�1��ܖ~�jG8/�3���V�-#��<zk��C�W�c�jl��D��ap<Out�gu%Pw_Ht�����	�$a�쐲�0��G��,S~��7�RE4v Y�3=ȒRl�c�}Z��x�~�Ymz`�&آ���p�i�'�� e��C�5�GLtk����bP��*�BJ�˩Y3�U���^�kSh�h���?�pG|*bJ>q��J���kO:/Q��^�BI�jmb��ߟ���,3͹�`�#�,��+]6
0�ӝ$�����^b_3.+u"ƛM:K�C�ݴ��y�|��� ���mZ��S��4r֯_��<~@�b�X: ��Ä�M�~́��u;��m;����5ZU&�P�:�@�_����kgQ!�,�3��5� M��x��{F��݇O
$�1��=�b�H����>+�h߈��7!��
���㚕�CK�n9pTWB8$�m���>l|�}E>�j�WD��k�NA�K�Am
 [�- N'JE�O	�Zu�����áƃס�,R������z��/0.���ӊ��e%�7/=�Ҋw�e 7LK_�aSʇ��S�LOːǾ�YV��NG�lR�l[{-D^�9rm����#�{ t�-�ǥ���9����G}�*��݄V�D�Z��G�c�����ڷ�ÎwXA�\�P��5��!ju��0������h3��}�f/s�.��{���H�U�P�u�b|�ˣ�H�s��c}��Z���m�ݾ==}>Y���NU�N�@���f�Zl�+��v҅QB��'տJgmj}J�����h"�%�ܜ�P�	��Wh�ĕ5V[�C�X���O�K���7@Ž?�B���y�+�1���Z�BVo�{��*�P����N�a<W�i�%����h3�'$�H�Ny��և.�5=vq\>ۦ�X�����=y��A~�m0;�8��U�"���>�����}��?����a��Й�DZaexO�Zw~�mG\�c��A�v��z�K~��t�p�����7�W��Ձb�Β��<���Rr����:�mА�m[A`������h4��1-ahp^��)��S��*Hϖ#��v��1�-�dfߩ�vӛ��Ujz��c�$����q��q���pr��s='�k����Gf/���K{G����h��uݤT��z���A�?6����DV���Z���S���x8Z�m���FG&}｜9|��V:�~���\�U�i���>yHu5(JVj|�4�9Iڋp�bF�{y;1k9�aa�p�H0H�h��Z�XF�i��8ȇzl���2�����v4O�鞚�e��]�Dy3��O{��}��Lu�
C��<}d�b�x�<pH��[A*��N�/��\�SkG$I� �2�q�B���YF�,���h��P΄&Z;��Tk?O�.�h�	��ؓZw��܌%(Z����*��7��
����}���,����t�2B�om��c���f���BX�o�<(�]�w�`*|�1)FXy3'�Aӂے�6�>ɥ�d�X(!:Z��#EE��*�;�\��E�Xr�D�$=l,Լܹ�	��v��`���&���̉G���[�a���]L�N(V:��
���6q�X��&j�=�� � ��5�?�F<��oh`$��j�g&o����(ȁq���Y�4�8�d[X9ֵ���>[�� �퇯�;���0'�f��X��y w�Z� ��6�gz1����µWD�qǿ;<R2@J)�! .����-�ƍ��	�H���ZNAC�)�缢��CcUU��;m1	�aH��2��xD�^�%^�Ū58֔X��I����I��� �"A��j����2�0�k+��<.5}|��u�<L��J0�ݩgՌ�)�I#s��Sq�>��\Cs��6q��j��HW�Y@�R�D��[{F�����F��,#n>�J���&��뺧l��E��`�/p��n��!�ŝ���$0p�G��S`���ڌ_�Ց�i1e^�X��da��."�Z�2|��L�$9�g�6U��S1@���������*�N/����ws�H#��#ob���N�k�Zn/5�c��� ���h_���UA:	�<�s�L�
�g�P���|���ȣ�BŨd{����V�� P�ڗa�h�+���3'�
���HȻ��V()٦�GӸ���s�=��b�����V8֐+�_�]-!X����B��6Jǀ��Q��ĀǋׂW������ �ǰ�(��-�_LOCK_DEPTH))
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
	if (debug_lo