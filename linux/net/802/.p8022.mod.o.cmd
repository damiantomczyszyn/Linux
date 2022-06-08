tic void zap_class(struct pending_free *pf, struct lock_class *class)
{
	struct lock_list *entry;
	int i;

	WARN_ON_ONCE(!class->key);

	/*
	 * Remove all dependencies this lock is
	 * involved in:
	 */
	for_each_set_bit(i, list_entries_in_use, ARRAY_SIZE(list_entries)) {
		entry = list_entries + i;
		if (entry->class != class && entry->links_to != class)
			continue;
		__clear_bit(i, list_entries_in_use);
		nr_list_entries--;
		list_del_rcu(&entry->entry);
	}
	if (list_empty(&class->locks_after) &&
	    list_empty(&class->locks_before)) {
		list_move_tail(&class->lock_entry, &pf->zapped);
		hlist_del_rcu(&class->hash_entry);
		WRITE_ONCE(class->key, NULL);
		WRITE_ONCE(class->name, NULL);
		nr_lock_classes--;
		__clear_bit(class - lock_classes, lock_classes_in_use);
		if (class - lock_classes == max_lock_class_idx)
			max_lock_class_idx--;
	} else {
		WARN_ONCE(true, "%s() failed for class %s\n", __func__,
			  class->name);
	}

	remove_class_from_lock_chains(pf, class);
	nr_zapped_classes++;
}

static void reinit_class(struct lock_class *class)
{
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
	memset_startat(class, 0, key);
	WARN_ON_ONCE(!class->lock_entry.next);
	WARN_ON_ONCE(!list_empty(&class->locks_after));
	WARN_ON_ONCE(!list_empty(&class->locks_before));
}

static inline int within(const void *addr, void *start, unsigned long size)
{
	return addr >= start && addr < start + size;
}

static bool inside_selftest(void)
{
	return current == lockdep_selftest_task_struct;
}

/* The caller must hold the graph lock. */
static struct pending_free *get_pending_free(void)
{
	return delayed_free.pf + delayed_free.index;
}

static void free_zapped_rcu(struct rcu_head *cb);

/*
 * Schedule an RCU callback if no RCU callback is pending. Must be called with
 * the graph lock held.
 */
static void call_rcu_zapped(struct pending_free *pf)
{
	WARN_ON_ONCE(inside_selftest());

	if (list_empty(&pf->zapped))
		return;

	if (delayed_free.scheduled)
		return;

	delayed_free.scheduled = true;

	WARN_ON_ONCE(delayed_free.pf + delayed_free.index != pf);
	delayed_free.index ^= 1;

	call_rcu(&delayed_free.rcu_head, free_zapped_rcu);
}

/* The caller must hold the graph lock. May be called from RCU context. */
static void __free_zapped_classes(struct pending_free *pf)
{
	struct lock_class *class;

	check_data_structures();

	list_for_each_entry(class, &pf->zapped, lock_entry)
		reinit_class(class);

	list_splice_init(&pf->zapped, &free_lock_classes);

#ifdef CONFIG_PROVE_LOCKING
	bitmap_andnot(lock_chains_in_use, lock_chains_in_use,
		      pf->lock_chains_being_freed, ARRAY_SIZE(lock_chains));
	bitmap_clear(pf->lock_chains_being_freed, 0, ARRAY_SIZE(lock_chains));
#endif
}

static void free_zapped_rcu(struct rcu_head *ch)
{
	struct pending_free *pf;
	unsigned long flags;

	if (WARN_ON_ONCE(ch != &delayed_free.rcu_head))
		return;

	raw_local_irq_save(flags);
	lockdep_lock();

	/* closed head */
	pf = delayed_free.pf + (delayed_free.index ^ 1);
	__free_zapped_classes(pf);
	delayed_free.scheduled = false;

	/*
	 * If there's anything on the open list, close and start a new callback.
	 */
	call_rcu_zapped(delayed_free.pf + delayed_free.index);

	lockdep_unlock();
	raw_local_irq_restore(flags);
}

/*
 * Remove all lock classes from the class hash table and from the
 * all_lock_classes list whose key or name is in the address range [start,
 * start + size). Move these lock classes to the zapped_classes list. Must
 * be called with the graph lock held.
 */
static void __lockdep_free_key_range(struct pending_free *pf, void *start,
				     unsigned long size)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i;

	/* Unhash all classes that were created by a module. */
	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			if (!within(class->key, start, size) &&
			    !within(class->name, start, size))
				continue;
			zap_class(pf, class);
		}
	}
}

/*
 * Used in module.c to remove lock classes from memory that is going to be
 * freed; and possibly re-used by other modules.
 *
 * We will have had one synchronize_rcu() before getting here, so we're
 * guaranteed nobody will look up these exact classes -- they're properly dead
 * but still allocated.
 */
static void lockdep_free_key_range_reg(void *start, unsigned long size)
{
	struct pending_free *pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	pf = get_pending_free();
	__lockdep_free_key_range(pf, start, size);
	call_rcu_zapped(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);

	/*
	 * Wait for any possible iterators from look_up_lock_class() to pass
	 * before continuing to free the memory they refer to.
	 */
	synchronize_rcu();
}

/*
 * Free all lockdep keys in the range [start, start+size). Does not sleep.
 * Ignores debug_locks. Must only be used by the lockdep selftests.
 */
static void lockdep_free_key_range_imm(void *start, unsigned long size)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	init_data_structures_once();

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_free_key_range(pf, start, size);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_free_key_range(void *start, unsigned long size)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_free_key_range_imm(start, size);
	else
		lockdep_free_key_range_reg(start, size);
}

/*
 * Check whether any element of the @lock->class_cache[] array refers to a
 * registered lock class. The caller must hold either the graph lock or the
 * RCU read lock.
 */
static bool lock_class_cache_is_registered(struct lockdep_map *lock)
{
	struct lock_class *class;
	struct hlist_head *head;
	int i, j;

	for (i = 0; i < CLASSHASH_SIZE; i++) {
		head = classhash_table + i;
		hlist_for_each_entry_rcu(class, head, hash_entry) {
			for (j = 0; j < NR_LOCKDEP_CACHING_CLASSES; j++)
				if (lock->class_cache[j] == class)
					return true;
		}
	}
	return false;
}

/* The caller must hold the graph lock. Does not sleep. */
static void __lockdep_reset_lock(struct pending_free *pf,
				 struct lockdep_map *lock)
{
	struct lock_class *class;
	int j;

	/*
	 * Remove all classes this lock might have:
	 */
	for (j = 0; j < MAX_LOCKDEP_SUBCLASSES; j++) {
		/*
		 * If the class exists we look it up and zap it:
		 */
		class = look_up_lock_class(lock, j);
		if (class)
			zap_class(pf, class);
	}
	/*
	 * Debug check: in the end all mapped classes should
	 * be gone.
	 */
	if (WARN_ON_ONCE(lock_class_cache_is_registered(lock)))
		debug_locks_off();
}

/*
 * Remove all information lockdep has about a lock if debug_locks == 1. Free
 * released data structures from RCU context.
 */
static void lockdep_reset_lock_reg(struct lockdep_map *lock)
{
	struct pending_free *pf;
	unsigned long flags;
	int locked;

	raw_local_irq_save(flags);
	locked = graph_lock();
	if (!locked)
		goto out_irq;

	pf = get_pending_free();
	__lockdep_reset_lock(pf, lock);
	call_rcu_zapped(pf);

	graph_unlock();
out_irq:
	raw_local_irq_restore(flags);
}

/*
 * Reset a lock. Does not sleep. Ignores debug_locks. Must only be used by the
 * lockdep selftests.
 */
static void lockdep_reset_lock_imm(struct lockdep_map *lock)
{
	struct pending_free *pf = delayed_free.pf;
	unsigned long flags;

	raw_local_irq_save(flags);
	lockdep_lock();
	__lockdep_reset_lock(pf, lock);
	__free_zapped_classes(pf);
	lockdep_unlock();
	raw_local_irq_restore(flags);
}

void lockdep_reset_lock(struct lockdep_map *lock)
{
	init_data_structures_once();

	if (inside_selftest())
		lockdep_reset_lock_imm(lock);
	else
		lockdep_reset_lock_reg(lock);
}

/*
 * Unregister a dynamically allocated key.
 *
 * Unlike lockdep_register_key(), a search is always done to find a matching
 * key irrespective of debug_locks to avoid potential invalid access to freed
 * memory in lock_class entry.
 */
void lockdep_unregister_key(struct lock_class_key *key)
{
	struct hlist_head *hash_head = keyhashentry(key);
	struct lock_class_key *k;
	struct pending_free /x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
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
  include/linux/kstrtox.h \
  include/ELF                      0      4     (               èüÿÿÿS‹X\‰ØèüÿÿÿƒÀ   èüÿÿÿ1À[ÃèüÿÿÿS‰Ã‹@‹HÔ‹C(=	˜ tHw&=	˜ tW=	˜ uº>   ‰Èèüÿÿÿ‰C|1À[Ãt& =$	˜ uğº=   ‰Èèüÿÿÿ‰C|1À[Ã¶    º<   ‰Èèüÿÿÿ‰C|1À[Ãt& º?   ‰Èèüÿÿÿ‰C|1À[Ãt& èüÿÿÿWVS‰Ã‹p‹@(‹~Ô=	˜ „R  wp=	˜ „U  v3=	˜ „`  =	˜ …   ¶K|º?   [‰ø^_éüÿÿÿ´&    v = 	˜ ty=	˜ ub¶Vp¶K|ÁâƒÊ[‰ø^¶Ò_éüÿÿÿ´&    =	˜ tYv=$	˜ u0¶K|º=   ë¡t& =	˜ ty=	˜ u¶K|º<   ëƒ´&    ¸êÿÿÿ[^_Ã´&    ¶Vp¶K|ÁâƒÊëŒ¶Vp‰øÁâƒÊ¶Òèüÿÿÿ…ÀxÍƒ{|º   ƒÚÿ%ü   	Ğ¶Vp‰ÁÁâƒÊéPÿÿÿt& ¶Vp‰øÁâƒÊ¶Òèüÿÿÿ…Àx‹S|‰Á€É€…Òu$‰Á¶Vp¶ÉÁâƒÊéÿÿÿ¶    ¶K|º>   éÎşÿÿf¶Vp¶K|ÁâƒÊééşÿÿt& ¶Vp¶K|ÁâƒÊéÑşÿÿt& èüÿÿÿW‰ÇVS‹@‹P‹Rèüÿÿÿ%   =   …0  G¹À  º8  èüÿÿÿ‰Æ…À„  ¹@   ‰úÀ   èüÿÿÿ€0  1É‰ØÇ†4   °  º
   j èüÿÿÿ¹ 	˜ ºğ   ‰Øj h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ$j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ j h€   j jj hÿ   j j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ j j j jj jj j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ j j j jj jj j èüÿÿÿ¹	˜ ºğ   ‰ØƒÄ j h€   j jj hÿ   j j èüÿÿÿƒÄ …ÀtHt€   j ¹$	˜ ºğ   ‰Øh€   j jj hÿ   j j èüÿÿÿƒÄ …ÀtHt€   j ¹	˜ ºğ   ‰Øjzj jj hÿ   j j èüÿÿÿƒÄ …ÀtHt€   j ¹	˜ ºğ   ‰Øjzj jj hÿ   j j èüÿÿÿƒÄ …ÀtHt€   ‰^l‹¶ü   …ö„d   ‰Øèüÿÿÿ‰ğ[^_Ãt& ¾íÿÿÿëî¾ôÿÿÿëç´&    v èüÿÿÿUWV‰ÆSƒì(‰T$d¡    ‰D$$‹†”   ÆD$ÆD$	ÆD$
ÆD$ÆD$ÆD$‰$‰Ğ% ù  ÆD$ƒøÆD$íÆD$ ƒåÀÆD$ !ƒí<ƒøÉƒáıƒÁƒøÒˆL$ƒâ0ˆL$ƒêƒøÿˆT$ƒç
ˆT$ƒÇ@ƒøÛƒã@ƒøÀˆ\$‰ë¶L$ƒàˆ\$‰û|$ƒÀˆ\$ˆD$ˆD$!¸ÿÿÿÿˆL$ˆL$f‰D$"‹$¸   ¶®0  Áåët& ¶GƒÇ<ÿt	è¶¶Ğ‰Øèüÿÿÿ…Àyâ‹D$‰†4  ‹D$$d+    u
ƒÄ(1À[^_]Ãèüÿÿÿt& èüÿÿÿUW‰ÇV‰ÖSƒì‹¯”   ‹D$…Àtz¶—0  Xÿ‰Ñƒá9Ëtgƒø‡®   ƒèƒâüƒà	Âˆ—0  ƒâ…Ö   1Ûº9   ëv ¶“r  ƒÃ€úÿ„­   ¶‹q  ‰èèüÿÿÿ…ÀyÛé”   ¶    ‹4$t& ƒş‡Í   ¶—0  1À‰ÑÀéƒá9ñt[Áâ‰èƒÊ"¶Òèüÿÿÿ…ÀxG‰ÂƒÈƒâû…ö‰Ñ¶—0  EÈ‰èÁâƒÊ"¶É¶Òèüÿÿÿ…Àx¶‡0  ƒæÁæƒàû	ğˆ‡0  1ÀƒÄ[^_]Ã´&    ¶‡0  ƒà‰Á‰4$1Û¸   Áá‰Îëv ¶ƒ  ƒÃ<ÿ„>ÿÿÿ	ğ¶‹  ¶Ğ‰èèüÿÿÿ…Ày×éÊ   ¸êÿÿÿëœ                               `                      D                                                                                                           °       `  Ğ        60 Hz 50 Hz 6%s: Standard: %s
 6%s: Channel: %d
 6%s: Input: %d
 tw2804 èüÿÿÿVº   S‰Ã÷€4   ù  ¸    DÂspPVh   èüÿÿÿŠƒ0  ƒàPVh    èüÿÿÿŠƒ0  ÀèƒàPVh3   èüÿÿÿ‰ØƒÄ$[^éüÿÿÿ‹W·GŠ  Q QP‹GTÿ²   ÿ0h    èüÿÿÿƒÄé©  ƒÅht   UèüÿÿÿX¸ûÿÿÿZé  ƒÅPh(   Uèüÿÿÿ¸êÿÿÿƒÄé  ¶‡0  ƒÅƒàPhL   Uèüÿÿÿ¸ûÿÿÿƒÄé   6%s %d-%04x: chip found @ 0x%02x (%s)
 channel %d is not between 1 and 4!
 error initializing TW2804 channel %d
   error initializing TW2804 global registers
 èüÿÿÿº    ¸    éüÿÿÿ¸    éüÿÿÿauthor=Micronas USA Inc description=TW2804/TW2802 V4L2 i2c driver license=GPL v2                  tw2804                                                          `                                                                                                         Ğ                                                                          °       Ä¥ Ğ Ğˆ 	
ğğ@Ò€€€€    ÿÿÿÿÿÿÿÿ !" #‘$Q%& ' ( ) * + , - . / 0 1 2 3 4 5 6 7 ÿÿ9 :ÿ;„<€=€>‚?‚x ÿÿ GCC: (GNU) 11.2.0             GNU  À       À                                  ñÿ                            
                   ‹     /   °   «                               	 =       h    	               O   `  f    \   @         g   ğ        w   h   0    	 ‰   Ğ  <    –     u    ­   p       ¾      p     Ğ   ˜   \    	               ì            ÿ       €           
                 6     *     Q  B        h      0                   r  `   0     ‚      P     “                          ¼             Ó             ì                                       *             C             P             e             ‚             ”             ¦             ·             À             Î             â           î             ı      
           0      tw2804.c tw2804_remove tw2804_g_volatile_ctrl tw2804_s_ctrl tw2804_log_status tw2804_probe tw2804_ops tw2804_ctrl_ops tw2804_probe.cold tw2804_s_std tw2804_s_video_routing global_registers channel_registers tw2804_s_video_routing.cold tw2804_driver_init tw2804_driver tw2804_driver_exit __UNIQUE_ID_author268 __UNIQUE_ID_description267 __UNIQUE_ID_license266 tw2804_id tw2804_core_ops tw2804_video_ops __fentry__ v4l2_device_unregister_subdev v4l2_ctrl_handler_free i2c_smbus_read_byte_data i2c_smbus_write_byte_data _printk v4l2_ctrl_subdev_log_status __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init v4l2_ctrl_handler_init_class v4l2_ctrl_new_std __stack_chk_guard __stack_chk_fail _dev_err __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__tw2804_id_device_table                 !  !     P   "  o   "  ˆ   "      "  ±     °  "  ğ  "  a    t  &  –  '  ¥    ²  (  Ó  )  İ    ú  *      $  *  .    N  *  X    x  *  ‚    œ  *  ¦    À  *  Ê    ê  *        *  9    Q  *  k    ƒ  *  ©  !  Ñ    ä  +  Ş  #  ÷  +    ,      s    †      #  Ò  "  ÿ  #  S    g    q  #    #  5  #  ¢    F    –    z                   h                                                             &     +   $  ;     @   $  S     X   $  ‡   
  Œ   $  œ   
  ¢   -  ·   
  ½   -  İ   
  ã   -  d   %  ”     ®     Ê     ğ                  .     /          1  @     L     `           ¬     ğ     ø      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.1 .rel.text.unlikely .rodata.str1.4 .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   …                    	   @         Ğ              )             à  €                   %   	   @       à                  /             `                     8             `                     4   	   @          @               E      2       €  K                 X             Ë  ô                  T   	   @       @  °      	         g      2       À	                    z             `
                    v   	   @       ğ                  ‰             t
  
                  …   	   @                        ”             ~
  Q                  ¡             à
  ‚                     	   @          8               ©      0       b                   ²              u                     Â             x  (                                   @              	              à  .                               X  Õ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/linux/sched.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/IOMMU_SVA) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/asm-generic/percpu.h \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/pid.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/rcupdate.h \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/irqflags.h \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
    $(wildcard include/config/DEBUG_ENTRY) \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/processor-flags.h \
    $(wildcard include/config/VM86) \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  inclcmd_drivers/media/i2c/tw2804.o := gcc -Wp,-MMD,drivers/media/i2c/.tw2804.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"tw2804"' -DKBUILD_MODNAME='"tw2804"' -D__KBUILD_MODNAME=kmod_tw2804 -c -o drivers/media/i2c/tw2804.o drivers/media/i2c/tw2804.c 

source_drivers/media/i2c/tw2804.o := drivers/media/i2c/tw2804.c

deps_drivers/media/i2c/tw2804.o := \
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
    $(wildcard includ