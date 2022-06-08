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
  arch¤$9_q-¥rz‹Âíºäâ$Xñzb!úø^ÆF»ð Q\Éyñ”n80ú¹Ã‚()bÚ¬$~^¹ª¼Ãyú\@W ¨27ó–Qí jž†¹;&ó”J¨x&:ÌŒÍ‹¤¿ˆñÈq71x`»hæøÞUKp;Þ¤°Igh d­ðcñÍý½áú2¹ÔÜ:\Kœ¡Y»¸;‡…­Úá9UÕÍQeIri“QPÇEJxjëZ•´J+¶^ïaµ¥©GôAÛ¡U`úR!¤d4­Õnw…U¡Î%™û?›c´òªïÝjxïkSnß÷dÅ«=¥©úSMs¥D¢¾‰ÉV|kåÔO6uÊ¼ÿº0áÚðËO…NZ§™áÃú‹N…q \ãÝÎësÃ…¤Ó§ÌEërDò~Û¤¡Þ›Þì£Ý+ÁY„—H´œªœ^¸Ó8“íû› þçU•«`©˜Ü*õð¼Mè¤!è„ü;‚½ƒ„vi‰,”Äœw‡×9{Ý’ÕµJEGpÑ
wLK;™eÒ–h9C…Þ£,ŽewPÌ•ËÖp°Î+¬Þ;Íå)z­) £T;=uTòiv:Ž(p–qâ$ç”£­/à/ÑÓI’ýo?÷ ¹Á*Þ`X/Ý]ßéó£Ü7š–5þ–¯\¶Š®Au»Øà©Xñq9†[œch)}˜„O¤å0¿
Z&äf0iúæZ|â;¯Pj®æø¶¤ðŒ¥Î\Á	 b†èê«5D}èá€r±æÀ³üÜH¾f‹‡€ØšpÝ	uÄ$¶,	z"Z†;ÖCt¹*wDä©3^0äÐ\J;ðGW«Ô5¦u¥~JÈúSô§5··çcÁ{lò…—ªÎ¾©m‡¥ƒ?¿üÆƒ,[æ‹_”™1“D’çþÈOëJ§?z §UÒ;0\ó™‘ãíZ>ÛCŽNƒ§‘ü:Ú°•tm0`µ) Np9+awk­aÏY3~NÀüe8¨Iç[SÙ+^§¼C@áÇ|ˆÁ…eb‘'²‡«†Æ¯·9˜|áËÎz¢.jŸ’>é¹L 3Y¼¿Nløå¶ìŒa½ ?Ôu	Ô![„‘1ÅíVèT-½Ï|ê¤lyw4MÒÑ$â6ß_DÔ&‡Ê±“‰pWØKo•èÒ_õÅýÈÿ¿";Ýã¾ý-J †á;:n5ä·k÷æ~ÃGøå°ž>à°‚ÄçàÄÍ¹Cêm”Fð¸g–‘ÿÔ·µoŽéÍtÐYjäâ<ãJ_sB)SÌ€×ë§é:Þôâé+ŠWàa÷P{¹…æmþºž›ºw&Í’»Ìˆ˜èZêùuõà-ÂÆ3°G,ÿ=VZeÎIðÏ›”x•'ÈÌ®©A“—!¦ö«¾:<K$ŸÚq,W¯¼ Gö´šœ¦–.†å¯°#Â”­njOŸy$b.˜ÂK^`ý9ú=1r¼_WàöóïÄTšlbÑáÍ?¼•@Õ˜Ÿ \·ÙÆ“æ´Ç©lâ¿é¦0ŽFýIä/mrî»"ìà;4Ž¶•l3;z`š‘JÉ£ÃŠ¨¸Î‰ Ÿlž§l¤oS™«­¢ž„ÁœOÃî¹µî¹¸UÓaÜ¢Ëâ²CòJLø_½M4ÛßŒ‡qJœ`ƒ•ãÓ•‰YëÈ©Ñ_
)ÓéˆæS­·®ÌÕ‹áÛç±­Ékì„¿9œ$VÄå„fŠ•§è´Ö/9ý+É’öÙ‘pñÐô âmIí{ªßŸ*CßJKYÑK¿Xûƒˆ‰ ÷íXWz×lqÃßÒ:eGõ¹.Æwóü>’sE 8(ç·Œè¹G‡KŽ‹tã>ÇŒ8x9uõFšì‹xZÀî,›ž@køG'W<ÞÿXŒGšþ^KGYO†³Ü;nã8ÔÌÀN†Ì29àˆÞ£tŸ‘Qá‡‹ ó\MóïÞEQPî6ºPÆKð!IÃº"&Ó1kÖvÆå„ÉHžv"9Zy—>Á>ÞÍðwäNòe0Uƒ¢D®^î¾÷Þ]Üóé Óa*š¨´ç‘hCí‹{Í`°¨µ®)ÑWƒL»Dîc·tÒû¼0l€ñ,ZoB…ìjgzÃ•×«z+.<õœ@#rxD_;‡6Ë`ežPäg%Ì€.3mŽ`µd›¢§œƒ2¦ì8›²	ÂB±“bÍx¶Ii3ÉÍß]]â(ïs·=(z>+#´æWPFHüz‰ÞŸì„rV<ŽØ®2 uq0Îõ~ONŸ¢¤+ã¶¼z¨P†/|Zƒ±¸±h¾¡¸áQ‹ËA±lB¤}H\ZóŸË‹Pî¢?gN,{¨èF¦-	-“„€1­L ~Xh üaO)FW{B2y²êí°&e….SSÛjÞUXž„Ð6lö:·ì£#.ü%xf7Íh³ä>$™¾zª¬€c5×$	»*ã”Ž“ñ¤f|§=<¿e‡øØ”ÎNý6¢‚oo±‘…z¦oDå¥&ÁAO+Ýèñ	Hü³\Z¥xÎZ;pW€Ñ"ðž+Ê®;—ø·UÇïŒÛÚÙª•*å9n€dÕnU0\.#Ïä²t’=A7V!¾&K¦³KM®(d2Z×Þ„Oœy€P#Y©dm(„ôSô,‡ï;Ššrï4[:|·)m{`(tžYû]r…Ì}8V
&ã¥_–úLvÎÒ¨§ŠBl9-b_îzY’^-ie\GL7†…ÙŸ*,’!•³Ùc‹Õ‰tþˆ§«>CuSmŠ¬Wm³ÅØg™›A„Š—u#3=jÔf¡ê(ký¾·¸Åä(G„æ›Äh5øüfRdŽYÕXRè[óšT	nOù¶–¤¼*I¯•áüB˜Õ4úúõF1l’ÕCñgšàsRO¯…U¼òWê¸;/ô@Éó
Jy;ƒ2Žß„£ô/Á‚‹“a8lj¾Ëè:ÓÃ‚™†öj 0Ñ”˜ÔãrFYÜ¯»ýÊ®ðÀ<1SÈ±F¸<™Ü"¨YæjÄ ªžV9ÐaH	TYâxÝ ø&wWªÒE¡ñSyü¯Ð¶å« Æ(ž>FÖ]O¿×pâ}¥[g”É¹3¤t1ÁÎnŒÖ<ñD‡>ÇšÎ¡aç@lœNÓ¨@Dc‡ìÊçŽÌòû4Ø¤‡
NaY-7@Z(ùôÃsº÷HÄy¢%”52¿¹µ–0òzîGŒc#ñ{Ø®y ¶[²‡ÛY±©Žì<mšÞïkãÐ~¹šÓ¶†¸_®–‰rÊ¶µâIØkñ~nÂ~ñSÿõöÆÖÈŽÉi3¦íŠ­ùï(7t”4îT¬ÆDÝ "RÃ¨4CˆwÛ-£„“ëom=Î‘ìúýÆ™êï‰igaMgz©u>UEÆ¸RqHºyP]k«Dô¡æ¥‰—Ÿ›7Ÿ;,3²À÷Ú”J¾%é+`§òžsÌÈ3µéXtn¥Ø}_s®;Ï'á]Ì%H> ÓŸ–D±iW¸`:}XG–™LîMœ/:Ç|œe5
ï¤åC#î³¢ê-”Ý›Fo„FqDDiÜá¥c\$h»1®öÔw~U"ÑÄA¿Æ5–ç³ærÎE¯ñÙ$Ò»]#¦eõîÕÅùGÞ•®ù÷äÒžšû+Õ®ýÖ-zÎ°ÛÇâaJDûHXÀ&ó(‹	ŠÈÓ´Ìh!æ[ºŠ¼0µL‰ùóx09ý1Ÿ­':·3…·K¹ˆÃ»°t$Ã†Ÿ\àÂÉ¶WŠ£š%¦ÈkÍ×]-øeAÖC/¸g¬"v¦ÝvZ«sý¡O
iH-þß]•Ž½úU£Û„ÅVCT Hr:Ž"a÷ð)j×v™ˆ›¦[Ë=q—žJH¼qµ!:¶ùRkDdaçòøãCÈëO'Â^M¾½jÃ°é…å“C¯|%ÿœïA„óR}´üŽ¬7‡çšË hì2#,åÏž€Ìã˜»’Æ,µrÐY¡·Ôìýê÷Âgf9QGÌ9—à.Rˆcå°«8ÞÖƒª=ÝkÃ]§@)¼·”S"1äüF©å	Â‚]¾
œÇyþ¬Ð"UçÂDñ€ÆüH‡8ëú åAÆ²}$Œ½%¤%ñ‘_Q @äº;3ªpë—.ú=ö‘9ÚºîïoY‘FÓØOßbõý°à·®·wœç£‘¸ú´Û>d_º·žºb©‰®]}PZa œáÛ§­3FYž³Cé’:Ë]kÙ*²«ï¬CêÅØùþÓç:yŽIQsO¶m	ûµ(Elà¦n.,ß‚P7K—žór,½{i“út—!CMSS¢Q­Öy]ê€¡|Ëð@gÖè$9n¯ò+œì¥üà„°ÙÚô¿x„
ž£¼’¢ìUÄ8+?V¥eIŸ5E_‚]£¹óˆ+)ÛLÊÎ“/˜$(k–«ÔbàÊÒèÅ¶)Ã¡ÌüqÒ¨?Ùš»³Óé3óŒ-qµ2Ië>ëú3r3Þæ
#8¢$¼kÜZ@ÅæšIˆ/°û	1¡G+€î­Óx?¾C÷•Så¶üY&qV€S6q©e´%C‹nØZ{:¦´¬'ú •ª"èàIÊ°_l„åó€&‚>	x^*º7iÜ
TÂ¸à³×óÌjhtÇØGˆñ.°–9Äx H§w¬*ÖÊ\A–Ñ™¾N-<o0`+3ÿÍÅ—Ë!.¾}UŠ^jtJBéI[¢1¿ƒä­IA":$¨ÇôP4±YxØ2å¢Lîo]iI¨t8pÁÈwï+È$_'83¢jØÀÑÂpV=€ô§ë›Jô™\Ôheä‰óâ8	wø¦f'ú£; å§ËðÈ>Îr |DOÀaE#’H»ýàvAo°GÀu°ó8ÂaÚ¤Íþ…¶c8ÆE{5pœJœX  ð"³È[˜ý\ˆŒ9£|µØ¸'Ã-r€ëdR†ïÆí0‡¯Ê0³Ëœ;Ãr9Ÿ¦/,JxçûAÔ¹ÖRþµWpŸÎ±{ÈqõcŸÊ
ØØüxÀ~ä¶¬wÚ¶\Ô×A
)÷ûVá*Jv°üV­Þ?l¢,|i]ùŒn5öD“¿Opî¾g"…¹­ òd’ï¤Týˆ8;é)eŒMjÉÕºR®U:Ä)ž™X±5ÎIÈSfý÷PGJ7äscDÂf”ÚÍR¦3Ñ5F_öÎdUÇ[%Â¨*û‰(šØÉûÙ
Øw¿dX`Âò8¬W8}OqÌƒ¦Úå—Ýú°^OÁ¥Ö5mÆÑ4+F£´-Ë†%™»ôeoÙ\¢Ñ*ß,ÏDƒµÞÞZú…½#×É×êjÂV9ª ä=ýPèÞCéàB·>}|­©k¨9¾RH]OT)ª¤õBî[s‹ƒu3EÅçq\pš³ÛûåŠñ€¢unuÌ·‚¨–à"¡æ°Žc„DŽtVº;GüžŒåW[ïúAŒé›
±tNd³FXL[µUhå÷ÍÅãb„,’e\–ûE›À™•ä3&ôôÝãlÜß;·IÎ9º[ÅZ,ÝJB`öBª Í%—ØVQ¤æ«î{²¸÷·ö†ÓG‹MtxÌ‘7á-\³¼ÌÀdú*ËŽT±º>8\è{BôŠ’c¯úKÿW(bVÚ‡GJš˜U½åÂÙ°ß6Ù-Sž“l_JÚÛ¢ÕfÆÏHâÕ¡LFrqÍ"9ŒÖ÷@vÆ¨·ž"[†ÎŠŠ¯ˆ®#7(O›§v<U#‹^mÁ2-ß'ô©º(XÍ_›êÞfšË9ì2äÑØ¸!qyÁ¦ó¬¢Dc6>XË!o€ú}“ TØÕ q3™(kW6wkÀ¬@|Ø8]îs†ƒË%ýœîk¿Gste‚^æ`ÅX·MtÉ¿ëHMÒï§ü1 kIØg½¨9ÁÍ$«SbÀOÐs‚AÄm.!9Æ
eŽ^Å¥ÐÆ
9f¨ù¹<ýèL ŒkºÔû›P¼€Ë 2øž8°I*áåYžýëVËóäT…‘añ‚Jõ²ýåùq¤mì†rõSïç£tÆ½lÿ“±ˆ…þ¥D55C”þ^/$‹•¨&)Èç#	’õPMyÃŽ…4ÆUgØY”’ÌÒ&s¾;¥]ñÏµ|×[rGŠ×¿6GtaDãØklÐ\ nƒ?:Y™ËB·Êee²{üöh.èùÖÒ9_žˆ]Š»'„Å²ñIJ˜úáWš$]Œ¼pSO<ä½qï—åŽÝ{+š ±:xúIÎfoZ•*ð¨‰ú×y^ùŽá`î¹Ïeù?©å{ÙøÜQ:™ÿñfMbûìcr©(i-ñ>™X»ÐjõJe Ÿž|ÝGW&Ô¯{i¶ž}skÑûa=bíŽ¯¨óûOÆôyË`oGlÊAìÖûõ~.,çP¯YþF–»œ÷Èž4>®1ìîÌð³[±Í ë|·¯fáÉŒëÀó<\ËªÃ»‡—aO‰ëí*Ã¬Œß8([ÖUmgòÂŽj·Ÿ¬^³Óãîù¯ŽgOéïõ×ðäÊÜwæ%Õòp˜·ºz9?F‘þ$·ŒãVß'<†ÓÆž·ËÅ´ŽÍròX¸
V&ÁHCø(ÃÍELK4|ÄŽ2Ñ4á*¥¿›s8æöàÖÁÜâ,9Qš-+1Š…¹Ø©èà“:Ûú#ñLÝÁ»N–}\ãgdÇ :‹Â­ÎKÅ’j Xs7ÞžšJê}³_;²B)O[w…"8àÂù\s_0À&d9ØCüÇfiJrO7b)K£àßõ’el,‚ùGÑy²”ç({[ÐÚ…’ŸçáÑ5çå×í+c{°‡-	?ä29Ôîg™¿ŒãªÞ7ÚãBb<ì‰žðô4¡lv‰”„ãNb’íËÐqðkÿ=nŽ Ü&<ð5‹¸3â,.Nxeýn·6•Ðªpm~iŒ.¼T¬ÔÚÛx²ËáEÏ ‹ºóÖN=AÈP‚eØù“«_“W\‘~SònÔ(Û¤ññê»(ó75RiWù8gâ³•ïâe>s'BG\a”¦šlÛ€"™å`V8‚Dƒ`¼¡W]i¿:wíåÏ=´Šk2ú³Ìº±Ò>Œ²°W@5ÝüÔZ‡‚ÑtºîJÄ”ïà‚Û›n's¥Foà*»Ýíg?¨§€lIè>ÕQ«£¢À÷Ý<+©öì×Ÿ	Cã²,½Þ92÷‡	_‘Vþ1åçÕ`’r<»TÙxÍÍ†‹Í+H$Wœ= Þ–P2û_Ú e‚ÅZøù*¾ab‡žö.éûD7¼¹¾™<ùïÎ¹ÔŽ^~gM)jŽBzJ°ÇI²lµ~nÔã•^ÔÛíð6èh~†ûoO­GþÛbJ6#
Ï'—cÝƒ–lÅ¡ ¸:>X¯™ÑÄïÜÊ=Þ)xÝÿ µrêÏmCfl>Të÷‘Á	fM½gëºQ¡÷J¡š13|"’§I˜@4÷B¼¯‰º©ˆ‡KcBcÄéÛ“†|ðFZ€/W”Ô( 8V«â\^l‚Æq]k·æ#nÉîîë—R÷Kqåä—Cü£éÚ­${>²ôhþ¯^,ÿ$ìphñ¿+à,¨´ñ(€yÕ=vÒÑ”å'žcnã1J0ìSbºÚ¤cÍU(Ë’¡½áè=(BÅ&ðœÅ‘u[Ò†ÑqÄñ˜)ùà_ú	Ws_,'ÍB¬RN±…Ó¥©f™¿óAð½[iéÕ'øÌ¦ì|_M‚W*7”&{'éŒ6š‡nËš‹	XŸ¹Þ’ I¨y¢@ªŒË{€¼Âs9HijQ«4§/”/«ß×¿™©§çw.¼Ðß·$vîµ‘û“CŒ²
Îùý‹6Q¦tfˆ©èKÈ²n°Q†…KŠÔâ¸¶Qõ´0PØh2ù]BÎ=aÚ>õªx¨Ùæ´œtÒtÅQ'UøÅ§{ZË©téeVoß±:PvúŽ%B©Ãž¬û´G¢þÑÚ\ú¸³%>:ŸŠ¯. Ð hgi!ª¯=Õ·«3X2ùWÓüZ…°)-_vYË­"t+ò~ŠÉ=Ð£l¢Ù4×o$‹{°½o²ŒO®rUíqTæðâ•GôÀ‚Ýð,EÕŒd4ø±J†ˆÏÞ{Wê ø¿Lµ´ë¸ìÁÈo¨¹‚ï˜r:ÿšýîEÞ‹lÎz¡¢j™ãÕ¤Í

¯Õâ}¿/ËÅ^Ž¢P´dØ×Se`}èŽsk×uùá‡½E7÷NhK«Ò¹Û–?¿ŒºökûžJ­Q‰¶Ñ4;ßCû°œèìuR.üYycˆ¯$¹>¸#paGM"ƒ¯“ã/A~H¸oz“Z /ÆOpƒTë-ËŒ+Ÿ'Æå‘=ðñ´¸¸ÚÒË¼&Ìã²ÊZš58H®\Âiž)7^Öži]lÐ(®¼QŽ1:(¼°¹ˆjGÉ›¸ÙžÓÔ›Œ(ÿÈáºãl›´¿¼ùI\º—Ñ¿[ àV'çDŠ­†yh×ó²±P_@8æ*T«ÐoFè’„¢¹ZÃdW¤{e©[‰%Ùq	!ZðßtÐ&©r	&T9jKÚ\€§Qÿd|î·k=Çv’ð-L±pÞl›ÚELžé®£|ãtÑâ@’?Á&po¤yøy»©ó:Ÿjt8Jj¿?È×:Û-´(ž'Èt•'W´ðU ¹{U©ãkÀ= íˆ0>Åµãt2 œ<¾Íû ÁÑVÕx`$Cñ•†*BÑ²ƒy^ìØjÔ{Ñæ”ìgÜlAU€Ù:»=Uš~8EŠ­ýÍ÷!ß‰ÕOÇÄ>±"ÃáKw£î¥?1ýÖÌ°M°XãAl}r3VÓlmñ;õ{Á¾¤Ð0^ÆÓNDbõ~ÖlÖ2‡ß¢5w}”w¹ûßaF[<8o¡Cqwû#bÙÿ|ë5‹¢Wÿq­Pï0F?>¡¬@ŸA_Â>Æ÷ñ*- ÅÈg®TC€;¼ºr¹]«˜’“÷óŠÑM6”xìtwîœ7;£–X’ëfÀ¡Ùîw„ÊÏÐ\[SåÞÍ~	õ$5° pI
i¦æ3ù,s´îJÀÕÝSHgÊ±ÆL	1V÷óÐŸ%]ÕÊœ$Ø@ÓëWÀf'Ed,Yç0¼rÌ%'Y±Áûþ4ÿ¼ÿCz×ÆƒÃç0‹ëX@øRKµ‰ƒ~ælF$e®M”C==Mo¯78ÝÍ“FÆ|Õé'Æ¨*óƒr;m hvMØ-Èÿ½SäÇzÀ%›@ìž0œW
‚ËÞ=pkÏ1|žJ‡]Ìë"²ô¡È…ác¨òøú°›>åáOêräU€”oß~ëmóµÉ’jnÉFÎ€È-µG}aqÝ’}ê$¢YhƒFi:‚J…9_,Æ<ÁôYÍ®%°~Û'C[s7?êVÖþÎO`ñ”œíkìÎ)@Ëº«Ï_ô>;mØ‰´W¶U}nº¹h{ŠoÙZÍ&ƒÇ5ü¦¸bz»¶ç‚#›ÙÖSjÖÔ(Z0_¸ž‘«3£bœ<¾NµDì2¬ª›Îw þ’Âu7Tp@mRÎ’ìôcÀþÁLÊTâ‚ñ5l„^3<8›ToÜ{j¯\î³<ïg¼4 ?VÔìþ\s}G/þ\^_Y¥Ü÷Þ„îK»6ÙLºÈ Pt—•ÁuGs¸§=Û`áñ×ó&¦xUtÍ³ìPÌ'™«~ª#ðãoN­?¡H"s<álµPtÁ¿çVe¶xü:Œ¬ûŸ\H†J€GÉUà5e0>:QµT¸{ÐÁÝˆX–5éj4E_ÏhûØ&Œµ¤!ä£X¨³'ßTÐï#ýÃì—bã<ƒˆ–twâV¶Hz›OK`ÔÍ%:=:D ®ïJ¬¼L»Ö%ø–ŒL´w¬Sì&(A58’SÎ®¿Ûq¹wu®#Þ¶od»ƒ»wü0Ãa¦˜Ú”j¦u46Áó^UZ°qÜ(‰²ê™±N+§š€-¾v\¿µ|hZä®4˜ÖÝ-¤.ê	!GŸv…P”«®vóÜ9ò‰ñÁ˜‹jdµ‘~ºFÜµ¡¥ZýåšÔ­ÞÕ._W¹3B’žLe>R§¢Jóf¦áN#ƒY„iêAjœïLmë¼N°ÆÞV©ÇÞs¾Lªµ³ŸžÄH:hReã-0éÇ2""®.¸lL¿ƒÅZg2¿ëdI0	h”i¢Dún O+g g\±K^Ÿ›Ñ3Ø) ¨ª!/»²ù#~ôéÆ ž=a~@C,öhú0[xY«S˜üƒ9þäf÷…I¼ÝøAÿe"Œ¸Rþ?|?Þ±Ñ‡Eé'LÉ{r‡’=iÔ_R[©¯e²…Çå÷*«X]ögÇmÞÆU¸¤¾l€h$L _xLª+§›/>Ÿ	›8»j;w?3ZŸl¦ï-xEXÒ¬×ö²…œÀž‹ÜäWyU¹"µ5ø+£yKvµ®¨ýËI]W}#
™Vk±g$ß“÷ÝÛ Àì(ÒÉÜôQ³Ó-pñ6Ùp@ÿ× ·£èÍÈ&;‡g•úSæY0ÆZ÷{{’ŒQû¦CÀo†êEtIX,bO Å|Ïù]‹ð{P[3"
¹ž¡- ÂèÒ¦ñöèÇ"^Q
râ< Ü«GÓ‚ýSoå~ó§$§Ö&¾Æ{’ÜŒhÀ¬+Kö1Fû¹†³S¥u<’5‰­â¿[ÿï1ZUï#ÿ±Å?"5‰aÕJÏ·	pøýq•Æd	faƒñ?ø°7”\á´VNùyÐèZÞ;"WæH“ÿãöt¬f=É#·9Ž"ôú’„×øFÇù-gcZXã£	ãÄÎúlbÄ®8¾‡--ú ¢%ÎŽ9È®ò«|Ô†1ˆmÍz¼MKÈ¶R XÚsâ´Ã`|9÷eÂˆæc	'føwÿiÏ‹•)á±ï¬Èb™Àz… ºæ¼h›H0$¿ƒù`ÆŒ‚Á›¿J[;%G­ZCXo‘³ yÑÉ'R5Zë£:ìJ5ýø[õÚ×FãíO÷M´=äÈÄ¹FâDº´ÒŒé2}wÄ‰ ÛÃEÆ!ŒŒ÷òô4:yù0 õ u„»Ëû c”¿¬SBiÿ°*ñdð_Vô¡•Nl…¹'„î¿°Ô_;öª)bRz`W´Íª‹}á	1±*©v-ßEN$€ŒŸÃˆÀÿ&€Å7ãŽ€¨Ïj‰PŽ3×qÂ¾wwD$Ržœ$«?Që…Ì´;·^·ˆù%è.lìÞJaâ²ý–LF}ãÎ€6A· u³×—ŸÆç^=™öª?QgÙ’LÊ7ÁjÒ9ÁF´n£50®k½«%åŸRÌ¬ì÷õ\[Ï/'«:`ÆµÑ—.±ËØ*¦ÒA€MÕÀ­o*YsíJß’£j{ìJ\“ƒbªˆ¼øTdæ`WÏìÌw›fÿ»µSú_š"à¦P¾4ë`ˆÚu §YÜæ˜Jäi‰ œƒNÞ6p7"ýÃ"xå®•‘g‚÷‘pùž­kÇ´ªO-´ßâ¾ýYâŸ3ê¶@<¢Mù¸†3öA›çì:ÐçR/ôTI–o]û¥ì‘âñ:ÔW8#“CÌTÇ¾ê„¿qŠÐeCœSÖ|c›ÇÐ£œ)¼~¢%yÆðŒÁÓsŠho¼€¯oäi£ßä(’‡$‹‡jÑ>,–³ÅXòHL.è}øùŠäWy"äx GO‚úÊ5ì`èé,Mi‹¥öD>Ï¹3¸²Ö¦È2¹ø;üÜXºzrèo²8Õ81ÊÊdÑ¢36äžGH^²K3£³¶¯öt¹_TeäJ;&½][•ˆË¾G¦éFŸ-I™Â›8FðŠ0qÊ­,‡8¸Œ„ºUB©u|ÔCßò!^m±TÞ­4üÚû”Ós&äöÝÁ‡+CÒƒÌ lÝ‰·àÝx?Ä–$vJ}µ¿µA8¸T6ÁÎçÚlóæDŠRöÁKnÍp…R—V¾\e"ÙýŽB)¹û__wÏ‡#³MuŸBà/‹oÔ­6³¬Ô=3oÙÑpSêÌâ­*3ÚéÌ˜S‹@óX¤O¡ë–¼hEqPrü]HøÿV—¨¾;Ä¨nüèBç*nÎˆ5gd}¤#¼O§Ù¹¥Òn»¢ƒþ¨,â»Ž:â?Ñ¨v«ŽîÌœr}1¥agC^¤S$£œ+öl)påSŽ[?”!}<ÄTÄÐ¤©É’ñ–‰µ2ƒ…‚o¢3r;¬æ7ú“à)¨’yÏ»“²\+Y(ï-,{Æ'@‘í±~:	,Æ—k TË]ëD@¥DØž¢vØ‘¥B63Èm×¿W'’2PÏš)^‰8H+Xá²¨N›ÊC2ÚÓ§ã»Ú¯e?åQm¬€,ü!¬$pI,‘JËô)Í–égò`²ÛËûò¿¥è¹x*4wXau>-0€üH±Bì•¥7Ôv<hë®e2ÃÉR¯bÀÊÔ®‹þk-?ÒÅëKR ê±%Ðß×¡ªÊ #­º±º)dõºy š‚S•±é	ÊÕÆˆ©{˜Œ‰®›è8^/Tk›_á3½/A‡–«¯q2šxi›¨¯R•Ð±äÁRf¶ÖøW¼´óÃw^ÃÝ÷,-`ÜCHk.æÛ}û$cÅ5Ž½8…[`“Ì¸¦!7uÿ‹„iG¢7âô£ùÂ—€[ÓÕçBˆqëBéG„=©ÀÌk¨;Éuo&Ú½Í=j>¹'QŸŠáL³áRhu Þþ"ôGe&3³ô0NÍÚ–€¸\jÅc°Ê¹ÔšžÐMd–ÇŒ&™ÐÕµÀß’©ÜÑöœ¿¬O)Ü`Ðd¥ÌÊ§$¬@”còv54U·ÁzõÀ£;*
ùá{ca—
ƒ{`IUu9ÖšðÃ©ýÄUo×7ÏcŸ¬þSœÖo®L+–üZ¡÷¹¶Ezú¨f¬›,ï©ÿ21eäÜËþTŒ5eK‚ÐŽµ;×0cá7ÝÉL}EôÂ¶PÛÙö×J:NÂFQt‹KRþè„…«¯ß€€ËŠÔµÍˆœóbÿ&yVcƒÑy1¶W´ƒXoøæ¿([¢ÌC]‘¸¿b‡rõyp]mÆ(pPµ3ê&dÕ¾ßÝ¯=Ñ#Ï®œíŽ—ìè¥)öUA_‹¸’ADL(‹ö;óa3?è¥Ü†§ s‚çû?†ÃŽü5Jfgïø\ª•gú¸>ícÓÞ¡0äï
rFp-“¾Wä&Ï”éG>,=®cq¯œ¶ïŒ“®¯‹š4bK×Î3!$à
‡wéF„vÑP[Cx>ü»Ò£_np+W¢_0±Ù~ýÂ<l¬€;hÔr(-ÇRÎÈþÐhZ0g0suÅÌêZüµ:W¿ÛñÐ>ãbûM»Cå"«¼Céò’ÕÒÄ"•ìÜ«{m1ÔÖEñ)×©aÀmÓ%Ý QáÉ_+ »±ð0`ë`s-¬+Eû´ràÀu±ø^É²S’,·¼™Æi¦ÄC%„p.»À@\@„·¶Xç÷
ùP—oürô ™Aö»+4`‘\Hƒµ
k²®ŒáÍ›å$×˜ëÞ37W ‚¡7‰+pEÉ}žþ¾ºÜE–Õý6{ŠRr&
}[×l²íþß=¡+4²‚E‹YBDú“FöX©ax¾¨qÎÒë£ô™~#$jí5a”fÝlÄRlßt…áA9ì¥ž ˜Sò¹(q‹
m¾jœ”À3Ð.)Š²œ†"ñõâZ¡>R ‡‘áãg<óÓ¶˜7°({œ5>$êmDy’Ð·È«„aåo?JÉ–jôþ”=@„ÍÐæ<Ü«Þg_V7vpÁ‚˜ëÜÀM2øùæ‚ÞDT·©Q™àô1 KE¤ï¦21«™1,ýónÝæt/Öaú¢çÐÜÞ°Ç:ÑY±\ÀÃÝöG™KzB* =0ÕÌ–\O²ß;ë›D|™ ûïæöP;.U­l±À´tÍuÆcìÍ !-ùW(ªúu’ñÓÂóehÀ™SGg@¬;ÿ€í>V@½$êå4wÙe
œ>Œ„¿ØK¢l“™U¶ãzi:fJ£@XKp]f??q‚²®¦s(ÎÛÇlOªÿãÌÚMŸÿÇïñ	ÜÈ;“|ÝbßªÒç‚îo­¸ÊÞ_9üÚŠ‹”×õ‡?ß/GýòŒÐ¨˜ptMYb15nÏß.Qò1áº³(R«!ÿËì8#·¢)Ñ|á£•GÚÊz / ˜¾3í†á¡ü „l…Ä
Â9ažeÐC‚O§}8]w[%cwŠçh½]àÊå`!ºèª-À<3L¨ävÒ½Á1@$	½cõÙýF	’r™è:7B
°
	x
ÚE)ªÓŠôøg	¶éÑ³K#Ç"`÷‚¿TƒhêŠMÄr’ªU|?¯.Iñì†ƒæ‘çÇ±ÓL‚eÊJî8\YÁi=yÁÌ¬|Åïx8næ
Îo¸æ³’v\S>¡fèR ®ð©îÎÏÁìó¤ï¾û«¾íK˜Ep»” Âü¢Ž?%gtÝG]$…uƒ¥Üö‘‘x"Z€„ÔRË¸}ON×ÆÅTí¦Q¬yƒ1áN«¦§ì"¦(0lËnüôŽ†'^eT–Œhªë|	þùóö”õ®×üÖ°D[Ì,¿C±*ßF.Ÿò2cu`(ØÅÛ)Ït\ae·Ê|¨ØG?&Ä—ã®	èŒBÈè0H*t@È‚h%bO¤"ïþ»™ÍÞþ•æZÄÈçÂÁKbÏ"j¤+B;j—u5ŸüÙ÷oëØTY¡¾í§2r
j¨÷wh i8lrî”¥ê‰¤g@vQ‡#´ÚštAñÇ£È˜ÍíZfu©mSªÓ_qT‘þ«oh@)<Ç•7^ÜÅ$ðïw.sÌÃ6}¤zW¢_î¦ÔðvD†§)ìÍ›bçä´U–ËÂ«È0Äà@ZÏ’qaúnÍ…¢l'™ŽàÓW$”8¹ÇË:£ÆJÿítÒÈÉ¼×Š`ö×L#
}¢rQÇ/º
ó|Ùœm@ß‘.;N>xášZINLÌéÔM€>Z‘*ƒpYqÓ o'ÞZ(ºôCã`IFµžÕ	`•iÖ±‚¾Ã5ñOhW#³IDßµ¸iOÔÇ­ÖOêzäÓœêÿõ9ˆõŸû›+Š,Ð…Ýûd|Z2]V	£µ~É©eñãÏ©(ÒæÂã­O¨íd¼¯
42
¹Â¾æÑ<Ñ9!°•[Í[í"ëšC6ÃN6Ôë-*ðŒ:4óC|ùHPe ÈÓs²rP‹tWè¥RA´—Ï‚3”§Üá<AÅ‰MZv	It’:ì‚˜3HãÖæH	 F¢û''µ'Æáx€ßííº7Þ#3§³Ô««$;–/Cd<nÒo˜ÑÅc2Æ>ÓÔe^l§Wd©yGwø~ÃÄãPÄÞUòñ¨—2“hpõ	¦ï]Ê€ò#À¡~[2Om=…­—wÃ„ë¶ëNuþþ@qéGË¡#ý”yHÁåÜA”b¼Ù^ÞÚP†PW´/}a(È}¾Ñ,›ÅNÊ‡ÌQjh?üZI_:Çá^=:Ÿÿ¼ý“éòW•[@vzX€°”ÇÖ‹%:J´0ÐZUˆ„Ë’“T|·†NË /<¹3e¾ÛÓæ¶ëÉ0ØJ§­ÇµQ>@î¤…t¾Í×½¹›ŠàäÎÁZÃo‰Õo¶|–-PQâçÍýn„iÔFN
Öh í‡!·Nì‚gÙµH¬ãŸ=+yuú^ÓêJL‰åÐŠ×¹¥Þ1´ÅT–<	¡áàßäŽyí¨ˆ9Ú¹wÁNö!v‘ƒ[Õlo_¸…óGhºòwggå¹—y¢Æó„Sýø¶6|Ë¸º/Òù¾ÑêGè¢ov¬!Ïyƒ|€J³³Är¹¦ÚrÕÊÖž^²ÁÝMåónQ ……Ë	ØU“‡hU‘èk`bó…Ñ­cs¹½Ê8Æ!êùÙÙjÑ—>¿!Z4{UƒrÎx¥õp(¶tº/WÒL ‹ ØÞ·Ã´o(i€Ü¿¸×ÝGlÝÛ¢Ù¼È3è}Ì.,;¶ûhÚý1¿ìw©eá=‰r¾¶ìz% Üú>vÛJa¦1î=S,¡<+Ìg?iX
oû€ˆJ´êáSo+,V†¡’ú'By'üæ±0»ÅÞª¾ëÇÃ_j’ð×²=¤ù3Ö1Ú6\7ËR6Ç®
6]þWA¹ž.š‡IAåÎqGÃµªý~Ë ž®¯‚Ôs)K
[%™¬Â¸ì^@ËíS3f”FfV[ùbc”¿ä6îR<hÏMlo3ÒÊ?AÿŠy3Í»k:"#÷—?âÐñ5xgÓqÏŒ.*õŸµ²5€¨p-ÅÑDp£Y•Ï—ÑUïº¾eÜ]r7ÕS	0û4Î¨9@[‡öÆoQ‰³è3t›#á‘¥D¡C=#ƒo]éºØ‹Wó1¥¼«<.óôqøV Õ:³Zâv÷\8Rp‰À»Ãæº¢é²Ç)p:šŸŒXj<&ãÚdÊ2Uœo¨Ó½Áu:(ÛfÔ„ò€±ôÖ³K;5¶Ê-„ÊãLÚyÿâm8ó[D|"»Ïò—í…ÙO×#¹÷öúøÆ)êÞížf°‚	¢B95ý}P±æm^ˆxãs"‘\ÐêPESzSJ¸™å‹mÐä¤º7é¨.B:ÏlÚ=[­­FÃ´ç§IK?ñP9 (#A…q~'§ñØÙÜ¸|3pžî\Ó¦&ÛL%`z(bÙ3(h« ãÝ*—Õs&K—xETÊ<˜Í~åôT bŸþdGr…šík¼€ñl¤‡~î8h½:z!¾)äM”(²Kf@Ü¦
[<ó¾vÛsZ¹ê=I²Vë)Ž¯1Ö ï/HÜZšØÕ8Ôè"–Xzn­_òþØõ)×ÔEOuÂ¸LÆ {Ý«‚æ ^±Q8¥ô|í$ÓNV­]ðô/¬@®éI[ŒËWfaw'Ýú_DÌõñ>ˆ-ÂÆÚhñ@(-y/¶üKÈgiÿn²ýø ƒÚÌñhÆNÈù^ZÒ‡—_Çk¢‹;»_Ã%ðl<·%Ðù²Ö3^tRM:pp}Öq„÷j™ÖïwFgÓxû²/)¤ó¶jú—ŸÞáàA¶åÖ"K¼4¶¾>]÷1žÔ¶n"àC¶–Ÿà¼dˆxHQ®b<½„÷;ž¡ª;m:slÎ«¤Jiù2Cv#Š8ü€>ÉˆŠPNâXúàŸKÕÖ¦]Qq™¼&¨<O¾Ž€tOú¢?c°% [=üM`m3Ùíþq467A~ôÆb57‘#Ñ,¡µôÂ;êiÇ‰!0ë+!¾Û,ÌM_¿¯ƒßjïuëY„KDVUR¹ÿãzßÝFü@
ÿ,	G[æÇ…:MÕ‚íÐ½³÷­˜§³'Ÿö‹3x|ÆÁ¹Páª +»–oÒÓŽœÍw_¸ðd|‘Mú
Ä.Þ¦¥Åˆ~7Õ•éÔ°Ùõó<¢ á5mf¦ýånßkx(š:7 ,áFÄîç 4oÁ¼;rUÁÃõ\Nø¨O³˜4˜TÄƒÑæ€1Ôºz®
1¤‰<`„§¹€xRJyÏQ™µºT‡‹]ûeCÂGT1•r=a¸âê-[.~I¬z—ñÜô†Åäz³Â%r CÝÑ^,îg3àZ-4^Áî.>qR²9š·hè„zšNäˆ¥mÄ µí6f!cÆüìeúCYƒÑÛ§…DPtºÍ6€ÞðÔ~F‘ %]hÛ!Ù´šC Ñ Ø­Géj‡¤èÎàæ!K¶.>¹›à¬ÅÓ@ì,²âŒí`†Â<V¯0Ì{rŽ/Sµ}ÇÌRW=,lRúr7»•ö6rT]HP±;Ô-J"êè)Ž½œaÌ$O#cÂÌq>ñœ~ÎmÍ<‰>à„&OŒé¾’ðË¹6ìÔÛH|NsYgþ"_ö‰ÿþ`[ZûÿÂ¶…+D‰É=cC	¿‹iÁ ÷–fhöÕöÐ®—¦³uoÁŽ¼ñÕKR)uóøM`ŒdD ˜ÕgRòÖOÜ„« ÙDAª‡å|>p¸MP¸`ñWü'—vðÕšç–Š	cR?eù£Þ' H è¿URíôñT%ð|je-˜ŒÁ»º`ß€ÂŽ!úÏ±¹;óuÆLç‘ŠPÍóe–ÃNŠßý–(f¡¼\hdé@u;×Íµðêñˆ€®*íšÖôè8£Zv÷Áu€;ˆ¡‚à]Ö_âìµ\ÓïYÏ1®ýÜ–~šjG8/î3æ¯ú½VÎ-#®€<zk©ÜCç¢Wûc­jl âDâÄap<Out gu%Pw_Htÿ¡‡Ÿß	¬$aðì²¢0Š“G¥µ,S~ñò7šRE4v Y£3=È’RlØc®}Z«xõ~¢Ymz`¦&Ø¢²™…pˆiâ'©« eÑÞCó5ËGLtké¾‡ÏbPžç*®BJÌË©Y3¢U¢Ä^’kShh½…â?ŽpG|*bJ>qøŸJ¨ÆÐkO:/Qÿ¦^©BIŒjmb€­ßŸôœ½,3Í¹Ù`ù# ,Ñî+]6
0ãÓ$Ž°«ËÂ^b_3.+u"Æ›M:KÓCâÝ´×Þyˆ|’éë ££ËmZŠSÇõ4rÖ¯_¤¨<~@Îb›X:  ÌÃ„·M«~Í¾äu;™ûm;³¦¢Ã5ZU&“PÁ:î@˜_ÑÚþ†kgQ!†,¨3Óò5„ Mýìx‹‰{FÁÝ‡O
$´1¨â‘=ñ§¿bÄHç÷Ž²>+¤hßˆë¨þ7!¤¡
°ÿÕãš•ŠCK½n9pTWB8$’m‘ùÓ>l|×}E>žjÉWDÚ÷kàNAüK¾Am
 [Ú- N'JE¿O	âZuÆÁ¢ÄÃ¡Æƒ×¡À,R—üò¹Šµz´è/0.ÔèöÓŠå†e%²7/=ÓÒŠwe 7LK_”aSÊ‡®ìS§LOËÇ¾ƒYV›ˆNGªlR´l[{-D^ý9rm™Ã½#Ý{ tí-âÇ¥¯µù9€½ØG}à*ËèÝ„VçDÞZáÉG§c·ø‘ÚïÚ·ëšÃŽwXAœ\áP·Î5êò!juýÓ0Á›¥µ¾šh3ÁÈ}Šf/sÒ.·Ä{ø…èHƒU”P uœb|†Ë£ÅHó¼s´¾c}ûúZ“è¬mæ«Ý¾==}>Y”Äî¥‹ŽNUìN˜@žô¦f¦Zlœ+Úâ·vÒ…QB’î'Õ¿Jgmj}JþŸ·Œh"%¿ÜœµPî	°ÞWh’Ä•5V[úCóXúä¼×O†K©šü7@Å½?øBºŠà²y€+ƒ1ÓÌðZ“BVoË{åžÚ*ÑP†ºöÜNÅa<WÚi¬%îÂŸ–h3þ'$ñHäNy·¯Ö‡.Á5=vq\>Û¦ˆX«Õï÷€=y£¨A~¢m0;8¢Uñ"æþ¬>´¿Úâ™à}»¹?§ˆŽµaŠÅÐ™šDZaexOZw~¾mG\ØcŒA´v¼äz†K~ Štžpõ¸Óðþ7¶WŽ´Õb¨Î’ÊÖ<õ‘òRrž´üô:ÐmÐôm[A`°ž•õ¬¡h4þŠ1-ahp^¼×)®”S‰‘*HÏ–#âÃv‚Ú1-ödfß©Â…ðvÓ›ÇÕUjz…¾c´$ÔåâÌq¹qôü¢prµÃs='èk£‚ËÊGf/´­ÊK{G…—ä„Ùh¡íuÝ¤TÕç‚zÃ½ÁAÃ?6Ý÷¡¡DVƒ²¬Zù…SÀÑí½x8Z m¤’¾FG&}ï½œ9|žÔV:®~ý—Ò\òUÊió¬´ß¥>yHu5(JVj|Ð4Ç9IÚ‹pÓbF¹{y;1k9õaa¯píH0H–hÄâZ²XFªiêú8È‡zl¾Üý2‰³ê¨ÀÏv4OÑéžš‚eÞð]“Dy3ƒžO{¾ð}€òLu×
C¨ƒ<}dÅbÊxÛ<pH–à[A*’‡NÍ/Ý¯\…SkG$I³ é2Òq­BŸŸÔYFà²,ØÅüh‚¤PÎ„&Z;¤¤Tk?Où.§h¸	¬ËØ“Zw´ÙÜŒ%(ZÂëÑÔ*ª7¹Õ
âø¼¨}·Óë,íÙÜÔt“2Bèomæîc†Š†f´¯­BX—o<(ó]öw `*|Ô1)FXy3'ð¯AÓ‚Û’–6°>É¥ùdÕX(!:Z±©#EEÌí*š;£\ý¾E¨XrŠD³$=l,Ô¼Ü¹Œ	è÷ví`°‰‡&÷ËÑÌ‰GÉÀ©[öa“Ž‘]LäN(V:‚ÿ
ÝúÕ6qöXÈÅ&j¬=µü Ë ƒí5Ì?ØF<¡öoh`$ï„çœjÿg&o¥¼—Ö(Èq€ÄÿYú4é8£d[X9Öµ¶ùò>[½Ö ÿí‡¯¹;ŒÐ¶0'þfîXºÔy w¾Zˆ î©ÕÈ6†gz1¿üÖÞÂµWDéqÇ¿;<R2@J)¡! .…†‡Ž-ÈÆÖý	¨HÂýÍZNAC•)–ç¼¢¿õCcUUë¾ô;m1	³aHˆà2ª–xDÈ^µ%^ƒÅª58Ö”XŸ¯I½íýõIèƒÅë ™"Aæèj÷«™¿2±0æk+àÂ<.5}|ÜÿuÑ<LÝÇJ0ÁÝ©gÕŒ¤)€I#süSq>Žî\Cs¯ã6q€ój‚àHWòY@¤RáD°Ä[{F‡„ äýF¨Ç,#n>¿JŸ—œ&Šÿëº§lèŒ¯E‹ž`¨/pŒ¡nŒ’!ÇÅçÐÀ$0p”GÆÜS`•ÿäÚŒ_á˜Õ‘ i1e^÷XÂãdaþÎ."ÄZî2|ïÈL‡$9‡gÇ6UÑÝS1@·íàœ³Ë–®ã*÷N/ÈÝâÛwsÇH#‚á#obýÇÒN½kÖZn/5´cÇãþ šÊàh_ÓîöUA:	Þ<ˆsðL‰
¿g™P¹‰¢|¥­ÁÈ£ªBÅ¨d{ÖÃëœÔVÙÇ PöÚ—aßh¯+‘‘3'È
±ˆ‹HÈ»¬¦V()Ù¦²GÓ¸£sÒ=ƒbßÊèÇáV8Ö+£_è´]-!X¾û±¸Býñ6JÇ€»¨QíÁÄ€Ç‹×‚WÃÅýº²œ ˜Ç°ê(üñ-ð_LOCK_DEPTH))
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