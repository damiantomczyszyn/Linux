RCOPY) \
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
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/i2c/tw9910.h \
  include/media/v4l2-subdev.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/uapi/linux/v4l2-subdev.h \
  include/media/media-entity.h \
  include/uapi/linux/media.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/cdev.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \

drivers/media/i2c/tw9910.o: $(deps_drivers/media/i2c/tw9910.o)

$(deps_drivers/media/i2c/tw9910.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ELF                      ‰      4     (               Ë¸ˇˇˇã@\Ë¸ˇˇˇ1¿√Ë¸ˇˇˇSâ√ã@ãPãRË¸ˇˇˇ%  ` =  ` unçCπ¿  ∫¿   Ë¸ˇˇˇÖ¿t_â⁄π@   Ë¸ˇˇˇπÄ   1“âÿË¸ˇˇˇπA  1“âÿË¸ˇˇˇãC∑Sçà  QçQRˇ∞   ãCTˇ0h    Ë¸ˇˇˇ1¿Éƒ[√ê∏Ìˇˇˇ[√∏Ùˇˇˇ[√fêË¸ˇˇˇSãòî   É˙uπA  1“âÿË¸ˇˇˇ1¿[√çt& êÉ˙uâÿπA  1“Ë¸ˇˇˇ1¿[√È¸ˇˇˇ                                                                                                                                                 ∞       6%s %d-%04x: chip found @ 0x%02x (%s)
 3%s: input %d not supported
 uda1342 É¿pRPh    Ë¸ˇˇˇÉƒÈÀ   Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2                               uda1342                                                                 `                               ∞        GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ                                                            û     (   @                      	              
 4   ∞   D     F           
               ]            q       Ä     Ä       
     î            ´       0                   ∂   `        »              ”              Ò              
                          ,             F             N             \             p           |             ã      
     ö      0      uda1342.c uda1342_remove uda1342_probe uda1342_ops uda1342_s_routing uda1342_s_routing.cold uda1342_driver_init uda1342_driver uda1342_driver_exit __UNIQUE_ID_license266 uda1342_id uda1342_audio_ops __fentry__ v4l2_device_unregister_subdev __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_write_word_data _printk __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__uda1342_id_device_table         	          "     @     K     P     ^     l     è     î     ±     À     Á        	                 h                                                                       H     h      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rodata.str1.1 .rel.text.unlikely .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   Ù                     	   @       ¸  x               )             @  Ä                   %   	   @       t                  /             ¿                     8             ¿                    4   	   @       î                  E      2       –  (                 T      2       ¯  &                 g                                 c   	   @       ¥        
         z             5                    v   	   @       Ã                  â             I  
                  Ö   	   @       Ï                 î             S                    °             Ä  p                   ù   	   @       ¸                 ©      0                          ≤                                   ¬               (                                ,                	              <  Ω                                 ’                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ELF                      ‰      4     (               Ë¸ˇˇˇã@\Ë¸ˇˇˇ1¿√Ë¸ˇˇˇSâ√ã@ãPãRË¸ˇˇˇ%  ` =  ` unçCπ¿  ∫¿   Ë¸ˇˇˇÖ¿t_â⁄π@   Ë¸ˇˇˇπÄ   1“âÿË¸ˇˇˇπA  1“âÿË¸ˇˇˇãC∑Sçà  QçQRˇ∞   ãCTˇ0h    Ë¸ˇˇˇ1¿Éƒ[√ê∏Ìˇˇˇ[√∏Ùˇˇˇ[√fêË¸ˇˇˇSãòî   É˙uπA  1“âÿË¸ˇˇˇ1¿[√çt& êÉ˙uâÿπA  1“Ë¸ˇˇˇ1¿[√È¸ˇˇˇ                                                                                                                                                 ∞       6%s %d-%04x: chip found @ 0x%02x (%s)
 3%s: input %d not supported
 uda1342 É¿pRPh    Ë¸ˇˇˇÉƒÈÀ   Ë¸ˇˇˇ∫    ∏    È¸ˇˇˇ∏    È¸ˇˇˇlicense=GPL v2                               uda1342                                                                 `                               ∞        GCC: (GNU) 11.2.0           GNU  ¿        ¿                                  Òˇ                                                            û     (   @                      	              
 4   ∞   D     F           
               ]            q       Ä     Ä       
     î            ´       0                   ∂   `        »              ”              Ò              
                          ,             F             N             \             p           |             ã      
     ö      0      uda1342.c uda1342_remove uda1342_probe uda1342_ops uda1342_s_routing uda1342_s_routing.cold uda1342_driver_init uda1342_driver uda1342_driver_exit __UNIQUE_ID_license266 uda1342_id uda1342_audio_ops __fentry__ v4l2_device_unregister_subdev __x86_indirect_thunk_edx devm_kmalloc v4l2_i2c_subdev_init i2c_smbus_write_word_data _printk __this_module i2c_register_driver init_module i2c_del_driver cleanup_module __mod_i2c__uda1342_id_device_table         	          "     @     K     P     ^     l     è     î     ±     À     Á        	                 h                                                                       H     h      .symtab .strtab .shstrtab .rel.text .rel.data .bss .rel__mcount_loc .rodata.str1.4 .rodata.str1.1 .rel.text.unlikely .rel.init.text .rel.exit.text .modinfo .rel.rodata .comment .note.GNU-stack .note.gnu.property                                                         @   Ù                     	   @       ¸  x               )             @  Ä                   %   	   @       t                  /             ¿                     8             ¿                    4   	   @       î                  E      2       –  (                 T      2       ¯  &                 g                                 c   	   @       ¥        
         z             5                    v   	   @       Ã                  â             I  
                  Ö   	   @       Ï                 î             S                    °             Ä  p                   ù   	   @       ¸                 ©      0                          ≤                                   ¬               (                                ,                	              <  Ω                                 ’                                                                                                                                                                                                                                                                         