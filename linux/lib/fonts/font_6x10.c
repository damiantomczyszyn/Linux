d include/config/GENERIC_IOMAP) \
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
  include/media/i2c/upd64083.h \

drivers/media/i2c/upd64083.o: $(deps_drivers/media/i2c/upd64083.o)

$(deps_drivers/media/i2c/upd64083.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    cmd_kernel/bpf/cgroup.o := gcc -Wp,-MMD,kernel/bpf/.cgroup.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/bpf/cgroup"' -DKBUILD_BASENAME='"cgroup"' -DKBUILD_MODNAME='"cgroup"' -D__KBUILD_MODNAME=kmod_cgroup -c -o kernel/bpf/cgroup.o kernel/bpf/cgroup.c 

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
  arch�ی��§� ��i�g�g]����3;m�"���W]�.XϢ1�@���K�wB�h2���	0[������@��[Հ{�آ���x��H�A�Q~rb)�Ao��X�+���{�P��fÒ����q$�6dq%G4d��)� G
��%<�n��9ŕ�q�E�^ǦA�GW��-`pw�����Ɏ��vu\�<�H���-B熩��&&�GFF�ô��b�Z�X��iA����t˶B��&$�l^��2p�*�*�'L�/>���\p��{�~�i������p��&^�"�˦��S���rD������l����"���2V�%R�HHnGrv;�P�8��s�3�u[��n05�J�a�R;��:#w�}����T�/�'Z�vs9T�������=����O�:��[^$��������������~�z�(.���V(na	���m[�w=-����޽����
Ȥ@�5F.���ڶQ�:s�!��>f����A;��i��҄��]��Z��s\�Ұ����'��˩iLX�M�IK�	������p��E���s�r\Փi�v�g��zC��z٠a
�Rss����G��+M4H�qW�n�&�����JF���j�Djj�	p̸�7K�[��ɤ!��1�?��<[:�*^>��=�^r�8���j�4�Y����N����ؼ��:�9��Rq"%��A�����ݼ �;ɚ
��y���^�����1���$�
QnMrw�4�V0���G������ξ��(vJ]��H�RӃ�[�љ�}�=-h1�'���R�֋x��:��%]�Gn<�[e�����ڮ^5>$��>�"�p�ʠ�9+�D���2���7|��\3\��?&�BC�
������)?��vMK�a5ʤW�ځ�W�������GV
��̟Ym"͆�'��z�t:�)4��$��,�P��>b�^R���u��V)�u�:��Gg�j�s<nt7��O��@�֍��Ӿ�,������8Ta&l����k�m薈R�q?N.�x��cD8%-]��!EgX�_�FÃ޶tnM
T`~���oᛃ�����Pb z@Cu�G�*��g,��C��ՠh}>Xz�2���q�U!(>��l�3������
Τ��	jS�����W�Է��y:�f{]0D��1��=��������,K=����uh�x��x&5�n����}I��,"A�X^C��A!�V�eQp��`�!�k�ktl�}	��s��p$jD[H�]����H!��\�0�����74"懭�E��y��4p��gs��*�dܒ��³+P���w\�Ne�dK�������i)N��tql�~���8����Ϟ_�d+N�$%�8̯��<���ߦ�&Ni�L[��_"���ݤ�[s��e=�2E{�CA4�X���$�I9�Amt��h�E��k�~>%�s���A��Ioq��m���e�΍�F��yX�	�-u@���0���w|K`Fkx�ESB��G}͸١Z,j�� H����5.�a.-�EՒ�-��*l�guy�b����e�L��ɳ���x*��4�:���GO���p?�*�di����Dƽ�+��{�����O��m\NYm@+��G����B�,e�>a��x7��|g��(�k�<�E/�Ë��W6ۛ�՝q�6(C�=�S��<+;�ߧ�o*�f��m��Ϊ�avl&p��^-��?�O�Sf2�d]x �_�
F>%�&�s��ɞ鎭�:�F<5{,�(�(A�l�b���$��E�s�^ ͟��DB�f��y�{��P�c�,|ϖL�g5��6��,5�>�<�T�3�2�'&Z"���4q�4��9�Dة���
���u�u�T�u�Ӏ6o�ɦ�r4A��)Hm�Ǔ�x�_U'h�	�a����M��3�/m`?����� �f(}'G�\��Ӱ�ڢ7�c��
ZBC΁����+4�WSe"�Ap�Y���=I��TxQ��V'}p��ў�PZ=O��!�^h�<��c��2x���A1��I#�7���5S������祎�j����N �����h:ɁK�U$�5=�9eаF��}|��8h��`š\~�߾�����x���X:;��КϾ�g'x�Y�Lo4�5~4�?k@D]�����⧼��vM�Eer�	��%��Oc���;kM�`~8��6t���?f�+�B����e�sr�NTN���b�x�>�Tx�#��ܫF��o��6-���g��qZ�HľY�@��v�惌����4��GԏA�2D�3����p.6}S_12�����Z�Ʀ���O`<�>h��)��GT!O�ٞ;��)�\����d����vb#���dP�L��WSw�j; h^�di �vQ�Y��j��y�M%����C�~N}L�GZ�AH�RЉ���œ�w���y�m�b �`�s���Oފ�L֛�s(����ꓗK[L�6f�8��ms��)5��_��>窼�F�.���`}G��rv@ld�6������?UU��Ca۬1�,��-0��tT�Nl�@�� D̗V4���;�ADu�/q��L�6X������9w��A2����`W������u.�=���!.pi���fc�������lua4@�@�]`���8V-���5�>�����{��5"!X�o���=��T,4��&��g�J&�t����fz�$���1Җ�����SA���8� �<�ɐӟ�D�uM�w`C�5�䙁��8��n�*0�|�������,�h�u�MV.|h)Q�Fc1$9��w��X���m�봞������F@1U�M�=���,��0B_,3�=V������q%퍒R4��}|%#��^&,���|i{��Lz7����gb�7`Kls���_,ScŸ�m�4#����8��@�w�����6\�@�F0mܟO8���u��4�ry)�{�YLw�jDM=2�ߒ?� i���sF1h�= iL�!�{pV� �jTȤ���Rb��Y�lH9�> s�(�Jɘ7iO�_����]��DsRPP#T#LX�ʟ<R��sN����$�*��e�pz�P�T9]5�l����e2?*�f3�g�ϣ�S0B��m���N=��k[�� �#$��pbO�QԥJ�n���}j�W�1�/��b*����yt-��k�s��M��R'��9�HMv�>���p㌠�3��ܲ���D�Ʋ��3������
�q���K��X�� �����MG�w&E.4 �>E�xZ��!.�&���{�>�_�`h�)lUD��qƴ�I<>�P�I�E5�]Y	It��1�îŎ$��]��-G�	�7�Q�;L2Ƥ ���3�p���t���^<���Ԣ��n[��ג�+#�OLvb�Y���KR8�(��F?Dτҕ���m�шB��l���k��.��R�n��&��a\r^�z�b�Cm��E��*+��O�^�O.I�5ى@��(�~�?j�Ɉ�������xD+��R$���=��H~��S��g�j���|��cenU��.�>�]dκ����pGk$�I�rt���b'�̅ ��|��'��\M��N��ֿ���D���S5#��,Pm4�6z\��b�a�	h$b�����:��Ϻ�GG#�UBz��"�X�F���/r�ҷ>^ȩˁ �\Ke-�曒��P��\)���wM��e�eFާJ��󎐗"�X9��������~��xr����I�q����b��=�l�+8�q&\e�,s���u�����
��6����� ��֝ضJ�:V�B�'A�V�Ƀ8Ƭ��=H���s�m��y��=҅�E"�_4o�ϛ
X�L�	 acl��0D���W2-��7(�*	����=�L�����������GaJy�v[������0�Ƽ�|J���;�l�n����9g-�j�d� G+���y�TG�̪h�����-�Pkz?=�L���0���<鄃���N~�]�s�\J^���YT�l���:�u�rM4�e�.4�k��A��id��S�m+Ǥ�P��r�� �"7�ee]��d��&�?��)%�ٚ�ǟ�H���vu����9��y蹄�kM����1��ť��υ7%���A~���t%R}����(k}KUp�5��'��S�Ua㒊"���j1Q��Hq�iB���6$S�{e��ё�\�W�슦�L�z��%��T\���yMUt͵J��3�`ف�������[�;��{n��1:�S�+�C��P�[?e���Q�9�F�O����!9�5*�N9�f��1�����K�?����]qg�P&|��r[��a�sjT�Wv�^�j�@$T}��F���6�_( � �(%�Ji���2����B�?�K���Yg��"kgo��p�"n94�@�I%[ʬ^���Cy��TIZ��`��F]���m#���.���Bx��M����?�������n����(�v�psc�Fu�-��J��n̯�$�C��[ilDc-����r�jY$.K9�}W05뮊qc�}ǃ;�i���`Y`��̇ap`���Z�I�/��;�x|��p�}V��n�_4�^1"�`����� A���0u Z��yu��~����w$����RE<r�j8������'�j�a�zB���K�6�	�&tK�d�Hj���@���}]�$�ɧ;�E���0ާ�r�y�a�}�>9�}��)��\�����ݐ�&&�0�W���LgfO��#�Vl�U�F��1Ag�>m�NVf-�B�׉"8p��ֽ��Y��nG�� �O��Z�Efzr�'�e�%q�0r�fK�t�y��UM=ߜ�Qg�5�;F���Q��W��#'��|G#�q]%��;��R('��i�w����l�P�ӬV�_��<'$�����o��8�*��k�:z�U����"R8 �`^B���s�{��1=9�����w[�\�6:7�u���z�t������JbcFB�P��ב�F [�go�h@��̕R�G
ןL�OZ���@�3�rQXM!Rۡ?qLH�^�q_�#f!,�lQ(E*����/�ԼTI{a��8/ݟ�p=��"��>�<�p��]/WQ,2��PRk���;{F�����jOx-�ïX	��	C��s`���L�BK�0����9I�9K��TD�+3�S$�7ݳ{�1��nw�c纋;g�&Lk���"<��u&�����iŽZ��ˉݿw-aU�a�vs@��FAP��4��J����Z�2���p��Lu8��& )	y�53�� �B>��R�̒1_C֔vJr���Г����q0�8���Σ�*�\�5p0טM^q͟�f`�.sG�w��oPm��><2�vժ�nHU��g m��	�U�M<�'X3��-?�`�qR����4mY���#q��&i���hc�s�*X�Z�-��X������{c;˸I��5Y�T�(����X���F����g��
��m�V��>5��Ɖ�����l�i�q�~�LJ~̍��p���z�e�ot�4�N��(��e�̜��щ��5�A(!�G� �-j1k�e�ԋJ�@�yNِ$�1���N�j,H6�	.d�00�uje��P_�E�V~��
�p%k��*���������x�HT(L�ȧu��OW�z�k4_hb
�I�B�5��Q1��I5���H��VϘ��+��+G���_�[�v��3��ҝ��V�2z��m]$8��zݚFٕ:�]G��^��m��p�ށ~M���#3{ P�	���˥�D3�Ģ~��ޔ��x��<�i��~�Yb�b�^M����I|tt�~�J��ݫs'�8��r� /%�qF�+���X��@+��s�h���*��}M��o�����<S�nv� ��<�U4ou��-n/�	����Ο/9K&�BS������`�Ii�~Gf�9,�6�@
�cӂ����86RD���[��!|8�|u[�M}K��:.������5�P��4n�����5��j��}��qa�����±�	��m��W��ZjL)-��&I��2�ht�[���3v�3i��G��_���z���VPv�8vQ��o�P_$�H�P�Z�P5WI�%]*ӎ�}=���m�2�2+�k�6�oE����wm?�F%�L�~�y��zcws�����G��Ͼ-�<[�@85�6ԏ���$����Q��T��;y����Li�p��ܐڏwG����2#�a��|R74`��_�9Q/���w�/z;B��r� �-:�*N��Sd@��D^A�=�QT��*�=�@��Yʏ���'b���\s�L���NW5� 'A�r8{a�,�Ffmy��s�5��[Λ��d)�iח̎�E�P��@�EM�����CM���.>������T�}�X'���?�칤NQxZj:�:U�+��8�Ǐ�"���l�r]��W�1��3���!���\�t��d��F����L-��I�6�x�ܚ�9M�����@����d��=f8Xbze*�ۇ# Od�V�����kp�@T�W{!N �V�Ғ}�?m7
h�p;!d�%��d�#�t��N�9X��x��¦o`�Vu��j0�y��q�<GX��A�X����K��v�1_#Ѯϑ���
��c�4z.�p*P������`�5q)u��p8}��,
a �q��TN�x��0q�1���+�������&�P��~�������2�X�F���g9u�F<	[�Xy�:��{f>�_H([� ��0�i�˜�[x�K	L�'.jk�`0�$SD'c�����[�����x���1���Q+nx�
0���_Y��Ck�$��ߞ�6�S�8��eO�tkHd���(R�K���}�kn���ӝ)�m� ѳ�
tZ
E?��C	��{�r�jE����I]���������A+t����"���nEQ�����j_Qm~����#�;l�]j�2P��Q��<v6�~-(��H�a+�6``99zV�q�Q[��?��D�ұ���6]�~X��_,\��k��J7"�Z���?���J�	U�i����7�2Ⱦ%���)����|w��+�f#B����)�B\�SF���+�0�EA���|(��R�{ц��U�c�*��\2.vw&�)�h!��Cm�g�V��NsТ�
�-��%q��P$8[�RD��W�!�5���.]��V�0	C�b'��"w3��@�E�u��y���u�H���a�K1�5mŎ�z�i�ɼO����dB5�/��5���\��f���X��K/(�c�8n�S�:��ުM<Lz5�y��gk}�z%�rP�p]
c��,���L �pF��,��C� ��A���'�>r����~��$P�	0�Ԍ9���-��M�������֣��2|&ix(��X��yT��?^ݧ���V��ם�oÏ��<lJ�Z"���펖B��t��Ѷc?i:	d��/���&`w7w�;�+��Ǭϔ�ժ���/��#�̯k,��j/�~o],�����ݶTP��(
�K���w��UEb�Jr_N,�����	�$9(96����I\�o>5��l��% 4�ov�/���c+D"�Y|�
�����J�U�ktɇҋ̧�����g?�x6�<�,�z�WZ�BԚ �*`'�a�1gW9��!!>��{T~�����5sˆ�Ȧr�A��J�Y��@�5`e�؀��R ��/�uiK��0���t��0r~�+�!�Sސ�vr9�_�F�S�֤\Ij�H����&�O�Z�@����@w#}�t��I&ߎVXL�؞5@Xdc���t�?%b��ɬu�'�&��~�|�K�j4���ڰ��"�����H��5�LX�0'���7�k2�u�D �{�,[�����nʢSg�9�.3�����oS�>;+.��Đ#_��!��_W����jSUPw�&�U'�}9Eg��9|xl=J��B�.��cw��KPp��v��
���D���89�t~�}�H������URE��s`2ڍ�c���-Gt�O�Ѡ�G��9��a�������4IN�(�����G6��pl"�qk:\��N�	��=Rr��A{�%kڍ=�[�T���9"�Ǥ�(L>p��A�2�XR���w��5{5OY��qD��
Ҳ&�"��#�8%W�ލ∋1866��"R�b;)���#)[(�}���f�K+����	/59��,�UG�!p����r�R��$m I���Ei|��\ul�4C�j�|H;i[ x�XѴm��#De�J�Lԥ����"%��튆�l�T)x��Y�;�َ����̷��M�#�3)�R���~S��SWt����������ǥi�&�=h�S��ҜdO��clr����zj;�细{-�KH]_�"�Q�;y`�b:��D��R�E�<�Gb�h�ȵՎ�\"��;$|ިk�J�|΄$ ����@h9&[��/��L;�h@�&x&�2���d��[j�x�Øڰ�������7ڥԪk؅�&������uj���r��~��B��q}1�n/��HҙNl4ˮk�N�+u���6EP��Y	�RV�����q�e�@c!+�E��a+�y�*J�(Tx��g���d�y/�E��A�kz�wrn(���t�+�+T�./Z
�}�>ϴT�~�=�=<��+c)cPh�|4���bT��
�ЫX;`����@�~!���m�S�]�����#Kx���=Q��F cO��0��4{k�q�Ձd�|�����$�ci�!6���LC�@g
D|s��K}�)������Sފ�7���Y�Kq��x�B��*�o�a2|�S#�E�D��XWF/�U}�^< $B(�S�� ��Q�|np��1����j&�����!A?ROW'{
�W�����J��u�;{��~���t�L2WT3DA?����o�Z��3�����6�QJ'����Z�7>���f<[�;�p��(t6)�Q�z�g�DK�g�v�[S�}��L��r�U�����C��L�aF���0D��t�0��*���5���_�'�4���4�)t�`�v����[<�ĵD�0�d1��LE�}��n�b�LU�`t��KkՄc'�H���#\�D,���zq�����o�In0tW���%�s\���@}}����Κ�	������@C���l���u�L�?gb :q�|r7x���]�>�����d�[��vf�#�Ǿ��;���#�NSe�_@U��1���c�;��|�'gSa�Jl�����']i}�6����L�ægWo��s�\�U)���.�qF� ��#S�|/d���-!�B�<h YrJ�u�����>6d�=��ku@����o+mT.�/��� NXSt�[k�pp���*�~�AmX(��CBxToJ���ٚ��E��r�����TxU�"�����_��eض�p�T��/�������{��=^59�Y�z?�&�2���o9I0�k�����F�fB�๐m��nЬ�S�vw�G��oCa����^rx*�p�D��1���=� ��?g�ĈB�m	L=����4�x� �`,&�n��81���L^t�>\�k_A��|G�J؆ �MȽ0��O2�x��d��m���0��.f=<��ba�I�%N��h�V���g>���]����x��FMW�Z��"��Ϝ�S�=7$6V����j�� �I�~=XH3��ki�>qq�t;�_kא;��'2�?��Vo�P�Ɩ���uÑz��~��4��9Q�����e}5%���W��T\��p��v��
Eǩ�3S����Z����K��CT�Y�rC��*��Ue�+���{�J�r��?�{4�Z�P�A����d�U��p������so�J�=��y�Q�-���#�S�v:�V�t
 ޖ���*��ζ�DU�q�	ޘ���w6����q0����c�5����_2J���v
z���#i"�2Ң��^���D�k+��PPp~�L�.ĭ���W���
eo{�:T�DL
@�*���9I��+R�WQ���W�|���^�{�n��kfָ����I���y�o�}�)S�0��A��G�C�$����[�cXX�6����-&�2tsՔ�:F]BF��6�������1�2����LO?+MB���d�8Q|�;������rV�M�*8	�� ��p�@���f���u�<�Y�aDS�)�ʆp��vq)�+*��n�S�%ȱ�<��֒�����?j`�����H�G�Gv�϶n��0��J��"j�����!�:�'1�]H
A��{h�鵔�`��?������M���rX���e�T���\�._��q'm@]u�K���k�}�]w�df'�os;��-ЖG��1��s }��n���x�}Zlv_��E����t�p�#fM�#]���\��/P��#�L��R�8�L
�*�j�)��kzW}�8���iu�D�Az�X��<w�� j��'Bd#�nIu�ԑ2�����lJ��C?�OæMq�$��Q:����|;���9Ã���a�I��c)��'�.;l�����6���ۡ�}6�)�U���:��2�w��B��w��G퐚T@$]+�E{	���*���y���C�NMu��µ7�1�xQ�Q7�T֭�!����XN'{�Eh��e��i+`���:F*uzŰ���:u�@�������C/��6���l�\�tLZGI5��mJ[c�}�-�.���#j��%3O�28����}��t�qH)��*@�"D�h�_���t�O��¥q3�ڞ�߇�q4�u�!D���hj��
 ����Y��:u��ީt��0�b���0Ҵ�)w4&]��f{��^�D$��+÷ND#ke7j�]���k�+B���ޕB[�"��_a�.��>��>'����bDz��JP�"�2�p��`yH�#*mH��ZVV'�}7�?sA�E�,J;���j���ۢ�vK��U�B�#(6��6߳���QX����qP��I����tDx-1�ρq G��������󟌽N�'�K��y%�9��-� �23EDG���Oi�!^1��wrJ����� f������l���~lra�GDw�8����F�]��7?C1-u�(�L��銎Ǟ���?�#I;�e���
�4,tá)K�[�,P�3�R2o�5G��z��,K�cI ר�NwqA&\��G��d<J%��վ��҃���v�UrU(f�gM�� �nL�e^=ӡ�y�E�����[a�Ϙ�ց�a��Oq��dq�}�ɀ�g)�'�*Y`Ht��s���z�쑽V;H���LY:��Rq����:{�s���rOb��,\X<���Ė%5���� ����Hja$\���g��:)uU��w��Xv쀒9ܺND ˘�
���1�<�m�/���a�i�J��`����굶b���M@3�	-�N���'.-�C4��?Y�����K��;����O	f���*�^��!���׈�^�}hfe ���y�BQ~&EYo`�+����f�{���⧘|��M�=g��6��z����t�J� ���d�|t�����n��_�'�P�y��O���ċEE}�aB��#eF�l��߲��%o�ȶ��󠔦���u&��������0��R��h6���Z�I�t���2G�:�f��Fi*7XzH�ﴖ�m���-2�K�z�,���-c�⅍��_r��g�z�Ix�-"�s�h����9We)����}��]���7+�q���W�=�Ưi�f�S�O���Y��H�����Y;��H�^^N���P���]T%>�Y�,�,�����������@&b���� c`�����nkVְ|�\�.����y{��uD,A�'>FZ[΁w�pN:Q���.���V�R��h�$�/� W!����5���l[~���')����SB��B8�Ocvz�Ntt��5p�&��7��5E�&�N�1�hz����oVI�;�҄�{�53)���ࡩ����L6?�(י��=˼���{ɵ���J� ���I�!#�X��?J���?�g)/$E�nJ�5 V�?�&�J�p�KI�#�fmw`İ[$�Dc���a�`K�j<!k�r�8pX�ud�tTb�f?T�P�ς��=���)<N��N��|���rX�eo�B�Æ�:�3H#7W();�����1��kUo��G���`��4�t5�&ί礂�l�v��2%���n��R��6�.R�����l4Ħ�h�-�b�;��_]a�&�'��Vv�z=�R��9��5��Z��_%���N/Nt8�U6�j[˹>��tG�vw8"����K�o��a��D&E��R���L�OL�� &D^���7IC�-~�T�G��퇷I�,2����UQ,��.W��e ߞ����O�������d��p0��(_>���Q�t�@EB��tŞ�.�n��EZ�v�1T�p@�����/�!L�ܝl1h���`?�J�S�%�̪��F�L��N�\J���;C-`t��"坁5�Ә�.��Gxy�Q�G!t�!'f{����dg���/=�D���]%��,mк�U��س�� ��]z�#�pa��k�^>�<�7M��J���IԒ��B,⊒�������@"�;C���n�S�O��7+z�$ $E��%8��&�����lZP��ۣ�m3�$��j�C���(�d�z婅 g�g?�q��JE�^�{_�ҫ�/��X�Y�ʸ%t��i�\ˀv�A��l�+����mA�wSc��/��
��?��~3�ܷeR3 -$�!Èh���xL����5��
��D���m�ȫp�S�\��]�\�4���������`ߗ��<w>mY����*l]��2[݃� �7\�W��)=2(���e�M�s	G�Jl��ˉ
��H/Ѹ��ķ�jJuv8���!$����������x��R>^|r)�;����?� ]x�ɂ3��4p>�QV$)k
ͳ���XC����1ZB���ɝ.�iD��Ⱦѹ�m�-�:	S��s1��Z޲.����2� ~�Q�d��?ϯt�Z��	m�f���E+K)���G��A��y}P�֙x:�s�l�zr;� �����iY����n�1wz3���Q�ΛdM��$q'���/��k!xE�����E [@t��);�7@��&uC�d�<�{��� �u9�C�mgϺS����C}��8NEϳg�H�.CE/I�qKW[��@LW���7���_\�I�@�u��"�.	���K��dV�6h��]��������>�X_
�M�ʼ���8	���C'H��w�o9�K�ʆ�/��$�ܟ_Q��԰>�%{�vl�k�������SPU��PL�|�s=���#T4����&y�ͼI��1Z�1�YeRt!R�Z^�N����K����	-���e�[�5�>E����B�mȬ/�ݪ~��a����D�w�����e���h�B^�D�y((C=�o^FF��C{���xx�^�j����\Ǣ�i��n�$��F"� ����H�W���ѭ�lOK�0�iU�#�O�|��� �W��=��+/�@C^��qʽ�kEH�����~��x�y�P�RE�R�.�GF���,��o�����ѱ�T�M*�2<�c��'�/0"_��1�rҳ_��T;3
���tM=�I�m(v_L��w�/��Xf2f<K��U�H晉�!@f�Dep��w1Vu�(�)��%�3VR�%���n�&^����S �'�I�xX��8bX��K�!�''>|p2���{��l�-�\����?�zb��5[��=a�嗣W�ӎk[���%9`=��2�Z�@��t�ٞj��++�0G�g�����Hk�VІcUc�%�S��Y�婁[�@��O[d�^�ݯ�1Z��P;Yp�d����W�v��]APv��|�WD��I�o���qk$jO���矇��d����C""b�zE"�n��{�QoVW�Ҹ#o�t�?]S!,����;@�&�����5�}�]�,@��&�b��K�>.C��k]��b�z&�!G8`X��6��ن����Pc��!��Y}9K�s���$%嘛2�~u*�.���t`%f �S6�y��R)�6b4���>���0�IA�¶##�����.��y�b[����Q�r�3{|�a2����Ad���!�o֨m�ɏ������rkf�(��Fr� �T��_��Q�e��L��u��߻��[O3���@%v`��hUGB�pH����+ӬQ�\ɯ|��-�����y��vֆ����6�s�k���bÊۄWJ�e�yL���WN��E%��b�}W��D��V(�l
S nx���M/��&}�w�yڸ����XqKh��,�T��r��e�M�s_HUD�ou������x�
]���	�G{�����.ĥҹ���r��������+2v+?�l	��GB�{��VX�p�F�@������8�'i!'LI6f�//�ĸ�|w�	J@�����MP<~2�J�~�;����\X�d�*ō��^*"�F��`,��xѓ�*��/����3t�2�ulm1��|�f�z�+�R� ���&�0����y�H�l���X�V<�n�0t�ǟ2"b�~�I]p
��8(?��_
H��pbr�U
�������r���6BϜ�'��~��T2?+儣ϖ���P[gfÕs�X����m���As9ae_2&����P.��W9PmR+D��Yk���awa6�����!�ߤ}��	{�2]Ӧ���-0������/�	F������#q��tw�@#ƨ:�fk_A�([����5�˩�[��C_��n�|��k�c���;�r�ҕ� ��=�72�eN��P��?ڨM�熕��k���~	z�j���>W�U�Lwg�n�������Q(�E�;s�����H��=����߶I}� o�`=�۟q�-e��K������ �����|���8���T��]���؁B���OVP�����)�(b(5ik� 
ڞ��u�X����S3�*�����G"Yc��o]���T�f�h�x�y,n�.<�7�XAF�cf^��ep���Zm+{����B@�yr4P}5e�YTI4��(2�T���V�
�GXǥ���-��?�Nμ^ԇ���/ �E��j��a��`�o�Aj	���qK��L� ���s�o�ʳ��D�:�~ ߳��62���Zкp�"��0,�0�X��~s#��<rp�,�_`�9i�D��y_("o�ۼ��N.W	xs�Ң"}[U�x��J���[ke��P�iw%�)~�aRr?�wkyO�������g�Rij�!���'~Д)V$�-(v�(�
�ڰ��n>�p�1�Z�)6W�(�dM|�K�5>����^�ֵ��ۤ�6,d�=��vb�~��ٯ:��d9oQ\�%�&�vľT31���$s�T�@l�!���$��'�����~J M�Q���)&�������olbI��8��u�ñ��W!z>� |6�_�en�QӖI<�-��3d��e ˜�6ZS�������Rt3Áb����z2���J�⤉���ofY>�MzAb}K$7�G���S'$��q@"�SM19ud�%sV��,���g���T����9���O�Zw��9���Ț9���ه�������<�%F��/�(>)�@�CD�Eg�*�n��p<�h����a+�.��R���~j�cf��l���섻�W�&*�V�|3},�2N����#]w/h�_��G�	>�X�{��Z������3*�|DπtvP��p{�����p�>9��i�=g���Щ��zS��xW�Zy�z�PLKxnLZ@�v������T����)�ͤ�)���o��l������Dhyg_O�sR0�1\4"U���b��0-���"�n
(��4��K��h��.����(��q�+�I&�y���C
_Kc�Ͼ��T����Y��3�B��ZBL��y���`̇���C�;O�'��;Q�W���<��9J��z��jl����'3� 7�bUU��u��A�c�>	��A8�.:$h��v�2��2�����ļ�M{�B��]���{��6���A�ԫ#�(�i'�$�`�Ѧ��̪ߞlV�ho��� -> @hlock and @hlock -> <whatever __bfs() found> is not -(*R)->
 * and -(S*)->.
 */
static inline void bfs_init_root(struct lock_list *lock,
				 struct held_lock *hlock)
{
	__bfs_init_root(lock, hlock_class(hlock));
	lock->only_xr = (hlock->read == 2);
}

/*
 * Similar to bfs_init_root() but initialize the root for backwards BFS.
 *
 * ->only_xr of the initial lock node is set to @hlock->read != 0, to make sure
 * that <next> -> @hlock and @hlock -> <whatever backwards BFS found> is not
 * -(*S)-> and -(R*)-> (reverse order of -(*R)-> and -(S*)->).
 */
static inline void bfs_init_rootb(struct lock_list *lock,
				  struct held_lock *hlock)
{
	__bfs_init_root(lock, hlock_class(hlock));
	lock->only_xr = (hlock->read != 0);
}

static inline struct lock_list *__bfs_next(struct lock_list *lock, int offset)
{
	if (!lock || !lock->parent)
		return NULL;

	return list_next_or_null_rcu(get_dep_list(lock->parent, offset),
				     &lock->entry, struct lock_list, entry);
}

/*
 * Breadth-First Search to find a strong path in the dependency graph.
 *
 * @source_entry: the source of the path we are searching for.
 * @data: data used for the second parameter of @match function
 * @match: match function for the search
 * @target_entry: pointer to the target of a matched path
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 *
 * We may have multiple edges (considering different kinds of dependencies,
 * e.g. ER and SN) between two nodes in the dependency graph. But
 * only the strong dependency path in the graph is relevant to deadlocks. A
 * strong dependency path is a dependency path that doesn't have two adjacent
 * dependencies as -(*R)-> -(S*)->, please see:
 *
 *         Documentation/locking/lockdep-design.rst
 *
 * for more explanation of the definition of strong dependency paths
 *
 * In __bfs(), we only traverse in the strong dependency path:
 *
 *     In lock_list::only_xr, we record whether the previous dependency only
 *     has -(*R)-> in the search, and if it does (prev only has -(*R)->), we
 *     filter out any -(S*)-> in the current dependency and after that, the
 *     ->only_xr is set according to whether we only have -(*R)-> left.
 */
static enum bfs_result __bfs(struct lock_list *source_entry,
			     void *data,
			     bool (*match)(struct lock_list *entry, void *data),
			     bool (*skip)(struct lock_list *entry, void *data),
			     struct lock_list **target_entry,
			     int offset)
{
	struct circular_queue *cq = &lock_cq;
	struct lock_list *lock = NULL;
	struct lock_list *entry;
	struct list_head *head;
	unsigned int cq_depth;
	bool first;

	lockdep_assert_locked();

	__cq_init(cq);
	__cq_enqueue(cq, source_entry);

	while ((lock = __bfs_next(lock, offset)) || (lock = __cq_dequeue(cq))) {
		if (!lock->class)
			return BFS_EINVALIDNODE;

		/*
		 * Step 1: check whether we already finish on this one.
		 *
		 * If we have visited all the dependencies from this @lock to
		 * others (iow, if we have visited all lock_list entries in
		 * @lock->class->locks_{after,before}) we skip, otherwise go
		 * and visit all the dependencies in the list and mark this
		 * list accessed.
		 */
		if (lock_accessed(lock))
			continue;
		else
			mark_lock_accessed(lock);

		/*
		 * Step 2: check whether prev dependency and this form a strong
		 *         dependency path.
		 */
		if (lock->parent) { /* Parent exists, check prev dependency */
			u8 dep = lock->dep;
			bool prev_only_xr = lock->parent->only_xr;

			/*
			 * Mask out all -(S*)-> if we only have *R in previous
			 * step, because -(*R)-> -(S*)-> don't make up a strong
			 * dependency.
			 */
			if (prev_only_xr)
				dep &= ~(DEP_SR_MASK | DEP_SN_MASK);

			/* If nothing left, we skip */
			if (!dep)
				continue;

			/* If there are only -(*R)-> left, set that for the next step */
			lock->only_xr = !(dep & (DEP_SN_MASK | DEP_EN_MASK));
		}

		/*
		 * Step 3: we haven't visited this and there is a strong
		 *         dependency path to this, so check with @match.
		 *         If @skip is provide and returns true, we skip this
		 *         lock (and any path this lock is in).
		 */
		if (skip && skip(lock, data))
			continue;

		if (match(lock, data)) {
			*target_entry = lock;
			return BFS_RMATCH;
		}

		/*
		 * Step 4: if not match, expand the path by adding the
		 *         forward or backwards dependencies in the search
		 *
		 */
		first = true;
		head = get_dep_list(lock, offset);
		list_for_each_entry_rcu(entry, head, entry) {
			visit_lock_entry(entry, lock);

			/*
			 * Note we only enqueue the first of the list into the
			 * queue, because we can always find a sibling
			 * dependency from one (see __bfs_next()), as a result
			 * the space of queue is saved.
			 */
			if (!first)
				continue;

			first = false;

			if (__cq_enqueue(cq, entry))
				return BFS_EQUEUEFULL;

			cq_depth = __cq_get_elem_count(cq);
			if (max_bfs_queue_depth < cq_depth)
				max_bfs_queue_depth = cq_depth;
		}
	}

	return BFS_RNOMATCH;
}

static inline enum bfs_result
__bfs_forwards(struct lock_list *src_entry,
	       void *data,
	       bool (*match)(struct lock_list *entry, void *data),
	       bool (*skip)(struct lock_list *entry, void *data),
	       struct lock_list **target_entry)
{
	return __bfs(src_entry, data, match, skip, target_entry,
		     offsetof(struct lock_class, locks_after));

}

static inline enum bfs_result
__bfs_backwards(struct lock_list *src_entry,
		void *data,
		bool (*match)(struct lock_list *entry, void *data),
	       bool (*skip)(struct lock_list *entry, void *data),
		struct lock_list **target_entry)
{
	return __bfs(src_entry, data, match, skip, target_entry,
		     offsetof(struct lock_class, locks_before));

}

static void print_lock_trace(const struct lock_trace *trace,
			     unsigned int spaces)
{
	stack_trace_print(trace->entries, trace->nr_entries, spaces);
}

/*
 * Print a dependency chain entry (this is only done when a deadlock
 * has been detected):
 */
static noinline void
print_circular_bug_entry(struct lock_list *target, int depth)
{
	if (debug_locks_silent)
		return;
	printk("\n-> #%u", depth);
	print_lock_name(target->class);
	printk(KERN_CONT ":\n");
	print_lock_trace(target->trace, 6);
}

static void
print_circular_lock_scenario(struct held_lock *src,
			     struct held_lock *tgt,
			     struct lock_list *prt)
{
	struct lock_class *source = hlock_class(src);
	struct lock_class *target = hlock_class(tgt);
	struct lock_class *parent = prt->class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (parent != source) {
		printk("Chain exists of:\n  ");
		__print_lock_name(source);
		printk(KERN_CONT " --> ");
		__print_lock_name(parent);
		printk(KERN_CONT " --> ");
		__print_lock_name(target);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(target);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(parent);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(target);
	printk(KERN_CONT ");\n");
	printk("  lock(");
	__print_lock_name(source);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

/*
 * When a circular dependency is detected, print the
 * header first:
 */
static noinline void
print_circular_bug_header(struct lock_list *entry, unsigned int depth,
			struct held_lock *check_src,
			struct held_lock *check_tgt)
{
	struct task_struct *curr = current;

	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("======================================================\n");
	pr_warn("WARNING: possible circular locking dependency detected\n");
	print_kernel_ident();
	pr_warn("------------------------------------------------------\n");
	pr_warn("%s/%d is trying to acquire lock:\n",
		curr->comm, task_pid_nr(curr));
	print_lock(check_src);

	pr_warn("\nbut task is already holding lock:\n");

	print_lock(check_tgt);
	pr_warn("\nwhich lock already depends on the new lock.\n\n");
	pr_warn("\nthe existing dependency chain (in reverse order) is:\n");

	print_circular_bug_entry(entry, depth);
}

/*
 * We are about to add A -> B into the dependency graph, and in __bfs() a
 * strong dependency path A -> .. -> B is found: hlock_class equals
 * entry->class.
 *
 * If A -> .. -> B can replace A -> B in any __bfs() search (means the former
 * is _stronger_ than or equal to the latter), we consider A -> B as redundant.
 * For example if A -> .. -> B is -(EN)-> (i.e. A -(E*)-> .. -(*N)-> B), and A
 * -> B is -(ER)-> or -(EN)->, then we don't need to add A -> B into the
 * dependency graph, as any strong path ..-> A -> B ->.. we can get with
 * having dependency A -> B, we could already get a equivalent path ..-> A ->
 * .. -> B -> .. with A -> .. -> B. Therefore A -> B is redundant.
 *
 * We need to make sure both the start and the end of A -> .. -> B is not
 * weaker than A -> B. For the start part, please see the comment in
 * check_redundant(). For the end part, we need:
 *
 * Either
 *
 *     a) A -> B is -(*R)-> (everything is not weaker than that)
 *
 * or
 *
 *     b) A -> .. -> B is -(*N)-> (nothing is stronger than this)
 *
 */
static inline bool hlock_equal(struct lock_list *entry, void *data)
{
	struct held_lock *hlock = (struct held_lock *)data;

	return hlock_class(hlock) == entry->class && /* Found A -> .. -> B */
	       (hlock->read == 2 ||  /* A -> B is -(*R)-> */
		!entry->only_xr); /* A -> .. -> B is -(*N)-> */
}

/*
 * We are about to add B -> A into the dependency graph, and in __bfs() a
 * strong dependency path A -> .. -> B is found: hlock_class equals
 * entry->class.
 *
 * We will have a deadlock case (conflict) if A -> .. -> B -> A is a strong
 * dependency cycle, that means:
 *
 * Either
 *
 *     a) B -> A is -(E*)->
 *
 * or
 *
 *     b) A -> .. -> B is -(*N)-> (i.e. A -> .. -(*N)-> B)
 *
 * as then we don't have -(*R)-> -(S*)-> in the cycle.
 */
static inline bool hlock_conflict(struct lock_list *entry, void *data)
{
	struct held_lock *hlock = (struct held_lock *)data;

	return hlock_class(hlock) == entry->class && /* Found A -> .. -> B */
	       (hlock->read == 0 || /* B -> A is -(E*)-> */
		!entry->only_xr); /* A -> .. -> B is -(*N)-> */
}

static noinline void print_circular_bug(struct lock_list *this,
				struct lock_list *target,
				struct held_lock *check_src,
				struct held_lock *check_tgt)
{
	struct task_struct *curr = current;
	struct lock_list *parent;
	struct lock_list *first_parent;
	int depth;

	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	this->trace = save_trace();
	if (!this->trace)
		return;

	depth = get_lock_depth(target);

	print_circular_bug_header(target, depth, check_src, check_tgt);

	parent = get_lock_parent(target);
	first_parent = parent;

	while (parent) {
		print_circular_bug_entry(parent, --depth);
		parent = get_lock_parent(parent);
	}

	printk("\nother info that might help us debug this:\n\n");
	print_circular_lock_scenario(check_src, check_tgt,
				     first_parent);

	lockdep_print_held_locks(curr);

	printk("\nstack backtrace:\n");
	dump_stack();
}

static noinline void print_bfs_bug(int ret)
{
	if (!debug_locks_off_graph_unlock())
		return;

	/*
	 * Breadth-first-search failed, graph got corrupted?
	 */
	WARN(1, "lockdep bfs error:%d\n", ret);
}

static bool noop_count(struct lock_list *entry, void *data)
{
	(*(unsigned long *)data)++;
	return false;
}

static unsigned long __lockdep_count_forward_deps(struct lock_list *this)
{
	unsigned long  count = 0;
	struct lock_list *target_entry;

	__bfs_forwards(this, (void *)&count, noop_count, NULL, &target_entry);

	return count;
}
unsigned long lockdep_count_forward_deps(struct lock_class *class)
{
	unsigned long ret, flags;
	struct lock_list this;

	__bfs_init_root(&this, class);

	raw_local_irq_save(flags);
	lockdep_lock();
	ret = __lockdep_count_forward_deps(&this);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	return ret;
}

static unsigned long __lockdep_count_backward_deps(struct lock_list *this)
{
	unsigned long  count = 0;
	struct lock_list *target_entry;

	__bfs_backwards(this, (void *)&count, noop_count, NULL, &target_entry);

	return count;
}

unsigned long lockdep_count_backward_deps(struct lock_class *class)
{
	unsigned long ret, flags;
	struct lock_list this;

	__bfs_init_root(&this, class);

	raw_local_irq_save(flags);
	lockdep_lock();
	ret = __lockdep_count_backward_deps(&this);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	return ret;
}

/*
 * Check that the dependency graph starting at <src> can lead to
 * <target> or not.
 */
static noinline enum bfs_result
check_path(struct held_lock *target, struct lock_list *src_entry,
	   bool (*match)(struct lock_list *entry, void *data),
	   bool (*skip)(struct lock_list *entry, void *data),
	   struct lock_list **target_entry)
{
	enum bfs_result ret;

	ret = __bfs_forwards(src_entry, target, match, skip, target_entry);

	if (unlikely(bfs_error(ret)))
		print_bfs_bug(ret);

	return ret;
}

/*
 * Prove that the dependency graph starting at <src> can not
 * lead to <target>. If it can, there is a circle when adding
 * <target> -> <src> dependency.
 *
 * Print an error and return BFS_RMATCH if it does.
 */
static noinline enum bfs_result
check_noncircular(struct held_lock *src, struct held_lock *target,
		  struct lock_trace **const trace)
{
	enum bfs_result ret;
	struct lock_list *target_entry;
	struct lock_list src_entry;

	bfs_init_root(&src_entry, src);

	debug_atomic_inc(nr_cyclic_checks);

	ret = check_path(target, &src_entry, hlock_conflict, NULL, &target_entry);

	if (unlikely(ret == BFS_RMATCH)) {
		if (!*trace) {
			/*
			 * If save_trace fails here, the printing might
			 * trigger a WARN but because of the !nr_entries it
			 * should not do bad things.
			 */
			*trace = save_trace();
		}

		print_circular_bug(&src_entry, target_entry, src, target);
	}

	return ret;
}

#ifdef CONFIG_TRACE_IRQFLAGS

/*
 * Forwards and backwards subgraph searching, for the purposes of
 * proving that two subgraphs can be connected by a new dependency
 * without creating any illegal irq-safe -> irq-unsafe lock dependency.
 *
 * A irq safe->unsafe deadlock happens with the following conditions:
 *
 * 1) We have a strong dependency path A -> ... -> B
 *
 * 2) and we have ENABLED_IRQ usage of B and USED_IN_IRQ usage of A, therefore
 *    irq can create a new dependency B -> A (consider the case that a holder
 *    of B gets interrupted by an irq whose handler will try to acquire A).
 *
 * 3) the dependency circle A -> ... -> B -> A we get from 1) and 2) is a
 *    strong circle:
 *
 *      For the usage bits of B:
 *        a) if A -> B is -(*N)->, then B -> A could be any type, so any
 *           ENABLED_IRQ usage suffices.
 *        b) if A -> B is -(*R)->, then B -> A must be -(E*)->, so only
 *           ENABLED_IRQ_*_READ usage suffices.
 *
 *      For the usage bits of A:
 *        c) if A -> B is -(E*)->, then B -> A could be any type, so any
 *           USED_IN_IRQ usage suffices.
 *        d) if A -> B is -(S*)->, then B -> A must be -(*N)->, so only
 *           USED_IN_IRQ_*_READ usage suffices.
 */

/*
 * There is a strong dependency path in the dependency graph: A -> B, and now
 * we need to decide which usage bit of A should be accumulated to detect
 * safe->unsafe bugs.
 *
 * Note that usage_accumulate() is used in backwards search, so ->only_xr
 * stands for whether A -> B only has -(S*)-> (in this case ->only_xr is true).
 *
 * As above, if only_xr is false, which means A -> B has -(E*)-> dependency
 * path, any usage of A should be considered. Otherwise, we should only
 * consider _READ usage.
 */
static inline bool usage_accumulate(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		*(unsigned long *)mask |= entry->class->usage_mask;
	else /* Mask out _READ usage bits */
		*(unsigned long *)mask |= (entry->class->usage_mask & LOCKF_IRQ);

	return false;
}

/*
 * There is a strong dependency path in the dependency graph: A -> B, and now
 * we need to decide which usage bit of B conflicts with the usage bits of A,
 * i.e. which usage bit of B may introduce safe->unsafe deadlocks.
 *
 * As above, if only_xr is false, which means A -> B has -(*N)-> dependency
 * path, any usage of B should be considered. Otherwise, we should only
 * consider _READ usage.
 */
static inline bool usage_match(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		return !!(entry->class->usage_mask & *(unsigned long *)mask);
	else /* Mask out _READ usage bits */
		return !!((entry->class->usage_mask & LOCKF_IRQ) & *(unsigned long *)mask);
}

static inline bool usage_skip(struct lock_list *entry, void *mask)
{
	/*
	 * Skip local_lock() for irq inversion detection.
	 *
	 * For !RT, local_lock() is not a real lock, so it won't carry any
	 * dependency.
	 *
	 * For RT, an irq inversion happens when we have lock A and B, and on
	 * some CPU we can have:
	 *
	 *	lock(A);
	 *	<interrupted>
	 *	  lock(B);
	 *
	 * where lock(B) cannot sleep, and we have a dependency B -> ... -> A.
	 *
	 * Now we prove local_lock() cannot exist in that dependency. First we
	 * have the observation for any lock chain L1 -> ... -> Ln, for any
	 * 1 <= i <= n, Li.inner_wait_type <= L1.inner_wait_type, otherwise
	 * wait context check will complain. And since B is not a sleep lock,
	 * therefore B.inner_wait_type >= 2, and since the inner_wait_type of
	 * local_lock() is 3, which is greater than 2, therefore there is no
	 * way the local_lock() exists in the dependency B -> ... -> A.
	 *
	 * As a result, we will skip local_lock(), when we search for irq
	 * inversion bugs.
	 */
	if (entry->class->lock_type == LD_LOCK_PERCPU) {
		if (DEBUG_LOCKS_WARN_ON(entry->class->wait_type_inner < LD_WAIT_CONFIG))
			return false;

		return true;
	}

	return false;
}

/*
 * Find a node in the forwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 *
 * Return BFS_MATCH if such a node exists in the subgraph, and put that node
 * into *@target_entry.
 */
static enum bfs_result
find_usage_forwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_forwards_checks);

	result = __bfs_forwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

/*
 * Find a node in the backwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 */
static enum bfs_result
find_usage_backwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_backwards_checks);

	result = __bfs_backwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

static void print_lock_class_header(struct lock_class *class, int depth)
{
	int bit;

	printk("%*s->", depth, "");
	print_lock_name(class);
#ifdef CONFIG_DEBUG_LOCKDEP
	printk(KERN_CONT " ops: %lu", debug_class_ops_read(class));
#endif
	printk(KERN_CONT " {\n");

	for (bit = 0; bit < LOCK_TRACE_STATES; bit++) {
		if (class->usage_mask & (1 << bit)) {
			int len = depth;

			len += printk("%*s   %s", depth, "", usage_str[bit]);
			len += printk(KERN_CONT " at:\n");
			print_lock_trace(class->usage_traces[bit], len);
		}
	}
	printk("%*s }\n", depth, "");

	printk("%*s ... key      at: [<%px>] %pS\n",
		depth, "", class->key, class->key);
}

/*
 * Dependency path printing:
 *
 * After BFS we get a lock dependency path (linked via ->parent of lock_list),
 * printing out each lock in the dependency path will help on understanding how
 * the deadlock could happen. Here are some details about dependency path
 * printing:
 *
 * 1)	A lock_list can be either forwards or backwards for a lock dependency,
 * 	for a lock dependency A -> B, there are two lock_lists:
 *
 * 	a)	lock_list in the ->locks_after list of A, whose ->class is B and
 * 		->links_to is A. In this case, we can say the lock_list is
 * 		"A -> B" (forwards case).
 *
 * 	b)	lock_list in the ->locks_before list of B, whose ->class is A
 * 		and ->links_to is B. In this case, we can say the lock_list is
 * 		"B <- A" (bacwards case).
 *
 * 	The ->trace of both a) and b) point to the call trace where B was
 * 	acquired with A held.
 *
 * 2)	A "helper" lock_list is introduced during BFS, this lock_list doesn't
 * 	represent a certain lock dependency, it only provides an initial entry
 * 	for BFS. For example, BFS may introduce a "helper" lock_list whose
 * 	->class is A, as a result BFS will search all dependencies starting with
 * 	A, e.g. A -> B or A -> C.
 *
 * 	The notation of a forwards helper lock_list is like "-> A", which means
 * 	we should search the forwards dependencies starting with "A", e.g A -> B
 * 	or A -> C.
 *
 * 	The notation of a bacwards helper lock_list is like "<- B", which means
 * 	we should search the backwards dependencies ending with "B", e.g.
 * 	B <- A or B <- C.
 */

/*
 * printk the shortest lock dependencies from @root to @leaf in reverse order.
 *
 * We have a lock dependency path as follow:
 *
 *    @root                                                                 @leaf
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | <--------- | lock_list | ... | lock_list  | <--------- | lock_list |
 * |    -> L1  |            | L1 -> L2  | ... |Ln-2 -> Ln-1|            | Ln-1 -> Ln|
 *
 * , so it's natural that we start from @leaf and print every ->class and
 * ->trace until we reach the @root.
 */
static void __used
print_shortest_lock_dependencies(struct lock_list *leaf,
				 struct lock_list *root)
{
	struct lock_list *entry = leaf;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		printk("%*s ... acquired at:\n", depth, "");
		print_lock_trace(entry->trace, 2);
		printk("\n");

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

/*
 * printk the shortest lock dependencies from @leaf to @root.
 *
 * We have a lock dependency path (from a backwards search) as follow:
 *
 *    @leaf                                                                 @root
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | ---------> | lock_list | ... | lock_list  | ---------> | lock_list |
 * | L2 <- L1  |            | L3 <- L2  | ... | Ln <- Ln-1 |            |    <- Ln  |
 *
 * , so when we iterate from @leaf to @root, we actually print the lock
 * dependency path L1 -> L2 -> .. -> Ln in the non-reverse order.
 *
 * Another thing to notice here is that ->class of L2 <- L1 is L1, while the
 * ->trace of L2 <- L1 is the call trace of L2, in fact we don't have the call
 * trace of L1 in the dependency path, which is alright, because most of the
 * time we can figure out where L1 is held from the call trace of L2.
 */
static void __used
print_shortest_lock_dependencies_backwards(struct lock_list *leaf,
					   struct lock_list *root)
{
	struct lock_list *entry = leaf;
	const struct lock_trace *trace = NULL;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		if (trace) {
			printk("%*s ... acquired at:\n", depth, "");
			print_lock_trace(trace, 2);
			printk("\n");
		}

		/*
		 * Record the pointer to the trace for the next lock_list
		 * entry, see the comments for the function.
		 */
		trace = entry->trace;

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

static void
print_irq_lock_scenario(struct lock_list *safe_entry,
			struct lock_list *unsafe_entry,
			struct lock_class *prev_class,
			struct lock_class *next_class)
{
	struct lock_class *safe_class = safe_entry->class;
	struct lock_class *unsafe_class = unsafe_entry->class;
	struct lock_class *middle_class = prev_class;

	if (middle_class == safe_class)
		middle_class = next_class;

	/*
	 * A direct locking problem where unsafe_class lock is taken
	 * directly by safe_class lock, then all we need to show
	 * is the deadlock scenario, as it is obvious that the
	 * unsafe lock is tak