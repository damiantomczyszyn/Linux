
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

drivers/media/pci/cx18/cx18.mod.o: $(deps_drivers/media/pci/cx18/cx18.mod.o)

$(deps_drivers/media/pci/cx18/cx18.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ELF                      p'      4     (                �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ��     �           ��    �  �  s           GNU  �       �               GNU p�7���J��=O"��&��        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ��       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       	 T           ��d  �       y  �       �  �  %     �          �    �     s   d       	 �  �  R     �    �    �  �   @    	   �  0     +  H       	 6  @       	 A  P       	 L  �   -     e  �        �          �  �   
    	 �           ���  �   #     �  "       �  ;         G  	       P       )  _  7     @           H          P             \           j             s                          �             �             �  U   ?     �             �             �             �             �             	                                      >             F             \             m             u             �             �             �             �             �             �             �                          9             A             U             c             p             �           �     �    �             �             �  �  �     �             �              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     �     �   B  �   U  �     �   i  �   L  �     �     �       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  �  c  �    �  S  �    �  N  �  N  �  N  �  K  �  Z    N          �  I  �  _  �  R  �    �      I  @  I  R    Z    x  R  �  e  �    �    �  N  �  V  �  \  �  Y  �  a    N  #  E  C  V  m  X  �  a  �  E  U  ]  o  d  �  P  �  a  �  a  �  S  �  N  �    �  E  �  D  �          W  &  [  I  T  V    g    p  ^  {  E  �  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  �     �   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     �     �   Q  �     �     �   Q  �     �     �     �   Q  �     �     �   Q  �     �     �     �     �   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    �  Q  �    �    �    �  Q  �    �    �    �    �    �  Q  �    �    �  M  �    �  M  �    �    �  Q  �       N          Q     `       �     �     �     �     �                                                    $              B     f     c          B     f  $                     �   O  �  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   �                    	   @       �  �              )             �  �                  %   	   @       l"  �               8             d  �                 4   	   @       �"  H              K             X	                    G   	   @       <%  (               Z             �	  �                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       �%  P               �      2       }  +                 �             �  �                 �             @  (                  �   	   @       �%  @               �             h                    �   	   @       &                 �             t                     �             t  (                  �             �  $                  �             �  0                  �                           @       �   	   @       4&                                                       0          9                              9                                   <  �     A         	              �                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      p'      4     (                �������t?S�Ë��  ��t���t
ǂ$&      �����ǃ�      ǃ�      [Í�&    Í�&    �����UWV��S����d�    �D$1�����   ��  ����   ��,&  ����   �F�$    1ɺ����Tj �   h    ����������t   �   �   ��  �,$������Å��Z   �8�   �
   �h���  ǅ�      �x�(�G������G$�u���Dh   j P������ud�u�h@  jPW������؃�$��������   ��,&  �$���������   �    u-�D$d+    u��1�[^_]Ív �    t�������������   f�f�f�f�������@�@    �@    1�Í�&    �����1�Í�&    ������VS�p�^��������v���������[^Í�&    �t& �����UWVS�Ã��5   �$�ʅ��2  �h����   �}|����   �w`����   9��  ��4  ���7  ��1����S�D$�GT�D$����ȉщD$�D$ЉD$9�sv�t$�$�D$���������������C�t$��C�OT��)�9�C�s�s��C�GL9���   ����[^_]������-   ���  ��[^_]Í�&    �v �L$�D$)ʉы$�ΉL$������D$�L$+L$�$ȋL$��ʉ���4  ������T�����&    �   ���   ��[^_]Í�&    �v )���K��������[^_]�������    �   ���P����  ��&    ��    �����WVS�p���   ��T  �����1҉����������  �������ǃ(&      �   �����1�[^_Í�&    ��&    �����U��WVS��l�u|d�    �D$h�E��$�C��D$�Ǎ�   �������<&  ���  �|$d�H�D$\�D$��<&  �T$`��������"  ���  ��x��T  �D$����  s6�$�T$�    ������T$�D$hd+    ��   ��l��[^_]Ít& �Ѝ��   �   �   ��ǆ�    ǆ�      ǆ       ǆ  �   ǆ  ��  ǆ  ��  ǆ     ǆ     ǆ  � @ǆ!  1  ǆ(  b   ������<$�L$�o���   ǃ(&      ����  �D$������$�   ������$�����$�    ����������������������&    �����U1ɺ   W��VS���/d�    �D$1��GTjj ������ƃ�����  �G    �$��   �   �����1ɺ   j j �D$������\$�P   �C    ��X��`  �}d���������XZ�����  ��Pt����P��  ����������D$d+    u
����[^_]������S�@@��uh    h    �����XZ�6��$&  ��u��h    Ph0   ����������B�����ǃ$&      1�[�hh  ������    �   �����1�1�h    ������        h�  ��������h(   h    h`   ��������b  ��Ph   Vhl  ��������$��t������������h(   Vh  ��������b  ��j�h   Vh  �������뻃�Ph   Vh�   �������뵃�h(   Vh�   ��������b  ǆ,&      ��Ph   Vh�  ��������a���h(   h    �����Y[�b  h(   h    h�  ��������b  hd   hd   hp  ���������  hd   hL  �����^_��  Q�L$�t$Phd   h�  ������k�T$����tD�}|���3  �=    ��  hd   h$  �����XZ��  hd   h�  �����XZ��  �=    ��  hd   h�  �����Y[��  �H   ������@   �������PhP   Uh�  ��������p  �����hH  �����X1��    P   �           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   �              �  �      �                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       �  �  �     �    �  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ��     �           ��    �  �  s           GNU  �       �               GNU p�7���J��=O"��&��        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ��       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 �            �   U   ?     �            �           �      #     �   =          O   Y       �        4           B  8       	 T           ��d  �       y  �       �  �  %     �          �    �     s   d       	 �  �  R     �    �    �  �   @    	   �  0     +  H       	 6  @       	 A  P       	 L  �   -     e  �        �          �  �   
    	 �           ���  �   #     �  "       �  ;         G  	       P       )  _  7     @           H          P             \           j             s                          �             �             �  U   ?     �             �             �             �             �             	                                      >             F             \             m             u             �             �             �             �             �             �             �                          9             A             U             c             p             �           �     �    �             �             �  �  �     �             �              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     �     �   B  �   U  �     �   i  �   L  �     �     �       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  �  c  �    �  S  �    �  N  �  N  �  N  �  K  �  Z    N          �  I  �  _  �  R  �    �      I  @  I  R    Z    x  R  �  e  �    �    �  N  �  V  �  \  �  Y  �  a    N  #  E  C  V  m  X  �  a  �  E  U  ]  o  d  �  P  �  a  �  a  �  S  �  N  �    �  E  �  D  �          W  &  [  I  T  V    g    p  ^  {  E  �  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  �     �   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     �     �   Q  �     �     �   Q  �     �     �     �   Q  �     �     �   Q  �     �     �     �     �   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    �  Q  �    �    �    �  Q  �    �    �    �    �    �  Q  �    �    �  M  �    �  M  �    �    �  Q  �       N          Q     `       �     �     �     �     �                                                    $              B     f     c          B     f  $                     �   O  �  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   �                    	   @       �  �              )             �  �                  %   	   @       l"  �               8             d  �                 4   	   @       �"  H              K             X	                    G   	   @       <%  (               Z             �	  �                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       �%  P               �      2       }  +                 �             �  �                 �             @  (                  �   	   @       �%  @               �             h                    �   	   @       &                 �             t                     �             t  (                  �             �  $                  �             �  0                  �                           @       �   	   @       4&                                                       0          9                              9                                   <  �     A         	              �                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ELF                      (W     4     ( ' &             �����S��h    j������    XY��t��P���[������t& [Í�&    ��&    �����UWVS�����   �F�~��$�    �s   ���Y  ������    �_   ��4&  ��~��������   �����1  ��������Q  ��&    ���t�����9�u�d  ���)  ��    ��������t����������  9�u庈   �����������������1  ������   ���������������F�����L  �������t&��L&  ��t�    ��   �����ǆL&      ��@&  ��D&  j h   �щ¸    ������F��������80  ZY��t$��<0  ���0  ���&    �E �������9�u�h  ������vd�t$hY   ����������������[^_]����������WVS�Ӊ�����������d�=    ���&    ��t4��t0��W�����1ҹ   ��t���   u!������у���u�[��^_Í�&    �v �   [^��_Ít& ������U�ŉ�W�zV�����S)���D����  d�    ��$   1��    �B@    ��  ��  �$   �������tR�ù   �@�k   ��������R  �   �C�P   ��f�C���������u�Eh� ��	u5�����������������$   d+    u-��  [^_]Í�&    �v ��   ����   ��y��   ������v �����UWV�   S��|�$d�    �D$x1���5�   ���3  �   ��  ��Y  ������Å���
  �$�3�S�T$�C�   �D$������D$����  �C��<  �D$���3h}   j$P������3W1�h�   �������   �-�   ǃT      ǃ|      ����@  ��   ��d  ��<  ��   ��P  ��   ��D  ��   ��H  ��   ��L  ��   ��h  ��   ��x  ��   ��l  ��   ��p  ��   ��t  ��   ���  ��   ���  ��   ���  ��   ǃ�  �  ���  ��   ǃ�      ���  �\$�o��    ���~g�ND��~`����  ����  �,   ����  ���k  ��퍅�  I���
���
�ND�    ��  ������t&�n(��u����    ���F$    �F@    ��uڋ\$�  ��4�    ���  ��   ��X  ��\  ��`  ��M��  �9  ��B��  �B  �   1Ҁ�Ht��  �   1Ҁ�I��  ��&    ����Y  ���Y  ����o  �F���������Ch����  ��X  ����  �Cf�x$p��  �t$��������Ch������  ���   ��tN�f��tF�k���}"�f����f��t-f9�u��P�f9U$u��P�f9U&�v  ���f��u؍t& ���뎍v ��
�Iֹ����
��� �  ��)ȋL$i� �  9�L��  �ND����  �ͅɍ��� I�����2����t& ��-u{�  ��L�-  �  ��B��  �>  ��-��  �   ��M�q  �w  �    1Ҁ�J������ �  1Ҁ�K������-��  1�1��m����t& ���6��  �   1��S�����Ntn��b�V  �   1��9�����K��  ��   1��$�����   1Ҁ�D������Gt���   ��&    �v ��i�5  ~E�   1Ҁ�m�������nud�  �   1҃�߀�C������   1�������&    �v ��d�w����N����   1Ҁ�g�������h��  �   1��|�����k�C�����   ��    ����
����F(������&    ���������F(������
�������L$��
�R�   ��	9�B��  ����1�������Ch�C�P"R�@ P�|$Wh�  ������C�P&R�@$PWh(  ������Ch�� �pWh�   �����Whd  �����Wh�  �����Wh  ������Ch��$�P�Sx�P�Sl���   �Cp��X  ��P  �C�   ���1  ��T  ��P  ��L&  �  ��H&  ��(  �������X  �   �  ��������   �   �.  ������   �C  ���   ������t$hX  jV�����Vjh 
 h^  ��������1  �� ���i  ���Q  ���1  ���Q  ��    �X�Hl�@������ �@��t�@�    9�u����  ��  ǃD&     �2   ���  ��ǃ      ǃ     ǃ�      ǃ�      ������D$���X  ��$  ���  �   �CD��   ���   ��  ���  ���   ��  ���  ���   ���   ���	Ћ��  ��   �a  ��  ���1  ��������1  �   �l  ��������1  �   �~  ��������1  ��  �   �������`&  ��D1  ǃ\&     ��('  ��l1  ��l1  ��H1  ��D1  ��X1  ��X1  ��\1  ��l1  ��p1  ��X1  �    ��  �$���������  �D$1ɺ�������������  j �    h�  j h   ��H&  ��L&  �����������  �<$�L$ �   ��������L$ �   ����f�L$ ��������G,�   �L$��P&  ��������D$<?w��   ����  ��H&  ��L&  �    �  �   �������T&  ���6  ��   ��X&  ��( ���%   �=   �  =   ���  R�t$h�  ��������  ��&    �  2 1Ҁ�d�������   ��g���(  �   1�������   1�������ktK~���l�(  �  �  @ 1҃�߀�C������  � 1�������H+�   1Ҁ�F�k�����Dum�  2 1��Z�����bt��(  ��Kt��(  ��    �   1��4����   1��(���� �  1Ҁ�k������mt߀�j�  �    1�������(  �(  �  ��   1�������Ch�t$h�  ������ChZY�������%�����   �D$�����D$xd+    u�D$��|[^_]���   ��   �������&    �t& �����S�Á�   d�    ��$�   1��\$X��4&  �  @ ��  ���4&  �    �k  �����������  ���4&  j h�   jh  S�����j h  S������� j jh  S������������������  j h�   jh  S�����j h  S������� j jh  S������C`����t1�Pd���t(�R��t!�    ����   �I����   ������v ���Y  �D$\    �D$`   ���D$d   �� ��Y  ��   ��t�D$d4  ���Y  ��A���Y  1���������Y  ���Y  ���Y  �����Y   ����� 	�u�   1�RP1��T$�����1��L$d�T$�����[X1���$�   d+    u0�Č   [Í�&    �D$d�  �o����v ������&���������������f�f�f�f�f������f��	w����`  Í�&    �v 1�Í�&    ��    �����UWV�Ɖ���S�˃��Nh�|��Nvf9���   ���    �,�8  �C�$�������������  �� t���� ��  �$��������?t6�C$   �Nw�   1������C(���� �S4�C01���[^_]Í�&    �C$   �Nw�   �����C(���Y  ���Y  ��f��������&    f������U��WV��S�ˍLm ���@h�    �C0    �D�L�ى$�C�ǉD$1����)���4����Fwf9�vW�$�    � �4�  �������������  �� t���� �  �D$��������+1��C$   ��[^_]Ít& ������������S�x�ы�% �����X&   Q� ��t(���X&  �
   ��&    �v ��9�t��u�[Ð����X&  �
   f���9�t��u�[Ð�����S�x�ы�% �����X&   Q� ��t(���X&  �
   ��&    �v ��9�t��u�[Ð����X&  �
   f���9�t��u�[Ð�������x���X&  % �����Q� � ��Í�&    �t& �������x���X&  % �����Q� � ��Í�&    �t& �����U��W�   V��S�Ã�D�4��  d�    �D$@1���   i�p  ���R  ��t]��  ���   ��   ��t>j �����P�������ZY��t'���   1��T$@d+    �B  ��D[^_]Í�    ������ڐ�sp�{���j �������YZ��t
ǀ�      �Cp����P��j �����YZ��t
ǀ�      �Cp����
P��j �����]Z��t�ǀ�      1��g�����    �����f�D$1��   f�|$�|$���W  �$��   �����������.  ��t�����$  ��D$������Clǃ�W  l  ǃ�W     ���W  �$ǃ�W    ǃ�W      �D$$��j �L$�T$�����[= ��������������������&    ��&    ������U�hW��V�Ɖ�S������F�V9�t/�X����&    �v �CL�X�9�t9��   u���������[^_]É�1��������[^_]������UWV��S���    @�B  ���W  �$    ��|W  �D$�F���T  �D$�$�
   �����|W  1��i�p  �|$�E�  �E0  ��   �E@  �E  �E   �E �  �W�0   �7�} ���R  1���cx18��ǃ����    �C i2c�C dri�Cver �C    �C    �C    �C    �C     �C$    �C(    �C,    ������>�������0�8  �W  ��4$��(��p  Wh�  P������|$�D$��������~���   �������<$�  ��X&   Q� � =/� �  ��X&  �  � �� � �
   �t& �����t��� � t��u�
   �t& ��X�A �������u�X&  �
   �� � �� � �v �����t%� � =�   t��u�
   ��X�A �������u�X&  �  � �� � �
   �v �����t��� � t��u�
   �t& ��X�A �������u�X&  �  � ���0� �
   �v ��
��  � t��uX&  �  � ���0� �
   ��&    f������t��  � t��uꋆX&  � �� P� �
   �t& ��
�� t��u�W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��    ��9�t��u��W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��&    �v ��9�t��u�X&  � �� Q� �
   �t& ���
�� t��u�W  ���W  ���X&  % ��� Q� ��	�X&  ���
   ��    ��9�t��u��W  ���W  ���X&  % ��� Q� ��	�X&  ���
   f���9�t��u�F�~�X�9�t2��    ���   @t�Cd� ��t�H��t	1҉�������CL�X�9�uԍ��R  ��������Å���   ����[^_]Ív �$   �����t& ��X&  �   ��� �
   ��&    �v �����t��  t��uꋆX&  �
   �  ��$� �t& �����t%  =   �������u��{����t& ���U  ������Å��V�����������J�����&    �����VS���    @�a  ��X&  �� P� ��X&  ���� P� �
   ��&    �v �
�29�t��u�X&  �� Q� ��X&  ���� Q� �
   �v �
�29�t��u󍃜R  �������U  [^�����f�f�f�f�f�������V��S�    �v  ��X&  �
   �  �� � �t& �����t
%  ��t��u苎X&  �
   �  �� �� f������t
%  ��t��u�[^Ít& ������W���   VS��X&  �Ӎ���� �
   ��    �:�2��t��u�X&  �   �� �� �
   �t& �>����t��  t��u����X&  ҃��� �� �
   ����    ��>9�t��u����X&  ҁ�$I����� �
   ��7����&    f���>9�t��u�X&  �   ���� �
   �t& ��:�2��t��u�X&  �   ���� �
   �t& �:�2��t��u�X&  �   ���� �
   �t& �:�2��t��u�X&  ���   ������� �
   ��    �2�:9�t��u����X&  ҁ�;�� ���� �
   ��D�0 ��&    f���>9�t��u�X&  �   ���� �
   �t& ��:�2��t��u�X&  �   ��@�� �
   �t& �:�2��t��u�X&  ���+ ��D�� �
   �t& �:�2����+ t��uX&  �   ��H�� �
   ��:�2��t��u�X&   � ����   �
   �  ���t& ��0��� t��u�X&  � ����� �
   �t& �2���t��u�X&  � ����� �
   �t& �2���t��u�X&  ������ �
   �t& �2���  t��uX&  �&����� � �
   ��2���&�  t��uX&  �
   �1����$� ��&    f���
��1  t��u�[^_Ív �
   �  ��    �0����t��  ��t��u狁X&  �
   �  ��� ��&    f��3����t%  ���������u��������&    ��&    ������V1Ҿ   S�ø
   �������X&  �� � �
   �t& ��1����t��  t��u�1Ҹ
   ������Ch���   ��X&  �� � �
   �t& �
�29�t��u�1Ҹ
   ������Ch���   ��X&  �� � �
   �t& ��
�29�t��u�Ch���   ��X&  �� � �
   ��
�29�t��u�Ch���   ��X&  �� � �
   ��&    f��
�29�t��u�1Ҹ
   ������Ch���   ��X&  ��H � �
   �t& ��
�29�t��u�Ch���   ��X&  ��T � �
   ��
�29�t��u�1Ҹ
   �   �������X&  �� � �
   ��1����t��  t��u�1Ҹ
   �   �������X&  �� � �
   ��&    ��2�
��t��u�X&  �
   �  ��$ � �t& �1����t
%  ��t��u苃X&  �H   ��� � �
   ��&    �v �2�
��Ht��u�X&  �   ���� �
   �t& �2�
��   t��uX&  �  ��� �
   ��2�
��  t��uX&  �  ��$� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��� �
   ��&    f��2�
��  t��uX&  �  ��(� �
   ��&    f��2�
��  t��uX&  �  ��0� �
   ��&    f��2�
��  t��uX&  �  ��4� �
   ��&    f��2�
��  t��uX&  �  ��,� �
   ��&    f��2�
��  t��uX&  �
   �  ��8� ��&    f���
��  t��u�[^Í�&    �t& ������UW��VS�   ��P�D$d�    �D$L����X&  �$��L � �
   ��
��t��u�D$�
   �  ��X&  �$�� � �����t
%  ��t��u�1Ҹ   ������D$��X&  �$ � � ���  �|$�   �����������   ������G�  ��T&  �D$0    �$���   �D$0������ǅ���  �D$0��p�$���s  �D$����������D$0�|$��   �D$� �$9�s9�t& 9�}1��
   �t& ���9�t��u�9���  ������;,$r�;<$r��D$��4&  ���  �D$������t$�  � ��������<$����  ��������F��T&  �3  �D$0    �$���   �D$0�|$ ���������  �T$0�z�2���_�|$�t$�\$$�_�\$*�_0�\$+����  �D$�D$   �D$    ���D$,��D$�:���ƉD$��9���   �D$�t$���D$�x�|$�x��8Segm�<$�t$u��xHedru��    �
  �|$9|$��   �$��t~�D$ �t$1���D$�D$�����8�D$��   �����9$vJ9�}F�v �D$�\$�(�D$�4(�������
   ���9�t��u�9��t  ��9,$v9�|�;<$r��T$0�����D$0�8�t$��4&  ���5  ������t$1҉���������D$Ld+    �=  ��P��[^_]Ë�X&  �   �$�� � �
   ��
���t��  t��u�t$�2   ��v 1Ҹ
   ���������   ��X&   � � �u�1Ҹ�   ������t$��   �������j jh   �D$@PV������ǃ����E�����X&  �
   �  �� �����t��  ��   t9��u�1�����1Ҹ�   ������D$��X&  �$ � � ��o����  1�������t$��4&  ��t+������D$�  � ���������������t& ��|$�v����  f�f�f�f�f������WVS���   �Ã�p��X  ��������� X  ���W  Sh�  ������������1���[^_Í�&    f������VS�Ë@`���   ��tJ�    �-  �ف��X  t3��4Y  � ��t'�X��t �    �   ��t�p��t��[��^�����[^Ít& ��ȉ�[^�������&    ��    �����WVS���   �Ã�p��X  ��������� X  ���W  Sh�  ������������1���[^_Í�&    f������U��WVS�����W  �� X  �����$�ӉD$��X&  �����t$�֍��� ���
   ��&    �����t��t��u�D$��X&  �t$�
   	Á� �� �Ɛ�����t1���t��u싅X&  �$�t$���� f1Ҹ
   ������t��t��u�|$��X&  �
   	����� ��&    �����t1���t��u��[^_]Ít& �����UWVS���   �Ch���   ��t5��X  ���   �������3� X  ��!�3� X  �� X  �����������[1�^_]Í�&    ������UWVS���   �Vh���Y  ���   �I���   f�|�TE��   ��t3��X  �É�������� X  1�!�3� X  �� X  ���<����������[1�^_]Í�&    ��&    �����UW��V��S���˃�	؋L$ �T$$�$�L$�T$u��[^_]Ít& ���X  �������3� X  ����#4$3� X  �� X  ������������D$�����������������3� X  ����#$3� X  �� X  ������������D$�������[^_]�������&    �����S���   �Ch��t��t_��t31�[Í�&    ���   ���   1����   �������1�Y[[Í�&    ���   ���   ���   ���   �������X1�Z[Ív ���   Gu��H|j�   ��j��1�����XZ뀍v �����UWVS�����   ��t��t|�������tc����[^_]Ð�Fh���   ���   1��҉$tݍ�X  ��������$3� X  ��!�3� X  �� X  �M��������������[^_]Ít& �Fh���   릍t& ��Fh���   떍�&    ��    �����UW��X  VS�É�������Ch���   ���W  ���   �� X  ���   Gu �H|�   ��	ꉓ�W  �@|�Ɖ� X  ��t�    u������[��^_]�������X&  ���� ��X&  �� �� ��X&  ���� ��X&  �� � RQVP�CPh  �������릍�&    ��    �����UWV��S�Ã� t��@ta[�����^_]Í�&    ��X  �F  ��  ��������C���   UP�Gph_  j P��������   �C����[^_]�������&    ���X  �O  �@  뮍�    �������ugVS� ��Ch���   GuM�    �B  �ށ��X  tV��<Y  � ��tC�H��t<�    �   ��t%�X��t����[^������v 1�[^Ív 1�Ít& ���[^�����������������ې�����UW�xV��S�É���������F�$������9�tK�nf���P��Q�
�V�F�0�P��@     �@$    �@    �@    �@    ��E �9�u�����[�C    �C    ������$��[^_]������������P��tS1Ґ�XӃ��ɉ9Pw�[Ív Í�&    �����V�pS�X9�t0��&    �C��t"1���&    �K����ʉ9Cw�9�u�[^Í�&    �t& ������� �@�@    �@    Í�&    �����UWV��S���  9�t#�B     �B$    �B    �B    �B    ���  9�tM�ωˍk��������D$��t'��p��~�3�F +F$C��C���������[^_]ËC�s�>�F�0�֐�A��>~��À  ��륍�&    ��&    �����W�zV��S��������9�t6�3�F��B��F$�6�vC+F �C�F    ��K���������[^_Ð��1��������[^_Ð�����UWVS�Í��  ��$�L$�$d�    �D$ ��8  �D$�D$�D$�D$����������  ���  �D$�M 9���  �D$�U�t$����D$;$t4�E���E���  ��9��W  ��ω�;L$�q  �U��;$űU �E�t$�B��} �}����  �����������   �D$�u�E    �E �E�$�ǋE9�t	9u��   ��t  ;4$t[�l$���|$�8�v )��V�N�F��8  �@��X  ��t  �   ������6XZ94$t�F    9�v���1��f��l$��@  t��M�T$�L$���  �2��9�u
�/��    �΋J�N�1����B��j ����������^�t$9�u׋D$ d+    u]��$��[^_]ËD$�V�F    �F��8  �F�q��X  ����t  �ƈ   �������Y^�S����t$��1�������O����������&    ������V���  S��Í��  �d������  ��[^�U����t& ������UW���  V�����  S��h  ���<$�'������  ���������  ���������  �D$��������  9�tY�t& �C�{9�t;��    �C�P��Q�
��l  ��l  �(�P��@    �@    �C9�uˋ�L  �C�9$u��D$��[^_]�������&    �t& �����UWVS�Ã���8  ��L  �D$���  �D$����������  �D$�Ƌ�x  �T$��h  �$�T$���/  9$�&  �v �D$�t$�p��x  ���m  ���$    �4��  �D$��&    �v ��h  ;D$��   ��h  ��P�Q�
�L$�Q�A�L$�P�����   �L2��P�
   ��&    f���)9�t��u󋏄   ��t  �
   ��)9�t��u�$��x  ���$9��k����t$Ή�9�u[��|  1���t  ��uaj �T$�؋L$�����Z�l$�T$��������D$��t��x  9$�������[^_]Ë$�t$�j�L$�؋T$������t$X뵋$�t$��0�  ���   �
   �D���&    ��9��o�����u��e����$    �l$�C���f������UWVS����X  ��t  ��8  �Z�p�B�$�ƈ   9�t �t& �C�SUW�щ�������XZ;$u��[^_]Ít& �����WV��S�    ��  �����  ��������&    �v ������ډ��������uh  ��h  9�t^��    ��h  ��C�B��S��K�[��8  �@j ��X  ��t  �   ������C��������������h  ��9�u�[^_Í�&    �t& ��������p  ���  U��WVS����8  �D$�    �   �D$���   ���  ��=   ��  ��L  ���  1��D$�   ��&    ��   @j ���  �����t$���t$����    ������щ����������8  �F���V�Y��X  �щ���t  �È   ���������l  ��h  ��l  ��F�0��p  ZY9��8  �   �(   ��-  ������Å��>  �   �   ��-  ������ƅ��(  ��t  ��,  ������F����   �C��ډC�C��L  �[�C��j �L$�������X  �V�6�v�D$��t  �T$�D$��8  �H�ЉL$���   �����Z�$���L$������=     ������������������   �    ��te�؉$������$RPhd  ���������������������1�Ít& �����������������p  9���  �|$��   1���[^_]Ë��   듋�p  �ԉ��������p  ��f�f�f�f�������V��S���   �Ӌ�8  ��  ��  ��҉���u�    ��    �Ѓ���  �v�v ���Ѓ���  �w��F4   1�[�F8   ^Í�&    �t& �����S���   �Z(�B   ���  ���  �  �B(�J,�[Í�&    ��&    ������U���  W���  VS�Ã���    ����������ƅ�t?��8  ��D  ��$t?��T  ��u4��T  �t*j ���������Y9�t8���  ��[^_]Í�&    �v j �������������  ^��>�9�t��ϐ�V�F9�t�F;Ft^���������F��|  P��x  �L$���   ��°  +�T&  P��D  jh  Q��������  �� ��>�����a����v �P�H��8  �@��X  ��t  �   �����XZ낍�&    ������P���t�P̃�>-�  ����f�Í�&    ��&    ������S1ɉ�j �������������C    X[Í�&    ��&    �����UWVS�����   �D$��8  ���  �L$��  ��  �D$�Bl    �$��/�>  ���5  �$�BT�߉�ɉJ���  ;z�  �$�z�B�D$�B�D$���Y  ���Y  �Jd�Zh���  =HM12�4  i$�  ���  1�j �D$�T$������D$������T$�BT�B    Y��t�B9BL��   �Z��u�B   �J��u_�$�z�B�D$�B�D$���Y  ���Y  �zd�jh���  =HM12��   i$�  ���  �D$1ɉ$������$������   �B   1ۃ���[^_]Í�&    ;JL�����������ߋ$;J������\$;Z������\$;��  ������Jh�Zd3��Y  3��Y  	�������
�����&    �i$p  �������f�j �D$1�������D$������T$�B    X�X�����&    f�i$p  ������f������W��VS�Ë�8  ���  ���8  �    �a  ��<&  ����   ���U  ��@  ����   ��D  jh  V���������@  ��t����  ��t���8&  ���<&  ���T  ���D  jh  V�������D  jh  @V�����ǃD  �������U  ���<&  �� ��M��  1��������X&  �
   �   ��L � �t& �2�
��t��u�j ��\  �   �   �����X1�[^_Ít& 1�����P��D  jh  V������������t& ���4  ��t���  ���������@  �����u���x  ���������  ��&    ��    �����UWVS�Ã�p��8  d�    �D$l1����  ����  �    �d  ��@  ����  �P  ��t���b  �   ���T  h   jh  @�l$ UV������D$(��D  WPjh  V�������(����  ��<&  ����  ��@  ���   ��T&  ����H  ��@  )�)�QP��D  jh  V��������������@  �����  ���  ǃx     ��t  ��|  ����������  ��t���  ��>���D�����D  jh  V����������P  ��t���8&  ���<&  �T$ld+    �  ��p[^_]Í�&    �v ����  ���_  �   �����t& ����G  ���0  ���0  1�ǆ8'      ǆ@1      ���ǆ�0      )�ǆ<1      �����   �M�����&    �v ǃx     ��8  ��\&  �3  �xu�%�
  @&  ��|  �������&    �j ��D  jh  V�����jj��D  jh  V�������,j j��D  jh  V�����jj��D  jh  V�������8&  ��0���2  h9  h8  ��D  jh  V��������Fx�(  ���  �   ���k  P��D  jh  UV��������  ��  ��������  �����~  ���\������  UYVY��D  ��  j Pjh#  V�������<&  �����3�����    ��  �   ��������4&  X&  �   ��L � �
   ��&    �v �*�
���������u��������&    f���4  ��t���  ���6�����@  u��x  ����  �������c�����    ���  HM12��8  ��  t'i��  1҉�|  ��t  �������x  �����f�i�p  ���Ս�    �xu�%�C  `�  ��|  �����t& ��    �   ǆ�1      ǆ�1      ��   ���  ǆ�1      ǆ�1      �����ǆ@&      ��$  �{�����&    ��   �i�����    j��D  jh  V������������v ��8  �Au�ʉL$<�$ɈD$����<���,'  �у����
�D$�$�$�T$<���0'  ��-  ��\&  ��4'  �A`��  ��tR�Pd�R��tH�R�$��t>�T$�   ��`&  ����  �   �I����  ������T$�Bu�D$�t& �D$��D  �D$���$��<�L$҉L$,��  ��  <��T$0��  �L$�D$4�D$8   �L$<�$�L$@�    ��  �D$,�   �  P�D$��������  X�   ����������  ��t���  ���}���1����  �n������  �   ��x  ��E��S�����&    ���4&  � �t������  ���������P��D  jh  V��������G����t& ��    �Q�����  ��    j�D������t8�Pd�R��t.�R�$��t$�ʋ   ��`&  ��tR�   �I��tE������D$��D  �D$` ` �$Аp0��,'  �T$�T$,�������	¸�  �T$0�����$붋$������D$�@u�D$�&����    ��  ���U  ��@  ����   j��D  jh  V����������T  ���D  jh  V�������D  jh  @V�����ǃD  �������U  ���<&  �� ����������4&  ��X&  �
   �   ��L � �t& ��
���������u�������D  jh  V��������[����������  ��&    �t& ������U���  WV�ƍ��  S1ۃ��D$��L  �$�D$�5���tk���  ��t����   ����������������tu�����  ��uǋ��  ��t�$���  ��up�������ǆ�      ������Í�&    ���x  ��t�ǆx      ���  ��t��D$�����뒍�&    ��[^_]Í�&    ��D$������Z���f�����������  끐�����U���  ��  WV�ƍ@S1ۃ��$�  �D$    �D$�>�Fx�|$�=    ǅ�      �|$ǅ4      ��8  ��@  ��<  ���m  ����  �$� �D$��t���d  ���  �}��1��E     ǅ�      ���)����  ��󫉵8  ��   ǅ4      �D$��@  ��<  ��X  �$ǅD  �����@��H  ���d  ��p  ����  ��h  ��t  � ��l  ��\  ��|  ��  ǅx     ��������  ǅP  ����ǅ�      ��������  ǅ�      ��������  ǅ�      ��������  ǅ�      ��������  �    1�ǅ�  ����� ���  ���  ǅ�      � ���  ���  j j �����ǅ�      XZ���m  ���D  �|$�����   ���  ��<  �t$h�  j P������D$ D$$1�ǅ�   `  ���f���  �F���  ���� ǅ      �Nh�yu���Y  ���Y  ��  ��(  ��0  �艕  ��������������x[�C��tA���$���  ���  ���  �\$���T����v �   �������������ǅ�x��1�[��^_]Ít& �ǉ�1����������[^_]Ív �P�T��������������   �   �   �q�����&    �v �Fh�@��   ǆ�      �����t& ��(  �Fǆd      R��d  U���   ���  jpǆh     jjR��  �������  ǆ\  HM12ǆd  �  ��i�p  ����`  �)����t& ��,   �  ��  ��������  �������2  ����������&    f������U��W1�V�   S���  �@���D$    �D$�,$�   ��    ���  ��tn��H  ���  ���   ����   ���   ��(  ����0��j �����Y��X����  ��  ��u���   ���}  ����  ����   ������   �������  �<��  �F����^����$���  ��t����������S  ���  ��th��H  ���  ���   �$��H  ���9��������  ��  ���!����t& �T$�,$��t�   ��������D$��[^_]Ít& ��   �I����  ��&    f������VS���  �Å�tM���  ���  Ѓ�4���  ��~)��L  ���  ���������tj ���  �������X[^Ív ���  ��t
���  ��u����  u݋��  ��x  ��u��ː�����W��V�   S��<!  �)��&    f�i��  ����  �uN�����  ���tW���  ��u֋�4  ��t
���  ��u�@  uˋ�8  ��x  ��u�빍�&    ��1҃����  ��������u�[^_Ít& ��������H  �(  �
��t���   ���u���  9�u�������Í�&    ��    �����VS���t^���  ,&  ��&    f���9�D  u6���  ��t��[^Í�&    ��4  ��t
���  ��u���@  t�v ���  9�u�1ۉ�[^Í�&    ��8  ��x  ��u���f�f�f������VS�ËpX����  i��  �Ѝ��  ��*sE���  �[P9�t)�    ������   �����   ���  ���i  1�[^Í�&    f��SP���  ��u䋖<'  ��L  ��t*��\&  t!��l  ���T  ���T   1�[^Í�    ���  ��uۋ��  ��t
���  ��uǃ��  u����  ��x  ���o���먍v ����  ������Z�����    �����VS�Ë�8  ǀP  ������@  ��t��to���T  r�    ��  [^Ít& ���������@  ��u荆L  ����  r>��l  ����  sƃ��  �u�����  �[^������t& ���T  � t�띍t& ����  ������볍�&    f������UWV��S��\d�    �D$X���  ������   �l$�$1����F�~9�tE�X���    �Cd�@��t�H��t	���������CL�X�9�u܃|$<u�D$8�   D$�$��$  ;$t9�    ��  ���  �G�@������$��������ËG�@�������u�D$Xd+    u��\[^_]Ív �    t���  �������&    ��    �����UWVS���HT�XXi��  �$�<+���  ����  ���������  ���  ��+�  ����   ��.��   ��l  ��+�  �D$��L  �D$���  ��u#���  � t����  ��   ���  � ul�����������   ��N���4&  s8i<$�  �D$��;�  jh  S������T$������[^_]Ív ��N����[^_]Í�&    f�����  r��D$���������   �    �k����O  f��D$�������u�    �@����%  �    �d  ����  �t& �    �  i<$�  ��;�  ��u)���  �uQ���  �t���  ��tQ��&    �v ��&�������������(����v �����������    ����������D$1����������  �뚋D$1����������  ���    �:  ����  ��B�����&    �v �����UWVS�Á�   ���   ��$�   �T$,�L$0�D$`d�    ��$�   1��FX�D$8�FT�D$H�    %   =   �E  �D$8��(  ���������������ŉD$L���������udi|$H�  �[ �D$8��   ��8�  �|$d�\$ u
�~T��  iT$H�  T$8���  �L$0�D$D�    ��u3%   =   ��  ��$�   d+    ��  �D$L�Ĉ   [^_]Ë\$D��8&  ��u���  ���  iD$H�  �\$8�D$\    ���  ����  �\$8�t$d�D$$    iT$H�  ��3�  �|$@��3D  �|$��3�  ډ|$X��3  ��3l  �t$l�T$�|$h�|$D�t& �\$|��l  �D$p    �\$|���t$@��$�   �\$Xd�    �D$x    �T$t�7�t& �T$p�������d�    ���   ��  � ���  �D$���  ��uD�    ��  )��   ��   ���  � t���  ����   ��d1  9�h1  �  �T$�����������  ��T  ���  �D$ ���0  �ع   �T$p��������  ���:���������0�����&    ��    ��   ��������a�����&    f����  ��  �t$�L$��<  �\$�Ή��8�t& ���  ���$�������j �L$�T$������    ��    �����X�ډ��������u��t$�\$��d1  9�h1  ������v ����D1  �  ��X1  �D$�u�D$0+D$$�D$����  �D$D�$    D1  �D$<;t$u!��  ��&    �v �E�0�u;t$�.  �V�F9�s�$�L$$)Ћ|$�L$,�L$���  �|$�L$()ߋ^9�G���<'  ډ\$�T$1ۅ���   �D$���  ����   ��\&  ��   ��l1  9���   ��@&  �BɃ�&��F�L$7�:9���   �|$T�ˋ|$�l$P�Q�t& ��N�j�@�9�s9�z u3�zu-�L$78Ju#�L$(��@&  ����  �J��������  �9�v��1�)��������u���F�l$P1ۉD$�|$T��&    ����   �D$�   ��������T$�D$�����������   �D$~���  ��u�L$(��l1  9�t1���1  ��1  �U$<$�$��U$9D$�|  ���t  �E�X9X�U  ;U �/����$�E ;l$<��   �t& 9E$�  j�L$ ��D$D�����^�D$���  ���P  �$\$$�D$$9D$0��  �\$\����  �D$���  �����v �    �  �$�����E �����;l$<�x���9E$u��t$D��@1  ����Ǆ��0      1҉�@1  �E ��1  ��1  �U���f��u�u�����t& ��J�̓���t1ɀ|
��5�����9�u��J��Ā�D�����J���������|* �����|* ������|*������\$(�׋l$P+|$ǃ@&      ��@1  �����@0  ��|1  ����0  ǃ�1      ���1  ǃ`1      ��d1  ǃh1      �F�   F�D$������&    �v �J��������  ����W����J	�����0�G����D$(�D$7�ǀ@&     �B	�~�)�����&    f��E 1ۉE$;l$<�s���j �L$l��D$D������    �L$p�    �����_������v �$������&    ��$������&    �����p �]���\$���V����D$���  ����   �E9�t�E;E�#  ��������%����t& ��    �y  �|$$ �  �    �\$$%   �\$L=   �c  �\$L���H����؋\$`�S�7�����    �    ���  �L$$��u��D$L�����������    �$�����������������\$D��\&  ���ۉ\$\�;����J	����� ������d����    �\$L%   =   ������  �t& ��t$ ��8�  j �t$h�L$<�T$8������D$X���w�������������|$$ �    ������D$8�\$d���  �� �D$@���U  �������    ������    �T$$�    ��������D$L����������D$L�����������r  ������  �v �����U��WVS���$����  �B�<$���   �^T�~Xi��  ����  ���T$���T$u���  ��tti��  ���  ��   �    %   =   ��  ���L$�   ������L$����   ��L  � ����   �T$������E�����[^_]Í�    �At���(  �L$�ЉT$�������������D$�D$������L$�ɋL$��   �     �B����  ��&    f��~T�>����$���  ������É�������������4 ��un�D$tg���W�����&    ����A[��^_]Ív 1��������)���  ��t��t�$����������1��������&    f�������b�����    	��������&    f��    ��  �   �������&    f������UWV��S���   �OT�_Xi��  ڋ��  �������  ui��ud��u
���  �t���  ��[^_]�������&    ���(  ���������������ǉ��������u�     t���  ��    �  [�����^_]Í�&    f������U�h�WVS�É�s�������ǋC�9�tE��&    �C��P���Q�
�   �   �@�   �@�"  �@�   j ������C�Z9�u�[��^��_]�������&    ��    �����U��WVS�Ã��pT�xXi��  ���  �$�    �  i��  ����  �t9�    �b  �CT��t7���~   i��  ����  � tlǆ�  ������tt��[^_]Í�    ���  �t~���  ��us�    �(  1ҍ�l  �L$��������  �L$�uQ�CT냍�&    �v �$��L$������L$��u����  ��`��� �$��[^_]������v ���  �t��    �E  ��L  1҉L$������CT�L$�����t& �����S�Ë�8&  ��u�    ��  [Í�&    ������������  jPjh  S��������ōv �����VS�Ë�8&  ��u�    ��  [^Ít& ���������ƃ����  1Ҹd   �����jVjh  S�����j Vjh  S�������(먍t& ������U��WVS�����   �CT�sX�$�    ��  ��(  ��������{TtC��������������i$�  �KP9��  ��   ��������������1���[^_]Í�    ���   �������t�����������4&  ߋF�V�h�9�tC�\$�|$�׍v �Ed�@��t�X��t���Y  ���Y  ��������EL�h�9�uҋ|$�\$���������8&  ��!��������2����t& �1҉�������D���f����  ������ŋ��  �������	�PiD$�  ���  jh  V�������렐�����UWVS�Ã��������(  �ŋ�8  ��(  �D$�������������$���8  ��8  �     �  �   �\   ��  ������Ņ���   ��������}X��@  �ET��D&  �P��D&  �EP�艫�   ������}Tt�D$������$��[^_]Ð���   �������t܋�4&  � u
��8&  ��R���4&   ���w������G�X�9�t#��Cd�@��t�P��t��������CL�X�9�uމ��������������v�������������������������$�����U����$�����    �A����i  ������V��S�JX�ZT��  �p��  �@   �p�@   �zTt1��@MPEG�@   �P1�[^Ði��  ً�|  �P���  �P���  �P1�[^Í�&    �������BX�A����A�   �A�  �AGREY��0'  �Q��4'  �Q��,'  �A$    �A �A1��A(    �A,    Í�    �����W�AV��  S�ӋzX�Q9�O־   9�Lփ{Tu���    �t�Qۃ㠁�@  9�O�[9�L�^_�A1�Í�    �����UW��VS�rX�˃�\&  t��8&  ����   �F`����   �Pd�R����   �J����   �-   �S����   �m��t}�������ug��('  1�f�
ǆ\&     �WX�C����C�   �C�  �CGREY��0'  �K��4'  �K��,'  �C$    �S �S�C(    �C,    [^_]Í�&    ������끍�&    f�[�����^_]Í�    [�����^_]ø������&    ��&    ������S�ȉӋT$�KX��u*�yt���Ճ�6��yt���σ�;�B1�[Í�&    f������[Í�&    f�������9�RXu;�A����w0�A    �A    �A�  �zt���@  �A1�Í�&    ������f��������ȃ�wr���э��  ���  ��J�H�J�H�J�H�J�H�J�H�J�H�J�H�J �H �J$�H$�J(�H(�J,�H,�J0�H0�J4�H4�J8�R<�H8�P<1��f������Í�&    �v ������BX���Y  �1�Í�&    ��    �����WVS��rX��u9�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u�1�[^_ø��������&    ��&    �������BX���Y  ���Y  �Q�1�Ít& �����WVS��rX��u9�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u�1�[^_ø��������&    ��&    ������VS�|$fd@u]�D$� ��uM�rX�F���X�9�t4�t& ����   @t�Cd� ��t�H��t�   ��������CL�X�9�u�1�[^Ít& �u��������썴&    �t& �����S�ˋJX��    ����tMw��t/����  �c1�[Í�&    ��uC����  �C    1�[Å���  �C    1���t& �����  �C    1��Ӎt& �����  �����[Ð������BX��Hw9�s���Y  �����1�ø����Í�&    �v �����WVS�ZX�Sh�Cv�|�9���   �    �΋��Y  ��9�t~����  ���Y  �D�1҉��Y  �?���� u���Y  ���Y  ���  ���  ��x  ���  ���  ��t  ����������������������������1�[^_Í�&    ���  [^_Ít& ������ލ�&    f������UWVS��zX��uX����������    �(  �G�o�X�9�t'�v �Cd�@��t�H��t	���������CL�X�9�u܉������1�[^_]ø������&    ��&    ������UWVS�D$�L$�rX���� �  ���Y  ���Y  1�1�	���   ��4&  �� ��   ��8&  ����   ������Y  ��  ���Y  �Vt���Fu������~t��  �f��  ��f��@�~t�f��  ������~t҉�,'  �����~t���0'  ���>  ��4'  �    �A  �F�~�X�9�t0f��Cd�@��t�h��t���Y  ���Y  ��������CL�X�9�u�1�[^_]ø������������퍴&    ��    ���������@X�������&    ��&    ������BX���Y  ����������&    ����������@X�������&    ��&    �����WVS��rX��ul�F���ύX�9�t$�Cd�@��t�H��t	����������CL�X�9�u܃$�Gt�   �  �����1�[^_Í�    �   ��  �����1�[^_ø�����ڍ�&    �t& ������WV��S���zX��    ������   w*����   ���]  �Q�����Q�����1���[^_Ð��u[���x  �A    ��8&  ����   ���4&  ��   �����������)  Pjh  W������������1���뛍v ����  �������[^_Ít& ����  �A    ����[^_������t& ����  �A    ��8&  ��tH���4&  r4��������Ã���C  �������Sjh  W�����1��������v ��1�[^_ø�����������&    ��    �����WVS���   �JT�zXi��  ���  ���Ã����É���u#��u���  ���    [E�^_������t& �[�����^_Í�&    ��&    ������WVS���   �KT�{Xi��  ���  ��������uG��uB�    ��t  ���  �����iST�  �KX1����  u���  [^_�������    [�����^_Í�&    ��&    ������W��VS���   �YT�yXi��  ���  ��t��u+�H 1���   ��ui��  ���  [^_�������    [�����^_Í�&    ��&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������V��S���   �XX�@Ti��  ���  ��t��u$1���ui��  ���  ��[^�������&    �����[^Í�&    ������UWVS�ˋrX�   �  ��������~l�    ������������  �k���� t���� ��  ����������V���   ��u���   P�C0h  j P������Fx��   ��CT[1�^_]Ð�����S�\$��tR�É�f%Dt<��~S��ug��b��uk��us��1����&    f����� t(��s�   ��[Í�&    �v ��f% t�J���v1�[Ít& ��u��   [Ít& ��u��~�1��܃�u���[f% @ø   [Í�    �����U��WVS�� �BX���  ��L  �D$�\$���W  �\$���  �@���  �$)к   ��N����  ������E     �E    �E    �⍃�  �E    �D$���  �E    ���D$�U��    �T$�D$������D$���e  �\$���  ��  �@�D$����  �D$���D$9D$u$�  ��D$�@� �\$�D$�C9D$�4  �D$�X�x9�sԋM �t$)��$    �V�����Dڃ�wl�v��&    �J�Z�����$�X1ۉ�r��J���	މH�p�J�H�J��@    ���@    ��`  �H�E �H�ȉM ���D��v��?v��D$�X�4$�G����C�����|$�_�|$G$�G$�} ?��wc�G�W �X9XrU9��������&    �j �L$ �T$�D$������} ?X������L$�    �    ��   �����1��� [^_]ËK$�D$;H s�j�L$�D$�����Z뼋D$�X�\$�X�d����\$�C �C$�|������  ��t���  ��������\$��������  u����  ��x  ���k����s����D$�H$�D$;H �%����n���������BX�A����A�   �A�  �AGREY��0'  �Q��4'  �Q��,'  �A$    �A �A1��A(    �A,    Í�    �����UWVS���zX�t���������D  �yd��   �$    1�f���1���v �<$u��)f�tC��f	1��t(�Wt��t��؍h���w��t�1�f�TC����u؃�0�<$t�$   뤍v �Ah    1��Al    �Ap    ��[^_]ø������f������U1�W�y
V�����S�Y�jX�Q�Al    �Ap    �Ah 	  �A    ��)��C\    ��`���1�f�F�E`��tm�Hd�I��tc�I��t\�=   ��t���t���������t& ����������u2�V61���&    �f��9�u��Vf�v f��9�u�f�F[��^_]ù������t& �����UWV��S���BX�Y�Ah 	  �Al    �$�Ap    f����   �$1�1��ht��&    �L~U��1��)���Zf�D~��	Ã�u�1҉4$�~0�֍�&    �LwU��   �����Zf�Dw��	Ã�uދ4$f��t,�N�F61ҍ�    f��9�u��Nf�v f��9�u�f�V��1�[^_]Ë$1�1��xtf�N��&    f�W���1�����Zf�Dn����u�1�t& W�ى�   �a���Yf�Dn6����u�������&    ��&    �����UW��VS�ZX������\&  ��   �C`����   �Pd�R����   �J���|   �   �w��tG�j��t@���������u-ǃ\&     ��('  �W�y��Vl����Ql)�)΃�p���[^_]Ív �������뾍�&    ��8&  ���j���������Ӎv [�����^_]Í�    [�����^_]Í�    �����U�   WV��S�˹   ��h�RXd�    �D$d1��|$�D$   �{��  �FT9ω$�CO��   9�L��<$u���    �zt�{Ƀ᠁�@  9�O�9�LŉC��  9��=  ��8&  ����  �K��HM12�   i�  �D$�  �l$i,$�  �D$�|$�D$   Չ�|  �L$���  �L$���  f��  �B`f��  ��t6�Pd�R��t,�z��t%�   �L$���  �j����   1��������FX�VT��  �K��  �C   �K�C   �~T��   �CMPEG�C   �C    �D$    �D$dd+    ��   �D$��h[^_]Ít& i�p  �D$�  ���l$�������&    ��  9������i,$�  �K�D$    9�*|  ������i��  �D$    Ћ�|  �S���  �S���  �C�_���1�����������D$�����F��������f��������=   t2 �   ��t+1���   �   D�Ít& �1��� @  ����Ð�   Í�&    �v �����U��WV��S1��8f�1ۍ�&    f�U����1�����Yf�D^����u�1ۍt& U�ډ��   �q���Zf�D^2����u�[^_]Í�&    ��&    �����S���P�X21�f��9�u���b�v f��9�u�[Ít& �����ǀ  �  ������W�   S�Ã�\d�    �D$X1��|$�$   ��C�D$   ������D��D$�C
�D$��P�����t+�Pd�R��t!�Z��t�   ���t&�z��t1�������D$Xd+    u��\1�[_Ív 1���������������&    �v �������  1��f������WV��S���   �H|��($  ���  ��uZ�A����t��uN��0.  ����   ǆ,%     �    ��   ��%  �����f��t9��&    f�1�[^_Í�&    �v ǆ,%      �    t���  ��&    ���e�����%  �f% 0f f���d�����%  ������f�1���t& ���� �\����,   �8  ��  ��������0.  ��uՍ{����  f����0.  �����Ǆ�0.      ��s��  ��&    ������0����:  ��&    f��������wVW��V�� ���S�� ����Z�9�t.��    �Cd�@��t���t��|  ��������CL�X�9�u�[1�^_Í�&    �v 1��f�f�f�f�f�f�������U��WVS��~A�ƍ��ˍ<Bύt& ��6��h"  jS���������9�u�� ��[^_]Í�&    �v �ω�� [^_]Ít& �����UW��VS1ہ�   �D$��$�   �L$�D$d�    ��$�   1��  ��t& ���[���  ����  9�u�d�    �D$0�D$8�D$8�D$<�    �D$,    �D$4    �t!��  ��  ��t%  =  �  �[���  ���w  �D$�D$ �   ���1  �4$���   ���   �t$ 	  �D$�D$������t$�.�l$�l$��������v�@9���  �L$�ȃ������~  ���L$�D$�P �
   �t& ��:�
9�t��u�D$1�P$�D$��~A�\$(�|$�t$$�t$f����
   ��&    ��
�9�t��u����9�uًt$$�\$(�D$1ɍx<�
   �v ����t��u�T$�L$�
   ��    �
�*9�t��u�T$�
   ��&    �v ��9�t��u�[���  ��<���
��
������š    %�  =�  �R  �$�   �T$,������D$�
   �t$�t$ ��X&  �� 1� ��1����t!�9�t��u�t$�    �9D$�  �    )ЍT$,�D$�$������    ;l$��   %  =  �	  �t$1��N$�t$�v �������u��7�D$������[���  ��  1���t�    ��  �������$�   d+    ��  �Ĝ   [^_]�<�C  �D$�D$    ���1  �4$���   ���   �t$   �D$�|�����J����D$�����Pk����  �D$��Ph  ������������t& ���T$������    �T$)ЉD$�6�$�T$,�����9t$������D$�����9l$��   �    ��   ����������t& 1ҍD$@�
   �������&    ��$�   �T$@������9L$����uR���=  �$�T$@�   ������    ������
   )������P�D$��Ph�  ������������t& �����   ����������z����D$    �D$   �t���1Ҹ,  ������0����    u<������5���k��D$�����P���  �D$��Ph4  ����������������k��D$�����V�t$P���  �D$��Ph�  ������������������T$�D$�L$U�G���PWk����  �D$��Ph  ��������+����y  �$�T$@�L$$������L$$9L$uZ��
�p��������T$�D$�L$U�����Ph  ������k����  �D$�����VP�D$��Ph�  �������������
   �f�����  �L$��9�u���  d�    �D$0�D$8�D$8�D$<�D$,    �D$4    W�D$��Ph*  ���������������������W��VS�B�Ӆ���   ���   �   �  �S �09�uP�p�@9�tF�
   ���9�t��u�X&  �
   ��@1� f������t!�9�t��u�[^_Í�&    ��    u.�K[^_Í�&    ��u���   �   @  �u�����  ��  �t& �����U��WVS��,�@�D$�E����  ��t�E    ��,[^_]Ít& �E@=  u>�]p�    �@  �.   ��������|$��4&  ��u���t�9�s��"  ��    = �8  �uD�D$��������Å��^  �T$�EL1����D$�T$���  �T$���J  �,$�5f����  j �L$����������@  Y�  ��;|$�  �$�t�`�@t��L  9��  �p  9��  �$��L�d��������š    %  =  ��  ���K  ��@  ���  ���]����D$��0&  �D$���W  �E ��t]�E�u�E9�t	9u��  9�tA�l$�Չ\$�\$�|$��V��,&  ��0&  ������69�t�N��uߋ\$�l$�|$j ���  ��؃������X;|$������t& �,$�    ���  �    ������D$�   �   j �1  �������P  �Z�����j ��\  �   �   �����X����f���4  ��tZ���  ��tP�M ��tI�U�u�ыU9�t	9u��  9�t-�l$������4  �V�  ������69�t�N��u��l$j �ꍋ�  �������Z�����t& ��E ����   ���  �D$��������  ���  9���   ���  �F؉t$������D$����   �VD���u�M9�tS�\$ �Ӊl$$�͉|$(���%��;G$w�D$�V�������FGDË69�t�N��uՋD$�\$ �l$$�|$(�PD;��  sj�    ���  ���  ������D$�����j ���  ��������^�����v j �L$��������X�����v �D$������������&    �v �t$�FD    ������   �F�F�V��B��F�   �   �F"  �F�   j �����X�H���f��N���i����V�  ������W����v �N���b����D$�V�t$��,&  ������G�����  �t& ������U��W��VS��Td�    �D$P1�����  ����  ���   @  �D$1�1ۍ��1  ���&    �����t��F�J  �1���u荌7�1  �    �$���)  k�t�L$�\$�� �ǀ�1      �� 2  1����$�T@����u�\$�T$��$�A �B�Q �A$9��  k�t��� 2  ���j  ����   ��$2  =  ��   = �S  �,$�EL�MH�P����  �}D���   ����7D2  �����,�   1���&    f���T&  ����9�r�k�t���1  �  �D$Pd+    �  ���1  �$��T�    [^_]�������&    ��T2  � ��,2  ����   ���1  tm��&    ��D$Pd+    ��   ��T[^_]Í�&    �    ��  k�tǄ�1     ��������   �  �D$�����t& k�t���1  u��D$Pd+    ub�$��T��[^_]�=����t& ����������T&  �ع�   �������ƃ�    �  � ����������1  ������$������������������GPh$  �����XZ����f�������f�����    �����WVS�L$�|$��~�D$$�����v �p������r�9�u�W�T$�D$�"���Z[^_Í�&    ��    �����UWVS��d�    �D$1��\$0�t$4�L$8���  ����  �T$<���~"��1��却&    �v �z����|� ��9�|�U�������Z�T$d+    u��[^_]��������&    �v �����V��S��8  �   ��  ��  u1�����RPj��D  jh	  S������   ����  u��  1҅���t[^Í�&    �v ��  Rj ��D  jh	  S���������u�j ��  ��Pj��D  jh	  S�������[^Í�    �����S�L$��8  ���   ��  ��   ���   ��  wp���   �  v2���   ��  �q�1��D  jh  S�������[Í�&    f����   ��   �1j j j j ��D  jh  S�������$[�f����   ��  ���   ��   �q�q�q�1��D  jh  S������� [Ít& ����   ��  ��   ���   ��  ���   �~   �q�q�1��D  jh  S�������[Í�    ���   ��  �1�q��D  jh  S�������[Í�&    ���   �x  ��I����	ʉ�  [�����v ���   �Z  �1��D  jh  S�������[�f����   �  v���   �  1�[Á��   u�1��D  jh  S�������[Á��   t��*  �v �q�1��D  jh  S�������[Ív �1��D  jh  S�������[Í�    �q�q�q�1��D  jh  S������� [Ít& ����  �Q��  [�������1��D  jh  S�������[Í�    �1��D  jh  S�������[Í�    �1��D  jh  S�������[��*  �*  �*  �*  �*  f�f�f�f�f�f�������UWVS��<�$�T$d�    �D$81�����  �z�Ѓ��|$�D$ 9��p  �D$�h���a  �x�|$������$��\&  �W  �D$�D$    �8?���u	�@ȉD$�$�xu�D$����X�E��  ���E  �   ��t& ����9��
  �;�u�{ u�{ u��K�῀��u�)�9���  �v 1��D$    ��1����l$�ō�&    f��{���  �{� ��   �{� ��   �C���<���   �$�\$,�@`��tE�Pd�R��t;�
��t5�   ���K  �   ��T$���7  �L$�T$(�������    �T$4����   �L$����$��@'  �T$(��D'  �T$0��H'  �$������P'  �T$,�
��J�H�J�H�J�H�J�H�J�H�J�H�J�H�J �H �J$�H$�R(f�P(�D$�t& ���9�������D$�<$�l$��@'  �D$����   �L$�D$�����H�D$������$��<'  ����  �$��8'  �D$���<$�L$��8'  �|$�GA )�A ��D$;D$ ������D$8d+    �8  ��<[^_]Í�&    �   9������$��@'  �<$�D$�t$Ǉ@'      ǇH'      ǇD'      �@@   ���C�F�C�F�C�F�C�F�C�F�C�F�C�F�C �F �C$�F$�C(�F(�C,�F,�C0�F0�C4�F4�C8�F8�C<�F<��<'  ��������<$�D$    �D$    ��8'  �D$   ���D$$���@0  1��l$��1����p�L$���L$���������k�+���T,��D-��S�P�S�P�S�P�S�P�S�P�S�P�S�P�S �P �S$�P$�S(f�P(����@9|$~8�S���t�K��{� �Au�A����q����L$�� ���L$�i�����    ���  ��l$�FD f�F$��F�����F  ��F ���F! ]�Fc�����$�  �����T$�|$�ˍ��DA������؃�
�K
�� ���F itv0�V$�~(�|$�F�N��������!�F�����F�������F�����F���|$$�D �F�$����0  �?����<$�D$    �D$    ��8'  ���D$$���@0  �D$���I����  ��,   1�1��FD f�   1��F$��F�����F  ��F ���F! ]�Fc����%����T$(������������,'  �T$1ۉ|$��B�|$��   ���l$���=�t& ���  �z�ƃ�����  �с   )�)΁��  �����  ;\$th�x��ub�x� u\�x� uV�H��῀� uJ��
9�u����  �z�ƃ�����  �с  )�)΁��  ������  i��
�J�J늋l$iD$@  �|$�t$�G�<$��8'  �T���8'  �D$�������  �V,�F$�0  �F ITV0������   �   �����������������f������UWV��S����4&  �Vh� �1  �Bp�$�Fd��tD�Pd�R��t:�Z��t3�<$�   �W���n  �y���c  j 1������Y��&    f��F�n1ҍX�9�u�_��&    �CL�X�9�tN�Fh�@��t#��   t%�Cd�@��t�x��t�$1ɋPj ���������X��t�������t�����[^_]Ð��X&  �� � �$�߃�ϋ@����   ��td���� 9�tg�Ë�X&  �ߺ
   ��   �� � ��&    ��9����t1ب0t��u��1�[��^_]Ë��Y  �@�D�L�$������v ����9�u��� ��X&  ���
   ��   �� � �v �)����t
1��0�l�����u��b����v 9�t����T����t& �j 1������Z����������WVS�X`��t9�Sd�R��t/�2��t)�Hh�=   ���Y  �T� ��t�?��tj 1ɉ������Z[^_Ít& j ��1������X[^_������UWVS�Ӄ���X&  ��1� ��X&  #��1  ��D1� ��X&  #��1  �0� � #��1  ��u7��	��$��uc����   �,$���Z  �$1�������[^_]Í�&    ��X&  ��1� �
   ��&    �u �M ���t��t���u��	��$��t���X&  ��D1� �
   ��&    ��} �M ���t��t��u��t/��X&  �
   ���0� ��&    �E �M ���t��t��u�	$�    ��  ���  �C  ��t��   uu��   u]��������   u-��   ����j �   ���1  �   �����Y������t& j ���1  �   �   �����^빍�    1҉������똍t& ��   ��������z����    ��  ���  �Z����C  f���������Ћ�4  ��u}V���   S��X&  ��� ���    ���  EӃH(p�P<��X&  ��� �ڍs��������҃����   �   D։փ���E։P4��L  �P81�[^Í�&    �1�Í�&    ��    �����ǀ4     1�Í�&    ��&    �����UWVS�����   ��X&  �� � ��X&  ��� ��X&  � � �H  �    �G  D����  ��   ���$PSh    ��������ԃ��4��  ShL   ������������\  �"  ��&    ��    �����V���S��X&  ��� �
   ��2���t��uX&  ���+ ��� �
   ��&    f��2�����+ t��uX&  ��'R ��� �
   ��&    f��2����'R t��uX&  ��$� ��X&  ����� �
   ��   V��$� ��    ��9�t��u�[^������VS�����   �    ��#  ��X&  � ��tl���X&  0��Ό��� �
   �t& ��19�t��u�X&  ��� ��X&  ���� ��
   ��   ��� f���9�t��u��1�[^Ív ���X&  0퍐� �
   �v �
�29�t��u�X&  ��� ��X&  ���� ��
   ��� �
�9�t���u��1�[^Í�&    �t& �����UWVS�����   �q�D$��8  �ŉ$�� �  ����  �q�ʃ���  �A   �A   ��X&  ��t� ����X&  ���  ��t� ����X&  ��	���p� ��X&  �����  p� � ����	��z��<$�   D��z4$����9��$  9��$  �,�    9��$  9��$  �����  ��1�����  ������	����   �D$�  +D$%�  �D$���  �w  �    �'$  ��X&  � � ��0���	�X&  ��� �
   �v ��99�t��u�X&  ��� ��0�% �  	�X&  ��� �
   �v ��99�t��u�X&  ��� ����� �%  � 	�X&  �Ս�� �
   �t& ��*�
9�t��u�X&  ��� �D$��X&  0��؍�� ��	к
   ���99�t��u�X&  ��� �D$0�0�	�X&  ��� �
   �t& ��99�t��u�X&  ��� ��X&  ���� ��
   	��� ��    ��9�t��u�1���[^_]Í�&    f��  	 ���   �x�����a���   ��  
 �b���������������뺍�&    �t& �����WVS�P�JԋP(��	� �  wa�� 	� ��   ��	� ��   �@|��X&  ���� ��X&  ��0���	�� �
   �t& ���9�t��u�[1�^_Ít& ���	� uH�@|��X&  �� � ��X&  ������ �%  � 	Ё� � �
   ��&    ���9�t���u�릐[�����^_Í�&    �@|��X&  ������ ��X&  ��0�	¸
   ��� ��    ��9��Y�����u��O�����&    �v �x|��X&  �?�� � ��X&  ��0�	Ӻ
   �� � �t& ���>9�t��u�@|��X&  ��� � ��X&  ��0���	� � �
   ��&    ��9��������u�������&    �v �����UWV�   S�����   �$���������X&  �
   �� � ��    �1����t%   =   t��u拃X&  ��  � ��X&  ���
   ������  ��  � �t& �9����t	1���  t��u鋃X&  �
   ��  � ��&    ��1����t��9�t��u닃X&  X� � ��X&  %����
   ��X� ��&    ���19�t��u�X&    �
   ��X� �t& ���19�t��u�X&  \� � ��X&  %����
   ��\� ��&    ���19�t��u�X&    �
   ��\� �t& ���19�t��u�X&  �
 ��4� �
   �t& ��2�
��
 t��uX&  ��<� ��X&  �����
   ��� ����<� ��f��9����t	%� 9�t��u鋃X&  ���
   ��� �����<� �t& ��9����t	%� 9�t��u鋃X&  ��� ��X&  ��  ��� �
   ��    �
�29�t��u�X&  ��� ��X&  ����� �
   �v �
�29�t��u�X&  � �  ���� �
   �t& ��2�
�� �  t��uX&  1����� �
   �t& �2�
��t��u�X&  �� � ��X&  ��������   �� � �
   f���19�t��u���l�����X&  �� � ��X&  �������� �� � �
   �v ��19�t��u�X&  ���� ��X&  ��  P ���� �
   �
�29�t��u�X&  ���� ��X&  0��� ���� �
   ���19�t��u�X&  �.%@��� �
   �t& ��2�
��.%@t��uX&  ��� ��X&  ��   ���]��� �
   ���19�t��u�X&  �(f��x� �
   �t& ��2�
��(ft��uX&  �� � ��<���   ��vJ��   )�������	�É����<$��0  �JD�ZH��0  ���   �����   [^_]�������&    ��X&  ���� ��X&  0Ҹ
   ������ ��    ��9�t��u� �  1۸ �  뉍t& ���X&  ���� ��X&  0Ҹ
   ������ ��    ��9�t��u� .  1۸ .  �>���f����������1�Ív �������T  ��t	1�Í�    ǀT     ����1�Í�    �����WV��S�ÉЁ���  ����  � ��    ��X&  Ћ8��   �X&  ����!ǉ�������	����
   ��&    �
�9�t��u�[1�^_Ít& ������UW�׉�V��S���D$�\$�$����%��  ��    ��X&    � ǋ/��   �����X&  ����
   ����!�	��<$��v �����t1���t��u��1�[^_]Í�&    �v �����WVS��4  ��u^�Ë��   �֋�X&  ��� �R8�������tRw(��uD��h�   �	  Q������F8��L  XZ��[^_Ã�t��u#���͐1�[^��_Í�&    ���뷍v ��믿�����ƍ�&    �t& ������UWVS���  �Ë��  ��X&  ��% �  ����� %   �   ������ 	�X&  ���� �
   �t& ��)9�t��u�X&  �� � ���  �؁�   �������X&  �� � ���  �؃�`���������X&  �� �  ��   ��� �� 	���  �� �  	��  �0��X&  �� ������   ��� �
   ��&    ��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   t��u拃X&  ���    ���uj��  � �   � � �����   u[^_]Í�    �΁��   j�  Q�����������غ  ��j��Q�������[^_]Í�&    ��� ��X&  ���   �
   ��� �v �1����t
��=�   t��u苃X&  ��� ��X&  ����� �
   ��   ��� ��&    �1����t%   ?=   ������u⋃X&  ������&    �0��X&  �� ������   ��� �
   ��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   �q�����u��g����t& ��0��X&  �� ������   ��� �
   ��&    f��1����t���t��u틃X&  ��� ��X&  ����� �
   ��   ��� �t& �1����t%   ?=   �������u�������t& ���� ��X&  ���   �
   ��� �v �1����t
��=�   t��u苃X&  ��� ��X&  ����� �
   ��   ��� ��&    �1����t%   ?=   �Q�����u��G�����&    ��    ��������   �����1�Í�&    ��    �����UW��V��S�Ã�d�    �D$1��$    �D$    �D$    �    �K$  �G�����  �$   �O�1���
  �7  ���.  ���  �D$   ��σ���&    ��h�   �  ��Q�������X&  �� � ���  �؃��	����������X&  �� � XZ���l$����  �D$���  ����j�  ��Q�������X&  � � ]%   �Y�l$��P��t�L$��t0P  ��1ɽ   ����t2��  ���\  ����  ��t�� ����  ������uθ@  ��	�u߀����������f$  ��&    f���u�D$   ��?������v ���
  ��?�D$   ��@�����t& ���  ��   �������� �  ��   %  � ��� ���$  ��   �� p  ��$  ��   ��   ��$  -  p =   ��$  �����$   �   	����D$   �D$   	������������D$   �� �����������i����t& �����%�   ��   �������$  ����p��$  �� �����   ��$  ���$   �H����  ��   ��   ��?�   �D$   ��	��X�����    ����	��U�����X&  ��� �
   ��&    ���)9�t��u��  �؉��  ��������i���1��T$d+    u`��[^_]�f���������&    �������D$   	�   �������    �D$   ������������	�������������������f$  �������D  ���   ������&    �v ������ы�@  ���   �������&    ������S��X&  �ҍ�  � �
   �t& ��
�9�t��u�1�[Í�&    ��&    ������U��WVS�˃���X&  �|$�t$�$
   ��  � !�������ō�&    f������u���u!�9�t�,$u��1�[^_]Í�&    �t& �������X&  �ҍ�  � �1�Í�    ��������Ћ�X&  %��  ��  � � ����    ��Í�    �������X&  �ҍ�  � � Í�&    ������U����W��VS�É�t$��X&  %��  ��  � � ����    �����[!�	�^_��]�����t& �����S�ҋ�X&  ��  � Ӌ!��X&  L$�
   ��    �
�9�t��u�1�[Í�&    ��&    ������UWVS�Ã�\���  ���  ��X&  ����O���� 	���   �
��X&  ����� �
   ��   �ǜ� ��    ��/9�t��u�ǁ��� �|$(��   ����  �D$�   � �  �<�
 �D$   ]�D$$&   �D$ 0  �D$  $ �D$   0�D$  � �D$    �D$ <   �$  
 �D$H    �D$D   �D$@   �D$<0   �D$8C  �D$4&   �D$0]   �D$,�   �8  �v �
��X&  ����� �
   ��   �ǜ� ���/9�������u�ǁ��� �|$(������	���  ���D$z   	��D$$   ���D$   ��D$   ��   �D$   &��   Z���D$  P �|$���   ��$   ��   f���D$D   �|$���   ���D$@   �|$ �� ����D$<&   �� |  ���D$8�  ��D$4   ���D$,z   ��f��ɉ|$H������� | �������Z�D$0��X&  ��� ��X&  �ҋ�� ��X&  ������ � ���    �%  ��tO������ǉօ�t@�D$L    �𺹈b���T$P��	i�  )Ɖ�t$P��	���  ����t$L����$  ��X&  ��p� ��X&  0�T$��p� �
   ��&    ��19�t��u�X&  ��p� ��X&  0퍐p� �
   �t& �
�29�t��u�X&  ��p� ��X&  ���� ���  - ��p� �
   ��&    �v ��19�t��u�X&  ��p� ��X&  ����� T$��p� �
   �t& ��19�t��u�X&  ��t� ��X&  0�T$$��t� �
   ��&    ���19�t��u�X&  ��t� ��X&  0�T$��t� �
   ��19�t��u�X&  ��t� ��X&  ���� �T$��t� �
   �t& ��19�t��u�X&  ��t� ��X&  ����� T$��t� �
   �t& ��19�t��u�X&  ��x� ��X&  0҃���x� �
   ���19�t��u�X&  ��x� ��X&  0�����x� �
   ���19�t��u�X&  ��x� ��X&  ���� �T$��x� �
   �t& ��19�t��u�X&  ��x� ��X&  ����� T$��x� �
   �t& ��19�t��u�X&  ��|� ��X&  0�T$ ��|� �
   ��19�t��u�X&  ��|� ��X&  0�	ꍈ|� �
   f���19�t��u�X&  ��|� ��X&  ���� �$��|� �
   �t& ���19�t��u�|$(��҃������  ������ɉ�  ��X&  ��   ��|� ����� �
   	ʋ�X&  ��|� ��&    f���9�t��u��\[^_]Ív ���D$�   	��D$   ]���D$$&   ��D$ 0  ��   �D$  $ ��  � ���D$   0�|$���    ���D$@   �|$���L�D$<0   ��:���D$8C  �|$ �� ;  �D$4&   �� B  ���D$0]   ��D$,�   ��  ����  
 ���<$�1Ƀ� �����|$H�L$DɁ�L;����:B
 ������D$z   � �  �<�
 �D$   Z�D$$   �D$   �D$   �D$   &�D$  � �D$    �D$ <   �$  
 �D$H    �D$D   �D$@   �D$<&   �D$8�  �D$4   �D$0Z   �D$,z   �W���f������WVS��4  ���   ��u��<  ��8  1�1�	��   �։�8  ��    ��<  ǀ4      	���   �ց� @  	�tf�ց�   	��>  �ց�   	���   �ց�   	��.  �ց�   	��>  �ư�%  ���=  ��  � ��҉׃��	�t& �   �    �;&  1���X&  �� � ��к   ��	��� ���������X&  �� � ���  �؃��	����������������������[1�^_Í�&    �v �   농�&    f�1��   �    �Y&  ��X&  �� � ���   �   �؃��������X&  ��x� ���{  �؁��   ������1����t& ��   �   뛍t& 1��   뎍�&    �   �������    �   �������    1��   �[����t& �����UWVS�Ë�X&   � � %��  ���   ǃ�     ��  ��   �荳p  ǃ�     ǃ�  ��  ǃ�     ǃ      ǃ     �������   ��D  ��P�CP��   h�  j P�����1ɺ	   ��j ǃ@     ������ 	� �0  ��j h�   j jj h�   j j ������	� �0  ����8j j@j jj jj j ������	� �0  ���� j j@j jj jj j ������	� �0  ���� j j j jj jj�j�������	� �    ���� j j j h�  j h��  j j ������		� �    �� ���  ��j j j jj jj j ������	� �    ���� j h �  j h�  j h��  j j ������	� �    ���� j h �  j h�  j h��  j j ������	� �    ���� j h �  j h�  j h��  j j ��������  ��  �� ��u�C��������ǅ�t���������[^_]Í�&    f����������[^_]�f������V�� }  ����D�  S������t����  ��  ���  v2��D�  �  ����  �r  �� }  ��   ���  1�[^Ív ��D�  �$  ����  ��  �� }  uҹ0�  ����������+ �  ��������tv�  ��������  ��  ��������  � 	  ��������  �	  ��������  �	  ��������p   �'  ��������� �   ��    � �  ����������+ �  ��������tv�  ���������� 	  ����������	  ����������	  ��������`   �'  ���������/ �,  ���������.��(  �����������f���  ����������+ �  ���������'R �  ���������O� 	  ���������O�	  ���������O�	  ��������V   �'  ���������_ �,  ������������(  ������������f���  ����������+ �  ���������b �  ��������Ym� 	  ��������Ym�	  ��������Ym�	  ��������X   �'  ���������� �,  ���������K��(  ��������\���f�� �  ����������+ �  ��������tv�  �������� ���  ��������UU� 	  ��������UU�	  ��������UU�	  ��������`   �'  ���������? ������    �$�  ����������+ �  ���������b �  ���������`��  ���������s� 	  ���������s�	  ���������s�	  ��������d   �'  ���������a ����������.�����&    �t& ������VS�P�ZԋP(��	� ��  vR��	� �N  ��		� ��  ���  �p|��   1����������  ��1��P�������X��[^Í�&    ��	� ��   ��	� ��  �@|����=��  ~9h�   �   �؃���  �����V��  ��1�����������X��Z[^Ít& �j �   ��  ��������������)���P뿍�    �  �����������   ��j�  �؁��   Q������ع   ��  �����[^1��'����v �@|��   ����	=�/  ~��   )���ɉ�1���  �������[^Ít& ��@|�� �1��@�����������)Ѻ�  �A0�������P���������Y[^Ít& �@|�� �1��@�����������)Ѻ�  �A0�������P���������[[^Ít& ��j�  �ȉ�Q�����ZY�P���������F���f������S�  ��������  ��j�؁��   Q������  ��������  ��j�ȉ�Q������   ��  ������������  vy�p8��  ����������  �������  ��������  ��j�؁��   Q��������  YXw[Ív �  ��������  ��j�ȉ�Q�����XZ[Ív ���  �������녍�&    ��    �����V��S���   ���  v6�  ��������  ��j�؁��   Q������   ��  �������YX�  ��������  ��j�ȉ�Q���������(����  �Ɖ�������  ��j�؁��   Q����������  v#�  ��������  ��j�ȉ�Q�����XZ��[^�f�f�f�f�f�f�f�������U�c  WV��S��   ��d�    �D$1��F�D$    ���   �D$������D$�|$����(  �D$   j��   �   ��h0�  �������   �  ��h�   h�   ������   � �  ��j�h0�  ������   �   ��������D$,�x� �|$�D$������   1���&    �$1��8��	���   ��f��   ���������Ƨ  ������   �������9�tH1���?u����
uɃl$�%���hc  �t$h'  ������D$ ��������D$������'  f���;|$�k����D$���v 1�h   ��   �   ��h   ������t$hc  �t$h4'  ������   ���\$,�������%��� �      ����������   ���������   ���   ủ©�?  t�v �   ���������?  u�{����u&  ���?��   9�u:1����&    ��/��?��	��   9�u�   ���������9�u��u&  ��'  ����'  f�f������U�JW�z���V)�S���   1���`�������  �B    �B^    �1�f��  ��������tU�� �  �$  uT�t& ��������������� `  ����`  f��{����f��{������	�f	��5  u�[1�^_]Ít& ���������������� `  ����`  f��{����f��{������	�f	��0  u�뮍�&    �v �����S���   ���������  �  ��������ع.   �  �����1�[Ít& ������U��WVS�����   d�    �T$1҉��\$���  �� �  �t& ��  ��9�u���������2   �  ��������   �  ���������  �  ��������E����  ���t& 1�1Ƀ�f�P.f�H�9�u�D$���&    f�� u������9�tQ�U0f�� �  ��   f����   f�� u�	�U f�� ��   ��   f��u������9�u��$  ����   ��    �������ډ����������1  u��1ɉ����������5  u�   �<  ��������   �t  ��������D$d+    ��   ��1�[^_]Í�&    f�f�� @�����@�����t& �f�� @�*����U �f�� �'����v �`������������&    ���������&    ��������ډ����������5  u�   �<  ��������&   �9����U��    1Ƀ�f�H.1�f�H�9�u��E^    ���E    �:��������f������U����WVS���R���   �
��u�z�t�@    �@    ��1�[^_]�f��z�u��Z�\$��Ut���u��Z��  ���|$�z���Z�<$�\$�����   wJ����   ����   � @  �P�T$1Ƀ�?T$�|$U���<$�P��x��1�[^_]Í�    1���	uY�D$�$�\2�|2	��`  ��`  ����	���	��	������(��uˁ��   �D$�   �u�����&    ��@    1�1��u����   �S�����    �Z����1ˉ���1ى���1ك�t��J	����1ʉ���1ʉ���1ʃ�t��   �	���f�f�f�f�f�f������V�  � �   S����������   ��1�h   ��������   ���   X�
   f��2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ���   �
   �t& �2�
��t��u򋃄   �   ��  �
   �t& �2�
��t��u򋃄   �   ��  �
   �t& �2�
��t��u򋃄   �   ��D  �
   �t& �2�
��t��u򋃄   �   ��H  �
   �t& �2�
��t��u򋃄   �   ���  �
   �t& �2�
��t��u򋃄   �   ���  �
   �t& �2�
��t��u򋃄   �    ��  �
   �t& �2�
�� t��u򋃄   �    ��  �
   �t& �2�
�� t��u򋃄   �@   ��D  �
   �t& �2�
��@t��u򋃄   �@   ��H  �
   �t& �2�
��@t��u򋃄   ��   ���  �
   �t& �2�
���   t��u�   ��   ���  �
   ��&    f��2�
���   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �    ���  �
   ��&    f��2�
��    t��u�   �    ���  �
   ��&    f��2�
��    t��u�   � @  ���  �
   ��&    f��2�
�� @  t��u�   � @  ���  �
   ��&    f��2�
�� @  t��u�   � �  ��  �
   ��&    f��2�
�� �  t��u�   � �  ��  �
   ��&    f��2�
�� �  t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ���  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��  �
   ��&    f��2�
��   t��u�   �   ��D  �
   ��&    f��2�
��   t��u�   �   ��H  �
   ��&    f��2�
��   t��u�   �@� ���   �
   ��&    f��2�
��@� t��u�   ��� ���   �
   ��&    f��2�
���� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   � � ��@  �
   ��&    f��2�
�� � t��u�   �@� ���  �
   ��&    f��2�
��@� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   ��� ��@  �
   ��&    f��2�
���� t��u�   � 	� ���  �
   ��&    f��2�
�� 	� t��u�   �@	� ��   �
   ��&    f��2�
��@	� t��u�   ��	� ��@  �
   ��&    f��2�
���	� t��u�   ��	� ���  �
   ��&    f��2�
���	� t��u�   � 
� ���  �
   ��&    f��2�
�� 
� t��u�   �@
� ��@  �
   ��&    f��2�
��@
� t��u�   ��
� ���  �
   ��&    f��2�
���
� t��u�   ��
� ���  �
   ��&    f��2�
���
� t��u�   � � ��   �
   ��&    f��2�
�� � t��u�   �@� ���  �
   ��&    f��2�
��@� t��u�   ��� ���  �
   ��&    f��2�
���� t��u�   ��� ��   �
   ��&    f��2�
���� t��u�   � � ��@  �
   ��&    f��2�
�� � t��u�   �
   �` � ��&    �2�
��` � t��u�   �
   �   ��`  ���
��t��u�[^�f�f�f�f�f�f�f�������WVS���  �ZD��ty��8  �    ��(  ��4  �      �������4  ��   u,�    ��(  ��(  �������1҉�������ǉ��������4    �������[^_ÿ������&    �t& �����UWVS�����  �^D�t$���j  ��8  �    ��(  ��(  ���������������ŉ$����������)  �Gh� ����   ����   �D$�@���
  ��4    �������4  ��   �H��   ��u1�    �)  ����������T  ��������$��x|���������4    ������$��[^_]Í�&    �v ��	�i�����X&  �� �� ��X&  ��  O �� �� �
   �t& �
�*9��6�����u��,�����&    �v �    �2)  ��4  ��   ��4  ��   ���V������T  ��I����$�����T�����&    f������U�v  W��VS��d�    �D$��$  �������$    ��8  �C���   ��������Ņ���)  �$��H���>�G)  ��G)  ���   1��=�t& ��rI  f�D$��t�   �T$����������   �$����9v*��&    ��J��D$<VtqwO<Tt�<UuK����9w޺]  f�T$��t�   �T$��������$������D$d+    uB����[^_]Ð<Wt��D�D$���z����   �\�����1���f�L$���D����X���������t& �����UW��VS��0��4  d�    �D$,��8  �D$���%  �;��(  �    ��  ǃ�      h�  �@�D$�   P�������Y��X���  ���  ���  ǃ�     ǃ�     ǃ�  ��  ǃ�  P�  ǃ�     �$������ƅ���  �$�S�C   �C    �щC(���C,   �T$��������ƅ��l  �S���  �$�T$������ƅ��D  �K�$�ΉL$���  ��������ƅ��  ���  �T$�$������ƅ���  ��4  �D$��8  �D$�@h� ���H  �  ���I  ��	��   ��  �����������  �D$���R  �|  �����������  �L$���  �  ������D$����  �D$��`   ���  h  �t$ �����Z��u
�  ������T$���  ���d  ǀ      �Ћ��  (  ������ƅ���   �D$�
   ��X&  ��$� �v �� � �����t%� � =�   t��u�$���  ��������l$��Uh�*  �����k�t  d�d   1���
��t  ��
RP��4  ��p  ��<  ��(  Uh�*  �������  ��   ��  ��$�����ǃ�     �m��&    �v �D$���  ���   ��t��������  �$��������  �T$�$��������  �T$�$������D$������$�������������D$,d+    �l  ��0��[^_]�f�����   ���n����t  �����������  �D$��U  ��  ����������d  �L$���  �Q  �t$ �D$$a�D$(    ����������=  �D$�T$ ���  ��������A  ���  ��������|  �����������t& ����������  �����������  �D$���R  ��  ����������v  �L$���  ��  ������D$���v  �D$��  �t$���  ��������T�����  ������E����t& ��,  �����������  �D$��U  ��  �����������  �L$���  �Q  �t$ �D$$a�D$(    ����������?  �D$�T$ ���  ��������  ���  ��������`  ���������h�  j�������  �����^��X��������)  h�  j������t  �����^��X��������)  ��&    f��,  �����������  �D$��U  ��  �����������  �L$���  �Q  �t$ �D$$a�D$(    �����������  �D$�T$ ���  �������t'���  ���������  �������������������Q  �����������  ������D$ǀ�      �D$�����Ph+  �����XZ������    �,  ������D$ǀ�      ��h;  j������,  �������XZ��� ����,*  ��&    ��  ������D$ǀ�      �y���h�  j�������  �������XZ���-�����)  �t& �t  ������D$ǀ�      �,  ����������e  �D$��U  ��  ����������R����,  ������D$ǀ�      ������,  ������D$ǀ�      �����h;  j������,  �����^��X��������Z*  �t& �h_  j������Q  �������XZ�������J*  �h�  j�������  ������D$$ZY���f����*  ��&    h_  j������Q  �����^��X��������J*  ��&    f�h_  j������Q  �����^��X��������J*  ��&    f�h  j������  ������D$$YZ��������)  h;  j������,  �������XZ���y����x*  �����f�f�f�f�f�������UWVS���\$�$���  ������   �$����	Ń|$��   ��   �  �
   �t& f�/f�f9�t��u��l$���|$��   ��t$��f�l$��	�����ōt& ��
   )�&    �
�9�t��u����wۋD$�L$�l$�������|�����   �
   �v f�/f�f9�t��u��A����D$�D$��t�$�
   �t& ���8�t��u��[^_]Éʸ
   �t& ��8�t��u�l$��������|$�L$�����z����L$떋L$�b����v �����VS�Ë�X&  ��1� �
   ��    �����t��t��uX&  1� � 	�X&  �
   ���1  ��1� ��&    ���9�t��u�[^������S��X&  ��1� ��!ʋ�X&  ���1  �
   ��1� �v ��9�t��u�[Ð�����VS�Ë�X&  ��D1� �
   ��    �����t��t��uX&  \1� � 	�X&  �
   ���1  ��\1� ��&    ���9�t��u�[^������S��X&  ��\1� ��!ʋ�X&  ���1  �
   ��\1� �v ��9�t��u�[Ð�����S��X&  ��X1� ��!ʋ�X&  �
   ��X1� ��&    f���9�t��u�[Ð�����S��X&  ��� � �������   	ʋ�X&  �
   ��� � ��&    ��9�t��u�[������W��V�pSVh
   �����[1�X��uSVh   ��������Ph1   �������ZY����uh9   �����XC��   u�[^_��4$h    �����XZ�|   �4$h=   �����Y[�d   �4$h$   �������L&  []�C  ��t��t�F,��������A����   �F,  �����)����D$P�D$P�D$	P�D$PUhD   ��������   �t$h   �������X  Y1�_1���  �t$hl  ���t$hD  ��jVhl   ��������D$�����9  �D$�����t$�t$h@	  ������D$ ���������������9  �3h�   ������������]X�9  R�t$hP  ��������   ����������������T&    � ���   ����������������������D$����  �t$h�  �����_]��T&   t.�    t�t$h$   �����Y^��T&  �����ǃT&      j �    h   ��H&  ��L&  �����XZ���1  �����������t$h@  �����_]�D$�����������L&  ��H&  �t$hx  ��������    ��  �t$h�  �����Y^��  �3�t$h�  ��������D$�����t����t$h�  �����^_�Ch���%  �Q	  �<$�K�wRP�D$ P������L  P������P�G�@tP��P&  P�A"P�t$(h�  �������,�    ��  ��H&  ��L&  �  �t$h�   �����XZ�f	  �vD�v(�6W�t$h�   ��������?  �3�t$h  ��������D$��������P�t$h@  ������|$�@   �   ��������   ���L$$��������~  �3�t$h�  ��������D$�����:���R�t$ht  ��������x����|$Wh  �����Wh`  �����Wh�  �����Wh�  ������� �D$����������q�t$h�   ������Ch�����Q	  ��  �p�t$h�   ��������B����Ch�@@t$�@   �����������   �t$h�  �����Y^�    t�t$h@  �����XZ��������D$��tN�t$h`  �����]X�����h   ��L&  ��H&  �t$h�  �������H&  ��L&  ���  �K|@늋sh�F��  �؍T$4������D$`�����  =�$ ��   ��   =�# ��   ��   =�# ��   ��   =�# ��   ��   =_# ��   wv=7# ��   wi=-# ��   w\=y" ��  wO=o" �w  wB=Q" �j  w5==" �]  w(=�! tN=9! w=$! w.���M  =  ��   P�t$h�  �����1���������������� ���t�1�������Ch�P�Sx�P�Sl���   �CpR�t$h�  ��������|$@��  ��\  ���  ��`  ���  ���Y  ����Y  u8�T$D�с���� ����� ��  �    �f  ���� 1����Y  ���Y  ;sht�؍{������C�p��FL9��G  �sh�F��tP�t$h�  ������sh���~ u�D$������������������������������C1ɺ    ��L  S�t$h�   ������D$����tP�t$h�  �������먋��Y  ��Y  uǃ�Y     ǃ�Y      ��\  ��Chu~���Y  ���Y  ���   ���   !�!�	���  #��   #��   	���  ���   ���   ��	�t4���Y  ���Y  ����  ǃ�Y     ǃ�Y      ���   ��\  ��`  �u1Ҁxp ��`  1ɀ|� �]  �Kv�xL �a  1҈Sw1�9��~  �|��z  ���Y  �T��   �,$�݉��Y  �\$�p���}  ����� u��,$�C|t�   ��������C`�Ch�P��t
��������Cd���Y   �  ��  �Cu1Ҁ{u �����������`   �  ��\  �Kx   ����  f�D$ � �D$$�D$,    �D$(   ��G�    �{�    ED$0�C�p��FL9��Q  �Fd�@��t�H$��t�T$ ��������vL��L��1�k����   ��\  ���D��������   �ۃ�������������xX҃������ưt(ǃ�Y     ǃ�Y      �5���1��������k�����  � ����ǃ�Y    @ ǃ�Y      ������@�z���w�� t,w��tKw
��t��t�ډ���������O���	}|�G����    ����������3����M| �*����Ct�m����M|������\  ��xJf�D$ � �D$$�D$(   �D$,    �����|$$Gu#��\  �D$    �D$�C�p��FL9��  ���Y  ���Y  ���Y  5��� 	Љ��Y  uǃ�Y     ǃ�Y      ��������D$��t'P�t$h�  ��������C�ڋ�L  ������������������D$��t P�t$h	  ������   �����������sl�t$h�  ��������Y  ǃ�Y  �����    ���Y  ���Y  ���Y  �    ǃ�Y      ��������9  �t$h�  �����Y_������Fd�@��t�H(��t�T$��������vL��L������t$h�  �����Y_�[����D$@��\  �Y���1��|$4 ����`  �Q������   @t�Fd� ��t�H��t	1҉�������vL��L������t-�    t�t$h<  ��������Y  Y_���Y  ���/����ưt-�    t�t$h`  �����X���Y  Z���Y  ���������  � t0�    t�t$h�  �����_X���Y  ���Y  ��  @ ������t$h�  ��������Y  _���Y  Y�������t$h  �����XZ�����	   �����������t$h�  �����XZ�t����CPh�  ������������ZY��tC���6&  @������  �CPh�  ������������ZY����  ���6&  @������  �  �4  ������,  ������4  ������,  �������  �������  �������  ������@Phy  �����XZ��  ��  ������@Ph�  �����XZ��  �@Ph�  �����XZ�  �4$h  �D$��Ph�  ������D$@�D$� �D$���%  �D$�������Ph�  �����XZ�l'  h  �D$�������Ph  ��������l'  �|$h3  �_Sh�  �����Sh�  �������1ҿ������������l'  W�D$(�T$.����	��T$/	�Ph3  �D$��Ph`  ������D$D���8�V'  V�|$�GPh  ������D$<�������1ҿ������������l'  �t$�|$�GPh  ������D$<�������1ҿ������������l'  �\$h  �������ShD  �����Shh  ��������l'  �|$�$�D�PW�t$4h<  ��������&  �CPh�  �����XZ�O)  �CPhD  �����XZ�t/  �L$PR��<  �t$hl  ������E�U �L$,�B��D$0�|$0�M �E�8����  ���L$�2  ��<  ��8  ��Ph�  ���������7  ��<  �D$��Ph�  ���������������������:  �D$�XSh4  �����hj  She  ��������������:  ��t  �˾d   1���k�d��
��
��RSWQ��<  �|$(�_Sh�  ��������   ��p  �����  ����   �z�����L  ����:  ��8  �FPh�  �������@  ZY�@  �FPh�  �����XZ�?  �FPhd  �����ZY��H  �4$�t$jPR�t$$�D$��Ph(  ������� �vG  �F��<  Ph   ��������gA  R�t$h�  ��������~M  �F���  �����Ph�  ���������M  �D$�t$h�  �����Y_�T$�D$��EЉT$�O  ��<  Q�t$h@  ��������O  �$�   ���|  k�d����d   �����   R���P�D$���`  ��<  Q�t$h  ��������O  �T$U��<  �t$h�  ������T$ǃ�      ���Q����$���`  ��<  ��t#�<$���|  RPQ�t$hd  ��������O  PQ�t$h@  ��������O  ��VhQ  �����1�Y[��R  ��Vh�  �����^X�������R  ��PVh�  ��������������R  ����<  Vh  ���������S  �4$P�FPh4  ���������T  ��Vht  �����XZ�U  i$�  ���  �CPhL  ���������V  �CPh�  �����XZ��U  �CPh�  �����_X�uW  �CPh(  �����ZY��U  �CPh�  �����Y_�V  iD$H�  �\$8���  �GPhn  ���������_  iD$H�  �\$8���  �t$LiT$L�  �\$<�����  �t$8Ph   ��������!`  iD$H�  �\$8���  �GPh�  ������    ���Q`  �D$��������  W�D$0��Ph�  ��������$�����#]  iD$H�  �|$8���  �G�t$4Ph�  ���������W  ���  �D$H��Ph�  ������    ���eX  �G�L$Ph|  �����XZ�L$��a  �G�L$Ph�  �����Y]�L$�	b  ���t$�   ���  WhH  ��������Db  �CPh�  �����XZ��c  W�����  ShH  ��������d  �G�L$Ph�  �����YX�L$�^e  �G�L$Ph0  �����XZ�L$��e  �G�L$Ph�  �����XZ�L$��d  ���  �G�L$Ph�  ������L$����d  �CPh\  �����YX�f  ��Sh�  �����XZ�f  �CPh�  �����Y^�_f  ��Sh  �����XZ�lf  i$�  ���  �FPh�  ���������f  �G��<  Ph  ��������qh  ��  ��t<��PWh�  ������D$��������$������h  ��Wh�  �����XZ��h  ���   뼃�Qh  �����YX��n  ��QhL  �����YX��n  ��PQh3  ��������������n  ��Qh(  �����XZ��n  ��Qhp  �����XZ��n  �CVQPh�  ������Sh���o  ��ShQ  �����X1�Z�p  �G�vPh�  ��������Tp  �F���Y  ���Y  Ph�  ��������q  �����UWVS��   �ZXd�    ��$�   1��{�slhx  Wh~  ��������C|t�T$4��������C�k�p��FL9�t�Fd� ��t���t��������vL��L�����Y  �L$4�؍�X  ���)  ��������Y  �؉�������D$8PWh�  ������D$PWh�  �������������� X  ���W  Wh  ���������L  ��(�������4&  �u  � �o  D�PWh�  �������$  ���������4&  Wh�  ������������� t4�N���t-�Q�V�����
R�F�1�k�d��P�v��v�WhH  ����������  9�u����1  ���1  ���1  ���1  Wh�  ���������$�   d+    t������Ą   1�[^_]ÍGPh�  �����X�����Z�$s  �GPh�  ����������^_�$s  �G�$Ph(  �����Y[�$�s  �G�$Php  �����Y[�$�9s  P�GPh3  �������������$s  �G�$Ph  �����_X�$�s  �G�$PhL  �����XZ�$��s  ��  �������  ��������  Vh0  �����X1�Z�~�  ǆ,%      ���  Vh|  ����������Y[�~�  �����Ph�  �����XZ�c�  k����  �t$$�D$��Ph   ��������]�  P�D$��Ph@  �������������@�  k����  V�D$��Ph�  �������������@�  k����  �D$��Phh  ��������B�  ���r@PWh�  ���������  �C��R���  PPWh  ���������  ��P�D$��Ph_  ��������q�  S�uD�D$��PhK  �������鞌  �E�F  �@  �u D�PV�D$��Ph�  ��������q�  �u@�D$��Ph�  ��������q�  V��<  �t$$hL  ���������t4�    %  =  ���  �u ��<  �t$$h�  �������駍  ��<  V�t$$ht  ��������B�  �,$�u �D$��Ph�  ��������_�  P�D$��Ph�  ��������q�  P�GPh�  ��������̒  �G��$2  Ph�  ��������̒  ���  �oRPPUh�  ������    ������  ����  �$�L$�   ��D�̄  P�D$�p\�p@�p$�p hu  Uh�  ������� ���  �CVPh�  ������   �T$H���[�  Vjh~  �����1���鈔  ��RSh�  �����1����ѕ  P�CWVPh�  ��������ˢ  �M  PSht   ������D$��������  �4$Sh�   ����������  Sh!  �������X&   � � ���D$��X&  ��� ��X&  � � ���D$��X&  ��� ��X&  � � ���D$��X&  ��� ��X&  ���� � �����  �D$������  =�   �  ���  �  ���  ��  ����  ����  ����  ��  ����  ��  ����  ��  ����  ���f  ��  D�PSh8!  ������D$��=�   ��  =�   ��  =�   ��  ���  �  ���&  ���	  ���4  ��  ���9  ����  ���;  ��  ���6  ����  ��
�.  ��  ��	�)  ��
��  ���!  ��  ���  ����  ���  �v  ���  ���d  ���  �U  ���  ���C  ����  ����  ��  D�PSh`!  �������  ��  ���D$D�PSh�!  �������  ��  ���$D�PSh�!  ������������ɍB��L$��<�2  h�  Sh�!  ��������$�Y  ��  PSh "  ����������S  USh("  ��������L$�ȃ����X  ��  PSh�"  ���������uB��   ��  ��  E�PSh�"  ����������;  ��  PSh�"  ���������1�[^_]������  �D����<$������P������PSh�   ��������L�����  �_�����  �U�����  �K����{  �v�����  �7����e  �b������T$�4�`  Sh�!  ������T$����������4�   Sh�"  �������������@  �����N  ����ShX"  �����XZ������  �����9  ��������  ������  ������v  �����4  �������  �����r  �m����(  �����m  �Y�����  �����a  �E����   �p����  �f����  �\����  �R����  �H�����  �>�����  �4�����  �*�����  � �����  ������  �����ҹ�  ��  �$D΃�pQPh�  ������T$���ɥ  �4$W�D$��pPh$#  �������������;�  �t$U�t$W�D$��pPhH#  ���������  ��   QRPh�#  ���������  ��   VSh�#  ��������������  ��   WSh�#  ��������������  ���@B ��   �L$L���RPVh�$  ������L$\���    �b�  Q�t$L�t$L�t$L�t$@h  �t$T�t$T�t$Th�  �t$TVh %  �������4�b�  �L$X��   VWR�T$\�D$XPh$  ������T$d�L$l�����b�  �    ��  ��1҉|$P�����D$T    ����D$PT$Ti���������1����@B �����RP�t$ThL$  ������L$h���    ��  ���L$P�   ��������� z R���P�t$Tht$  ������L$`���    ��  jyj�t$Th�$  ������    �L$`���D$\��������։�1҅��K�  �*�  ����p1���RPh�%  ��������X�  ����p��RPh�%  ����������  Shc  �t$h�&  ������D$$�   �h   ��h   ��   �������h � ����  ���������   �	  ���������  �	  ���������� V�$  ���������   �  ��h�� ?h�   ������   ��	  ���������X&   � � ������   ��X&   � � ��Ϩ ��   ��X&  �׹
   ��    � �8����t��0t��u�Ѓ� ��X&  �ǁ�   �� � �
   �:�
���t1���0t��u�  ��������  � h�� ?P����������D$�����XZ�D$d+    ��   �D$��[^_]�RPhc  �t$h�&  ���������������
   ��X&   � �����t	���������u��������X&  ��   �� � �
   �����t��0�� ������u�����hc  �t$hp&  ��������D$�����<����������0  ��  P�FPh\'  ��������t�  �FPh�'  �����XZ��  ��0  ��  P�GPh�'  �������� �  �GPh(  �����]X��  ��Wh�'  �����XZ���  ��WhD(  �����ZY�I�  ��P�����hv  Sh@)  ������D$������D$    Sh�(  �����Sh�(  ������� ��  ��hv  Shp(  �����Sh�(  �����Sh�(  ���������  h *  �����X���  h�)  ������D$ǀ�      Y�b�  h�*  �����Y��  hp)  ������D$ǀ�      ^�b�  h�)  �����X���  h0*  ������D$ǀ�      X�b�  h`*  �����Y���  h0*  ������D$ǀ�      Y�b�  h0*  ������D$ǀ�      X�b�  �����VS��4  ��8  ��Ph�  �����XZ��ty���   tp���  ���  ����������  ��������  �S����������  �S��������C���������������  ��������  �������(  [^�����[^������Sh  hd	  ������=    XZvjh�	  �����[���X�X�=    �  vh�	  �        �����Y�"  �    �    ������Å�th 
  ����������Z�h'  �����X��[ø    �����                                                                                   enc_pcm_bufs    enc_vbi_bufs    enc_yuv_bufs    enc_idx_bufs    enc_mpg_bufs    enc_ts_bufs enc_pcm_bufsize enc_yuv_bufsize enc_idx_bufsize enc_mpg_bufsize enc_ts_bufsize  enc_pcm_buffers enc_vbi_buffers enc_yuv_buffers enc_idx_buffers enc_mpg_buffers enc_ts_buffers  cx18_first_minor    cx18_pci_latency    retry_mmio  mmio_ndelay debug   ntsc          secam        pal      cardtype                     radio                   tuner                �          �  z[  ��������                                                R  Z  d  strnlen strscpy R  n  x  �  �  �              �  �	  �  �  �  �  �  �  �  �
                             �  $
  Q        y           0          �                                                                                                   ���     G               4    �  �"$          �  z[}�f                         �  �
  Q        q           0          �                                                                                                   ���     G               4    �  �"$          �  z[}'o                         �  �
  Q        Q           0                                                                                                                          ���     G               4    �  c
23
       B   �  z[y                         �    Q        1           0         p                                                          �                        �        @  � �     &               4   S  S	"3	           �  z[�	                          +     Q        {           0         p                                                                                               ���     G               4    �  f	26          �  z[TXC3                         h     Q        {           0         p                                                                                               ���     G               4    �  f	26          �  z[�                         C    Q        Q           0                                                                                                                          ���     G               4    S  �3$           �	  z[[ �                         �  �  Q       �                                                                               0  0  0      
   (                                              4       s##                                          	   Y  �  Q       �                                                                               8  8  8      
   (                                                      �"D                                               Y  �  Q       �                                                                               0  0  0      
   (                                              4       �"D                                           __fortify_strlen    strnlen strscpy                             �  �  �  �  �  �  �  �           L    qcx18_firmware_init                              `                              �)              �,                                              `       �                              `+                                                                          @-           +                                                          �(                                                               ;  `=  �;  0=                  �              
      ����                                 P %              7      ����       C                                                                                                              HM12 (YUV 4:1:1)                HM12                         MPEG                            MPEG                          UYVY 4:2:2                      UYVY                ����       ����   ������������strnlen strscpy                 �v  �l                          �i              j      |                                             �j       ~                              pj              �z      �|                              pv  v  �u      @u                      �t  @t  �m          �q   m                                                  0r  r  0o                      �k  @l              x  �r  �n                  Pr  �m  @m          P{  g                                                   n  D�  ��   }  `�   �  P�  �               �       �    @   �    @               /       E       \       �       0       t       �       �  	     �       P       �       �       	       +	       C	       [	      x	       �	       �	       �	       �	       �	       
       t       +
       E
        c
  !     �  "     �
  #     �
       �
      �
      �
        �
                                   5  7  ;  ?  C              �  4  �  �                     &  1  f  A  M  Y  e  h  l  p  t  v  4  �  �  x  �  �  �  �  �  �%  &  �  �  @&  �  �  �  �  4  �  �  �  �  �  �  �             �  M        	      !  +  1  7  >  E  I  M  Q  W  [  _     �  �  `  @                                 ��                                                                          ��          ��                          ��                                              ��           �  ��          ��  б                                      @�          ��  ��                                      ��          0�                          �xp�<4��80��xp��ZRҖ����ZR��XPД����XP��xp�<4��80��xp��ia�-%��)!��ia��KCÇ����KC��IA����	��IA��ia�-%��)!��ia��h`�,$��( ��h`��JB��
��JB��H@���� ��H@��h`�,$��( ��h`��xp�<4��80��xp��ZRҖ����ZR��XPД����XP��xp�<4��80��xp�        @                    adapter_nr                �  cx18_ext_init  cx18_reset_ir_gpio  cx18_start_v4l2_encode_stream  cx18_stop_v4l2_encode_stream  cx18_claim_stream  cx18_release_stream  cx18-alsa 6%s: eeprom dump:
 6%s: eeprom %02x: c %02x c
 6%s:  info: Removing Card
 6%s: Removed %s
 cx18 tveeprom tmp cx18-%d 6%s: Initializing card %d
 6%s: Ignore card
 6%s: User specified %s card
 6%s: Autodetected %s card
 3%s: Defaulting to %s card
 &cx->serialize_lock &cx->gpio_lock &cx->epu2apu_mb_lock &cx->epu2cpu_mb_lock %s-in %s &cx->cap_w &cx->mb_apu_waitq &cx->mb_cpu_waitq &cx->dma_waitq cx18 encoder 3%s: Invalid EEPROM
 6%s: Autodetected %s
 6%s: %s 6%s: Initialized card: %s
 4%s: Retry loading firmware
 1.5.1 cx18 6cx18:  End initialization
 xc3028-v27.fw Tuner 1 Line In 1 Line In 2 S-Video 1 S-Video 2 Composite 1 Composite 2 Component 1 Leadtek WinFast DVR3100 H Leadtek WinFast PVR2100 Toshiba Qosmio DVB-T/Analog Conexant Raptor PAL/SECAM Analog TV capture supported
 GoTView PCI DVD3 Hybrid Compro VideoMate H900 Hauppauge HVR-1600 rc-hauppauge 6%s:  i2c: i2c init
  #%d-%d 6%s:  i2c: i2c exit
 tuner tveeprom cs5345 cx23418_DTV cx23418_AV gpio_mux gpio_reset_ctrl ir_z8f0811_haup v4l-cx23418-cpu.fw 3%s: Mismatch at offset %x
 v4l-cx23418-apu.fw gpio-mux gpio-reset-ctrl %s %s 3%s: Max buffers = %zu
 include/linux/dma-mapping.h 6%s:  info: Stop Capture
 encoder MPEG 6%s: Disabled %s device
 &s->waitq %s %s 3%s: DVB failed to register
 TS encoder YUV encoder VBI encoder PCM audio encoder IDX encoder radio 6%s:  info: Start Read VBI
 6%s:  info: EOS %s
 6%s:  info: User stopped %s
 include/linux/thread_info.h 6%s:  file: Encoder poll
 6%s:  ioctl: close() of %s
 6%s:  info: Mute
 6%s:  info: Unmute
 6%s:  file: open %s
 6%s:  ioctl: Unknown cmd %d
 6%s:  info: Input unchanged
 Radio TV 1.5.1 6%s: Version: %s  Card: %s
 6%s: Video Input: %s
 6%s: Audio Input: %s
 6%s: Tuner: %s
 6%s: Status flags: 0x%08lx
 cx18 Radio Tuner cx18 TV Tuner cx18 PCI:%s  %#010x 4%s: unknown cmd %x
 stale good 6%s:  info: %x %s
 6%s: FW version: %s
 incoming 3%s: cx == NULL (cmd=%x)
 3%s: args too big (cmd=%x)
 4%s: Unknown cmd %x
 CX18_CPU_SET_CHANNEL_TYPE CX18_EPU_DEBUG CX18_CREATE_TASK CX18_DESTROY_TASK CX18_CPU_CAPTURE_START CX18_CPU_CAPTURE_STOP CX18_CPU_CAPTURE_PAUSE CX18_CPU_CAPTURE_RESUME CX18_CPU_SET_VIDEO_IN CX18_CPU_SET_VIDEO_RATE CX18_CPU_SET_VIDEO_RESOLUTION CX18_CPU_SET_FILTER_PARAM CX18_CPU_SET_MEDIAN_CORING CX18_CPU_SET_INDEXTABLE CX18_CPU_SET_AUDIO_PARAMETERS CX18_CPU_SET_VIDEO_MUTE CX18_CPU_SET_AUDIO_MUTE CX18_CPU_SET_MISC_PARAMETERS CX18_CPU_SET_RAW_VBI_PARAM CX18_CPU_SET_CAPTURE_LINE_NO CX18_CPU_SET_COPYRIGHT CX18_CPU_SET_AUDIO_PID CX18_CPU_SET_VIDEO_PID CX18_CPU_SET_VER_CROP_LINE CX18_CPU_SET_GOP_STRUCTURE CX18_CPU_SET_ASPECT_RATIO CX18_CPU_SET_SKIP_INPUT_FRAME CX18_CPU_SET_SLICED_VBI_PARAM CX18_CPU_GET_ENC_PTS CX18_CPU_SET_VFC_PARAM CX18_CPU_DE_SET_MDL_ACK CX18_CPU_DE_SET_MDL CX18_CPU_DE_RELEASE_MDL CX18_APU_START CX18_APU_STOP CX18_APU_RESETAI CX18_CPU_DEBUG_PEEK32 0 APU CPU EPU HPU  not  automatic detection mono stereo dual tri mono with SAP stereo with SAP dual with SAP tri with SAP forced mode not defined EIAJ A2-M A2-BG A2-DK1 A2-DK2 A2-DK3 A1 (6.0 MHz FM Mono) AM-L NICAM-BG NICAM-DK NICAM-I NICAM-L IF FM Radio BTSC detected chrominance unknown audio standard forced audio standard no detected audio standard yes no running stopped undefined system DK system L enable disable 6%s:  info: %s output
 %s %03x Chroma autodetect mono/language A language B language C analog fallback language AC language BC language AB BG DK1 DK2 DK3 I L FM Radio (4.5 MHz) FM Radio (5.5 MHz) S-Video MONO2 (LANGUAGE B) MONO3 (STEREO forced MONO) STEREO DUAL1 (AC) DUAL2 (BC) DUAL3 (AB) FM radio 0x0 NTSC-M NTSC-J NTSC-4.43 PAL-BDGHI PAL-M PAL-N PAL-Nc PAL-60 0x9 0xA 0xB SECAM 0xD 0xE 0xF v4l-cx23418-dig.fw dvb-cx18-mpc718-mt352.fw cx18 s5h1409_attach symbol:s5h1409_attach mxl5005s_attach symbol:mxl5005s_attach s5h1411_attach symbol:s5h1411_attach tda18271_attach symbol:tda18271_attach zl10353_attach symbol:zl10353_attach xc2028_attach symbol:xc2028_attach mt352_attach symbol:mt352_attach &dvb->feedlock 6%s: unregister DVB
 xc3028-v27.fw         @   �  `  P  p      p  �  �  0  �    @  p  P  �  �  �     �   $  �(  0)  �)   *   +  `+  �+  �,  @-  �-  �.  P/  �/  �0  �0   1   1  �1  02  `4  �4  `5  `7  �7  �8   ;  �;  �;   =  0=  `=  �?  0A  �I  �J   O  �P  Q  �Q  �Q  �R  �S  PT  PU  �W  �a  �c  0d  �d   f  Pf  �f   h  �i  j  pj  �j  �k  @l  �l   m  @m  �m  �m   n  �n  0o  `o  0p  �p  �q  r  0r  g  Pr  �r  @t  �t  @u  �u  v  pv  �v  `w  x  �z  P{  |  �|   ~     P�  ��  �  @�  P�   �  �  `�  Є  0�  ��  P�   �  ��  Г   �  ��  p�  p�  p�  @�  ��  ��   �  @�  �  ��  ��  ��  P�  ��  ��  а  @�  б  p�  ��  ��  ��  ��  ��   �  p�  ��  ��  �  0�  ��  ��  ��  ��  0�  P�  P�  0�  ��  ��  ��  `�   �  P�  ��  ��  ��  �*  ��  P�  ��   �  p�  ��  ��  6%s:  info: Stopping all streams
  6%s:  info: releasing enc_mem
 6%s: eeprom PCI ID: %02x%02x:%02x%02x
 3cx18: cannot manage card %d, driver has a limit of 0 - %d
    3cx18: v4l2_device_register of card %d failed
 6%s:  info: Stream type %d options: %d MB, %d buffers, %d bytes
   4%s: pal= argument not recognised
 4%s: secam= argument not recognised
   4%s: ntsc= argument not recognised
    3%s: Unknown user specified type, trying to autodetect card
   6%s: Autodetected Hauppauge card
  3%s: Unknown card: vendor/device: [%04x:%04x]
 3%s:               subsystem vendor/device: [%04x:%04x]
   3%s: Please mail the vendor/device and subsystem vendor/device IDs and what kind of
   3%s: card you have to the linux-media mailinglist (www.linuxtv.org)
   3%s: Prefix your subject line with [UNKNOWN CX18 CARD].
   3%s: Unable to create incoming mailbox handler thread
 6%s:  info: base addr: 0x%llx
 6%s:  info: Enabling pci device
   3%s: Can't enable device %d!
  3%s: No suitable DMA available, card %d
   3%s: Cannot request encoder memory region, card %d
    6%s: Unreasonably low latency timer, setting to 64 (was %d)
   6%s:  info: cx%d (rev %d) at %02x:%02x.%x, irq: %d, latency: %d, memory: 0x%llx
   6%s:  info: attempting ioremap at 0x%llx len 0x%08x
   3%s: ioremap failed. Can't get a window into CX23418 memory and register space
    3%s: Each capture card with a CX23418 needs 64 MB of vmalloc address space for the window
 3%s: Check the output of 'grep Vmalloc /proc/meminfo