cmd_drivers/media/i2c/upd64083.o := gcc -Wp,-MMD,drivers/media/i2c/.upd64083.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"upd64083"' -DKBUILD_MODNAME='"upd64083"' -D__KBUILD_MODNAME=kmod_upd64083 -c -o drivers/media/i2c/upd64083.o drivers/media/i2c/upd64083.c 

source_drivers/media/i2c/upd64083.o := drivers/media/i2c/upd64083.c

deps_drivers/media/i2c/upd64083.o := \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
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
	 * unsafe lock is taken under the safe lock.
	 *
	 * But if there is a chain instead, where the safe lock takes
	 * an intermediate lock (middle_class) where this lock is
	 * not the same as the safe lock, then the lock chain is
	 * used to describe the problem. Otherwise we would need
	 * to show a different CPU case for each link in the chain
	 * from the safe_class lock to the unsafe_class lock.
	 */
	if (middle_class != unsafe_class) {
		printk("Chain exists of:\n  ");
		__print_lock_name(safe_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(middle_class);
		printk(KERN_CONT " --> ");
		__print_lock_name(unsafe_class);
		printk(KERN_CONT "\n\n");
	}

	printk(" Possible interrupt unsafe locking scenario:\n\n");
	printk("       CPU0                    CPU1\n");
	printk("       ----                    ----\n");
	printk("  lock(");
	__print_lock_name(unsafe_class);
	printk(KERN_CONT ");\n");
	printk("                               local_irq_disable();\n");
	printk("                               lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("                               lock(");
	__print_lock_name(middle_class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(safe_class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_bad_irq_dependency(struct task_struct *curr,
			 struct lock_list *prev_root,
			 struct lock_list *next_root,
			 struct lock_list *backwards_entry,
			 struct lock_list *forwards_entry,
			 struct held_lock *prev,
			 struct held_lock *next,
			 enum lock_usage_bit bit1,
			 enum lock_usage_bit bit2,
			 const char *irqclass)
{
	if (!debug_locks_off_graph_unlock() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================================\n");
	pr_warn("WARNING: %s-safe -> %s-unsafe lock order detected\n",
		irqclass, irqclass);
	print_kernel_ident();
	pr_warn("-----------------------------------------------------\n");
	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] is trying to acquire:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		curr->softirq_context, softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		curr->softirqs_enabled);
	print_lock(next);

	pr_warn("\nand this task is already holding:\n");
	print_lock(prev);
	pr_warn("which would create a new lock dependency:\n");
	print_lock_name(hlock_class(prev));
	pr_cont(" ->");
	print_lock_name(hlock_class(next));
	pr_cont("\n");

	pr_warn("\nbut this new dependency connects a %s-irq-safe lock:\n",
		irqclass);
	print_lock_name(backwards_entry->class);
	pr_warn("\n... which became %s-irq-safe at:\n", irqclass);

	print_lock_trace(backwards_entry->class->usage_traces[bit1], 1);

	pr_warn("\nto a %s-irq-unsafe lock:\n", irqclass);
	print_lock_name(forwards_entry->class);
	pr_warn("\n... which became %s-irq-unsafe at:\n", irqclass);
	pr_warn("...");

	print_lock_trace(forwards_entry->class->usage_traces[bit2], 1);

	pr_warn("\nother info that might help us debug this:\n\n");
	print_irq_lock_scenario(backwards_entry, forwards_entry,
				hlock_class(prev), hlock_class(next));

	lockdep_print_held_locks(curr);

	pr_warn("\nthe dependencies between %s-irq-safe lock and the holding lock:\n", irqclass);
	print_shortest_lock_dependencies_backwards(backwards_entry, prev_root);

	pr_warn("\nthe dependencies between the lock to be acquired");
	pr_warn(" and %s-irq-unsafe lock:\n", irqclass);
	next_root->trace = save_trace();
	if (!next_root->trace)
		return;
	print_shortest_lock_dependencies(forwards_entry, next_root);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static const char *state_names[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE),
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static const char *state_rnames[] = {
#define LOCKDEP_STATE(__STATE) \
	__stringify(__STATE)"-READ",
#include "lockdep_states.h"
#undef LOCKDEP_STATE
};

static inline const char *state_name(enum lock_usage_bit bit)
{
	if (bit & LOCK_USAGE_READ_MASK)
		return state_rnames[bit >> LOCK_USAGE_DIR_MASK];
	else
		return state_names[bit >> LOCK_USAGE_DIR_MASK];
}

/*
 * The bit number is encoded like:
 *
 *  bit0: 0 exclusive, 1 read lock
 *  bit1: 0 used in irq, 1 irq enabled
 *  bit2-n: state
 */
static int exclusive_bit(int new_bit)
{
	int state = new_bit & LOCK_USAGE_STATE_MASK;
	int dir = new_bit & LOCK_USAGE_DIR_MASK;

	/*
	 * keep state, bit flip the direction and strip read.
	 */
	return state | (dir ^ LOCK_USAGE_DIR_MASK);
}

/*
 * Observe that when given a bitmask where each bitnr is encoded as above, a
 * right shift of the mask transforms the individual bitnrs as -1 and
 * conversely, a left shift transforms into +1 for the individual bitnrs.
 *
 * So for all bits whose number have LOCK_ENABLED_* set (bitnr1 == 1), we can
 * create the mask with those bit numbers using LOCK_USED_IN_* (bitnr1 == 0)
 * instead by subtracting the bit number by 2, or shifting the mask right by 2.
 *
 * Similarly, bitnr1 == 0 becomes bitnr1 == 1 by adding 2, or shifting left 2.
 *
 * So split the mask (note that LOCKF_ENABLED_IRQ_ALL|LOCKF_USED_IN_IRQ_ALL is
 * all bits set) and recompose with bitnr1 flipped.
 */
static unsigned long invert_dir_mask(unsigned long mask)
{
	unsigned long excl = 0;

	/* Invert dir */
	excl |= (mask & LOCKF_ENABLED_IRQ_ALL) >> LOCK_USAGE_DIR_MASK;
	excl |= (mask & LOCKF_USED_IN_IRQ_ALL) << LOCK_USAGE_DIR_MASK;

	return excl;
}

/*
 * Note that a LOCK_ENABLED_IRQ_*_READ usage and a LOCK_USED_IN_IRQ_*_READ
 * usage may cause deadlock too, for example:
 *
 * P1				P2
 * <irq disabled>
 * write_lock(l1);		<irq enabled>
 *				read_lock(l2);
 * write_lock(l2);
 * 				<in irq>
 * 				read_lock(l1);
 *
 * , in above case, l1 will be marked as LOCK_USED_IN_IRQ_HARDIRQ_READ and l2
 * will marked as LOCK_ENABLE_IRQ_HARDIRQ_READ, and this is a possible
 * deadlock.
 *
 * In fact, all of the following cases may cause deadlocks:
 *
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*
 * 	 LOCK_USED_IN_IRQ_* -> LOCK_ENABLED_IRQ_*_READ
 * 	 LOCK_USED_IN_IRQ_*_READ -> LOCK_ENABLED_IRQ_*_READ
 *
 * As a result, to calculate the "exclusive mask", first we invert the
 * direction (USED_IN/ENABLED) of the original mask, and 1) for all bits with
 * bitnr0 set (LOCK_*_READ), add those with bitnr0 cleared (LOCK_*). 2) for all
 * bits with bitnr0 cleared (LOCK_*_READ), add those with bitnr0 set (LOCK_*).
 */
static unsigned long exclusive_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Retrieve the _possible_ original mask to which @mask is
 * exclusive. Ie: this is the opposite of exclusive_mask().
 * Note that 2 possible original bits can match an exclusive
 * bit: one has LOCK_USAGE_READ_MASK set, the other has it
 * cleared. So both are returned for each exclusive bit.
 */
static unsigned long original_mask(unsigned long mask)
{
	unsigned long excl = invert_dir_mask(mask);

	/* Include read in existing usages */
	excl |= (excl & LOCKF_IRQ_READ) >> LOCK_USAGE_READ_MASK;
	excl |= (excl & LOCKF_  ƒ|$`"‡  ‹D$`¾€    ‰D$…Àˆ  ˜„   İ    ‰\$q‰L$$‰ó‰t$‹4$‰õ‹vhİ…ötz8  ‰l$ ‰D$‹D$‹l†‰êƒâuZ‹D$‹D$‹DÆ9Èuë>´&    ƒÂ‹ 9Át‹X…Ûuğ‹X…Ûtì‹[…Ûuâ‹ 9Áuåt& ƒú‡'  „(  ‹vh…öu—‹l$ ‹4$‹\$Ş‰D$‹€4  9Åt‹D$9„ì  …  ‹D$‹˜4  9İ„C  ‰Ø1Òët& ƒÂ‹ 9Åt‹p…öuğ‹H…Étì‹I…Éuâ‹ 9Åuåt& ƒú?‡1  öD$d„  ‹L$‹T$‰Ø´&    v …ÿt9xu9Ï…·   …Òt	;P„ª   ‹ 9ÅuÜ‹T$‹D$…ÒuéÕ   v ‹9İ„ª  ;Cuñû ğÿÿ‡Î  ‰ù…ÿ„ë  ‹$‰\$‰Ë1ö‹€”   ‹P@‹@<‰D$(‹D$`‰T$,‰D$0‹C(‹„°x  …Àt1ÉT$(èüÿÿÿ‰D´4…À„w  ƒştj‹C(¾   ‹„°x  …ÀuÑëæt& ¾êÿÿÿ‹D$Dd+    …>  ƒÄH‰ğ[^_]ÃöD$d…¬  t& ‹D$‹€4  9Å…=ÿÿÿ1ÛéBÿÿÿ´&    ‹\$…Ûtu‹D$‹k‰{‹L$‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èüõÿÿ‰Æ…À…ú   …í„Ÿ   ‰èèüÿÿÿ‹<$‹L$`1ö‹D$<‰úèüÿÿÿ‹L$`‹D$@‰úèüÿÿÿé8ÿÿÿ¡   ¹   ºÀ  èüÿÿÿ‰Ã…À„C  ‹t$‹L$‰+‹†8  ‰8  ‰C‰‹D$‰{‹|$‰C‹D$4‰ú‰C‹D$8‰C‹$‰Œ¸ì  èUõÿÿ‰Æ…À…²   ‹D$$àûÿÿèüÿÿÿéUÿÿÿ´&    ‰ò‰Øèüÿÿÿ‰D´4= ğÿÿ‡¾   ‰D´<éhşÿÿ´&    ‹D$‹Hé	şÿÿt& …ít[‰k‹D$<ÇC    èüÿÿÿ‹D$@èüÿÿÿéPşÿÿv éØüÿÿ‰è‹l$ ¨…Õüÿÿ¾ÿÿÿÿé.şÿÿ‹D$¾şÿÿÿ…À…şÿÿ1Ûéœıÿÿ‹D$<èüÿÿÿ‹D$@èüÿÿÿ‹C‹‰B‰‰ØÇ   ÇC"  èüÿÿÿéáıÿÿ¾şÿÿÿé×ıÿÿ‹D$<¾ôÿÿÿèüÿÿÿ‹D$@èüÿÿÿé»ıÿÿ¾ùÿÿÿé±ıÿÿèüÿÿÿ‰Şé¥ıÿÿ´&    èüÿÿÿS‰Ãö@,tƒT  1Ò[éüÿÿÿt& èüÿÿÿ‹C¨udÿ èüÿÿÿƒT  1Ò[éüÿÿÿ¶    ‹Cğƒ ëŞ´&    ´&    èüÿÿÿU¹   ºĞ  WV‰ÆSƒìdd¡    ‰D$`1Àl$‰ïó«†T  ‰$hÀ  èüÿÿÿZ‰Ç…À…¶   ‹^h…Ût"èüÿÿÿ‹ƒT  ¨…ì   dÿ èüÿÿÿ‹[h…ÛuŞ†4  ì  ‰ ‰@ƒÀ9Áuô†H  ‰†H  ‰†L  ëƒÇƒÿtxL½ ‰ú‰ğ‰ëèÉñÿÿ…Àtål$`‹ƒÃèüÿÿÿ9ëuò‹^h…Ûtèüÿÿÿ‹ƒT  ¨u{dÿèüÿÿÿ‹[h…Ûuâ‹$¿ôÿÿÿèüÿÿÿ‹D$`d+    u{ƒÄd‰ø[^_]Ã´&    f1Û¶    ‹L ‹„Ø  “ô   ƒÃ‰L–èüÿÿÿƒûuŞ1ÿë²f‹ƒX  ğƒ éÿÿÿ‹ƒX  ğƒ(…xÿÿÿ‹“X  ƒT  ‹Rèüÿÿÿé_ÿÿÿèüÿÿÿv èüÿÿÿU‰ÕW‰ÏVS‰Ãƒì‹ èüÿÿÿ‰Æ= ğÿÿ‡Ê   ‹S‰ĞƒàƒøtE¸    ‹[‰$èüÿÿÿ‹$1É‰ğR‰úSj è›øÿÿ‰Ã¸    èüÿÿÿƒÄöF,thƒÄ‰Ø[^_]Ãt& ‹C‰ê1Éèüÿÿÿ‰Å= ğÿÿws‹S¸    ‹[‰$èüÿÿÿ‹$‰é‰ğR‰úSj è>øÿÿ‰Ã¸    èüÿÿÿƒÄ…ítŸ‰èèüÿÿÿöF,u˜èüÿÿÿ‹F¨uRdÿèüÿÿÿ‰ØƒÄ[^_]ÃƒÄ‰Ã‰Ø[^_]Ãt& öF,t
‰ëé]ÿÿÿv èüÿÿÿ‹F¨u-dÿèüÿÿÿ‰ëé?ÿÿÿt& ‹Fğƒ(u¨‹VF‹Rèüÿÿÿë˜‹Fğƒ(uÍ‹VF‹Rèüÿÿÿë½´&    ´&    èüÿÿÿW‰×V‰ÆS‹ èüÿÿÿ‰Ã= ğÿÿwt‹F‰ú1Éèüÿÿÿ‹v‰Ç¸    ÿ ğÿÿwfèüÿÿÿ‰ú1É‰ØVè…ñÿÿ‰Æ¸    èüÿÿÿZ…ÿt‰øèüÿÿÿöC,t‰ğ[^_Ãèüÿÿÿ‹C¨uDdÿèüÿÿÿ‰ğ[^_Ã¶    ‰Æ[‰ğ^_Ã´&    èüÿÿÿ1É1Ò‰ØVèñÿÿ‰Æ¸    èüÿÿÿXë£‹Cğƒ(u¶‹SC‹Rèüÿÿÿë¦´&    èüÿÿÿUWV‰ÆSƒì‹Nd¡    ‰D$1À…É…ó   ‹F‰Õèüÿÿÿ‰Ç= ğÿÿ‡Ô   ¡   ¹0   ºÀ èüÿÿÿ‰Ã…À„ß   U¹\  º   èüÿÿÿ‹F‰{(T$‰C,‰Øèüÿÿÿ‰ÆX…öuS‹s,¸    èüÿÿÿ1É1Ò‰øjVSèÿõÿÿ‰Æ¸    èüÿÿÿƒÄ‰à…öuJèüÿÿÿ‰Æ‹D$d+    uvƒÄ‰ğ[^_]Ãt& ‰ØèüÿÿÿöG,uØèüÿÿÿ‹G¨u/dÿèüÿÿÿëÂt& èüÿÿÿëØ´&    f‰Æë©t& ¾êÿÿÿë‹Gğƒ(uË‹WG‹Rèüÿÿÿë»¾ôÿÿÿëŸèüÿÿÿ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì d¡    ‰D$‹èüÿÿÿ‰Ç= ğÿÿ‡ò  ¸    èüÿÿÿ‹CÇ$êÿÿÿƒø"‡<  ¾€    …Àˆ-  ˆ„   ‹”‡ì  ‹k‰L$ŒÇ4  ‰L$‹Œ‡Ø  ‹C‰T$‰L$ƒà„D  ‰Èèüÿÿÿ‰D$¹   FT$èüÿÿÿ…À…¬  F¹   T$èüÿÿÿ…À…“  ‹C‰î…À„¢   …í„š   ‹L$‰$…É„’   9ÈƒC  ‰D$Ç$äÿÿÿöC…~  ‹D$1í‹\Ç‹D$9Øtc‰|$‰Çë@t& ‹@(¹   T$‹@ ‰D$‰ğèüÿÿÿ…À…  ƒÅ;l$„Ñ   ‹ƒÆ9ß„Ä   ‹C…Àu½‹C…Àt¶‹@ë±Ç$    ¸    èüÿÿÿöG,t!‹D$d+    …ÿ   ‹$ƒÄ [^_]Ãt& èüÿÿÿ‹G¨…¨   dÿèüÿÿÿëÅ¶    ‹L$‹TÏ‹L$9Ñth‰$‰Ëë
ƒÀ‹9Ót‹J…Éuğ‹J…Étì‹I…Éuâëã‹$éƒşÿÿ´&    ‰$éoÿÿÿ´&    ‹|$éNÿÿÿ´&    Ç$    é¼şÿÿt& 1ÀéDşÿÿ´&    f‹|$Ç$òÿÿÿéÿÿÿ‹Gğƒ(…Nÿÿÿ‹WG‹Rèüÿÿÿé;ÿÿÿ‹L$‹D$‰êèüÿÿÿ‰$éàşÿÿèüÿÿÿt& èüÿÿÿUWVS‰Ã¿Ûƒì d¡    ‰D$1À¿D$4‰L$‰T$Áà	Ø‰D$d‹    èüÿÿÿ‹ƒÈ  ‹T$8ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtB~t& ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$d+    ukƒÄ ‰Ø[^_]Ãt& èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „KÿÿÿûéEÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‰ÅWV‰Î¹	   S‰ÓƒìP‹D$d|$(‰D$‹D$h‰D$d¡    ‰D$L1À‰T$$ºÀ  ‰l$ ÇD$    ÇD$    ó«‹D$lÇD$,   ‰t$<‹L$`‰D$@¸   èüÿÿÿ‰D$(…À„4  ‰ÁD$1ÒPD$0P‰Ø‹{èüÿÿÿ[_…À…  …öt‹D$‹…Ét‹D$‹…Ò…  èüÿÿÿ‹T$pd‹    ‹ƒÈ  ÇD$    ‹@8‹´Ø  èüÿÿÿèüÿÿÿ‹ƒp  ~‰D$D$‰ƒp  ‹^…Ût@¶    ‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$èüÿÿÿ‹D$(èüÿÿÿƒûu‹D$8…À…™   ‹D$0èüÿÿÿ‹D$Ld+    …7  ƒÄP‰Ø[^_]Ãt& ÇD$,    ELF                            4     (               èüÿÿÿÇ€À      1ÀÃ´&    ´&    èüÿÿÿÇ€À       1ÀÃ´&    ´&    èüÿÿÿ‹ˆÀ   …Éu‹€Ä   ÇB(p   ÇB4   ‰B81ÀÃ´&    èüÿÿÿ‹@\èüÿÿÿ1ÀÃèüÿÿÿV1É‰ÆS‰Óƒìd¡    ‰D$‹†”   f‰L$ÆD$ƒútAƒútLƒútGj ¹   T$èüÿÿÿYƒø…-   ‰Ä   ‹D$d+    u!ƒÄ[^Ãt& ÆD$ëÂ´&    fÆD$ë²èüÿÿÿt& èüÿÿÿ‹ˆÀ   …Ét	1ÀÃ¶    ‹R8èPÿÿÿ1ÀÃ´&    ¶    èüÿÿÿVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „B   ¸ûÿÿÿ[^Ã                     @  p                   "                                                                                                           @   p       €     @       (Radio)  6%s: Audio Mode: %u%s
 vp27smpx èüÿÿÿƒ¸À    ¹	   º    DÑƒÀpRÿpTPh
   èüÿÿÿ1ÀƒÄÃƒÆpVh    èüÿÿÿXZéÕ   ‹S·CŠ  Q QP‹CTÿ²   ÿ0h$   èüÿÿÿC¹À  ºÈ   èüÿÿÿ‰ÆƒÄ…Àu
¸ôÿÿÿéh  ¹@   ‰Úèüÿÿÿ‰ğº   Ç†Ä      è|   1Àéh   3%s: I/O error setting audmode
    6%s %d-%04x: chip found @ 0x%x (%s)
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ            vp27smpx                                                           À       `                                                                                                                                       @                                                                                         license=GPL author=Hans Verkuil description=vp27smpx driver  GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ                                                    ,   @   )     =   p                                   	 M       1    	               a   €   Œ     v   1       	      #     ¡   @  /     °   F   v    	 Ä   @                       Ñ            æ       €     ö       
           0                        0     )  À   ,     <  `   P     O           f          |           —             ¢             À             È             Ú             ô                                       +             @             N             b           n             }      
     Œ      0      vp27smpx.c vp27smpx_s_radio vp27smpx_s_std vp27smpx_g_tuner vp27smpx_remove vp27smpx_log_status vp27smpx_set_audmode vp27smpx_set_audmode.cold vp27smpx_s_tuner vp27smpx_probe vp27smpx_probe.cold vp27smpx_ops vp27smpx_driver_init vp27smpx_driver vp27smpx_driver_exit vp27smpx_id vp27smpx_core_ops vp27smpx_tuner_ops vp27smpx_video_ops __UNIQUE_ID_license268 __UNIQUE_ID_author267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev _printk __stack_chk_guard i2c_transfer_buffer_flags __stack_chk_fail __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__vp27smpx_id_device_table      !     A     q     y          ’   !  Å   "  à   !    #      A    S  $  Ï   	  c  	                 h                             	                                      "     '      6     ;      e     j      |   %  ”     ›   &  ±     B          ¸                  '     (          *  @     D     L     l     Ä     Ô     Ø        	   .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   o                    	   @       è
  x               )             À  €                   %   	   @       `                  /             @                     8             @  $                  4   	   @       €  H               E      2       d  +                 X               ¼                  T   	   @       È  €      	         g      2       L  J                 z             –                    v   	   @       H                  ‰             ª  
                  …   	   @       h                 ˜             À  0                  ”   	   @       x  @                             ğ  <                  ©      0       ,                   ²              ?                     Â             @  (                                h  Ğ              	              8  °                               ¸  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          éáşÿÿv èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „ÿÿÿûéÿÿÿv ‹|$‹èüÿÿÿ‹D$0‹L$‰‹D$4‰éPÿÿÿ´&    ‹L$`ºÀ  ¸   èüÿÿÿ¹   ‰D$0‰Â‹D$‹ 9ÈGÁ‰D$4…ÒtR‹L$‹1ƒøs$…À„.şÿÿ¶ˆ
¨„!şÿÿ·Lşf‰Lşéşÿÿ‹zƒçü‰
‹Lü‰Lü)ú)ÖÁéó¥éïıÿÿÇD$4    éâıÿÿèüÿÿÿ´&    t& èüÿÿÿUWV‰ÆSƒìp‹¶ä  ‰D$‹„$ˆ   ‰T$‹œ$„   1Ò‰D$‹„$Œ   ‰L$‰D$d¡    ‰D$l1À‰TLƒÀƒø rô1Ò1À‰T,ƒÀƒø rô‹D$‹|$L$L‰D$(‹D$‹‹ ‰D$4‹D$‹ ‰D$8¸   9ÂLĞD$(èŞÿÿ‰D$…ÀxN‹?‹l$,9Ç‰|$<Oø…ÿˆñ  ‰ú1É‰èèüÿÿÿ‰ù‰Ú‰èèüÿÿÿ‰Ç…ÿt=‹l$,ÇD$òÿÿÿD$L9Åt‰èèüÿÿÿ‹D$ld+    …í  ‹D$ƒÄp[^_]Ãt& ‹D$1Òèüÿÿÿ‹  ÇD$$    èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$ ‰ˆp  ‹[…Ût;‰|$ >t& ‹K$S0D$(èüÿÿÿ‰Æ…öu|$$ ğÿÿwÇD$$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹\$$‹D$èüÿÿÿ…Û…Ç   ‹D$<‹l$,ƒøÿ„Æ   Œÿşÿÿ;D$õşÿÿ‹T$4‹L$‰‹T$8‹L$‰…À„±   ‹L$‰L$L9Í„¬   ‹D$ÇD$    ‰(éËşÿÿt& èüÿÿÿS0‹s$‰ÅD$(èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „óşÿÿûéíşÿÿv ‰\$‹l$,éJşÿÿv ÇD$   é:şÿÿv é şÿÿÇD$    é#şÿÿºÀ èüÿÿÿ‰Ã…Àt%‹L$<‹T$,èüÿÿÿ‹D$ÇD$    ‰éşÿÿèüÿÿÿÇD$ôÿÿÿ‹l$,éİıÿÿ¶    èüÿÿÿUWV‰ÆSƒìh‹ä  1ö‰D$‹D$|‹¼$ˆ   ‰D$‹„$€   ‰D$d¡    ‰D$d1À‰tDƒÀƒø rô1ö1À‰t$ƒÀƒø rô‹D$‰T$,‰L$0L$D‰D$ d¡    ‰D$8‹„$„   ‰D$4‰ÂD$ èÛÿÿ‰D$…ÀˆX  …ÿuL‹D$èüÿÿÿ‰T$4…Òˆc  …À…[  ‹D$‰Ö‹l$$9ÂOğ1É‰è‰òèüÿÿÿ‹T$‰ñ‰èèüÿÿÿ…À…,  ‹D$1Òèüÿÿÿ‹›  ‰|$èüÿÿÿ{èüÿÿÿd¡    ‹ˆp  ‰L$L$‰ˆp  ‹[…Ût;‰|$>t& ‹K$S0D$ èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹t$‹D$èüÿÿÿ…öˆ÷   ‹\$4‹D$$;\$u…Ûxq‰t$t6¹   ‰Ú‰$èüÿÿÿ‹$‹D$‰Ùèüÿÿÿ…ÀuF‹D$4‹L$èüÿÿÿ…Éu5‹D$$L$D9Ètèüÿÿÿ‹D$dd+    …”   ‹D$ƒÄh[^_]Ã´&    ‹D$$ÇD$òÿÿÿëÁ´&    v èüÿÿÿS0‹s$‰ÅD$ èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „Ãşÿÿûé½şÿÿv ‰t$éJÿÿÿèüÿÿÿfèüÿÿÿUWV‰ÖS‰Ãƒì<‹|$T‹D$P‰|$d‹    ‰T$8‹“ä  ‰\$‹‰D$Ø‰t$ ‰D$‹D$X‰L$$‰D$‹²  ‰\$(ÇD$0    d‹    ÇD$4    ‰\$,èüÿÿÿèüÿÿÿ‹ƒp  ‰D$D$‰ƒp  ‹^…ÛtC~¶    ‰|$>t& ‹K$S0D$èüÿÿÿ‰Æ…öu|$ ğÿÿwÇD$ÿÿÿÿ‹_ƒÇ…ÛuÆ‹L$d¡    ‰ˆp  èüÿÿÿèüÿÿÿ‹D$…Àx‹L$‹T$(;€   …Òux‹T$8d+    uyƒÄ<[^_]Ãv èüÿÿÿS0‹s$‰ÅD$èüÿÿÿ‹[‰Æèüÿÿÿ…    œ$úƒCƒƒS èüÿÿÿ1Ò)èCSƒC÷$   „;ÿÿÿûé5ÿÿÿv ‰ë„¸òÿÿÿézÿÿÿèüÿÿÿ     cgroup_bpf_enabled_key  __cgroup_bpf_run_filter_skb  __cgroup_bpf_run_filter_sk  __cgroup_bpf_run_filter_sock_addr  __cgroup_bpf_run_filter_sock_ops                                                                        @   ğ  0  P  `  p  Ğ     °  @  À  ğ  `	  À  €                   p  °  ğ  °  0  p  €    À  à       0  P$  °$  @&  °'  (  à)  ,   .  ğ0   4   6  cgroup_id:	%llu
attach_type:	%d
      À      Ô  @        €      ¤         -  -      /  È/      T2   3      45  06      D7  È7      include/linux/skbuff.h / kernel/bpf/cgroup.c include/linux/thread_info.h °      ö	  ­     @       ƒ	 #     Õ 	 3  -   é 	¸$   èüÿÿÿ ›  “  Ş    ›$  &  &  s'  Œ'  s(  ³)  S,   ÿÿÿ	
ÿÿÿÿÿÿÿÿÿ strnlen           P      0                                          	                                                               @                           	                                                            	                                                             	                                                             	         
                                   °                                             
                                                                                                               À  `  à        p                       GCC: (GNU) 11.2.0           GNU  À       À                                  ñÿ              
              +             N              o             •   4          ½              ã   5            P          /            
 T  Q          €  s          ®             Ú  t            •          2             ]  @   ª    w  0      •  P       ­  `  
     Å  p  V     ä  Ğ  /     ú                           V     )  `  E    @  @  y                                 R  À  ¼     a      
     u  $        €     i     ›    Œ     ²     ü     Í     ú     ß     <     ş  à  <       €  <     -  @  <     P  À   <     o     <     „     r     –     Í    ©      #     ´  °  w     Ê  0  7    â  p      ù  €  …        ¡     ,  À       D  à       [     
    q  `   <     ”  0      ¨  \                     ½           û          :          r          «          X       ¸     Ç   `	  W    9  °  6    ¸  ğ  ³    <  p  6    ß           î             ù                          ğ  3     2             ;             A             N             Y             f             p             ~             ‰             ›             ±             Ê             Ö             ï                                       (  °  Š     A             H             O  À  (     l  ğ  n     …             •             ¥             ·             Æ             Î             Ö             ä  €  ™     ø             	             '	             G	             e	             y	             ‹	             §	             Ã	             Í	             å	             ø	             
             
             3
             G
             [
             t
             ‡
             Ÿ
             »
             Ó
             ì
                                       .             D             \             k             ‚             –             ¯  P$  Q     Â             Ş  °$      ñ               @&  b                 +             A  °'  Ù     X  (  E    o             }             Œ             œ             ­  à)  ¬    Ã             ç             Ù             õ  ,  b       .  å    6             M             f  ğ0  
    ‰                          ­             ¾             Ë   4      î             û                6  ›    0  ,        D  0        \  œ        o           †  ¼       š  Ô        cgroup.c __kstrtab_cgroup_bpf_enabled_key __kstrtabns_cgroup_bpf_enabled_key __ksymtab_cgroup_bpf_enabled_key __kstrtab___cgroup_bpf_run_filter_skb __kstrtabns___cgroup_bpf_run_filter_skb __ksymtab___cgroup_bpf_run_filter_skb __kstrtab___cgroup_bpf_run_filter_sk __kstrtabns___cgroup_bpf_run_filter_sk __ksymtab___cgroup_bpf_run_filter_sk __kstrtab___cgroup_bpf_run_filter_sock_addr __kstrtabns___cgroup_bpf_run_filter_sock_addr __ksymtab___cgroup_bpf_run_filter_sock_addr __kstrtab___cgroup_bpf_run_filter_sock_ops __kstrtabns___cgroup_bpf_run_filter_sock_ops __ksymtab___cgroup_bpf_run_filter_sock_ops sysctl_convert_ctx_access cg_sockopt_convert_ctx_access cg_sockopt_get_prologue bpf_cgroup_link_dealloc bpf_cgroup_link_fill_link_info cgroup_bpf_release_fn cgroup_bpf_release bpf_cgroup_link_show_fdinfo __bpf_prog_run_save_cb copy_sysctl_value sysctl_cpy_dir sysctl_cpy_dir.cold __func__.0 cgroup_dev_is_valid_access sysctl_is_valid_access cg_sockopt_is_valid_access sysctl_func_proto bpf_sysctl_get_new_value_proto bpf_set_retval_proto bpf_sysctl_get_name_proto bpf_sysctl_get_current_value_proto bpf_sysctl_set_new_value_proto bpf_get_retval_proto sockopt_alloc_buf cgroup_bpf_replace CSWTCH.197 cgroup_dev_func_proto compute_effective_progs update_effective_progs __cgroup_bpf_detach bpf_cgroup_link_release.part.0 bpf_cgroup_link_release bpf_cgroup_link_detach cg_sockopt_func_proto bpf_get_netns_cookie_sockopt_proto __cgroup_bpf_attach bpf_cgroup_link_lops __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_ops491 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sock_addr490 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_sk489 __UNIQUE_ID___addressable___cgroup_bpf_run_filter_skb488 __UNIQUE_ID___addressable_cgroup_bpf_enabled_key483 bpf_get_retval __fentry__ current_task bpf_set_retval bpf_get_netns_cookie_sockopt init_net kcmd_drivers/media/i2c/vp27smpx.o := gcc -Wp,-MMD,drivers/media/i2c/.vp27smpx.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"vp27smpx"' -DKBUILD_MODNAME='"vp27smpx"' -D__KBUILD_MODNAME=kmod_vp27smpx -c -o drivers/media/i2c/vp27smpx.o drivers/media/i2c/vp27smpx.c 

source_drivers/media/i2c/vp27smpx.o := drivers/media/i2c/vp27smpx.c

deps_drivers/media/i2c/vp27smpx.o := \
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
    $(wildcard include