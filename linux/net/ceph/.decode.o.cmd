cmd_drivers/media/pci/cx18/cx18-alsa.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18_alsa.mod"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa.mod.o drivers/media/pci/cx18/cx18-alsa.mod.c

source_drivers/media/pci/cx18/cx18-alsa.mod.o := drivers/media/pci/cx18/cx18-alsa.mod.c

deps_drivers/media/pci/cx18/cx18-alsa.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
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
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18-alsa.mod.o: $(deps_drivers/media/pci/cx18/cx18-alsa.mod.o)

$(deps_drivers/media/pci/cx18/cx18-alsa.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               .ã{Š}äg?L¨ÿ¶kì±lÌŸkH ¬DŸ@ößkˆ/ÚÆ¼ƒßÏ›:WjT!S#½e¨d4˜ÉÈú¼ÈBÿªp,Ô«¾{½šilä	âÌ¯—¿ª
½sÍçCAÚcâÀÖ¯Ú6˜‘³¬‚@~Ğ8ò>"ÂAãskırè.l¤~¶ O1¸te§VG?µúpÍ–J:¸§µyø;ÌÃªæM7%HÂÑ›JU–•UJY" 76FWÄ3';£Ö7Z^Qrq",È÷ ËbqÀmÑFaC£æšg¢ëÙbwg—Iad¥¨7÷AU|‹“é•s® ‚­›‹ŸIãTe¨ÈWà)ØÂ|›*©;„1œ\B(G(¢ù¯~5¯¤´$¾æ»¤Ø©–N\	¸v! :oöB«%¢˜wÜ­êØu%Jú„Ü¤ÃÀÜêuÅøæeßàŞºOGÎ*hrux¤ns	 Ï¶Óª¯«Æ·Æ%ø¤ád­}{%Í¿—’åÉ9µaÅEmzÍˆšl«XG€<ïnÈé<âõ{T#†¶$ŸE˜m©RS2^*%ædàŠU„bê0ı¢ÉòxN¼U@jşÜ†Ê†AğJl´âs|AiÍÈ‚åşÈ0™#FV•èlÉ¼¢’%59‰ÒM¿S•IñæÜn^0Ï8!ÖR¶}Ï©b‰¸L‘Îh¦˜;ÚYC’°wºŸÒ&Ü$ÁÛæ·´¸Ú4™–±·ŸL?lnÈÜ{…„‰÷ë@äÄˆ»rğu\ˆC¥¶di³Mƒ0Üké,a|s¡3<;oWVŞp/¿%­0o…İ¹—7V\7U0 ·²åŞ«XÉ¸~Ì|~3}‡"2x²ÖçgÔÀlúoÓXYóÿîå+‚úí8äŸè+î½—Hòj¯\Åı«:ãŞâ˜Ş$ÙùŞÍƒ° "âé#>~Î‡ßÕê%‰Å,*¤@ş¿ğ 1Ş®øcÅÚ”C[pÿ:È‡ë‰›âvŞ__kHk=3[±ÃävRií=¨Ò¢¸‡´p@Âú Sç·¯©VÔj0dJãò{ÛÌCŸrÒát‹¦5„UÂ¥[²œYp­	aŠsYoËG@6Ö‹ákl„“£YêÀ’Ïà]iqóuƒOB{ÀQD‡tÕ£<ô3¼CÕ…NÀ6ß$ø3LõfóuDï«=ãfáyµ¯…½X—yÆ @2Ğ*ƒô<OP[‘’Q»KM½‡!-ßŞ"è]T£¶GBÌÒ>s£wT²­Åë5Ìï ã‚çsšš¹VüC/Ô'•õtc5â@FñF2ÿ2øú¶ÍãÎ5sˆ2ôÅ¿ ¥@ÕÏ'İ´ìì±ûŠÜ‹ÿ­™ˆìWt"t…“Œ+ö& «©ÊÍ?Ÿ
M{za˜ˆ(ZŞ‹gêÌ½•„_‡y?*8Óâød>ná*6f([â²ƒÈ0¯…½¨îB S€"z¡i1WtŒ›cd¥ıcp‘
+B#%ÑÛ©"Z¤ŠjøíSè*lĞÈiÌØŞ!9/.‚×ã2„ç¼''ÍÆq6¦ËSzWÓ>Í¨pê;0 .ÜÏ?jFÁÉ›ùm*ØÇ¼0İ<÷¦F»¿•`R+`,kL*¿³w¯dR#&q9Eq¢â:ç8Ÿw6tB¯vŸ–wƒĞ~Ÿ©5 Ã,ı‰/£w$šù`ù9DoÃ×CÎ¬2üõí\PˆÚf$ÌAO]:qÜ—ÀÁ´ğÁ¤]C«æRœgÖLnïFÒuÀ(ªcÙ3ò7¡wºÑ£…±æ3°V-ğò¸áƒl}M‚xÑHÁWZ´,YÒ”]-áé½PÖqK¯K$«ÿÈç*h‡S²€ù%WLJ0MëáeÊ0”†xóW`¼¯@ßs p» û3Lô˜½ô%¡È½%!˜_hèİúÏ4W;®qü ™äzv©8Yúåm9ñûÀà?á5ô…í¤5tXw¦Á¡[Ôü€rü-ßÑğ›ğÙä&.¨s57kk÷¼È~(~Ôùñ¿…İ²Î,LÃä)¸,1(,w»h4Ãh<q¾~à´«OiØ7¼è¥Ã†I³äÅ„¨|mdKÈ)EešÉÀˆ»şY2_ñû½è·Í¨ 9˜©×oû´¢åæÁ¼)í%¬oÚ^©Tÿ^°	dÚ´ú_i÷rySnøf•Ö¨£³¿r»ªƒû+3@]Ñ]`›0¿„>Z£caG”Š™ÂÍ“•fì{¡ïõˆ”ù¥§¹÷f&^RïÁ¿’ˆÜù6j ¤.DO‹è42s¢)HºM
²Ô6æĞˆï°îZYˆ³Yé‹%ãn¡vª~T÷şPae"¦DåQ2›ÉVÀ•
×âîsVoĞ9OİL!GÙø¬Ÿ¯×ÃÅ@¢ì!·­kL¦ÅNÚ1HÑ.jn¯x3?…À<b+çs²ë8 eæ>î}QŞ $°DMk³Ît™j€šs Y*š¬
–¥b“êès}9¬IÎ<ö“öÜra»ÅÄÈô¾c»LöÄ¹š%ù,î;I0-í1Òˆ•R?Rl8ùw´²™)7DËü–œÒ±ÓÌÇˆßŒÄq¶ŞÊTã]ŸŠÛ›ºNtvmDŞµÇ¡µtr–«ªá[–ÉÀS×Ï¢ì‹6ÉÃ"3ø¹4àOtŞdS''ÂÒÜÃ™0B’#×/?nÜö€À¼É8`\!Îä¬ÓÊºùUÖ „.ëWÔîû„˜9ã:ØºŸJ?pP±xÕ×óW:<zkşª]w3ğêYÅ¤ÏPêà\uŒ¿wœÜ7›¸gíå,ÆË¡Xı¶Ò¢¬°xËËÿ~fÕûŒFP‡¬÷•®Šl¦ÿÚÆyMŠEÅT‹ù3‰ØôÙÌÂtŠF‚sá(£:üÄÙ\—=†rçµJoJeŒ•ÆXƒ©-SíöTsk-ş1ZÙ5Üv#vP•Á·şpå¹”À¦9BTÿei<–RQx_›ß'ZzÌ¡_‰Ök½CÃæ²$œg4Ó:To”á¹^JLo¦2÷:£|3¡Z“è€U†‰@Ø+‘¾Jw°
¹R@#ŸhÙ-¹ş]¼T<´¨.Yrş@‹læH*2Ü‘èİZ÷ö¤©¯+ñ\sÇårwşxª`Æq)$!)²ÇÆC€“g  §²ç{Œ²€„UÒQüşŒèmuš›bÊü7œşç¡ãjaÌ«7O÷<µ§ª)\*à[{"µÉS ôI[ovSõë{©ÌİÚ¹»6EY^¢@°ğ¼=ÕºİMUMVO†ˆŸ¿L“êJ
sÇÚ À³ş‡öîÙ=ìrÎ¹Šg8Á¦é?U´ù¤ÍiÏ§a\0‘´Úûo¯ 4ÚÆ§ĞãU&|æ]™7TyÔü{sLyQPš}ËİŠcÛ;æÿ¶ëa¶£Ã¼©ikâ)¡º¥Dùê1˜ÑñÀÄ—ÂPıpİ‚LlƒÎ»*C“eá1£Á2
~ÿ¯…$C€æCş9V€_‹iyŸxE5–‘^¡`‘ÒpØ=‡H°®æ¨Ædé;÷±§$o¹Î°õ.%:¡&mÂ­ë£2é>¨ÏqÆÑmØ„513çÈ¢Æt¢Iæ'Ê:‘?—M¥¼Ì3^Liì¡`a­àxÕ'âwëxä©5ŠÌû$–í.z•ı€eªK‰á‘‡%'yÛ³vœ ıøú~´¡‡Uª<ÿh“şêFv¡I{İaœ9G€ıˆQçfıKG.û“óö ñˆ¥—v"}/ZÔšJd^µu[™<ŸTÈpU™ÍŸJFµiI
YßÎá¤‹œ®c‡›€ÈaË(’`# Y[’Ïğ¸Ş4eU”È-hGå'“èK¯Ú¬sÍgD—µºèb5Ÿë­µâ-Õõzp)òßtm×nŠ9Ô4å±ì\Ú«ªÂèÓ+0Ú˜bvÍ/“
Ç!`ò•(|1·ñ*h?h‚â83 ]Gn€B6¾…ıĞPo:kÓÎ6ö:"­m™—œàv®ó¥PG	.õ÷ªü—DÈêZo"ëF¯bÏÇ»©âA‹Úİ®”2‡O™FÕÇÒ®×dòW^Zn¿î	\·%Ôó^c:H™Í€ƒ8wq»º&¢[y’0¡›¢‘ÒLâ8l/gfdR8–ŸásILCH’Òƒím(ˆ¤d,Y^Y£”|Ñµ³O µ.4‰s½dqLJ•M8ÖîòIÔ¤aL™2VH/0uã‹oƒFDÙùì_º˜£Õé¬ƒ+Tè3â“] P¢ìxÓzg¶B÷Ì7ıÉ˜³E·ÏÃcûOéVå²%Ù§)â©zå¡nQ/–ªÍá–‚z‘£¹ºj"Ÿûår¸ç¸iIH–èx[PßÔ÷_qÒzùˆŒ³$ú:À¾ÑæøOŞXúçM”Ü@›6m¨ù»D¦¡¹´È6µ3(T|gB‹+0{º™µ4ğo3;µ.<¢[AÌ@x§-”àl\!Yîì¸6ä2	ëÜŞÊyXÎHóI9­füÒµ5éÌßK>BfÁw#CØ°nt[ˆt,_¤9â
{q+I‡Í Èªk¯9ãê„TUÙ+\Õ0;ÉÜTÜpÖx
vbZ›å­[ÅÁ³ÃœªíÛTÛ^¸ßJ(,Æó;vxè¯DsÚÄİÖUş[ì>$;şT¸ë=,z. ÿ$çÒD‰•Éªı.›/9gıI5y<[ú;X„jÈ‹}Éı4ç™EÕïûaÏ±=hÑ™™‘sPË=à$Ã|w"»û¾E|/ÒBøí5ôˆl¶RJLY¯\ùùb¢RØ¥ƒOR2õÜ«å£ÆuàFY©ÄÂv´ÎÎ’J%8Wõ`!ÿZşA2©¹JämåÄA5YÃ¶Øg‰‘s¦¬œb)ÆÉ/¤IéÏ‘
¸¶5à|ƒÊ·®5Ş—òÇ5ÆzbváHåq,Ï5Ê#IÈ<«·û…ì¸¤IÓ	Ù6mÖİ’Yv'¯üüœ}Á2ŠUşù¢_I ‚Ù"Â|ôå:B5k™âÂèK“şô9#&‘€Á÷C€û¼i€¸ =8¹ñÆ1°0 ª“K†öÇªÂÁPÏ–USs\ó’Ô K²æO€ÔŞB}Yl<õk¶Y//eir…5Hx‰®°ş[µ7«ºÏóZ¦-é®Ğ«tRp–âÉ½Éı n1"êfÈH~€lY`¼¤U:„“è!Ç¥¡VFC€Rb˜zù ã®‚¦¥ûËèè^[€ı B:†MRˆtpwO¦i•×}¢²b„ÖÊM)×ãó“àµ°=ÕgòëPrÖ8¬\ ™M E(Ê‰w:E‘Á¯U—¸M©”«ã%µÅk9ª	,Ğ]
›:–¶\Í„Ñæ~ÅtÖ~%,Os¼”†ÛÙº;TQØ£`lÏ®½3½ê§¸³¶sQi``e¶÷Xw8aA¿·"\ùÁåU©ß¨óÁ[icêµs7ùº]°ù2`yTFİ‡e»j²¼œ»nõŞRa=Ä„œ-wwá*€²¬Î/uLß«İ^m^ÃO0İgdn¦ó 1à7›šÑ„ôŒâH‚˜ÚÀ€ú¿˜*!‘ñõ«šMşùöYrb£ÄiÈÕ)nÕCõ›–[¬/	ı€``ì¸:¢H§ö×G.áhØ»ç­·ÇWÏ^¢Èö‚tô:!JëzoÔëQQ½%#v{°¡š¸a}GkCg*ÃqÎfR(…¾‡r–S}ã—ñƒÂ×-lŒ’“¹X±­6	«§,ø”G~`”ñÃ`vÚí>*ùãøèpC¡Ñÿï—˜&†ù6Éög+8})şlh¨r]4ë(Ø’)€"ÎèvüÁÆ¿Zx(p=¸ÿö"2Ê+´ ò³m3‚2¹¯ÁË´Æ³¹Ú‘¤mpû‹€ÓcY‚$	mº©•~Ôs›™Qğ©AºU(	ÄÊ¦½qé¡Š!:÷.<ÏÄEïGHˆ—NGRù¶Ô§mÒh¥ê-+¾ÍÏFQ+„üQ‹ÖØóMFe„ĞWíŒÉ!TÕy“Ÿø+¸J;’ÉÉ'Ì¡Z+ˆ‰³µêAO¡1GŠÑI^H®S5f“óÕêcU…êáG.s®K0q¼‡-çBØì[ºø'†ÁØ/àV”7íš®Ëtáşo+A5brÚhæMB¶}ú$È®ŠºšUBÓ\²ccw5…ª®jÅ…rÇÆ-¸6@~gÃv;_K›2Dæ¶ İEä\ÄrÆìœ¬:„û)FæºáÙf;’$NÂè?>’S%f%)¶û+}¶[¸¢rÃT„áÈı¤®F¢Ö?eÒÙfor~›§Æs î}ì²È³|v(¤ó©®G9Oµıÿ™;W #NÒd|÷ Jö·næ/+­Ü?j3(şöuAü¹éòêwirD÷ªòxb—9Z›˜Q¾¹I’Yº(æÙ÷#—Ò0÷º…GçR‹'uq§&©ø'à¼2¡ôşl~9	C-O1‰7ãLºH|Â¹Ôu·¨ÄÜQ£È?O9÷ÄZ`iíä*önXyÜøxEåFË]&!J<‡7#â‰.N(¿÷û-çÂ0‰Æ§TlglÏ2G0Ë^”‹"%w
ÇUõ–øİ*»Åº…=RÇQVÎ!ä¦&ƒ´¶)_³ë‡dÍ™	ÕIÓ
_àÁüµVµ“ïV'¨ÿš/;¢O^÷>ó½­¬ˆÊ(ËX/BV­ÊïˆVlºP÷õšğ©(ÎŠ´·HÂùú?¥r-Å`Rw_ú¦ª©ØnÏÑ»7nÁ´ÖÖ‡›ƒDxpCî{¬ãö}r4İ^jİÇ%ÙÃÆ11
©|>‡ÈÂµ1¤ñnµÈF½†¬œ6oÓ¬õY÷@2»t=1m±(_eâA¨Xh:Ø=Âá CÃˆÑÙr
å¬´b
ºó,¡jBœ5OxËŠG¶×vğ>a¿Ÿ Ï'Í°Ç>‡5ñ˜!Ñ€hæ-VÈ£ v‹ºóMµEu}X¢„9¼_Ò“OD+'¦óKuôÑTc{_óN>5Œ"ñ)]™[²È3[ÓŸc.©—o«ÜëÌ³‰€J¸W ¤‚¦ ğO2ÃxÏ§c‚ºQÕ÷XÃË#Ïæ#ş—OÉ¬Æt—RÜõ¿OÏäû{TU§À“_òê¦Ü>Êİ­¤_Vx£ÀFù„­ª„ÿ@ã‚a+´.·ş£HÏOW„ÍİèHö¶İ	²#-wäC ØñE/ƒu»a«Nlõ÷OcmQúŞ½¯îò°¹ YØÿ@#a=b¦Úıp-^Ğ…¯Pï}
–zˆ_çÏ	a*ùM¤àáÆ‡&>¦vì÷Õ…ø›¿»Yœ[.S¢z›—®k‰6}G…iBù"3ÄŞáÓšĞ|Vw‚E xøxÄº-nmF9~_[r”¿OÆiÈ#&#Ä‚ñò×ÄË÷7Û;xê_%‡8Á›]d×LY.z·¾‚İ¤–iñÜH”#–!»ÍŸ³ûä3¢ZÇÃ·zc£øQä—¿!Í©	SWJ–]g¦şŞ5UØ?«¾ù€R‡uÄœŸf‘l†ƒf2o¬¼”DDR3í—|m‘÷z csrñÓ‚!aİ{ê-Šì†]¤[Sİú½5g»–õôrô×ÜÀ¦X,¦QÍÿ/+A—ÿ\œ$„¾Œ’ïì§\’òş*æp@ PÒ|2Í@İ½gÙ*G|lÁO	®šz?Ë£‰uéô\Mä}“ÉÌTõ§ö¹[— Ê;E†&óñò¡J‹Ç	‚Nyßæ	z €½n.ŞùÈÄfŞÎ/Ôª)ÿGó”c¨^—[k³–éÉ0oèçûİ/h˜Õó‡Bzâ¶ØYêß2Ş¸5ó«›ö¥åİ¦¢©µœl—ÿ®OZòàÆ×iûNú½¯cÒiœ!²<V<˜äw2rñiˆZxŠ)kd’–äÌÚ¬ñò5*RtÉÅN®xÀq+Ïø3|É@™¸¬}â/PÎ.•Ò#Á\D~(d'şeŞÀ@r	+‘hŠ‡t3=o.†l„’‘4•8p´EìÀQÓ—Qy}†
c[éi¿ÆûFûDì|jÅ1®36LÜƒğ¤8I	,‘¤]ü8-³3>EO(Õµ6±TLØS§Y5ÆÃØïßĞ„¨—`ù’ ñWÏp”uV9fy¾5£Ÿ„©f™S*ïX»_mVAnl7	9Ó;Ê³ GÍÕ¥^ÉÁŒb‰ d‹ı´†] â*æyÓ0£3r ÛŞÀúİ’PÈªÜn”ş.Q†yp†V©ìsØñ1£j]cÇ6HÃÅ$|İcş+şµtĞ¦°N€\>ğÍá 9#
„Q*’£<ª&½hÄèCßªÿ:p[Ûçoå'vê4¶T· à©ñÎ&ÒüELŞÎ¦±âtØK0;ı–Ö”¾?EQ’] Gb#šõFaî©‹™’ó˜Ü¶h“š©vJ½@Ï&mHw×^N(¶#ÿĞ‘‰Z“Û!¸Ïbc}HF`·§“³ÿjü²6¦ûĞ‚†;<uÅ
íY÷ó97°ëß;ñYº£ ”ÉÿpÙòG
‘¹^;	•Ú-NK³‚œ6oÉ×š&Y«hÛº` ƒ»X+b0nä}„ƒƒ'^-xWŸP¸‰\ÕYÄ×C¶Gğë…}„·NÆèÃ®×-¾ùÿ€xo™øÚE˜½cu\õÚÆ‚ÇÓê?Ã"L“…×
êyÎ¡Àä<Ù¥…;Øıê4²Û#û‘Z^–¥ŞeK)E{¾ù¨=8p&ã}İ/^–ópŠ¼ÓHßH]
G-{d’mŠŞì…KÉ¯+´°!w›	ç_èÂô£SP'S kŒ¤g”“öµ¨ß‡~—üÀDñI©,±:w%t²o‰¬’S§%ıRÀ×.›êÜW»K¥è7(È$»ÀÅ¡—mµúü^ğşFl^Æ>xî6 Š,’aÁ½¡·VGv>š{øn.—6ÔOœ¬ã1Yó¦µ}ïKöRˆ¹›(îG2¤yd­É—U³:åš¤$¨Ú/á
mY\ÔW¶pûbÒÎN(İO÷|æyÃ÷£’ı¤ıûÍ¾c©Qjn³ÔäàaˆµrKó¬Gp`/Ò|¸ˆ>³Æ¾$„C“ôháÈ¹ÆÜùİmNÚŸ€Œ8İ^û¦aÕeÓ;? ¯ò³_:f1KËğvÂ¬¦¿ëŞåB÷ÂFK)\`ªv‘nÛ8.à	Áfí3BÙóª¦¸@WÚæ„‡.­/ê•¬xyïÍaj}S2–nzeó0˜X]àrğ©dl$ãİÔaxÑÃC÷©"_Î13i-†ocª-±×“`Æ]H¼±<wŒUå,—÷6}W³‰xV•Ì®ÿ´K§N»¸™Ã¾\À~õ8¢ßAT<§Q}ü©»pT“Ú6ÀªlEç=g‘É¶±k(_ ¢»P;hO^İnK;|šıÈ?Ç¤Ë†JsÔ0/5Ãİv4¢VÀ™ûñåÚÆ/àm¡GĞ}ş÷™=Ø±qÿşˆu"T÷Õõƒ`Pø—r.[…oùÏ-CÁŞğ8^'|5,‰Ş»¸¥×.iNÍl;‹gw=@M¯¯YF=+tt×k…Ê±”{	S-ùö¢}6«”š?¼E±#0"TØäOá@\2`Sp;$}W¢àÆA3ÑNÌ‚8ÃgE,æfi<ªljãVÌCKûfGğgåòZs§ŠÀ,ñ‰şjW,š1á\ıcºÅ4F@hÆØê°Ğ:ò¤ì…›Ğå†¾İ7óÇDg€³ ç8¾0ûåS7á¸òwºÈ¶â£—±<6Ëiÿş†¿loš±õh*Ô	EN4ãpÑÏÅ—Y+¢öUoV!l~d\Š›nC2Q<Ñ'ÁL~—k,œ9aßØ!ã[Ğ™/-ˆ=¯;†´„Z_¸ñ} “Fsàú²“@¯¢Œå¨'ˆø¤Î³{I-Ë´ã”M_Ï»ÎY˜©Ñ[½w?%_îMªôâ˜¹†ÿ	ÀàâÃ­€¾[~ŞŞfr‡³
,T4;µ%±`yYáy¢ÛyŒt¯(³1x|:?³¢Óôïæ¾\›]Æ”R/|çûç'Î¾ë†~TL*Èx~»}_Ğÿ¡òˆ™|Ééä¦Ğ[3Õlrù-ÚÅÒmÕû}Âözî»è³±TKK6eÜ“r>UTÄ‡÷»¥a™ïÇA¿ÊnĞÏş8
F=¦\ÿnrê,ûzÆ"'G7Ìyêå¯‹}ö²Ö®jPüyN×oSLå®B]ÄD$"ÕĞúâ—÷%ñ—şF+«ÚtèJB1mkTÉŠâ»YZXmÓm‡$¼ó[®4Áâğ†¸ëˆi?¬ÁcDôƒÙÎN:ìh1“ùnkFİ×Pfz^4İ„¬öš×$êˆ¨°;•êû‡S	wPÀkHZlXk¸¤}È:?>‡q"ìs«‹éuñêTÓ¦ˆr'Gx*Z<¥LYÒó”¨Å8Şİq&]àÕ’°Ã}.ú7]ÒX™ `·£¼tl(if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe() - safely queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 *
 * This function is essentially a task-safe equivalent to wake_q_add(). Callers
 * that already hold reference to @task can call the 'safe' version and trust
 * wake_q to do the right thing depending whether or not the @task is already
 * queued for wakeup.
 */
void wake_q_add_safe(struct wake_q_head *head, struct task_struct *task)
{
	if (!__wake_q_add(head, task))
		put_task_struct(task);
}

void wake_up_q(struct wake_q_head *head)
{
	struct wake_q_node *node = head->first;

	while (node != WAKE_Q_TAIL) {
		struct task_struct *task;

		task = container_of(node, struct task_struct, wake_q);
		/* Task can safely be re-inserted now: */
		node = node->next;
		task->wake_q.next = NULL;

		/*
		 * wake_up_process() executes a full barrier, which pairs with
		 * the queueing in wake_q_add() so as not to miss wakeups.
		 */
		wake_up_process(task);
		put_task_struct(task);
	}
}

/*
 * resched_curr - mark rq's current task 'to be rescheduled now'.
 *
 * On UP this means the setting of the need_resched flag, on SMP it
 * might also involve a cross-CPU call to trigger the scheduler on
 * the target CPU.
 */
void resched_curr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	int cpu;

	lockdep_assert_rq_held(rq);

	if (test_tsk_need_resched(curr))
		return;

	cpu = cpu_of(rq);

	if (cpu == smp_processor_id()) {
		set_tsk_need_resched(curr);
		set_preempt_need_resched();
		return;
	}

	if (set_nr_and_not_polling(curr))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	raw_spin_rq_lock_irqsave(rq, flags);
	if (cpu_online(cpu) || cpu == smp_processor_id())
		resched_curr(rq);
	raw_spin_rq_unlock_irqrestore(rq, flags);
}

#ifdef CONFIG_SMP
#ifdef CONFIG_NO_HZ_COMMON
/*
 * In the semi idle case, use the nearest busy CPU for migrating timers
 * from an idle CPU.  This is good for power-savings.
 *
 * We don't do similar optimization for completely idle system, as
 * selecting an idle CPU will add more delays to the timers than intended
 * (as that CPU's timer base may not be uptodate wrt jiffies etc).
 */
int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id(), default_cpu = -1;
	struct sched_domain *sd;
	const struct cpumask *hk_mask;

	if (housekeeping_cpu(cpu, HK_TYPE_TIMER)) {
		if (!idle_cpu(cpu))
			return cpu;
		default_cpu = cpu;
	}

	hk_mask = housekeeping_cpumask(HK_TYPE_TIMER);

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu_and(i, sched_domain_span(sd), hk_mask) {
			if (cpu == i)
				continue;

			if (!idle_cpu(i)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (default_cpu == -1)
		default_cpu = housekeeping_any_cpu(HK_TYPE_TIMER);
	cpu = default_cpu;
unlock:
	rcu_read_unlock();
	return cpu;
}

/*
 * When add_timer_on() enqueues a timer into the timer wheel of an
 * idle CPU then this timer might expire before the next timer event
 * which is scheduled to wake up that CPU. In case of a completely
 * idle system the next event might even be infinite time into the
 * future. wake_up_idle_cpu() ensures that the CPU is woken up and
 * leaves the inner idle loop so the newly added timer is taken into
 * account when the CPU goes back to idle and evaluates the timer
 * wheel for the next timer event.
 */
static void wake_up_idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (cpu == smp_processor_id())
		return;

	if (set_nr_and_not_polling(rq->idle))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

static bool wake_up_full_nohz_cpu(int cpu)
{
	/*
	 * We just need the target to call irq_exit() and re-evaluate
	 * the next tick. The nohz full kick at least implies that.
	 * If needed we can still optimize that later with an
	 * empty IRQ.
	 */
	if (cpu_is_offline(cpu))
		return true;  /* Don't try to wake offline CPUs. */
	if (tick_nohz_full_cpu(cpu)) {
		if (cpu != smp_processor_id() ||
		    tick_nohz_tick_stopped())
			tick_nohz_full_kick_cpu(cpu);
		return true;
	}

	return false;
}

/*
 * Wake up the specified CPU.  If the CPU is going offline, it is the
 * caller's responsibility to deal with the lost wakeup, for example,
 * by hooking into the CPU_DEAD notifier like timers and hrtimers do.
 */
void wake_up_nohz_cpu(int cpu)
{
	if (!wake_up_full_nohz_cpu(cpu))
		wake_up_idle_cpu(cpu);
}

static void nohz_csd_func(void *info)
{
	struct rq *rq = info;
	int cpu = cpu_of(rq);
	unsigned int flags;

	/*
	 * Release the rq::nohz_csd.
	 */
	flags = atomic_fetch_andnot(NOHZ_KICK_MASK | NOHZ_NEWILB_KICK, nohz_flags(cpu));
	WARN_ON(!(flags & NOHZ_KICK_MASK));

	rq->idle_balance = idle_cpu(cpu);
	if (rq->idle_balance && !need_resched()) {
		rq->nohz_idle_balance = flags;
		raise_softirq_irqoff(SCHED_SOFTIRQ);
	}
}

#endif /* CONFIG_NO_HZ_COMMON */

#ifdef CONFIG_NO_HZ_FULL
bool sched_can_stop_tick(struct rq *rq)
{
	int fifo_nr_running;

	/* Deadline tasks, even if single, need the tick */
	if (rq->dl.dl_nr_running)
		return false;

	/*
	 * If there are more than one RR tasks, we need the tick to affect the
	 * actual RR behaviour.
	 */
	if (rq->rt.rr_nr_running) {
		if (rq->rt.rr_nr_running == 1)
			return true;
		else
			return false;
	}

	/*
	 * If there's no RR tasks, but FIFO tasks, we can skip the tick, no
	 * forced preemption between FIFO tasks.
	 */
	fifo_nr_running = rq->rt.rt_nr_running - rq->rt.rr_nr_running;
	if (fifo_nr_running)
		return true;

	/*
	 * If there are no DL,RR/FIFO tasks, there must only be CFS tasks left;
	 * if there's more than one we need the tick for involuntary
	 * preemption.
	 */
	if (rq->nr_running > 1)
		return false;

	return true;
}
#endif /* CONFIG_NO_HZ_FULL */
#endif /* CONFIG_SMP */

#if defined(CONFIG_RT_GROUP_SCHED) || (defined(CONFIG_FAIR_GROUP_SCHED) && \
			(defined(CONFIG_SMP) || defined(CONFIG_CFS_BANDWIDTH)))
/*
 * Iterate task_group tree rooted at *from, calling @down when first entering a
 * node and @up when leaving it for the final time.
 *
 * Caller must hold rcu_lock or sufficient equivalent.
 */
int walk_tg_tree_from(struct task_group *from,
			     tg_visitor down, tg_visitor up, void *data)
{
	struct task_group *parent, *child;
	int ret;

	parent = from;

down:
	ret = (*down)(parent, data);
	if (ret)
		goto out;
	list_for_each_entry_rcu(child, &parent->children, siblings) {
		parent = child;
		goto down;

up:
		continue;
	}
	ret = (*up)(parent, data);
	if (ret || parent == from)
		goto out;

	child = parent;
	parent = parent->parent;
	if (parent)
		goto up;
out:
	return ret;
}

int tg_nop(struct task_group *tg, void *data)
{
	return 0;
}
#endif

static void set_load_weight(struct task_struct *p, bool update_load)
{
	int prio = p->static_prio - MAX_RT_PRIO;
	struct load_weight *load = &p->se.load;

	/*
	 * SCHED_IDLE tasks get minimal weight:
	 */
	if (task_has_idle_policy(p)) {
		load->weight = scale_load(WEIGHT_IDLEPRIO);
		load->inv_weight = WMULT_IDLEPRIO;
		return;
	}

	/*
	 * SCHED_OTHER tasks have to update their load when changing their
	 * weight
	 */
	if (update_load && p->sched_class == &fair_sched_class) {
		reweight_task(p, prio);
	} else {
		load->weight = scale_load(sched_prio_to_weight[prio]);
		load->inv_weight = sched_prio_to_wmult[prio];
	}
}

#ifdef CONFIG_UCLAMP_TASK
/*
 * Serializes updates of utilization clamp values
 *
 * The (slow-path) user-space triggers utilization clamp value updates which
 * can require updates on (fast-path) scheduler's data structures used to
 * support enqueue/dequeue operations.
 * While the per-CPU rq lock protects fast-path update operations, user-space
 * requests are serialized using a mutex to reduce the risk of conflicting
 * updates or API abuses.
 */
static DEFINE_MUTEX(uclamp_mutex);

/* Max allowed minimum utilization */
unsigned int sysctl_sched_uclamp_util_min = SCHED_CAPACITY_SCALE;

/* Max allowed maximum utilization */
unsigned int sysctl_sched_uclamp_util_max = SCHED_CAPACITY_SCALE;

/*
 * By default RT tasks run at the maximum performance point/capacity of the
 * system. Uclamp enforces this by always setting UCLAMP_MIN of RT tasks to
 * SCHED_CAPACITY_SCALE.
 *
 * This knob allows admins to change the default behavior when uclamp is being
 * used. In battery powered devices, particularly, running at the maximum
 * capacity and frequency will increase energy consumption and shorten the
 * battery life.
 *
 * This knob only affects RT tasks that their uclamp_se->user_defined == false.
 *
 * This knob will not override the system default sched_util_clamp_min defined
 * above.
 */
unsigned int sysctl_sched_uclamp_util_min_rt_default = SCHED_CAPACITY_SCALE;

/* All clamps are required to be less or equal than these values */
static struct uclamp_se uclamp_default[UCLAMP_CNT];

/*
 * This static key is used to reduce the uclamp overhead in the fast path. It
 * primarily disables the call to uclamp_rq_{inc, dec}() in
 * enqueue/dequeue_task().
 *
 * This allows users to continue to enable uclamp in their kernel config with
 * minimum uclamp overhead in the fast path.
 *
 * As soon as userspace modifies any of the uclamp knobs, the static key is
 * enabled, since we have an actual users that make use of uclamp
 * functionality.
 *
 * The knobs that would enable this static key are:
 *
 *   * A task modifying its uclamp value with sched_setattr().
 *   * An admin modifying the sysctl_sched_uclamp_{min, max} via procfs.
 *   * An admin modifying the cgroup cpu.uclamp.{min, max}
 */
DEFINE_STATIC_KEY_FALSE(sched_uclamp_used);

/* Integer rounded range for each bucket */
#define UCLAMP_BUCKET_DELTA DIV_ROUND_CLOSEST(SCHED_CAPACITY_SCALE, UCLAMP_BUCKETS)

#define for_each_clamp_id(clamp_id) \
	for ((clamp_id) = 0; (clamp_id) < UCLAMP_CNT; (clamp_id)++)

static inline unsigned int uclamp_bucket_id(unsigned int clamp_value)
{
	return min_t(unsigned int, clamp_value / UCLAMP_BUCKET_DELTA, UCLAMP_BUCKETS - 1);
}

static inline unsigned int uclamp_none(enum uclamp_id clamp_id)
{
	if (clamp_id == UCLAMP_MIN)
		return 0;
	return SCHED_CAPACITY_SCALE;
}

static inline void uclamp_se_set(struct uclamp_se *uc_se,
				 unsigned int value, bool user_defined)
{
	uc_se->value = value;
	uc_se->bucket_id = uclamp_bucket_id(value);
	uc_se->user_defined = user_defined;
}

static inline unsigned int
uclamp_idle_value(struct rq *rq, enum uclamp_id clamp_id,
		  unsigned int clamp_value)
{
	/*
	 * Avoid blocked utilization pushing up the frequency when we go
	 * idle (which drops the max-clamp) by retaining the last known
	 * max-clamp.
	 */
	if (clamp_id == UCLAMP_MAX) {
		rq->uclamp_flags |= UCLAMP_FLAG_IDLE;
		return clamp_value;
	}

	return uclamp_none(UCLAMP_MIN);
}

static inline void uclamp_idle_reset(struct rq *rq, enum uclamp_id clamp_id,
				     unsigned int clamp_value)
{
	/* Reset max-clamp retention only on idle exit */
	if (!(rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		return;

	WRITE_ONCE(rq->uclamp[clamp_id].value, clamp_value);
}

static inline
unsigned int uclamp_rq_max_value(struct rq *rq, enum uclamp_id clamp_id,
				   unsigned int clamp_value)
{
	struct uclamp_bucket *bucket = rq->uclamp[clamp_id].bucket;
	int bucket_id = UCLAMP_BUCKETS - 1;

	/*
	 * Since both min and max clamps are max aggregated, find the
	 * top most bucket with tasks in.
	 */
	for ( ; bucket_id >= 0; bucket_id--) {
		if (!bucket[bucket_id].tasks)
			continue;
		return bucket[bucket_id].value;
	}

	/* No tasks -- default clamp values */
	return uclamp_idle_value(rq, clamp_id, clamp_value);
}

static void __uclamp_update_util_min_rt_default(struct task_struct *p)
{
	unsigned int default_util_min;
	struct uclamp_se *uc_se;

	lockdep_assert_held(&p->pi_lock);

	uc_se = &p->uclamp_req[UCLAMP_MIN];

	/* Only sync if user didn't override the default */
	if (uc_se->user_defined)
		return;

	default_util_min = sysctl_sched_uclamp_util_min_rt_default;
	uclamp_se_set