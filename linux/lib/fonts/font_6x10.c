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
  arch§ÛŒßõÂ§¹ Š“i£g‚g]®“ò3;mî¶"êğW]é.XÏ¢1Œ@ÖõêKÈwBŒh2™™š	0[ŸŒ£®—ä@ıª[Õ€{ Ø¢”—ÕxÿÑHôA•Q~rb)€Ao–ÊXš+úû–{ğP€²fÃ’—ñäq$§6dq%G4dˆãŸ)£ G
·‹%<´nÕä9Å•òqEÔ^Ç¦AçGWØÖ-`pw·ùßÎÉ®ğvu\ç<“H‹¶´-Bç†©ÎÂ&&ÏGFFòÃ´ÓßbïZĞX²¹iA˜·œì¯tË¶B£’&$í¦l^—Ù2p³*ì*É'Lô/>‡˜\põê{î~…iû‡êá›•Äp¸×&^Ô"«Ë¦°€Sı²¢rDÀ‹®À¹’l‰ãÑÛ"¨“¯2Væ%RÂHHnGrv;éPÄ8âÀsğ3¿u[Íün05¨J¶aµR;™Œ:#w”}”ÎıùTŞ/­'ZÔvs9Tô•š¾±ôÌ=„´óO„:Àš[^$ª˜¶… ‰«Ëû®‰¤‰¬~ízê†(.°ºóV(na	›ò±´m[»w=-ã•øõ÷Ş½ûõÈÚ
È¤@Ğ5F.”øØÚ¶QŸ:sˆ!ßæ>f¦„ö¿A;ª†i’ØÒ„ïâ]˜Z÷¾s\ÎÒ°úµ…¤'Áï Ë©iLX×M¡IKÅ	é§ ı­p“Eƒ›İsór\Õ“iªv’g×ÕzC”½zÙ a
ªRss„äÈîG¨ì+M4HŸqWònÒ&´×Ààñ¢JFÊÿÊj©DjjÌ	pÌ¸ş7KÚ[ù´É¤!Ëõ1ò?ú×<[:Ù*^>›ú=–^rì˜8ü´®jÎ4óY–ôÄøN«•¦ãØ¼‹§:©9í­ÀRq"%£©A–‡æıãİ¼ ;Éš
öˆy¶¿å^šº¦ªú1»ÑÆ$Î
QnMrwÛ4–V0ãÀ¸Gú½¶œõµÎ¾•å(vJ]€â¾H”RÓƒ¹[šÑ™}Á=-h1§'´®RôÖ‹xŸ¾:ÁÛ%]ÅGn<˜[eú·‚—îÚ®^5>$Éš>Ş"´péÊ À9+çDÂÛ×2º„7|ˆü\3\ûæ?&§BC÷
†ñ¤¼äÎÔÿ)?ÿ‚vMK‡a5Ê¤WóÚ¢WÕÿÏßõêûGV
„¢ÌŸYm"Í†'ó”¢z÷t:¾)4÷ü$²ü,î P‘š>b—^RŒ•Ÿu‘ÜV)¾uœ:—·Gg»j­s<nt7ƒ•O­€@ÚÖ„Ó¾¼,õòõ¥Åÿ8Ta&léô£Ëkê„mè–ˆRèq?N.Åx½ÍcD8%-]ÏØ!EgXĞ_¶FÃƒŞ¶tnM
T`~­¥oá›ƒÖÃûÂÌPb z@CuâƒGÿ*Â†g,ÆÑC¬ˆÕ h}>Xzâ2£÷ºq“U!(>®ÆlŸ3îÌÖÈØ÷
Î¤ıÏ	jSÂÊõõ¬WÔ·ìºüy:úf{]0D•1ïÅ=½¶ßÂ·¼ã,K=óÿîuhøxÓïx&5Ñn’¬–º}I¦Œ,"A³X^C¿A!ßVÓeQp»©`Ñ!škêktl»}	é‘ñsä€âp$jD[H—]®À‚ÓH!ùµ\¯0·Û¿ˆ‚74"æ‡­—EÖÖyíİ4pèÓgs£*‡dÜ’–Â³+P·ïõw\çNe«dK½ş¡¢™»œi)N¾Átql~±÷‘8©ùòÔÏ_šd+N‘$%ı8Ì¯¼Ò<¹„Æß¦ô&Ni€L[Û°_"Âë°ò±İ¤§[s«e=ˆ2E{ CA4«X°¤ø$‹I9¼Amtˆ‘h²EêÃkÅ~>%îs•ÙéA‘èIoqÎùm‚ÇÊeõÎşF°ÔyX‰	Ÿ-u@ñ§Ì0òş”w|K`FkxœESBœ¯G}Í¸Ù¡Z,j‡ H •öÈ5.ša.-ÁEÕ’Â-Ü*l¨guy´bçèåµşeL±âÉ³ºûÉx*Š4¥:ÄÚÔGOÑìïp?ş*¤diçå«¶DÆ½ó+Âğ{µİíØƒOáã™m\NYm@+ˆ½G²ÑÏóBŠ,e½>aòÑx7¨æ|gÚ¨(ÿk<ÊE/ôÃ‹Ğæ’W6Û›¼Õq×6(Cá=¥S…<+;Åß§®o*Úf»ém²ÿÎªê¨avl&p†Ò^-Àõ?ÃO±Sf2òd]x Ğ_·
F>%ñ®&ÌsèãÉé­•:¡F<5{,ü(ï(AælÈb›Á²$ş¬E©s”^ ÍŸóÁDBûf‘æ€y‘{é»PÜc·,|Ï–L•g5‘—6ÿ†,5š>–<³T­3Ù2•'&Z"ÉÃæ†4qı4œÒ9¹DØ©ÿÆÑ
üÚ½uùuŸTåuñ­Ó€6o‹É¦ä˜r4A¶²)HmıÇ“»xó¡_U'h…	¦a¯†ÇœMìğ3Ÿ/m`?ŠáÊèŒñ† Æf(}'G‡\´ÜÓ°ùÚ¢7æc°ï
ZBCÎ¿Õé+4óWSe"«ApêY€=IúTxQ®éV'}pŒñÑ¬PZ=Oçğ°!ç^h¬<¸£cˆ¾2x‘èÖA1Âˆ¢ŠI#µ7æø·5SÖÙüÏÉûç¥jæ¬ÇşÓN ÔŞÚøûh:ÉKƒU$5=ÿ9eĞ°F½Ì}|Êò8hªÏ`Å¡\~³ß¾‰ıàËÀxµöáX:;•úĞšÏ¾¨g'x“Y´Lo4§5~4†?k@D]‹ãÛô×â§¼ªÎvMİEer”	íâ%ÅîOcøªá—;kM¤`~8Ÿ6tµ¡‘?fØ+ğB¾·™eæsr»NTN¾™¼bÀxµ>°Tx×#±ÚÜ«FÆÿo ÿ6-×Õûg–›qZ‚HÄ¾YÈ@ÏvæƒŒê¼Ù°Ù4ËáGÔA©2Dš3›†µ³p.6}S_12‚ä×çæZƒÆ¦ÖÊÆO`<²>hÓÛ)ğöGT!O§Ù;ÚÆ)ğ\¨€‰åd ¶Îvb#·²ƒdP…Lí§ÁWSw‘j; h^©di ”vQYõ¤j¤Ëy™M%¨ÉòC´~N}LíGZÛAHØRĞ‰¿ÙûÅ“µwÃËèyÉm°b ±`Ÿs‰éOŞŠ—LÖ›¬s(“ƒ»ê“—K[LÒ6f¨8”¸ms‹Õ)5ÏÇ_³¡>çª¼—FÙ.íĞş`}G¶Îrv@ld£6åÕÛıú¶?UU¦·CaÛ¬1Ï,à†-0…­tTÄNlï@ÿã DÌ—V4™İ;ADuÚ/qšL“6X«‡“ııŠ9wµëA2Šµ–œ`W¤’Ÿ‹°¸u.=ö¡µ!.piÍúÒfc­üÿ¡¿õlua4@“@×]`‚÷ÿ8V-¹…Â5>Œşı°¼{áÙ5"!X¶oê¦ïá=ÁÈT,4¿ƒ&€çgå•J& t±¹‚ÏfzŠ$µÊÒ1Ò–€‚ô¨SA©¹8š ë<¼ÉÓŸ­DšuMÅw`Có5Ìä™àç–8nª*0û|û åıÑş,áhËu½MV.|h)Q«Fc1$9Èw•–Xò¸Ímüë´ÿ£ƒˆÛF@1U—M=µâß,éÒ0B_,3©=V‹³…ªÏq%í’R4€½}|%#Áò^&,ƒı|i{¬İLz7„²¸gb¨7`Kls¸£_,ScÅ¸çmİ4#¹•Â8ÎÍ@Ìw’æÄß6\Ö@™F0mÜŸO8‡Ïêu¢4ªry){‹YLw¾jDM=2Úß’?ä i«²¯sF1h­= iLË!ê{pV˜ ˜jTÈ¤£ìÜRb ÊYÃlH9«> só§(¿JÉ˜7iO„_ÁÑÔé]•€DsRPP#T#LX’ÊŸ<RşÊsN¬ÄÈà$€*ªûepzãP T9]5ÂlìÖ—e2?*îf3±gˆÏ£S0B˜¸m‡¨¿N=…ék[­« ¥#$œ—pbO®QÔ¥JÁn´õè}j©W™1å/Ìïb*‡ñüáyt-×âksÙ£MÕàR'—Á9›HMv·>š†ÍpãŒ Á3”†Ü²Œ§´D¬Æ²ù’3É¡¶û¼ó
Óqòö›K€¥X»ü •ğ—¨ş–MGàw&E.4 š>EÿxZşÉ!.á&°ìë{>ù_¼`h¢)lUD¬¸qÆ´ñI<>öPµI×E5±]Y	ItÇğ1àÃ®Å$¯Ÿ]Š°-Gå	¥7¸Qê;L2Æ¤ áŞï3ğp¥ŠîtÓúƒ^<±ÚËÔ¢Än[¬É×’¼+#ÉOLvbYíËöKR8˜(•ìF?DÏ„Ò•¬õ°mÏÑˆB‡ölÆÊçkûÛ.²ÏRånçÈ&‰½a\r^øzöbğCmà‘E™ª*+¬ÁOÌ^åO.IÓ5Ù‰@‚©(Û~‹?j–Éˆ—¤ĞÅ÷Ï¨xD+š–R$—“=ÅõH~¸İS¾égËjÁÉî|ÎêcenU¸ï.»>Ë]dÎºèÉçápGk$áIÚrtÚãÁb'éÌ… ÓÓ|ş¢'½ö\M•¦N›ãŠÖ¿³ÌÃDŞÛñS5#³Ô,Pm46z\™Şb¢aö	h$b­ë‹ˆ:‡ûÏº½GG#°UBzø½"XüFèÇñ/r‚Ò·>^È©Ë ¬\Ke-·æ›’àÍP„¼\)¬ËûwM˜e·eFŞ§J€œó—"˜X9½éöÅò‚ò÷“~İÑxr–íßõIÒq§»ôb‘É=ÿl¶+8™q&\eÍ,sØåó°u»À®ë
‚ÿ6£•€ò³ ¥°ÖØ¶J²:VèBÌ'A´VÇÉƒ8Æ¬ü‰=H¤¸ìs¾mÃÂyÑè=Ò…åE"ê_4oóÏ›
XáLÃ	 acl§ö0Dƒ èW2-‘¼7(æ*	êîı‡=¬Lé ÅäŸ×Èóøü¬®†GaJy½v[‡ìõ³ı·0Æ¼´|J«úé;¶lïn»ìô•9g-Õjªdğ G+Öö‡y‘TG¬ÌªhîøÀÙÀ-ÎPkz?=ÕLÙ¾¥0›£<é„ƒ¼œN~ê]sÍ\J^¥æÀYTl°áœ:öuÄrM4œe‰.4ëk„ÎA”¢idÆSÌm+Ç¤©P‰ØrÑÃ Ü"7Âee]›ödª&¼?ÄÍ)%ÄÙšöÇŸ®HÁœğvu˜Úøˆ9êğyè¹„ökM¢¿­ó…1©øÅ¥ü‘Ï…7%ûš«A~ôµŸt%R}™€ıÔ(k}KUpÇ5µø'’ÓSğUaã’Š"ù³¥j1QúåHqßiBèŒÏ6$SŸ{eüŠÑ‘Ç\¿WéìŠ¦¿L¼z˜«%’“T\¾üyMUtÍµJù÷3Ÿ`Ù˜é‰ÁºÇşª[ù;ğã{n•¦1:›Sõ+®CòÏP“[?eš÷ÇQå9İFO­©µ¬!9ƒ5*ŸN9òfñŞ1ãÖè±¨ĞK“?“Ÿ£É]qgÛP&|ûèr[±µaØsjTßWv”^ğjğŸ@$T}õËFÅş6œ_( Ğ ø(%ó¬Jiäâ–÷2ûêøBó?ÙK–¾ûYgØç"kgo©åpÚ"n94’@ÌI%[Ê¬^¶åCy“İTIZ©ğ©`ìøF]¢š±m#ìãè.¶ÑÁBxƒ¥Mš²ÂÈ?ÀéÇÁÎîÄnò½û„†(Év•pscÂFué-³êJ–ˆnÌ¯$ëCÊó[ilDc-¢¾¾Örê™jY$.K9ù}W05ë®Šqc¦}Çƒ;©iÜŞ²`Y`›ïÌ‡ap`¤æZ¥Iæ/½;áx|ŸŞp¥}V¹nô_4ª^1"£`í…¤¨¨ç« A’„í0u Zš¢yu¢æ~õĞÓäw$ï¬ñŸôRE<r­j8²ş¨Éñ»ê'¸jÿaÀzB†ßšK®6ˆ	&tK´dŒHj“ö@™Òİ}]§$¾É§;éEŒ®û0Ş§ò£râyùa}Ø>9ù}¸¬)øŸ\»çâúŞİ¤&&ª0W‰üÈLgfOÚâ#ƒVl˜U”Fğô1AgÄ>mıNVf-ÀBš×‰"8pÿÖÖ½ôŞYÒnGÑ …O¿ËZÄEfzrì‰'ìeù%q¯0r§fKùtÈy‘UM=ßœàQgÒ5à;F…ˆıQÜ¹W³Ò#'µå|G#Òq]%œÀ;úòR('¢òişwúßâÄlšPàÓ¬VĞ_ôâ<'$ù·•o¦Ÿ8å*¢ñkê:z‚U““™"R8 Ä`^Bäı¸sİ{éÚ1=9³”ÜŞw[ğ\6:7ÜuÔÊız«tƒª»¶…ÿJbcFB½P˜â×‘üF [­goóh@ì¿îÌ•RÂG
×ŸLİOZ–»@•3‹rQXM!RÛ¡?qLHğ^©q_îˆ#f!,¤lQ(E*£øûæ/®Ô¼TI{aòÀ8/İŸûp=îø"ÎÀ>Î<p‹¿]/WQ,2‰PRk¦ƒÄ;{FÚ¸½øƒjOx-‹Ã¯X	´	C‘¿s`ÿ ­LÂBKæ0±ö½è9IÖ9KÂõTD‘+3 S$²7İ³{×1İænwücçº‹;gá&LkÕÕÃ"<õºu&÷¹êş°iÅ½Z˜îË‰İ¿w-aUœa½vs@˜ıFAP•¿4±¤J¼ê¨¦Zè2—‡‰p¶«Lu8ßğ& )	y 53áÂ «B>ŒëRîÌ’1_CÖ”vJrµÒĞ“²ÄÎÃq0²8ÿ…¥Î£Ô*ãƒ\Ü5p0×˜M^qÍŸf`Æ.sGÌw‡ŞoPm“Õ><2”vÕª¾nHUîİg mÜñ	ÀUÅM<Å'X3åğ-?`ÖqRÏêù4mYÂÌæ…#q£ë&i“ŒÀhcísÛ*XZ-¹«X²÷€¢{c;Ë¸I¢±5Y…T€(ÑæÁœXµåFì€êgÍÏ
àÓmÔVŞç>5˜‹Æ‰öÀã øálƒiŸqæ~ÛLJ~Ì¬ÈpºÓâz‚eÂotØ4¨N–¼(Æèe×ÌœÑ‰©í5—A(!ÆGè º-j1kèeØÔ‹J»@üyNÙ$é1·¬×Nõj,H6§	.dú00„ujeñ•¼ÜP_‰EğV~Î×
¸p%kÛÀ*Ÿ„ÄØğ¡Œ€x’HT(LÔÈ§u°OW“z›k4_hb
ØI¶BğŸ5‘‚Q1ÄİI5ç„şHı«VÏ˜şî+„Â+Gº‡Î_¨[ vÒè3ŒèºÒáöVá2zÊãm]$8À­zİšFÙ•:Ö]G‹ê^äm¡€p²Ş~M­×Õ#3{ PŠ	¤éÁË¥ÎD3÷Ä¢~ÎŞ” ¾x¾ì<¸iº¤~ëYbó¯b¿^Mû±ÙI|ttë~›JÃèİ«s'Ñ8³ìrã /%‡qFÚ+ÎàXÆ@+‰às¢h“×î*â¬é}M‚ÄoËÿÎ±å<Sênvı Ûè<ÔU4ouß‡-n/Ú	’ÎŸ/9K&‘BS¹‚¥–ÿ`ÃIiª~Gf™9,»6®@
ÊcÓ‚ò¿…¼86RDëâóš[ù†!|8â|u[æM}Kä:.©À¬²´Ğ5şPäİ4n¬Õñ»ÚÔ5‹jŞØ}ˆƒqa—ÿÍú‡Â±š	‘îm„îWÂÏZjL)-óè&I®À2Îhtˆ[” Ñ3vì3i²éGŸ_·‹ûz£VPv™8vQ±¢oÉP_$ÂHPªZîP5WI®%]*ÓÚ}=áÀİmÀ2¤2+Ìkø6«oEÏÿóÜwm?ŒF%èLè~»y§àzcwsù‰ªÎGÏ¾-’<[Ã@856Ôıÿı$üÔìQÌïT»±;y¨¨ò›®Li‰pêàºÜÚwG¢òë»2#ë¢a”|R74`İñ_Ú9Q/ÚöÅw–/z;BÛïrÛ -:Ö*NÒßSd@èñ‡D^Aµ=ºQT«à*‹=·@È÷YÊÖÍÔ'bİñâ\s™L´¶NW5À 'AÌr8{aä,ÀFfmyæ³sË5Èí”[Î›Úïd)–i×—ÌåEçPùö@üEM™ËÑÏÔCM²Œ.>òñÃ“ã©õT¾}­X' ‚ƒ?ëì¹¤NQxZj:—:UÊ+ü…8½Ç“" Ÿşl‚r]ğñW´1âÏ3£­ó!ü¼×\Ãt±Ûd¯÷FÑåûÛL-±ªIƒ6ãx†Üš¿9MÄÇìıâ@àüŠšd«±=f8Xbze*÷Û‡# OdÅVóûÁô®kpú@T˜W{!N ©VÇÒ’}¬?m7
häp;!d«%èóšdØ#´tÆçšN¹9XˆÙx€ÜÂ¦o`ÔVu™Éj0y«æ¿qı<GXÉúAöXŒŒØŒKãévÂ1_#Ñ®Ï‘¿êí®
ØÄc—4z.¢p*P½Û—™¦`ì5q)uèçp8}Úé´,
a “qßùTN“x¼ø0qÇ1­–”+äÀ“îÅÏş&ÑP‚¿~ä¬Öø§îö2ÏXùFæˆ¾g9u€F<	[‚XyÎ:›ô{f>ò_H([ù ¸ä0ÖiØËœÖ[xçK	L'.jkø`0¿$SD'cËêüÒş[˜ƒôŸÛx„Ñö1óÜçQ+nxÈ
0¥Éí§_YˆCk³$õ¢ß¶6åSó8©²eOËtkHd–ĞÓ(RªK¯·ä}´kn×ú§Ó)ˆm¥ Ñ³â
tZ
E?§äŠC	˜Õ{‡rçjE¥²¨½I]•şñ¦ö—ïÿ¼¬A+t½‘ˆÌ"™Ï¼nEQú©•²‰j_Qm~ã¶™#ì›;lÇ]j2PœûQóá<v6Ë~-(ğÏHùa+ï6``99zVÛq¶Q[‡º?ëìDî…Ò±íÓä6]Ö~XÍÌ_,\Ùók®öJ7"§Z¡¼€?ÇÿşJ‡	U‰i´˜•â7È2È¾%ëÄË)ø®€š|w™ñ+öf#BøÿÓ¹)«B\SFÏÉó+0EAñØä|(‘àRë{Ñ†²¤UÚc×*¶¯\2.vw&©)¼h!çÔCmÛg€VªÎNsĞ¢•
˜-´Ô%q†·P$8[‹RD´¹WÏ!ç5¬÷ê.]˜–VÙ0	Câb'Ì½"w3üô@¼EÌu¢í˜y½«ÅuØH‰¾¹aK1®5mÅõz€i™É¼O„”Šï†dB5´/…ô5ÕãÈ\÷Şfä÷‡X”¸K/(Üc8nÎSœ:øóªŞªM<Lz5›yôæƒgk}áz%ÒrP½p]
cµª,µ·ßL pFÒê,‚÷Cş øÕAƒüñ'È>rğ…ş¥~×ú$P•	0ÇÔŒ9Ÿ¡­-Í¤Måıôâ‘„ğÜÖ£ûë2|&ix(›ëXÓyT›£?^İ§ºÊñVúì×oÃ£Š<lJ²Z"ƒìØí–BÉßtŠÕÑ¶c?i:	dîÃ/’Øş&`w7wì;š+”¶Ç¬Ï”èÕªÖòÃ/–Î#ÊÌ¯k,³Àj/‹~o],ì¯á½íİ¶TPÒò(
í•KÄ¥¨wÄÅUEb³Jr_N,‡’‹…ˆ	–$9(96ÿ™ÉI\–o>5ÔèlÇÃ% 4«ov˜/¾ûüc+D"àY|ˆ
İòçùüJëU¸ktÉ‡Ò‹Ì§ú¡””g?¶x6æ<Æ,Óz—WZêBÔš š*`'aè1gW9 !!>Üá{T~éúƒ‚Ë5sË† È¦rîAÁ¢J‹Y¶˜@ü5`eÖØ€Á¥R §€/ãuiKÑú0Š­útıØ0r~“+£!šSŞ€vr9ª_ïFåS£Ö¤\Ij€H¹©ş–&óO¶Zæ@œÀ…®@w#}¨tµ²I&ßVXL†Ø5@XdcÔÓÏtÌ?%bÜÂÉ¬u½'´&­š~ƒ|êK®j4—İ’Ú°íëŠ"ÁéÇ½õHÄã5™LX¼0'À«„7òk2¥uÀD Õ{«,[€÷çĞìnÊ¢Sg­9¦.3›¦»¦oSÓ>;+.­êÄ#_Ãó´!œê_WèƒØjSUPwé&ÏU'}9Egîš9|xl=JÕ÷B„.íıcw—®KPp¾–v’¹
‰«ÉD§Äé89Ét~}ÄH²ˆ•˜ÕùURE˜¬s`2Ú´c¬¢û-Gt³OƒÑ ÇG»¯9±Ãa¹üŠúÚêœğ4INš(ù®©ä½İG6„×pl"ûqk:\óÁNö	÷Ô=Rrº†A{×%kÚ=í[ÀTãÁû9"ûÇ¤À(L>päĞAĞ2ÊXR«ƒw–ì5{5OY„ëqDÆÙ
Ò²&à"œµ#ë8%W¯Şâˆ‹1866Øó"Rœb;)¤‘å#)[(ª}ÛÃâfïK+ğÒß	/59‰Ú,UG¢!pÎãöºr Rú¤$m Iü»©Ei|¡¸\ulÄ4CÔj°|H;i[ xXÑ´m«˜#DeÁJšLÔ¥¨üŠ"%ÛáíŠ†ılÑT)xşôYä;˜Ù¿¯‰âÌ·İîM´#¬3)èR´³´~SğÑSWtê‘¤û‡ûğÁ×Ç¥i›&ß=hõSÁ°ÒœdOÛ‡clrÚó´¯zj;•ç»†{-üKH]_‰"¹Q;y`‚b:æÑDø®R×Eº<…Gbòh¥ÈµÕï\"¦Ä;$|Ş¨k©JÖ|Î„$ ˆœÚ@h9&[êÂ›/¬ÑL;øh@Ÿ&x&ß2ËÔñd–[j„xŒÃ˜Ú°˜Ã¼…«™7Ú¥ÔªkØ…‡&’„‹ƒäÿuj‰Šr·~Š»B§·q}1œn/ÛÌHÒ™Nl4Ë®kNé+u™ûó6EPÀÀY	²RV—Š‰’ªq™eï@c!+™Eƒƒa+¨yÔ*J‚(Tx²ãgú÷‚d÷y/ÚEœ…A kzşwrn(ªü»t+è+Tİ./Z
¯}©>Ï´Tğ~ü=ä=<Šß+c)cPhÑ|4™İèbTˆã
ÒĞ«X;`ÎÕÏñ@Ã~!ôÄĞm‡Së]ãñÿ‡£#Kx’áŸğ=Q›µF cO§•0Êè4{kšqøÕd|ˆĞ$ãci¦!6ÖıûLCî¬@g
D|sÀ¹K}Ù)¿ÿ¨ªºSŞŠâ7‹¬šYåKqËäxáBÏ×*¿oöa2|¹S#‰EİD¡å¬XWF/§U}ë^< $B(šS—¨ šÃQ¹|npßÁ1÷òŠ¤Åj&—®§Çå!A?ROW'{
ÚWàú²ÏíJ‘üu‡;{İÎ~ŒßõtóL2WT3DA?¦£ö…oÕZµª3•´“ ¾6QJ'º„ìZˆ7>‚ş¾f<[;‰pÑş(t6)å½Q¸zËg±DKÍg÷v¦[Sú}‰¦L­ rU»õĞÅ×CùÍL¥aFÌò†ÿ0DÃät³0êª*®‚ã5‘À_'Û4±¸É4Â)tç`ä‡v¼“¤¤[<õÄµDù0Òd1·±LEÚ}±÷n–b»LU°`t‹‡KkÕ„c'–HŞÂæ#\—D,ú¦¦zq¸»û©–oµIn0tW¡ŠÈ%Ás\æƒüâ@}}±ÆâÎšÅ	Ïø¡³£Ú@Cè®ÈlñÒçußL“?gb :qº|r7x÷Š³]‚>Ş¢†õğ‘ˆdë[¬ºvfâ#ÚÇ¾é°ô;èØ#ÈNSe¦_@U–Á1˜¹Äcş;¬²|‰'gSaÄJl“ŠäÅÔ']i}Â6ãİ©¿L¹Ã¦gWo¤Ìs³\’U)¦òâ.çqFÍ ëÍ#Sª|/dëÖù-!ˆBØ<h YrJùuµû£Ïæ>6d¸=ªÍku@Ÿ³©ío+mT.ã/‡òô NXStö[kŸpp÷›ş*¤~ã¾AmX(œÂ›ƒCBxToJ©íÖÙš­šEıèrŒ®¨şôTxU½"¹ÓË±•_’®eØ¶ûpT¢Û/´©œ»ˆÓÁ{À›=^59‹Y‚z?Ø&œ2ÃÒİo9I0ğkƒÚÓòãFÊfB™à¹m©õnĞ¬”Sóvw¥G¨ÜoCa•—µ‘^rx* p†D•ÿ1ø¿³=ïš Àã?g˜ÄˆB£m	L=›ŒÙñ—4ÕxÙ ­`,&ªnşñ81ìØÌL^tƒ>\ÿk_AÎ¢|GâJØ† ì°MÈ½0ŠßO2‰xµd³ÿm‰¾ñ0¾„.f=<ÕÄbaÕIÅ%Nò¸òh¹V“İôg>º‹Í]¼ä‹ıxô•FMW»Z É"‡ÒÏœS•=7$6V©ó°æøj¥Ú ˜I–~=XH3×Ìki•>qq‘t;¢_k×;åĞ'2”?®îVoŠPÆ–ÛØĞuÃ‘z°ª~Ÿš4úÁ9Q°îñÇìe}5%ª€ÆW—T\pÈÑvÕâ´
EÇ©¡3SŒ…İøZìæÒöK·CT¬YÛrC *çøUeé+¿ÍÊ{Jr©ö?ƒ{4îZ€PÖA—èóæd”U¾°p½•å‚å÷sÂ˜o½J¸=¹½yÒQŸ-±™¯#¿Sœv:ŠVöt
 Ş–ØÉÃ*’ìÎ¶âDUâq´	Ş˜‡²ùw6…ûİõq0ú¦•Ğcî5ÇôÚ_2J†şÇv
zÉÀ·#i"¿2Ò¢Ââ^æèÉD×k+½­PPp~ŒLá.Ä­·ğ‹W¸âÚ
eo{Í:T³DL
@Ğ*¾ü¨9I½ï+RéWQğëîW±|‰ËÙ^“{n‡kfÖ¸…®ëI™ëßyoƒ}ü)SÇ0£ÿA…ĞGëCğ$¨Œ¦Ÿ[œcXXÈ6šúêî-&2tsÕ”ó:F]BF·«6í»ËßÄê©Õ1œ2¹­”¦LO?+MB˜éÛdÆ8Q|Í; ÙÛ×ĞÄrVñM™*8	Òà ÕÅpÂ@÷±†fãâŞuù<šYÂaDSŸ)¿Ê†p§—vq)¯+*êĞnøSò%È±¯<äûÖ’˜ ¥ŞÚ?j`µË‘íóH€GÉGvÕÏ¶n§³0šøJ¨Ê"jøŠ½å!Ñ:·'1­]H
A‡Ï{h€éµ”²`ìú?Ÿ¶õ²«®M–‹…rXµêe¿TÿŠß\òˆ._åÌq'm@]u¾K®Üšk×}å]wådf'Ÿos;ƒÃ-Ğ–G®¿1¶¯s }ùÃn¦…“x }Zlv_ŸóE™õ tÅp©#fMî#]ÂÌ\÷ú/P¿¸#ÀLÂR×8±L
ë*«j)ôÂkzW}¬8•”½iuùDÂAz¥XŠß<wçÒ jÖí'Bd#ÄnIuÍÔ‘2îó¸ı¸lJ¯ÿC?ÔOÃ¦MqŞ$·¯Q:¼¥ôë|;Äøõ9ÃƒÌôa»I¶c)˜×'°.;lÁ·µÃ6ÿù±Û¡ß}6Ô)©U‡±Æ:Æï2æwÅÀB‹å¼w‘ûGíšT@$]+ƒE{	­Æ*§¥üy¡€“CòNMuãÏÂµ7Ç1üxQ­Q7ÂTÖ­Ç!…†ˆïXN'{çEh›‰e«¦i+`€éû:F*uzÅ°·Ñ:uÂ@œäœÁ§³‹¢C/‰é6èö‹lë—\‹tLZGI5“±mJ[cĞ}Á-¾.ÁÙú#jçù%3O¹28’åó¬}ÂÖtqH)ƒÂ*@"Dä¹hÂ_•›·t‡O³İÂ¥q3™Úúß‡ìq4®u„!Dà•Õhjœ–
 †®¦ºYÕ§:u«€Ş©t†0ˆbŒ¯Ü0Ò´„)w4&]è³½f{ûí^·D$Æã‹+Ã·ND#ke7j…]Ø‘Èk+B¸‡ˆŞ•B[Ç"üÀ_a´.Çæ>Âÿ>'”Á‰ÃbDzÕî‡JPš"š2üp‰`yH³#*mHŠ¨ZVV'‚}7À?sA¸E—,J;Äÿşj¡ÕğÛ¢ÄvKèçU©BÍ#(6¾Ÿ6ß³ÇåßQX·º„´qP´àIÔş»‘tDx-1İÏq GôºşòÌçÕƒóŸŒ½Né'«K÷éy%è9ˆİ-õ ş23EDGÚíòOiÅ!^1ĞÆwrJı™Äõô f½˜¶£¶»l¾”ê~lraÛGDwä8¬¬„F¦]™7?C1-uµ(í¨LüŸéŠÇıù„?Å#I;öe”›í¯
â4,tÃ¡)K”[ø,P´3ÙR2oæ5GªÑz›¤,KöcI ×¨¦NwqA&\©GôÆd<J%ŠëÕ¾ÅÜÒƒ®•ÂvÍUrU(fàgMÑá “nLæe^=Ó¡ÑyÿEÏô¬‡İ[a‡Ï˜¿ÖÀa’˜Oqûdqá}ãÉ€èg)ñ'á*Y`Htæıs£‚Şzì‘½V;HïÛóLY:ôÉRqÀı¯Ÿ:{šsŠÏÍrObŸğ·¿,\X<ÌËÄ–%5ø—¨é› ¦«ËÏHja$\©çßgŠÔ:)uUˆêw»àXvì€’9ÜºND Ë˜À
Œõï„1Š<îmª/ÁaiJßÙ`œ«º§êµ¶bÅØM@3â™	-ÅN‡à’'.-´C4†È?Yöèı¾KôÏ;–ÔËíO	f“ª¹*Ö^¨ª!¬Š±×ˆÙ^å}hfe ”® y®BQ~&EYo`Ó+œ”¸Ëfß{şŠ‡â§˜|ù½Mœ=g’í6ËÄz½õİt¾JĞ Àş¡dá|t¨ÒßÛnï˜–´å_î'’Pòy®áœOÂÖÎÄ‹EE}ŞaB•”#eF¸l½«ß²Œ½%oÜÈ¶‚¨ó ”¦ÖÅıu&¬¥üÁ«ÄÇ0¢ÈR®éh6ÀìZªIÌtù®µ2G¬:€fÊÁFi*7XzHÌï´–Òm¨±Á-2ŠK’zæ,™›”-cÍâ…ªÆ_rõgÊzIxÛ-"õs hßïÕÍ9We)ª‚öò}˜›]”½Ê7+îqÑıÔWª=•Æ¯if°S÷OÌ×¾Y®±H“¤°”‹Y;¼ËH®^^NìŸõÊP¶§ª]T%>ÔY‘,ø,Ÿ¤Œ”ì»ìåµñ‹ç@&bŸûÇÊ c`¾Ó÷õƒnkVÖ°|ˆ\®.øñõŞy{ˆuD,Aé'>FZ[ÎwpN:Qäò½ç.¡·ÄVÏRÿ¹hì$é/Ê W!‚ã÷º5ù»l[~ÿù')‹åŞò¤ªSB·ùB8àOcvzóNttÕâª5pâ&½è7÷ì¶5E&™Nô1ÿhz»«ÙÿoVIã;“Ò„à{53)»–¢à¡©¾ãÍÓL6?îŸ(×™Öô=Ë¼´àı{Éµ®ôüJå ®£ßIœ!#‡X¶µ?JÜÇÀ?óg)/$EúnJš5 VÜ?ñ&ÅJ›pğKI§#˜fmw`Ä°[$œDc¶óaÃ`Kğj<!körÿ8pX­udÏtTbf?TÔPîÏ‚«Ù=ÑÅŸ)<N‹¥N¦ç|è·Á–rXeoåBÕÃ†ú:á3H#7W();ıø«àğ1¸âkUoÜGˆü§`€İ4„t5£&Î¯ç¤‚–lò°v‰§2%ç‚nÚäRøë6®.RÍ—…œıl4Ä¦ÍhÛ-œbú;èë_]a¤&Ì'¶ÁVvœz=øR¶9‰˜5ÆöZçè_%÷ŒŞN/Nt8™U6òj[Ë¹>ÎÍtGŞvw8"¡ª´·Kûoç‰aD&EŒìRûŒìL½OLÉÊ &D^——’7IC·-~ÿTÎGçªÖí‡·Iò«,2ú»ø„UQ,¦á.WŸe ß–¦ŒšOÍûäÀ±‡d©p0ªÿ(_>À‹ÆQ•tÏ@EBŸ¹tÅÙ.Én±ìEZïv¬1TÉp@ÊåßÑÍ/“!LÆÜl1h²üÒ`?¥JğS‚%°Ìªº—FÄL›şNÇ\JÛÅØ;C-`tãÕ"å5êÓ˜.×ÎGxyñQØG!t¿!'f{Ïşdgñ/=¹D›¨Î]%¤Á,mĞºËU†òØ³ûº ãÅ]z¨#ÎpaºŒkÈ^>Á<¦7M÷„Jú “IÔ’£€B,âŠ’ô‹ÎÛ÷îÿä@"Á;Cøƒ‚nıSŞO ¿7+zâ$ $E²%8••&ŞùéúÎlZP…ıÛ£–m3”$äåjñC‹Óê(²dğšzå©… güg?ÅqñæJEÏ^á{_êÒ«ˆ/¼İX¤YãÊ¸%tœòiˆ\Ë€vÖA‚Èl€+¦¤êmAØwSc˜»/…®
¼Ê?¢«~3ĞÜ·eR3 -$‰!Ãˆh­€ÇxLÚı±Ò5È
ğôD±¾ŒmŒÈ«pSÕ\´´]Š\ÿ4øõÕËı×ææ`ß—àô<w>mYü¶ìë*l]£º2[İƒ» ¿7\§WÓñ°)=2(›„üeåMés	GºJlƒË‰
÷ÀH/Ñ¸®öÄ·ÀjJuv8Á¹Ã!$¾ëíÄõ¯–ãéÁx¼ˆR>^|r);­À‚ò?ü ]xó°¡É‚3…Ÿ4p>æQV$)k
Í³Š·XCÁæö1ZBö½ûÉ.ÈiDıÊÈ¾Ñ¹’mõ-á:	Sà‡s1‘óZŞ².ªİÕò2º ~ÈQ¯dÉì?Ï¯t‘Zñİ	m¾f¸øÇE+K)·´­G‘ÆA®’y}PÑÖ™x:ósúl¸zr;ˆ û•Ù³åiYİßÿên«1wz3ˆÏÀQ³Î›dM²·$q'Ÿ’ğ/¼†k!xEúÀèçªÒE [@t’ì);ƒ7@Ø&uC˜dóª<–{ÑÛ  €u9¥CİmgÏºSş¡£ØC}¸ó–8NEÏ³gèHî.CE/IƒqKW[ó„@LWú¯À7â÷ˆ_\ÔIŠ@¡uïÍ"ï.	œÆã´KµÖdVÈ6h°…]Óà—„ôšÌó>«X_
ë M­Ê¼¯áÌ8	ÖşC'HêõwÅo9¯KÀÊ†´/¬©$âÜŸ_QìÅÔ°>ı%{€vlúkÆô¯­¹ˆ®SPUÂèÂŸPL­|Ös=‰¤ä#T4õ¹Ñè±&yŠÍ¼Iï“ï1Zé1²YeRt!RÙZ^›N›ªÏøK¢¸‹Î	-©şÿe¯[†5£>E¸‚Ñï•BÅmÈ¬/ˆİª~×Ña¿±÷«DÌwö´åö˜eü§¤hÆB^‘D—y((C=ío^FF®¡C{éÎşxxÎ^»jı‘“É\Ç¢¤iãÌn©$’ÜF"î˜ ĞÀøÑHÕWÕûŞÑ­ŸlOK 0µiU“#œOÍ|ë‰Øí ã—Wİ¦=‡˜+/ç@C^¨îqÊ½ÎkEH¬Œ°ñÜ~¢ğxøy´P®RE„R¶.ĞGFˆü•,Îãoÿ‘²Ñ±ĞTÂM*ı2<Ác‚—'€/0"_Ñò1¯rÒ³_”İT;3
ˆŸˆtM=ÎI£m(v_L¸Ìw/àÓXf2f<K½÷U¹Hæ™‰Ş!@fºDepÎËw1Vuº(í )ßÉ%î3VRÛ%šÉén­&^ªå½Ê‡S Ç'ÔIĞxX¿©8bXäÿK²!Š''>|p2´ˆº{™Ôlû-ñ\ÎÁê?ãzbéğ¢”5[éä=a†å—£W¶Ók[Åàä%9`=Íæ2“Zˆ@×à¬tàÙjÅõ++È0G g°Òÿ˜™HkÓVĞ†cUcÂ%ŸSğõYÑå©[@O[d^şİ¯à1Z«ÉP;Yp‚dŠ“é´àW°vßè‘]APv„§|ôWDÒÖI¿oŠêİqk$jO÷¿ûçŸ‡Ğådöø¬ÄC""büzE"n°{•QoVWÒ¸#o·tã?]S!,½·ùÁ;@Ù&€˜‚ø»5¿}Æ]ï,@ªæ&°b¸…K‹>.Cõâk]˜Üb¼z&Ú!G8`Xìÿ6†üÙ†ÖÏ¦PcúÛ!ÍÅY}9K¥sÊù¢$%å˜›2ó~u*ˆ.ğêÄt`%f ï¥S6©yòÇR)Õ6b4¨¹Ï>ÀÉô0’IAÂ¶##Äˆ¶¯ş.úÃyÒb[ƒÉ‹ã§Q´rë3{|Öa2À¯† Ad»‰Õ!èoÖ¨mîÉÙş¿™±˜rkfŠ(²…Fr‡ ÉT¸_ÚQ„e£ L‘u†¬ß»ÿß[O3Õù§@%v`®ìhUGBÏpH¼…©+Ó¬QÃ\É¯|¤½-²Š‰„yº©vÖ†š½¦Ì6‹sçkÃçÈbÃŠÛ„WJ¡eç­yLƒŞÖWNî€êE%‡•bÀ}W–ÓD¼éV(Úl
S nxìŠº±M/÷ï&}¸wïyÚ¸—­ÂîXqKh¯ÿ,øTı½rƒÂeÜMôs_HUDßouÊĞáœóxû
]ìú	†G{—æ€ãøü.Ä¥Ò¹Î÷Âr»¢‘òøñÑå¿+2v+?ªl	ìÿGBÖ{üèVXªpÎFê€@Ÿé©°ñé…8Š'i!'LI6fî//•Ä¸ï|wø	J@¥ÌßŞŞMP<~2‚J¡~°;­ïÎñ•\X¦d…*ÅÃÕ^*"ÃF˜ï`,–xÑ“*ìë/Åä®ã3t´2ªulm1ª|çfµzş+ÆRè «õ&‚0ªöÔá±yµHàlªš’X¹V<Ãn§0tˆÇŸ2"bñ~ØI]p
§’8(?²Á_
H­èpbrğU
ª­©Çøü©r¼øû6BÏœ¤'¡~ÊşT2?+å„£Ï– ëÒP[gfÃ•sàX—¸¥óm‰±êAs9ae_2&õç÷†P.ì›ÒW9PmR+D¯ÊYk–Š¤awa6ÚÆÇêºÄ!Óß¤}À	{æ2]Ó¦ı¦Ã-0ÇÑ‹ Şç«/¹	Fı¯°ºáì#q€ùtwê@#Æ¨:ãfk_Aê([¿¾Š”5ÁË©ú[À˜C_ŸÌnÉ|»²k´cËÃò;¿r£Ò•à ‘ß= 72ÎeNûPæ•Ü?Ú¨Màç†•şÂƒék¤´ê~	zãj¼”¤>W´UˆLwg³nš » ª·Q(™Eš;s¤­ëÚÖHó=ĞéË¼ß¶I}“ oÙ`=ÛŸqş-eá”å¢KŒ…­‡œı “‚ÖŞğ|•§ª8¢ıT¡«]ÇÜ§ØBÁšÓOVPßíõ¸„)°(b(5ikÃ 
Ú¥·u’X¾¸ãÖS3õ*ÎæàÚ÷G"Ycşáo]£ŒT¥fƒhÁxÚy,n’.<Î7øXAFÑcf^ğìep¹ÏéZm+{¬Š·¸B@Õyr4P}5eüYTI4˜(2°T§ìğV¯
ËGXÇ¥‹€¢-Šò?·NÎ¼^Ô‡÷ı²/ éEôøjï…Ãa•À`Šo‡Aj	µ¼ÛqKéÙLö ±ÛĞs˜oİÊ³¹ÊDî:Ü~ ß³±Û62ğ¬ûØZĞºpñª"ëú0,–0ÊXæë~s#‚ò<rpá²,¦_`ƒ9išDòØy_("oªÛ¼óÃN.W	xsñ°Ò¢"}[Uæx¨ŠJ¤ğƒ[keªûPiw%Æ)~çaRr?¿ÂˆwkyOä¹‰»œÇãgÃRijå!ƒŠõ'~Ğ”)V$ô-(vÖ(Å
àÚ°Üìn>ßpğ1ŞZ¨)6W®(¼dM|‹K¸5>•œ±ç^˜Öµ§ØÛ¤ü6,d¨=ÚŞvb½~ÒäÙ¯:ÆÕd9oQ\ñ%Ö&ÎvÄ¾T31®ù¿$sTô@l!Ïò´$Íà'ªñû—€~J M©Q¥…ü)&ãäîğÃæ†olbIûî8ŒŸuî‹¨ºÃ±×ãW!z> |6_”en“QÓ–I<Ñ-¤‚3d¶·e Ëœã6ZS†¤Á‹¦Rt3ÃbÅÃûúz2ó¬ô­Jçâ¤‰ÃúÔofY>ÖMzAb}K$7ĞG¹øS'$®î•q@"ÖSM19udÜ%sV›ê,™¥Ägƒõ©T›üÅ9€¹ŠO¡Zwù9˜§öÈš9éÍûÙ‡ÆÜÿ‡Õ…<Ô%F¶¼/Ï(>)’@¿CD„Eg*ˆn«í‚p<ãhı®˜¤a+§.ğÉR…„Ñ~jßcfÙÀlş…®ì„»ôWğ&*®V¦|3},‡2NÃö©ğ#]w/hÌ_‚ÓG¡	>ĞX®{‡„Z¾¦–…¶é3*ã|DÏ€tvP’³p{Ïş¬òŠpà>9ÖÃiò=g¶–ë•Ğ©ŒõzS§‘xWœZyßzßPLKxnLZ@ vÍíÖÕÀİT–ÓÓğ)Í¤Ú)’áoÀÖl­‹­˜½ŸDhyg_OòsR0¸1\4"UöÒb‚¯0-‚ô"n
(Öï4¶İKúÛhôµ.¤ŒÊË(ì†Öqß+¢I&y›½ğC
_Kc•Ï¾ ÏTİ®½‚YœÉ3Bã…ÌZBL ÿy×Úç`Ì‡°ßÚC¶;OÅ'—ƒ;Q§W§‰ß<Æçƒ9JßËz¡Äjl™¦¶ª'3‡ 7ÃbUU³ßuÛA•c˜>	ıÆA8×.:$hÏĞvÑ2ÿğ©2„ÍŸ¤Ä¼ÛM{¿BÀ³]ğÿÊ{Êá6““ÄA’Ô«#ê(ñi'ª$í`…Ñ¦“ÌªßlVého©ìÄ -> @hlock and @hlock -> <whatever __bfs() found> is not -(*R)->
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