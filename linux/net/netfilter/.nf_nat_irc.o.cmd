nfig/GENERIC_IOREMAP) \
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
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
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
  include/media/i2c/uda1342.h \

drivers/media/i2c/uda1342.o: $(deps_drivers/media/i2c/uda1342.o)

$(deps_drivers/media/i2c/uda1342.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ude/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/segment.h \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
    $(wildcard include/config/BUG) \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
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
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
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
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/includeJ �a�|Q��G1D��)��U�A66���vy�7����rU�(f����6��Nm�)��$4�X���C�Y�+�օhh�谵� �@��^���7��-��Og׊P�VI�e	�;�%[N=r\(��U`�����}�0��Nmf�2�%��:��d_l�&�ݧ�v*�^��0&?�i��nA-�R`�n����};S"�Ɨ�:�S��*�
����u�����k"X�?��m�t�奌{PḦiŎ�G�l��Ϩ�R�4Q����Jq�`5�O�~d�����>dف?s&6�~K�0����'xI�nf'������9}����C��!�徲��Eve]�q��LGem�a8f P�S&;�M�1�$�-�l l t�H�wA�4�9�#�����T����b�D��Z��Ga�0e5��1"���5_]�ߧ��,^i�\����F���N�b�`f�����4ډQ=2�Y���%Aa��>��)v]Vd^�)��8�)){�������, b�"w�Gf��:B�9�Ʌe�3�N�񸇴�J��NC���H�y|M�-���i�<��!�\��ɞ��ê*UpcĂ
TǕ��g(�`�2I9�He��@N�]���\Hjz�૵3��:	����Ѐ^�J�+Myh�Ǟ:�{�n��o��g�1Llvr;������-��]���2��������/.洏I���-�R�!���1���������;�������#��h�{��V���?l7Ip�B�{8�P +X�L���J8�T�܈�63�ǐ��3��+�����µ����:=:�c�;-�t���j]$�I�� $��!O�OB��v�s�-��a���@t^��	-�Z%�T���tt��P^��x�?B�/{��c�8�f��jmޘe��#�&���sO��$Ӗ��v/9<ᵒg���m�Њ��4,9�s4��?V	��v�ij�#���f;*LPUv�n��e}���D��D�.8�!v�=�Z_8�2��<	Z�*cy�T���8�Ѭ�tE�^�k ���+��_h'��nk�U��nm��ĩ�Wͥ�zҍ���c�����l��b�hJ=V#���3�p���^�$\��hpU�IW$.=��0����^�R�K&�b��¯^ǐZ0�\@�ұ��Ǽ��*5��-@��ל�7���~���G1��*���jB���_X����`L{TÌ^�L�}E�m�L*�B�����^��sP�.m��x�Rtf.������_D�	&r�>����:r�\׭��2[&�RB�q�;t4��KZєp�^�ulf^LCӐ~�U� ?k $>X�\�G����K�e��z��LB=.B�`�zh�^���k�?�7Ym�PBeOȐ����z>U�Zܲ$��o~cTG/�ܽ�<v8���Db���C	���d�g���*�����ꥻ�W>jcS��ĄXhmE���ٮ���d� �w>B[�%�!m�Б��Ik�����F��L�_������K��y�H�W�;�B˗��]O�����RV1)�":T�����R�K�[g�H�*��m�����W�P�Wp��o��[n20"&��Y�������FH���:b�ۼ��<�1Q���(|ŏl.oN��5������E������DA��3���$v_B�L�B�%D�Q�v`�J@�Fƥ��ଣ��T��g=&��yџC�_�t:t�F���xX���Q�=�)!�L�JS ��^8r͘Cp����߾RtyhL`f�h����p+"���w-K�@D9l�+
ΣԜţH�e�������3(I}@e[O�z�M�Ts�j���K�����a5n=�<=i��DX�Ry%��������il�t�qR0��2�S��$�s~#��'��?��s�5�}Cp^R�y��O�c-3#����хyXzmq�d�TX�U�Xb$VSq����gk_LO��Զ��X�i!oG���uB�X���ڨ5�f�u���M�����T[f��mPu2��'��f@L|E���d��X�i�wU�w�}9*Y'�U]Ѧ�̉�v>��n�T��^�KJÿ���H I��b�Z�<(�NR�A]̸S,�r)���P�T�'��ཱི�?����.��QQV�dK�k�b�ݐ�a���5ˆD�S����Y�C�c.�ۀB�&u*��& �%Z3�S(�\�ܱ`,�Z����ͣ�Tg#�b���=
��#��st���fv$�gj�TNm9��7�n�48'wr��|�=y�_��t��2��Yr�8(� o[�IC/ p��V���_��/�jLqw��G��
�6Xn��ðo��hҊ��©n(/H6���f	�F�_nV� �/x�~�
�s�a,I�q�ۚ���o��)�ECo6U��=��K��*F~q��h�k��U�����S�Uz{8����c>Xfv�b�g��V�*�@�u��M�V���w^�Q&�'��̩Uhd	"���e�3v��# ��op_� wG#��i����� H����h�)<B�F� ܶY{�{o}������o�(���$����1Z�nX����=V�8��ڼ]��$�B�|�T&���k}��O\��0�&�|�0���c)�P�oJ���0P�|���X�C�S�i�H��W����$���I�����+Ăsb��n�����?�Z���*�U+��y����ӎ�����DK��=�Ub��=,��O)�ޛ
l��@���~���K��9N���nH�n��"�����̃O �x/�*�/��i�O������Z���d��n��Q�G�Z���=�a���s�R�������p��$5n���Y��u�6�֩�"��܄ѓ�]�]���������q"bvZ�̰G[�)W9���a���i:�z%�ȕZ��L`�e��;W>�� lj���� y5�O��X�H��<�2�� gs��:xՄH����]�8}��p�/��{���U���t�&+KY��A�M�{���&��a�Jmh��>�����0oK�l)�<�����\R�@�Z��9�m@=�O}�q��q�(�_X�2�*[wĴ��I���w��X �tt��\�ATD��n�2{������RH!�M�!�2�3��Ӄ9����g�-Jl����w��k�׼-Ӡx ��le���#��`��
q�[����ǰ�ges����J�ͭ�n9��E!��\i��$(��N�l�G�NI���W߁^#���f} Qz뀄��Yrt�m�F9�tނ���W�I���i�Kj?!��)Ho��k!�0Ü��A�E�Z|ϧ�g�?sfQ��m�sOm�	/���)f�uqqE��q:X��'�T�Wo���[����|�}���
�A�M�����3k�ܐ����'
{�H9}c+�*#ù_�3���5୬u~�1�蕖�{�0NҶ��a��?��=U�xH��`��:����H����w˭�#.��C(��o�1�W�����iӦC�n�ji����*�#���´��H��rY�$t��/l����D�1Pp�@��sf
�%�l[*���XF�UGU�sP+��e/#�m23@v�t������������:aH������t�>=� �÷�]o����]�o�(�/"99�i�5�$��C̑�(缱��p�Px�����FM-��WP�j�K����)�U�e�Oz�����v�M�Y�u�d�h5�G��U����J���()�����pPy$+F�IWS�ߦ��]s^��>>���� F�VC�rm,I�ab���b&�/"�˯U|g��"��ƣB��c%�
Q����]��[Q��	���@�R���$�'�D���Iμ HΫ{����7б�<Aɽ: Ϲ�6����f6{谭��4�c��'��{R�m�Dh�ՕI|vΪ�]	?���S��m�q�h��E|�����.U��Ɣ�/r&_,
������wþD-��6k<e�H��#rx^n�����ݣeɕX���V�%JƉ���%�t�6[�aŅ�P3׊���6t���$�^��>k�E<��i6�7�l��O9�GG�� ^忦~�<�X���j�番�l�?��Q��\?B��I8vJ�gTMmSZz@�c��}r�r�)6���I#_U�I��*�6���
'���\�Q��+<��h�=y$��?~�G}af��m 2�U[tU1W#c�f7*Ϊ���������lA�\6?x�%m��B�U\���1��'���J�Q�z�G%y�����|��7���9��� �=�Dզ�����o��2��(�\�ҷg.��j��;��_~�7<O�f]�Ѱl�jG��pY�V~B�d�*��<���$���w`��}�w˖��iV��I�Y��~Fm�$8��!�K��VG�2�N^�=�	��9��4Ν7��K�ƙ��Y`ي�� �<�D���2K�I��
z�#��oۋ��ô{f�y��R���F��qR�I�i'���d�*1�h�}�C�`z�(Ta�wP+@<��>�K�0�ēM��r1�c�;l����,�UY�D<�@R�Wt�BhsDj�92�~��!s����&t�䠜��.�0�vkV�ibG�t���o݇+�Z/ �,�P�]� ���r�[{dY?�)*����3S��n���&yb(���m��e���?�i_���$��2��;軩�d�@�ϟ��i3��b[1T?���T����gP�_$FG�pݺa�`"��蕦��Q�!��̀v�(i1�
;���0��k��P*�Ny�Ȑ4�D�.�۽
S�xԮ�[&��sH�M"�h��m ްf�%sY�N���6�9�����hnA���q&.��_|)�y<b�+�T4�&n���?n �R��@�U�A3��B%�xk/7�'/�H�& �J?a�X��L�G�����O����T�=7�����o������N�؃s�r�)?��^82��d6vt@ޕ�D!�zyn���-G�G��&�"U	�,at�0�⡓w���)�a�F�Y���Xu���fqZvi��Ir��ӝܤ�"�*������
���l�}���.��n�ԣA��%��@����^�0��!��f�����Lw��s�;T��>\�DpV�+ݬO�}v�j�������r"�x��c\�^���|����)X�1������I��V���F��݄!�|�Ȝ��u��I�I�,�V�o�F۲�i~�X�{��P��/J��,�:�0�s@ykk��K|���C����Dv��U� ���TR�w)
� �k�/H��9�!�1�._KN���b2�`�_)8`�)YI�vh8��$�4��A,�Q����[ ;׃	Nyᤝ��e�|0Xy�nª )� �qy�X�g�q����vw���N�h1������~jh��g�⥳n�ز�0�l�%������(�u��6^j�篊�t2��C��ٚJm.�5���.�H���F��k�~�1`�XN�m�t35�z^��SM$�D~�|��?��Gq�bi{1B<��lo&ܧ#����@c8`_e�ߒvoc�̝�|� K{�/�3:��1'U&�<�-�te���]9��$+�f���}����g\WZx��Z��-rr�حb����c�ee��.Pu�a��,�6l���J�V?<�MJ�><"8#�_l�O��A����z��w(�Z����!"KAi/�u{i"$�e�2��07��C�·|t~��TLoU����T۔��JԾ�ނ����Y-��^�]f�U/�5x(�V��'������<K�Kc/2�qW��ɉ^�ۘ:='�(De[x�Bٞ�?�{�2�����Z��*<� P��V���5�5l�(g��U|��ϙB�k��a�e��V+}�O1.Ĝ����%��P���a_�W��m��W*g���f�&7��y9�q��З�������K�d?�����Ҝ��9��)�g<ڃʼ}~��Ǜ�7Q9=��S�{�Y��:7��#Z��fj*V�¶ck+�_G�#&��j��l�Y$�|~��X��u�:��ڀ�O�KislA �gͭ�(	�t��A�W����XǡWk��&&�w��տ{+���ĽI�Cƻ'��C��BN!-41�_UL��&7t/���1oH�a���Y�hb*CZ�|����Z��
x��/v��E�r*gbn���c)�ii�)5z�o3�2j��ڷ�Nb��m�s)K>1M	���L��w�����Gܑ�9��}7�UOj]� ��FpH�w[:�o��ւ+��j6\�?�j^�q���Y���e��uy�m.0�۴i�kl0�Ʊ�{�%��4��_f�O�|��S�-J 5��l͇誖�S����R��LY������[[��-(ϸ�rD�n"GSþSv��?c[�^�u_L/~���+� ��t�B�4�1K�"��p���0�'���|�{����5s��,�Y�N0���x�}�tl��J��<��\W��y�
5��`e� �8ٮs(���|�Ǝ��&�l���W��T��u�>�� b��ٜ�-�T�ߧ�ۅ�e9��
�G���>SwSj���/�nʕ�@��ph�M���v ��d�����B��5�^�F#���i,='�dƯyA�;��E����p1���h	u<�-� 3_wx�L.��ƒ�V�+}�ͭ7�a�`�,�=pH@}urU;ڡ�J��Qh}����@�/�3>�v�`�sۑ����#�����{~�e~�O�(���R���j�WNRq���ֿ,�(�����p��.Q��i�R�ʶ6�%��Rt����=Z,�a}i?�3x��RvR-Qe��eL��lf��3��]3
����~���z_�k~�J��g0�-=�$��b�Vz��`��ApRz��|&fOC+�v��Ђ��p���L$u_۠���oZ�N�uN�)��u�7�~D�y_��q
0���U��.���R}2[�Im�<��Pv�ȟ�j^<P��~��T�i���d�h ����moZUw7�ӓ��6S�Ŭ�4EWe��.|q��� ^��M�,�P��S徍$>*ScZ%�:��Q����^d���*{ �����f&�@�r�:�� 4L
�N�d#��G=A� �V�(�m
ϒs�ヶҍ�K$�Dǀѭmc$P���N��	}�}2�$54�H��6�Q:@z��D��#zf:a��)�S�S\ �P�ĥ���W)HO�LS����.C%��[�ƺtO\9S�aq�]���f��<��J��,����̐��m=�1J�E/*��^e<zeZ��%C�7�yd����9�0N\�܌~�FȺy�?�t:P�c�����^�%�7$�oySVT��q8-v0����!<��������u_#��y��'����,T�S��Y�j����YiӸ�GJ,	J,F[g+��MV�@�� ���Gy�o\����b��h�yM��P��H���᧘��ǩ�ߤk��ɬU 輻R����wZ�kr��Q��w~�4�D�h�	,�&������AM�G!�$`!0޽��Bͫ�<���D��9�/��]3���������=u�rIx#�?��J�0�a3�����x����	��:b�&"�`T�<����i�&�S��:��3�=����Z�S�ڑ�qwvF?���*�&ݥ�̣���I�̾�j�mO�*�:��.ZD�������ͽf�
��-�щ�B�0XF�p�e%sR:O�6��SV��wЎ�'��ŝ�j�����/#�t�H��Rc}�H�#X�\dC���mԶ��{���������vw+�]��%�ɑ����F��4F嗻���ܧ��^8B�ig�]p�L����Hd���Ci�l�!�Sٍ�-
��b�ΞPo�Z����]��P�
'�Pt�
���6)�-\(f���F:�aΐ�(�F�b���[� �v��Y��"���7�ZH��j�v2Ӯ��aNTO��<�Ya �r���A���w�A0\�1��4��z�&PCMsOvUL��y�_#�޽[�J*$۴��b��n��A=��~� &+uϚ5J
Oq�Qo��`�=Ӊy�J/�Xh������݊W/�ߺ�_b���UH��� ��W�� Hڑ"��q�Ak2M0�Z�"ߡ��P�9��:p:�[H'
T��X$W�L���s�!.������[ޭv:����������O,�F�b@Ҕ#"]��87q
I��@I1�鸿6�p�u��
s�R��
[u�9�B'�K`��!�±�R�e��J��lw�����R��t/{���6�R,p��鉯宾1:�6F��%C|�����[L_��|�#�IOy\H�J�^dT� �|_�Ze/_�W���k���f)3!��+7T����o�C�}m�R|H�9��P��WM}To^cɝz\�$�U�2ME!�*tk~�:	���,�	�W�M��:t��S�@�1���
����g`�M���_l�@�(SA	/m#e�4��hkQs&��ݍt�? ڗ)5� 	�s��7�랐�9�Ȳ!�P�y���S�_��-�1�%�j��uȷ�ߺl�z3��2�D�g�/dk.��ԸA
(ߦ�K��k�O��T��5�>Og��8���C�j7���[;9��3�.�#c�c�,��'/���7I+���;Pb4������_pC��Ԉ��!�S�ˎf�
>�����R�C5�*�ő��b�G��cQ��[�i.�K%�\���M s�R���Z7V~�������٩�1��Z{�!�+�  �j$ݎu;��d,��c^�)liiN#f
� G���:K׵s������&|8�h&涋�󮏁���xdw�L߬ߪm�+_�r���aKaf��%7@�Ɋ4T�Q��Э�|�'���}�	p�H����!�s��I$\��+��� PL�"�mw��k�5�s�珴��2�!�Χ�Y�$���C�{�=���!���$���'��H��8�ocѱ����̨5�S,S�=���M �#� ���f�u*JM���
��>��#ž�9�Ju�=,�@��n̥���⋫<���ُ*=�l%�#-��w}� h�yx߽\i��J3p�uC�ޮ�~����e/e�-��9G_��Q�N�'nyr����Uxr�o�x�@�B���'rE̍�ZS S���|��jh��m�[+��x�k���P�ZJ[J6lP�]����*�&��P���?]D����Ŵ|֞�q��3�`˗����A��]g�Ru?�4\6Ҁ�t����ݪ�#��%�h�����H�<���Q���I4����u!�B�i'E�4�*�����
"HF�{
�h����+mRGN���9�T��g@i`�cZ��>$��[���)ٷ"|8`[l�|@���d�]��n��FsN���6�����
��XZ�2���VCSh(��f��)�!���á��JAA�RI��f�qh4ƷH���mB�e��#�C=�x�KGȦiTg�.VJ$7���b�70�fp:��҇�� ��g� Pl��[R(-m7��l��}&W�*���� w0l�7�Y��ϔw�t�T��?V����E�Æɖ�y�� Յ�r��֑�Y����h�wnQ��d�{���ɔC2+���-r7~����_\G��:m �P���� ��ҧ��ȸ3�j)D�m%�*鷎@U[�M�����,�Ҵ%���[���n?T��L�_��?k����3?�Bo�<���㇇X����m��ab�Srbkh?��H*X?iQL�Ի�{�n?���O� �Ks�5\�$sֿ�H��Q�$U�s�1�q����~�YAAu̅��%�nqK�d�쟓�c�nű]=��-���tp�f��FOe���"Kh��(~��^3�d>k�p�J���fJ ���{����:�>��� *%SOA�2&�Aʱ����t�9�ȑ$3���T̃���a��A���t�Q�6^�x����ъ�-a��t�]ˇ���J�k5��ʴ(M��7<d(��~s_�jbv�N\Ӣ��� �D�lz��r�"i�/}�w�Q(�n�p�_A�_�����R�k�[�U)Q�ڂ�_�:\��DD#�kS�t�� ~�Q%/fdR?n�s>hTx�4I�8|p�	���(�9q�vl����[��pQN�,��U��䇕CJ1g=��@T � K�ٝA���Rx�̓��o!�ؐ�XC�Ʀ�eijԈDDG�'�3[�A��_;8��~Ҙ1b�����BML꒶=����#���P�� QBɟ�|И0Bz!&�K�m�tȫ���#vI�I0�̂5�!:^g�� u�J'GsO�z���0�KOg����Pm�c�7������9-'�Lf���;
� ��`x�7�4��o��6��F�IqV�q�[	�hitٻ?�n/B���g�f����]��AǚQ�e^1ˉa��C����b�M[7��5��-�WP�,���6n�"����lsXN n��|d>��2�\%��#j�:��rImD�M-[~ZH�O�Of�@� '��>��OJ}++�(�U9�}��.�kf�x�
߽�QF��U�*��?q�P|�f?SQ�@1�GC|h�'n.�H�?�Dy	J,��_Ły)h��y���?|b^��;ӱ��W-��31�Jc�=�'���aY���҉K�!�~<D�q.ߪ�0�"i"�hH}�@�>X+���3)
m�$�"N;z��ꃥ���#�Z��.�;�y� ޯ�3�*
�a���?ɺs'�z-j����&��;U�ܜ�!Qď���~�F?5�9W��.A�E�SG����㵢�p�{��.����y�ǚ'	�פ4����t���CЮy�f�ýmM���^	�-���"*�Me<�+��j����-/�|�ٜ�a/�����(���4Y�� c��m
�V���+P#+Mi�f�z*n��Q�I�D���K�x��3�9&�l�$��4����h���7�ߔA�r�҃�N���.F�=9m�ET�r�q�
��]&��n	h��S�̐9�hVM�dy�P����m�����A/���b�N�BH}�㑺�~�.`�������Dŀ�/-���"�4��Q#w[s$�{��X.�v���%�~ܠ�ـke?7�ۮ
I�*��zr�H�ki��	6zjA22�]�5瀴D��b�~���N�9�ζ Elp�մ���=�XD������Y�^^w>=��x�P��,rIK������4��G!�~�
\d��7/QY'�ʫn�-�/)\{*�&���#g�n4�;���:���z����韺A�bu����N��#c�����dk���RE�i�i�6V1Pi�~r�b�l�I�9���X:�Μ/�*���(��U��䵼-�%��yU~Lt��M+���OA���3�qz�#����ѡj��q��A�;W�>�A�Ǭ��۟L�{����Fb�G="�ȶG��KWȎ�q���b<-�
�]�dp��qBT��N��/��y�����l����5g"���ߴ���'E�����@E)3�Jf2�W�����dzd�{ڊ��,��*�A.� ��&��=�(�C+�\����K�l|�����綖I۴PW�-.g.��	�f�J5Z�����������Z�6^H�ؚY���D��/t�V�V����<�C��#?2$b�b��'�P���}N-��M�@�x�e[4ETJ5��]\�5M��0� �^��,@�<�/�V��D�w4�l�|�n����Qy�����$�b���\�(_�p�Y��;s~��%�m�s|]ˆGϮRHF��Hd0Z�s��F!��Qh��	��B8��s�<��ܨ�#���g+6�^���;��I��� J�οq��2ƨ�H$2 �`V̐�5R��*Pmӓ��^�w��`�1 �Q�>�.1��r��
|"4ش&��>�u�B�a�M�>A'm�	m��U�lY����S�N�����na�����o�O��.��h��:��<��mB�u`��+��~|�tU��G�PL��v�|���6��5�-����!�0 $�Igj[j�>k���1J�&'`\�nj��7�y�����[֤��}�iǐ�Os3�nO�������!�pş��k�t\m2����C@,���LV�m|�m����W��<�!+���?���j��n�I�/��H���o�����ǧ���<��jF�����gO-2���u�
b����L
&sb?�N����Ʋ��u����~i��WZ�&"K:�2�FН�0��{���s�0f�Тg9\G�@��ᓰ���`�ű�b�\��%�M�Ǥ0��G��%��`=\gY�����u!�ƹ�<?�٦��"�y�dw"����o�>�艡�(��*���9�(<T�ji�nm��ޫq�gu��B��ѹ[O2��Css\���S������a���1�P!�5i���?a�n�P���&6ۦ����d1�#6�� =�	���������6�9�ؘq��h�Y��?��hzՇZ���@p��� �FR�q����jwn%�C���g�U�5&'�'��&�px�m� *.aN�D�P�4��P�@0�"����Ұ鹜��
�	�r��:/ӓ?w�����`pNz��~�.�\O��G�qn$Ad�:m��B�Jt�<�2#2ƨ�sޤ����	��?���=�\{� �Al����h����D<��{��9�hi�ID)��b�F�5�:�N8>z��A;h����2��})WA �QՑ��|���a��L�l�X�S����;h��ҝ~ߟT!��rw$����ç�lݔ�K�%�ѿ)����Do��Zu[����f>(��6	܉�{��X�:�"��O�P��\�� ���xp�k�{��4~i�V�Y�s��*�E�.��Ĝr�.�'���B"� �*�!�j�	�C8-9vvpL��%�.�UD�A$ۻ6{�;��%����,�^�M��,N��h�0��a*IT�	L��Lt'��?�}R�u�~��d_��]NTv�EL�V�w����eu�MYO5���C�W��_��o^��$�I�n˰L��ug�T�;p�,��l�,��dE��Iu���{�{g��g�i3A�n%������
pfV�?t�@(%�;ö�{�8���R�&����w|�q�uMk\]
�^����%�o��&���+JՋ�	G���_����� I��p5��ޑĵ�: ��	����X��2����)+P?V�W-v/I7-k?�&�@�"qIg���$�� r ����C�[n�6�FLj�����<qx�?	���bQq�t1+�� �T�0�\Rs���ZZ��C�X���D(0����+�S[����Ɇ�J���b�S�=�����m�t��7�� ��`Q�^y	���K�!gVl��������2� �7`?�'vٖ��/��n���Z��&��q}�����ݼ�k���ڊ<�g�Re��yw�$q��� �}4r�'��v�"�^�f�؁���Q�S�jrF�ր��1=t���'��.���� D�E_ߐï��w��&�@��|���^��I�+�`�I�4B�"��K������Y+���J�*t���\rChJ�$E:�s��E��w��v����w���?/�qeFx���ڸW�0�X��A�W�|�����kH�'/��T��w<Θ��-���"J��` v���ǭ'Y�ǇUx�q�[��XMl�{a�@\�M1���"Tn�J�o�щ�Z9z��"���9]d�l�+>��4�F��>-����e��!��A��ӛ�����vd/eJ:�Ѭʯ��BȒ����O������{�G{eT���Օ�H��v"^�A��%�(ex� u%#_���l����8A���l��3�ID���<�	���gܜ	��1\p�,k��?7_3�)�Z2`�҂Ӭ��L����� ��f7���<�o�؍�V!����q�0ق�C���y?m�q��{&o���$�Q�5�7��ȉ�%r�,d�aX��g^�0g��To}������lߚ�P[u ������0��� }Ծ�վDZ�V���Up��_D_T���K�M��ߚp*�@r��u��"!V$�F�pB���~��c{���T\8Le>�;����	���b(���VtQ-�+���@>�.���4�.��	�즩"�
�@|׌��3H�F�a@��|hǑ����Ge7!MG������t����(��~�Yf)��n����
< ˰x�Q$a�
�|����'>��?�
3;#����_A�7����Q2����� <Ÿ�P��O>���[�%���oԆ�����0�dae_*
G�� l��u$������'�-p�{��]Ɖc8Yό|�.��f/t�bB"�J���ߒ����=%�x����UGbM�����~����f��dp0�#�Y�P����c��1�j1
K��E������l�*4�R��
�ꤿ�4j�8�R�&R��?�!i�L[y�3D�~�~d8E��o0�&~J#w	,bW�0w�4K�P�h����Pq|�#/[��"��Zx[�T8�w#��|G	���5���-��%��,�q1�5�6ͥC����My�B���}�m|BS��V��@T�r��XCQ���b�`�BiU��%�b]�6[c�!?����W�˼����̞�;��KN�����i��c]��XN�}*BeX�?��ƠNGw��_��0�p�1�0͟n��	6��{�`d�;.�ђVM�T������<���9�+V.,jQ.�̀���&S2�=�]����40p���cU��!�Rr��ur΃ ��j������������矁;�(��M���)������\����ڡۧ陿leI���Y���͑���G7ڙn�0D/��J��e��{>�zR-���>V��G�/�︰
ED����P-,\�r�^���Q?���<t�46��T�r�ٱS���ͲƝCU9�1%/Zb△��r�ʆ�+j@������0%Z&N�ttEς����k��S2�,����ⷑ�X��K�@��w���ct7;�=ic�r��`��H<�
no��.V��&�ڇ�P�g��A�m�A��Fs�U�zժ�$Z��<e���)8��D�آ��~��y����Ja7)��=�l��9�(~�7�a��Ċ1� cϑ: ���$꩜��QG�f�t*���6��ɯ�R�jH�`D�Wd�ac����QĆ����E|�����7Pk�,?�V��1f;���Nűc�����)gj}W㣑j��8'�	H?�U�ֿ`\z�ea�n&E~ -	�����2�.ϯ,eH�ц�
�6,sVX�Ʃ2@�i/��;c�r�*�K��l�����AX��8�=��7r��V���eO��Y�)�}�2Aګ/6
{n����B [ym��igi75)
��
,��(���`�]����� �-�Z�/^�9�.߮���ڀ�Y=��~[6�pu��g�!��N�&p�VboN�t&�����T�Kܬ9z������=�.���𫀍b�NA�a� ��j������M�@H#�U�c3� !;�j�U�D��(�U�1�L�*T`�8�y�T����=�$����Y7Omء�W���_em�B�|�G��k����zn�c�">���-�$��Udv�nɯ�3���8*=���`;�F�/�]�
ϭ��%�����n�˱��D�a�\`�#Iv�e��Eʴ��*Z�,.��r�OX�:f�?UQ �Q�i�)�Q���_����8�~Mo��Ӥ��EJ�ڸ��춦E�N���#��̰�|��D��|�E����_�	������SDm����O�Tis�tx�#��<�v>hH�O���p����N;��gl����n.L��R`�݄:;�Zw��Q곪��2���A�h��6M!Z�A�q���hʥc�Edy�ǙO>��Vy�)v�X�Ͻ<�n`�z�6���l����|~C�7>��W����e�2��Rd�wH�*�XU��B��Jw���Qlz�l����$ǥ�0ɺ���+!žyG�L�E��`k���#();
}

/*
 * Print out an error if an invalid bit is set:
 */
static inline int
valid_state(struct task_struct *curr, struct held_lock *this,
	    enum lock_usage_bit new_bit, enum lock_usage_bit bad_bit)
{
	if (unlikely(hlock_class(this)->usage_mask & (1 << bad_bit))) {
		graph_unlock();
		print_usage_bug(curr, this, bad_bit, new_bit);
		return 0;
	}
	return 1;
}


/*
 * print irq inversion bug:
 */
static void
print_irq_inversion_bug(struct task_struct *curr,
			struct lock_list *root, struct lock_list *other,
			struct held_lock *this, int forwards,
			const char *irqclass)
{
	struct lock_list *entry = other;
	struct lock_list *middle = NULL;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("========================================================\n");
	pr_warn("WARNING: possible irq lock inversion dependency detected\n");
	print_kernel_ident();
	pr_warn("--------------------------------------------------------\n");
	pr_warn("%s/%d just changed the state of lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(this);
	if (forwards)
		pr_warn("but this lock took another, %s-unsafe lock in the past:\n", irqclass);
	else
		pr_warn("but this lock was taken by another, %s-safe lock in the past:\n", irqclass);
	print_lock_name(other->class);
	pr_warn("\n\nand interrupts could create inverse lock ordering between them.\n\n");

	pr_warn("\nother info that might help us debug this:\n");

	/* Find a middle lock (if one exists) */
	depth = get_lock_depth(other);
	do {
		if (depth == 0 && (entry != root)) {
			pr_warn("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}
		middle = entry;
		entry = get_lock_parent(entry);
		depth--;
	} while (entry && entry != root && (depth >= 0));
	if (forwards)
		print_irq_lock_scenario(root, other,
			middle ? middle->class : root->class, other->class);
	else
		print_irq_lock_scenario(other, root,
			middle ? middle->class : other->class, root->class);

	lockdep_print_held_locks(curr);

	pr_warn("\nthe shortest dependencies between 2nd lock and 1st lock:\n");
	root->trace = save_trace();
	if (!root->trace)
		return;
	print_shortest_lock_dependencies(other, root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

/*
 * Prove that in the forwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_forwards(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_root(&root, this);
	ret = find_usage_forwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 1, state_name(read_bit));
	}

	return 0;
}

/*
 * Prove that in the backwards-direction subgraph starting at <this>
 * there is no lock matching <mask>:
 */
static int
check_usage_backwards(struct task_struct *curr, struct held_lock *this,
		      enum lock_usage_bit bit)
{
	enum bfs_result ret;
	struct lock_list root;
	struct lock_list *target_entry;
	enum lock_usage_bit read_bit = bit + LOCK_USAGE_READ_MASK;
	unsigned usage_mask = lock_flag(bit) | lock_flag(read_bit);

	bfs_init_rootb(&root, this);
	ret = find_usage_backwards(&root, usage_mask, &target_entry);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/* Check whether write or read usage is the match */
	if (target_entry->class->usage_mask & lock_flag(bit)) {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(bit));
	} else {
		print_irq_inversion_bug(curr, &root, target_entry,
					this, 0, state_name(read_bit));
	}

	return 0;
}

void print_irqtrace_events(struct task_struct *curr)
{
	const struct irqtrace_events *trace = &curr->irqtrace;

	printk("irq event stamp: %u\n", trace->irq_events);
	printk("hardirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->hardirq_enable_event, (void *)trace->hardirq_enable_ip,
		(void *)trace->hardirq_enable_ip);
	printk("hardirqs last disabled at (%u): [<%px>] %pS\n",
		trace->hardirq_disable_event, (void *)trace->hardirq_disable_ip,
		(void *)trace->hardirq_disable_ip);
	printk("softirqs last  enabled at (%u): [<%px>] %pS\n",
		trace->softirq_enable_event, (void *)trace->softirq_enable_ip,
		(void *)trace->softirq_enable_ip);
	printk("softirqs last disabled at (%u): [<%px>] %pS\n",
		trace->softirq_disable_event, (void *)trace->softirq_disable_ip,
		(void *)trace->softirq_disable_ip);
}

static int HARDIRQ_verbose(struct lock_class *class)
{
#if HARDIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int SOFTIRQ_verbose(struct lock_class *class)
{
#if SOFTIRQ_VERBOSE
	return class_filter(class);
#endif
	return 0;
}

static int (*state_verbose_f[])(struct lock_class *class) = {
#define LOCKDEP_STATE(__STATE) \
	__STATE##_verbose,
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline int state_verbose(enum lock_usage_bit bit,
				struct lock_class *class)
{
	return state_verbose_f[bit >> LOCK_USAGE_DIR_MASK](class);
}

typedef int (*check_usage_f)(struct task_struct *, struct held_lock *,
			     enum lock_usage_bit bit, const char *name);

static int
mark_lock_irq(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit new_bit)
{
	int excl_bit = exclusive_bit(new_bit);
	int read = new_bit & LOCK_USAGE_READ_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * Validate that this particular lock does not have conflicting
	 * usage states.
	 */
	if (!valid_state(curr, this, new_bit, excl_bit))
		return 0;

	/*
	 * Check for read in write conflicts
	 */
	if (!read && !valid_state(curr, this, new_bit,
				  excl_bit + LOCK_USAGE_READ_MASK))
		return 0;


	/*
	 * Validate that the lock dependencies don't have conflicting usage
	 * states.
	 */
	if (dir) {
		/*
		 * mark ENABLED has to look backwards -- to ensure no dependee
		 * has USED_IN state, which, again, would allow  recursion deadlocks.
		 */
		if (!check_usage_backwards(curr, this, excl_bit))
			return 0;
	} else {
		/*
		 * mark USED_IN has to look forwards -- to ensure no dependency
		 * has ENABLED state, which would allow recursion deadlocks.
		 */
		if (!check_usage_forwards(curr, this, excl_bit))
			return 0;
	}

	if (state_verbose(new_bit, hlock_class(this)))
		return 2;

	return 1;
}

/*
 * Mark all held locks with a usage bit:
 */
static int
mark_held_locks(struct task_struct *curr, enum lock_usage_bit base_bit)
{
	struct held_lock *hlock;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		enum lock_usage_bit hlock_bit = base_bit;
		hlock = curr->held_locks + i;

		if (hlock->read)
			hlock_bit += LOCK_USAGE_READ_MASK;

		BUG_ON(hlock_bit >= LOCK_USAGE_STATES);

		if (!hlock->check)
			continue;

		if (!mark_lock(curr, hlock, hlock_bit))
			return 0;
	}

	return 1;
}

/*
 * Hardirqs will be enabled:
 */
static void __trace_hardirqs_on_caller(void)
{
	struct task_struct *curr = current;

	/*
	 * We are going to turn hardirqs on, so set the
	 * usage bit for all held locks:
	 */
	if (!mark_held_locks(curr, LOCK_ENABLED_HARDIRQ))
		return;
	/*
	 * If we have softirqs enabled, then set the usage
	 * bit for all held locks. (disabled hardirqs prevented
	 * this bit from being set before)
	 */
	if (curr->softirqs_enabled)
		mark_held_locks(curr, LOCK_ENABLED_SOFTIRQ);
}

/**
 * lockdep_hardirqs_on_prepare - Prepare for enabling interrupts
 * @ip:		Caller address
 *
 * Invoked before a possible transition to RCU idle from exit to user or
 * guest mode. This ensures that all RCU operations are done before RCU
 * stops watching. After the RCU transition lockdep_hardirqs_on() has to be
 * invoked to set the final state.
 */
void lockdep_hardirqs_on_prepare(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs do not (and cannot) track lock dependencies, nothing to do.
	 */
	if (unlikely(in_nmi()))
		return;

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (unlikely(lockdep_hardirqs_enabled())) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * See the fine text that goes along with this variable definition.
	 */
	if (DEBUG_LOCKS_WARN_ON(early_boot_irqs_disabled))
		return;

	/*
	 * Can't allow enabling interrupts while in an interrupt handler,
	 * that's general bad form and such. Recursion, limited stack etc..
	 */
	if (DEBUG_LOCKS_WARN_ON(lockdep_hardirq_context()))
		return;

	current->hardirq_chain_key = current->curr_chain_key;

	lockdep_recursion_inc();
	__trace_hardirqs_on_caller();
	lockdep_recursion_finish();
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on_prepare);

void noinstr lockdep_hardirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!debug_locks))
		return;

	/*
	 * NMIs can happen in the middle of local_irq_{en,dis}able() where the
	 * tracking state and hardware state are out of sync.
	 *
	 * NMIs must save lockdep_hardirqs_enabled() to restore IRQ state from,
	 * and not rely on hardware state like normal interrupts.
	 */
	if (unlikely(in_nmi())) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;

		/*
		 * Skip:
		 *  - recursion check, because NMI can hit lockdep;
		 *  - hardware state check, because above;
		 *  - chain_key check, see lockdep_hardirqs_on_prepare().
		 */
		goto skip_checks;
	}

	if (unlikely(this_cpu_read(lockdep_recursion)))
		return;

	if (lockdep_hardirqs_enabled()) {
		/*
		 * Neither irq nor preemption are disabled here
		 * so this is racy by nature but losing one hit
		 * in a stat is not a big deal.
		 */
		__debug_atomic_inc(redundant_hardirqs_on);
		return;
	}

	/*
	 * We're enabling irqs and according to our state above irqs weren't
	 * already enabled, yet we find the hardware thinks they are in fact
	 * enabled.. someone messed up their IRQ state tracing.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	/*
	 * Ensure the lock stack remained unchanged between
	 * lockdep_hardirqs_on_prepare() and lockdep_hardirqs_on().
	 */
	DEBUG_LOCKS_WARN_ON(current->hardirq_chain_key !=
			    current->curr_chain_key);

skip_checks:
	/* we'll do an OFF -> ON transition: */
	__this_cpu_write(hardirqs_enabled, 1);
	trace->hardirq_enable_ip = ip;
	trace->hardirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(hardirqs_on_events);
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_on);

/*
 * Hardirqs were disabled:
 */
void noinstr lockdep_hardirqs_off(unsigned long ip)
{
	if (unlikely(!debug_locks))
		return;

	/*
	 * Matching lockdep_hardirqs_on(), allow NMIs in the middle of lockdep;
	 * they will restore the software state. This ensures the software
	 * state is consistent inside NMIs as well.
	 */
	if (in_nmi()) {
		if (!IS_ENABLED(CONFIG_TRACE_IRQFLAGS_NMI))
			return;
	} else if (__this_cpu_read(lockdep_recursion))
		return;

	/*
	 * So we're supposed to get called after you mask local IRQs, but for
	 * some reason the hardware doesn't quite think you did a proper job.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (lockdep_hardirqs_enabled()) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		__this_cpu_write(hardirqs_enabled, 0);
		trace->hardirq_disable_ip = ip;
		trace->hardirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(hardirqs_off_events);
	} else {
		debug_atomic_inc(redundant_hardirqs_off);
	}
}
EXPORT_SYMBOL_GPL(lockdep_hardirqs_off);

/*
 * Softirqs will be enabled:
 */
void lockdep_softirqs_on(unsigned long ip)
{
	struct irqtrace_events *trace = &current->irqtrace;

	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c, avoids
	 * funny state and nesting things.
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		debug_atomic_inc(redundant_softirqs_on);
		return;
	}

	lockdep_recursion_inc();
	/*
	 * We'll do an OFF -> ON transition:
	 */
	current->softirqs_enabled = 1;
	trace->softirq_enable_ip = ip;
	trace->softirq_enable_event = ++trace->irq_events;
	debug_atomic_inc(softirqs_on_events);
	/*
	 * We are going to turn softirqs on, so set the
	 * usage bit for all held locks, if hardirqs are
	 * enabled too:
	 */
	if (lockdep_hardirqs_enabled())
		mark_held_locks(current, LOCK_ENABLED_SOFTIRQ);
	lockdep_recursion_finish();
}

/*
 * Softirqs were disabled:
 */
void lockdep_softirqs_off(unsigned long ip)
{
	if (unlikely(!lockdep_enabled()))
		return;

	/*
	 * We fancy IRQs being disabled here, see softirq.c
	 */
	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return;

	if (current->softirqs_enabled) {
		struct irqtrace_events *trace = &current->irqtrace;

		/*
		 * We have done an ON -> OFF transition:
		 */
		current->softirqs_enabled = 0;
		trace->softirq_disable_ip = ip;
		trace->softirq_disable_event = ++trace->irq_events;
		debug_atomic_inc(softirqs_off_events);
		/*
		 * Whoops, we wanted softirqs off, so why aren't they?
		 */
		DEBUG_LOCKS_WARN_ON(!softirq_count());
	} else
		debug_atomic_inc(redundant_softirqs_off);
}

static int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	if (!check)
		goto lock_used;

	/*
	 * If non-trylock use in a hardirq or softirq context, then
	 * mark the lock as used in these contexts:
	 */
	if (!hlock->trylock) {
		if (hlock->read) {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_HARDIRQ_READ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock,
						LOCK_USED_IN_SOFTIRQ_READ))
					return 0;
		} else {
			if (lockdep_hardirq_context())
				if (!mark_lock(curr, hlock, LOCK_USED_IN_HARDIRQ))
					return 0;
			if (curr->softirq_context)
				if (!mark_lock(curr, hlock, LOCK_USED_IN_SOFTIRQ))
					return 0;
		}
	}
	if (!hlock->hardirqs_off) {
		if (hlock->read) {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ_READ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ_READ))
					return 0;
		} else {
			if (!mark_lock(curr, hlock,
					LOCK_ENABLED_HARDIRQ))
				return 0;
			if (curr->softirqs_enabled)
				if (!mark_lock(curr, hlock,
						LOCK_ENABLED_SOFTIRQ))
					return 0;
		}
	}

lock_used:
	/* mark it as used: */
	if (!mark_lock(curr, hlock, LOCK_USED))
		return 0;

	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return LOCK_CHAIN_HARDIRQ_CONTEXT * !!lockdep_hardirq_context() +
	       LOCK_CHAIN_SOFTIRQ_CONTEXT * !!task->softirq_context;
}

static int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	unsigned int depth = curr->lockdep_depth;

	/*
	 * Keep track of points where we cross into an interrupt context:
	 */
	if (depth) {
		struct held_lock *prev_hlock;

		prev_hlock = curr->held_locks + depth-1;
		/*
		 * If we cross into another context, reset the
		 * hash key (this also prevents the checking and the
		 * adding of the dependency to 'prev'):
		 */
		if (prev_hlock->irq_context != hlock->irq_context)
			return 1;
	}
	return 0;
}

/*
 * Mark a lock with a usage bit, and validate the state transition:
 */
static int mark_lock(struct task_struct *curr, struct held_lock *this,
			     enum lock_usage_bit new_bit)
{
	unsigned int new_mask, ret = 1;

	if (new_bit >= LOCK_USAGE_STATES) {
		DEBUG_LOCKS_WARN_ON(1);
		return 0;
	}

	if (new_bit == LOCK_USED && this->read)
		new_bit = LOCK_USED_READ;

	new_mask = 1 << new_bit;

	/*
	 * If already set then do not dirty the cacheline,
	 * nor do any checks:
	 */
	if (likely(hlock_class(this)->usage_mask & new_mask))
		return 1;

	if (!graph_lock())
		return 0;
	/*
	 * Make sure we didn't race:
	 */
	if (unlikely(hlock_class(this)->usage_mask & new_mask))
		goto unlock;

	if (!hlock_class(this)->usage_mask)
		debug_atomic_dec(nr_unused_locks);

	hlock_class(this)->usage_mask |= new_mask;

	if (new_bit < LOCK_TRACE_STATES) {
		if (!(hlock_class(this)->usage_traces[new_bit] = save_trace()))
			return 0;
	}

	if (new_bit < LOCK_USED) {
		ret = mark_lock_irq(curr, this, new_bit);
		if (!ret)
			return 0;
	}

unlock:
	graph_unlock();

	/*
	 * We must printk outside of the graph_lock:
	 */
	if (ret == 2) {
		printk("\nmarked lock as {%s}:\n", usage_str[new_bit]);
		print_lock(this);
		print_irqtrace_events(curr);
		dump_stack();
	}

	return ret;
}

static inline short task_wait_context(struct task_struct *curr)
{
	/*
	 * Set appropriate wait type for the context; for IRQs we have to take
	 * into account force_irqthread as that is implied by PREEMPT_RT.
	 */
	if (lockdep_hardirq_context()) {
		/*
		 * Check if force_irqthreads will run us threaded.
		 */
		if (curr->hardirq_threaded || curr->irq_config)
			return LD_WAIT_CONFIG;

		return LD_WAIT_SPIN;
	} else if (curr->softirq_context) {
		/*
		 * Softirqs are always threaded.
		 */
		return LD_WAIT_CONFIG;
	}

	return LD_WAIT_MAX;
}

static int
print_lock_invalid_wait_context(struct task_struct *curr,
				struct held_lock *hlock)
{
	short curr_inner;

	if (!debug_locks_off())
		return 0;
	if (debug_locks_silent)
		return 0;

	pr_warn("\n");
	pr_warn("=============================\n");
	pr_warn("[ BUG: Invalid wait context ]\n");
	print_kernel_ident();
	pr_warn("-----------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("other info that might help us debug this:\n");

	curr_inner = task_wait_context(curr);
	pr_warn("context-{%d:%d}\n", curr_inner, curr_inner);

	lockdep_print_held_locks(curr);

	pr_warn("stack backtrace:\n");
	dump_stack();

	return 0;
}

/*
 * Verify the wait_type context.
 *
 * This check validates we take locks in the right wait-type order; that is it
 * ensures that we do not take mutexes inside spinlocks and do not attempt to
 * acquire spinlocks inside raw_spinlocks and the sort.
 *
 * The entire thing is slightly more complex because of RCU, RCU is a lock that
 * can be taken from (pretty much) any context but also has constraints.
 * However when taken in a stricter environment the RCU lock does not loosen
 * the constraints.
 *
 * Therefore we must look for the strictest environment in the lock stack and
 * compare that to the lock we're trying to acquire.
 */
static int check_wait_context(struct task_struct *curr, struct held_lock *next)
{
	u8 next_inner = hlock_class(next)->wait_type_inner;
	u8 next_outer = hlock_class(next)->wait_type_outer;
	u8 curr_inner;
	int depth;

	if (!next_inner || next->trylock)
		return 0;

	if (!next_outer)
		next_outer = next_inner;

	/*
	 * Find start of current irq_context..
	 */
	for (depth = curr->lockdep_depth - 1; depth >= 0; depth--) {
		struct held_lock *prev = curr->held_locks + depth;
		if (prev->irq_context != next->irq_context)
			break;
	}
	depth++;

	curr_inner = task_wait_context(curr);

	for (; depth < curr->lockdep_depth; depth++) {
		struct held_lock *prev = curr->held_locks + depth;
		u8 prev_inner = hlock_class(prev)->wait_type_inner;

		if (prev_inner) {
			/*
			 * We can have a bigger inner than a previous one
			 * when outer is smaller than inner, as with RCU.
			 *
			 * Also due to trylocks.
			 */
			curr_inner = min(curr_inner, prev_inner);
		}
	}

	if (next_outer > curr_inner)
		return print_lock_invalid_wait_context(curr, next);

	return 0;
}

#else /* CONFIG_PROVE_LOCKING */

static inline int
mark_usage(struct task_struct *curr, struct held_lock *hlock, int check)
{
	return 1;
}

static inline unsigned int task_irq_context(struct task_struct *task)
{
	return 0;
}

static inline int separate_irq_context(struct task_struct *curr,
		struct held_lock *hlock)
{
	return 0;
}

static inline int check_wait_context(struct task_struct *curr,
				     struct held_lock *next)
{
	return 0;
}

#endif /* CONFIG_PROVE_LOCKING */

/*
 * Initialize a lock instance's lock-class mapping info:
 */
void lockdep_init_map_type(struct lockdep_map *lock, const char *name,
			    struct lock_class_key *key, int subclass,
			    u8 inner, u8 outer, u8 lock_type)
{
	int i;

	for (i = 0; i < NR_LOCKDEP_CACHING_CLASSES; i++)
		lock->class_cache[i] = NULL;

#ifdef CONFIG_LOCK_STAT
	lock->cpu = raw_smp_processor_id();
#endif

	/*
	 * Can't be having no nameless bastards around this place!
	 */
	if (DEBUG_LOCKS_WARN_ON(!name)) {
		lock->name = "NULL";
		return;
	}

	lock->name = name;

	lock->wait_type_outer = outer;
	lock->wait_type_inner = inner;
	lock->lock_type = lock_type;

	/*
	 * No key, no joy, we need to hash something.
	 */
	if (DEBUG_LOCKS_WARN_ON(!key))
		return;
	/*
	 * Sanity check, the lock-class key must either have been allocated
	 * statically or must have been registered as a dynamic key.
	 */
	if (!static_obj(key) && !is_dynamic_key(key)) {
		if (debug_locks)
			printk(KERN_ERR "BUG: key %px has not been registered!\n", key);
		DEBUG_LOCKS_WARN_ON(1);
		return;
	}
	lock->key = key;

	if (unlikely(!debug_locks))
		return;

	if (subclass) {
		unsigned long flags;

		if (DEBUG_LOCKS_WARN_ON(!lockdep_enabled()))
			return;

		raw_local_irq_save(flags);
		lockdep_recursion_inc();
		register_lock_class(lock, subclass, 1);
		lockdep_recursion_finish();
		raw_local_irq_restore(flags);
	}
}
EXPORT_SYMBOL_GPL(lockdep_init_map_type);

struct lock_class_key __lockdep_no_validate__;
EXPORT_SYMBOL_GPL(__lockdep_no_validate__);

static void
print_lock_nested_lock_not_held(struct task_struct *curr,
				struct held_lock *hlock,
				unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("==================================\n");
	pr_warn("WARNING: Nested lock was not taken\n");
	print_kernel_ident();
	pr_warn("----------------------------------\n");

	pr_warn("%s/%d is trying to lock:\n", curr->comm, task_pid_nr(curr));
	print_lock(hlock);

	pr_warn("\nbut this task is not holding:\n");
	pr_warn("%s\n", hlock->nest_lock->name);

	pr_warn("\nstack backtrace:\n");
	dump_stack();

	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static int __lock_is_held(const struct lockdep_map *lock, int read);

/*
 * This gets called for every mutex_lock*()/spin_lock*() operation.
 * We maintain the dependency maps and validate the locking attempt:
 *
 * The callers must make sure that IRQs are disabled before calling it,
 * otherwise we could get an interrupt which would want to take locks,
 * which would end up in lockdep again.
 */
static int __lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check, int hardirqs_off,
			  struct lockdep_map *nest_lock, unsigned long ip,
			  int references, int pin_count)
{
	struct task_struct *curr = current;
	struct lock_class *class = NULL;
	struct held_lock *hlock;
	unsigned int depth;
	int chain_head = 0;
	int class_idx;
	u64 chain_key;

	if (unlikely(!debug_locks))
		return 0;

	if (!prove_locking || lock->key == &__lockdep_no_validate__)
		check = 0;

	if (subclass < NR_LOCKDEP_CACHING_CLASSES)
		class = lock->class_cache[subclass];
	/*
	 * Not cached?
	 */
	if (unlikely(!class)) {
		class = register_lock_class(lock, subclass, 0);
		if (!class)
			return 0;
	}

	debug_class_ops_inc(class);

	if (very_verbose(class)) {
		printk("\nacquire class [%px] %s", class->key, class->name);
		if (class->name_version > 1)
			printk(KERN_CONT "#%d", class->name_version);
		printk(KERN_CONT "\n");
		dump_stack();
	}

	/*
	 * Add the lock to the list of currently held locks.
	 * (we dont increase the depth just yet, up until the
	 * dependency checks are done)
	 */
	depth = curr->lockdep_depth;
	/*
	 * Ran out of static storage for our per-task lock stack again have we?
	 */
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX_LOCK_DEPTH))
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

	hlock = c