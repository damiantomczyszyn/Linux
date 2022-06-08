(likely(hlock_class(this)->usage_mask & new_mask))
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
	if (DEBUG_LOCKS_WARN_ON(depth >= MAX/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ctype.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
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
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
    $(wildcard include/config/HYPERV) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MSI) \
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
  include/linux/filter.h \
    $(wildcard include/config/BPF_JIT_ALWAYS_ON) \
    $(wildcard include/config/HAVE_EBPF_JIT) \
    $(wildcard include/config/IPV6) \
  include/linux/bpf.h \
  include/uapi/linux/bpf.h \
    $(wildcard include/config/BPF_LIRC_MODE2) \
    $(wildcard include/config/EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/BPF_KPROBE_OVERRIDE) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/XFRM) \
  include/uapi/linux/bpf_common.h \
  include/linux/file.h \
  include/linux/rbtree_latch.h \
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
  arch/x86/include/asm/fsgELF                      °      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿS‹P(‹Xú	˜ tú	˜ tT¹êÿÿÿú 	˜ t'‰È[Ãv ¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    f¶H|‹CÔº   èüÿÿÿ1É[‰ÈÃ´&    fèüÿÿÿ‹€”   º   ƒÉ@¶Éèüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã‹@‹P‹Rèüÿÿÿ%   =   „7   ¾ûÿÿÿ[‰ğ^_]Ã´&    t& èüÿÿÿUöÆù‰ÍW‰×º   V‰Æ¸    S»   DØƒÃë´&    ¶SƒÃ„Òt¶‹†”   èüÿÿÿ…Àyã[‰¾0  1À‰®4  ^_]Ã    à                                                                                                                                  °       à          6%s: Standard: %d Hz
 tw9906 èüÿÿÿS‰Ã‹€0  % ù  ƒøÀƒàöƒÀ<PCpPh    èüÿÿÿ‰Øèüÿÿÿ1ÀƒÄ[Ã‹S·CŠ  Q QPÿ²   ‹CTÿ0h    èüÿÿÿC¹À  º8  èüÿÿÿ‰ÇƒÄ…Àu
¾ôÿÿÿé
  ¹`   ‰Ú¯À   èüÿÿÿ1Éº   ‰èj èüÿÿÿ¹ 	˜ º  ‰èj j j jj jjÿj€èüÿÿÿ¹	˜ º  ‰èƒÄ$j j`j jj hÿ   j j èüÿÿÿ¹	˜ º  ‰èƒÄ j j j jj jjÿj€èüÿÿÿ‹·ü   ‰olƒÄ …öt‰èèüÿÿÿé
  Ç‡0   °  ½   º   Ç‡4      ¶M‹‡”   èüÿÿÿ…ÀyƒÃ¾êÿÿÿSh(   èüÿÿÿXZé
  ¶UƒÅ„ÒuÈé
   6%s %d-%04x: chip found @ 0x%02x (%s)
 3%s: error initializing TW9906
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿ                 	     	ğ            tw9906                                                          à           €                   °                                                                                                                                                              @¢	ğ
Ğ   `~~ W@)U k&l6mğnAo­p  license=GPL v2 description=TW9906 I2C subdev driver  GCC: (GNU) 11.2.0             GNU  À       À                                  ñÿ                            
                   ‡     &   °   !                                	 =       ;    	               O   à   5     \   ;   R   	 n   `         y          ‰      :     ›      `     ¨       
     ¶      
                   Ä            ×       €     å       
                   ø       0       à   0       €   P     #           :     %     U             `             ~             •             ¯             ·             Ó             ì             ù                          +             =             K             _           k             z      
     ‰      0      tw9906.c tw9906_remove tw9906_s_ctrl tw9906_s_video_routing tw9906_log_status tw9906_probe tw9906_probe.cold tw9906_ops tw9906_ctrl_ops initial_registers tw9906_s_std config_50hz.2 config_60hz.1 tw9906_driver_init tw9906_driver tw9906_driver_exit tw9906_id tw9906_core_ops tw9906_video_ops __UNIQUE_ID_license267 __UNIQUE_ID_description266 __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw9906_id_device_table                 !     ]      }            ±     Ê      á     õ   #  !    7    =    e                        h                                              $     )   !  0   "  Z   
  _   !  q   $  ‰     –   %  ¦   &  °     Ç   '  Ñ     î   '  ø       '  )    =    [     m  
  r  !  „     .    y    ‰                 (     )          +  `     l     €     Œ     à          .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .rel.rodata .modinfo .comment .note.GNU-stack .note.gnu.property                                                         @   €                    	   @       Ø  €               )             À  €                   %   	   @       X                  /             @                     8             @                    4   	   @       x  8               E      2       \                   X             z                   T   	   @       °  È      	         g      2         I                 z             Q                    v   	   @       x                  ‰             e  
                  …   	   @       ˜                 ˜             €  Z                  ”   	   @       ¨  0                             Ú  4                  ©      0                          ²              !                     Â             $  (                                L  à              	              ,	  «                               Ø  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          sbase.h \
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
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
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
  include/linux/kallsyms.h \
    $(wildcard include/config/KALLSYMS_ALL) \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64) \
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
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
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
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
  include/linux/sched/mm.h \
    $(wildcard include/config/ARCH_HAS_MEMBARRIER_CALLBACKS) \
  include/linux/sync_core.h \
    $(wildcard include/config/ARCH_HAS_SYNC_CORE_BEFORE_USERMODE) \
  arch/x86/include/asm/sync_core.h \
  include/linux/ioasid.h \
    $(wildcard include/config/IOASID) \
  include/linux/bpfptr.h \
  include/linux/sockptr.h \
  include/linux/bpf_types.h \
    $(wildcard include/config/BPF_LSM) \
    $(wildcard include/config/XDP_SOCKETS) \
  include/linux/compat.h \
    $(wildcard include/config/ARCH_HAS_SYSCALL_WRAPPER) \
    $(wildcard include/config/COMPAT_OLD_SIGACTION) \
    $(wildcard include/config/ODD_RT_SIGACTION) \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/uapi/linux/socket.h \
  include/uapi/linux/if.h \
  include/uapi/linux/libc-compat.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/uapi/linux/aio_abi.h \
  arch/x86/include/asm/compat.h \
  include/linux/sched/task_stack.h \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  include/uapi/linux/magic.h \
  arch/x86/include/asm/user32.h \
  include/asm-generic/compat.h \
    $(wildcard include/config/COMPAT_FOR_U64_ALIGNMENT) \
  arch/x86/include/asm/syscall_wrapper.h \
  include/linux/skbuff.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/BRIDGE_NETFILTER) \
    $(wildcard include/config/NET_TC_SKB_EXT) \
    $(wildcard include/config/NET_SOCK_MSG) \
    $(wildcard include/config/SKB_EXTENSIONS) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/IPV6_NDISC_NODETYPE) \
    $(wildcard include/config/NET_SWITCHDEV) \
    $(wildcard include/config/NET_REDIRECT) \
    $(wildcard include/config/NETFILTER_SKIP_EGRESS) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/NET_RX_BUSY_POLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/NETWORK_SECMARK) \
    $(wildcard include/config/PAGE_POOL) \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/NETWORK_PHY_TIMESTAMPING) \
    $(wildcard include/config/MPTCP) \
    $(wildcard include/config/MCTP_FLOWS) \
    $(wildcard include/config/NETFILTER_XT_TARGET_TRACE) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/IP_VS) \
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
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  include/linux/siphash.h \
  arch/x86/include/asm/archrandom.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/HAS_DMA) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) cmd_drivers/media/i2c/tw9906.o := gcc -Wp,-MMD,drivers/media/i2c/.tw9906.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fall