dcard include/config/RCU_FANOUT_LEAF) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ELF                      p'      4     (                ???????t?S?????  ??t???t
??$&      ????????      ???      [???&    ???&    ?????UWV??S????d?    ?D$1?????   ??  ????   ??,&  ????   ?F?$    1??????Tj ?   h    ??????????t   ?   ?   ??  ?,$??????????Z   ?8?   ?
   ?h???  ???      ?x?(?G??????G$?u???Dh   j P??????ud?u?h@  jPW?????????$????????   ??,&  ?$?????????   ?    u-?D$d+    u??1?[^_]??v ?    t?????????????   f?f?f?f???????@?@    ?@    1????&    ?????1????&    ??????VS?p?^????????v???????????[^???&    ?t& ?????UWVS?????5   ?$?????2  ?h????   ?}|????   ?w`????   9??  ??4  ???7  ??1????S?D$?GT?D$????????D$?D$????D$9?sv?t$?$?D$???????????????C?t$??C?OT??)?9?C?s?s???C?GL9???   ????[^_]??????-   ???  ??[^_]???&    ?v ???L$?D$)????$???L$??????D$?L$+L$?$??L$???????4  ??????T?????&    ?   ???   ??[^_]???&    ?v )????K?????????[^_]???????    ?   ???P?????  ??&    ??    ?????WVS?p???   ??T  ?????1????????????  ??????????(&      ?   ?????1?[^_???&    ??&    ?????U??WVS??l?u|d?    ?D$h?E??$?C??D$????   ???????<&  ???  ?|$d?H?D$\?D$??<&  ?T$`????????"  ?????  ??x??T  ?D$????  s6?$?T$?    ??????T$?D$hd+    ??   ??l??[^_]??t& ?????   ?   ?   ???????    ???      ??       ??  ?   ??  ??  ??  ??  ??     ??     ??  ? @??!  1  ??(  b   ??????<$?L$?o???   ??(&      ????  ?D$??????$?   ??????$?????$?    ??????????????????????&    ?????U1??   W??VS???/d?    ?D$1??GTjj ?????????????  ?G    ?$??   ?   ?????1??   j j ?D$??????\$?P   ?C    ??X??`  ?}d?????????XZ?????  ??Pt????P??  ??????????D$d+    u
????[^_]??????S?@@??uh    h    ?????XZ?6??$&  ??u??h    Ph0   ??????????B???????$&      1?[?hh  ??????    ?   ?????1?1?h    ??????        h?  ????????h(   h    h`   ????????b  ??Ph   Vhl  ????????$??t????????????h(   Vh  ????????b  ??j?h   Vh  ???????????Ph   Vh?   ???????????h(   Vh?   ????????b  ??,&      ??Ph   Vh?  ????????a???h(   h    ?????Y[?b  h(   h    h?  ????????b  hd   hd   hp  ?????????  hd   hL  ?????^_??  Q?L$?t$Phd   h?  ??????k?T$????tD?}|???3  ?=    ??  hd   h$  ?????XZ??  hd   h?  ?????XZ??  ?=    ??  hd   h?  ?????Y[??  ?H   ??????@   ???????PhP   Uh?  ????????p  ?????hH  ?????X1??    P   ?           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ?              ?  ?      ?                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P       ?  ?  ?     ?    ?  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ??     ?           ??    ?  ?  s           GNU  ?       ?               GNU p?7????J??=O"??&??        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ??       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 ?            ?   U   ?     ?            ?           ?      #     ?   =          O   Y       ?        4           B  8       	 T           ??d  ?       y  ?       ?  ?  %     ?          ?    ?     s   d       	 ?  ?  R     ?    ?    ?  ?   @    	   ?  0     +  H       	 6  @       	 A  P       	 L  ?   -     e  ?        ?          ?  ?   
    	 ?           ???  ?   #     ?  "       ?  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ?             ?             ?  U   ?     ?             ?             ?             ?             ?             	                                      >             F             \             m             u             ?             ?             ?             ?             ?             ?             ?                          9             A             U             c             p             ?           ?     ?    ?             ?             ?  ?  ?     ?             ?              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     ?     ?   B  ?   U  ?     ?   i  ?   L  ?     ?     ?       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  ?  c  ?    ?  S  ?    ?  N  ?  N  ?  N  ?  K  ?  Z    N          ?  I  ?  _  ?  R  ?    ?      I  @  I  R    Z    x  R  ?  e  ?    ?    ?  N  ?  V  ?  \  ?  Y  ?  a    N  #  E  C  V  m  X  ?  a  ?  E  U  ]  o  d  ?  P  ?  a  ?  a  ?  S  ?  N  ?    ?  E  ?  D  ?          W  &  [  I  T  V    g    p  ^  {  E  ?  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  ?     ?   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     ?     ?   Q  ?     ?     ?   Q  ?     ?     ?     ?   Q  ?     ?     ?   Q  ?     ?     ?     ?     ?   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    ?  Q  ?    ?    ?    ?  Q  ?    ?    ?    ?    ?    ?  Q  ?    ?    ?  M  ?    ?  M  ?    ?    ?  Q  ?       N          Q     `       ?     ?     ?     ?     ?                                                    $              B     f     c          B     f  $                     ?   O  ?  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   ?                    	   @       ?  ?              )             ?  ?                  %   	   @       l"  ?               8             d  ?                 4   	   @       ?"  H              K             X	                    G   	   @       <%  (               Z             ?	  ?                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       ?%  P               ?      2       }  +                 ?             ?  ?                 ?             @  (                  ?   	   @       ?%  @               ?             h                    ?   	   @       &                 ?             t                     ?             t  (                  ?             ?  $                  ?             ?  0                  ?                           @       ?   	   @       4&                                                       0          9                              9                                   <  ?     A         	              ?                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      p'      4     (                ???????t?S?????  ??t???t
??$&      ????????      ???      [???&    ???&    ?????UWV??S????d?    ?D$1?????   ??  ????   ??,&  ????   ?F?$    1??????Tj ?   h    ??????????t   ?   ?   ??  ?,$??????????Z   ?8?   ?
   ?h???  ???      ?x?(?G??????G$?u???Dh   j P??????ud?u?h@  jPW?????????$????????   ??,&  ?$?????????   ?    u-?D$d+    u??1?[^_]??v ?    t?????????????   f?f?f?f???????@?@    ?@    1????&    ?????1????&    ??????VS?p?^????????v???????????[^???&    ?t& ?????UWVS?????5   ?$?????2  ?h????   ?}|????   ?w`????   9??  ??4  ???7  ??1????S?D$?GT?D$????????D$?D$????D$9?sv?t$?$?D$???????????????C?t$??C?OT??)?9?C?s?s???C?GL9???   ????[^_]??????-   ???  ??[^_]???&    ?v ???L$?D$)????$???L$??????D$?L$+L$?$??L$???????4  ??????T?????&    ?   ???   ??[^_]???&    ?v )????K?????????[^_]???????    ?   ???P?????  ??&    ??    ?????WVS?p???   ??T  ?????1????????????  ??????????(&      ?   ?????1?[^_???&    ??&    ?????U??WVS??l?u|d?    ?D$h?E??$?C??D$????   ???????<&  ???  ?|$d?H?D$\?D$??<&  ?T$`????????"  ?????  ??x??T  ?D$????  s6?$?T$?    ??????T$?D$hd+    ??   ??l??[^_]??t& ?????   ?   ?   ???????    ???      ??       ??  ?   ??  ??  ??  ??  ??     ??     ??  ? @??!  1  ??(  b   ??????<$?L$?o???   ??(&      ????  ?D$??????$?   ??????$?????$?    ??????????????????????&    ?????U1??   W??VS???/d?    ?D$1??GTjj ?????????????  ?G    ?$??   ?   ?????1??   j j ?D$??????\$?P   ?C    ??X??`  ?}d?????????XZ?????  ??Pt????P??  ??????????D$d+    u
????[^_]??????S?@@??uh    h    ?????XZ?6??$&  ??u??h    Ph0   ??????????B???????$&      1?[?hh  ??????    ?   ?????1?1?h    ??????        h?  ????????h(   h    h`   ????????b  ??Ph   Vhl  ????????$??t????????????h(   Vh  ????????b  ??j?h   Vh  ???????????Ph   Vh?   ???????????h(   Vh?   ????????b  ??,&      ??Ph   Vh?  ????????a???h(   h    ?????Y[?b  h(   h    h?  ????????b  hd   hd   hp  ?????????  hd   hL  ?????^_??  Q?L$?t$Phd   h?  ??????k?T$????tD?}|???3  ?=    ??  hd   h$  ?????XZ??  hd   h?  ?????XZ??  ?=    ??  hd   h?  ?????Y[??  ?H   ??????@   ???????PhP   Uh?  ????????p  ?????hH  ?????X1??    P   ?           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ?              ?  ?      ?                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
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
     P       ?  ?  ?     ?    ?  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ??     ?           ??    ?  ?  s           GNU  ?       ?               GNU p?7????J??=O"??&??        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ??       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 ?            ?   U   ?     ?            ?           ?      #     ?   =          O   Y       ?        4           B  8       	 T           ??d  ?       y  ?       ?  ?  %     ?          ?    ?     s   d       	 ?  ?  R     ?    ?    ?  ?   @    	   ?  0     +  H       	 6  @       	 A  P       	 L  ?   -     e  ?        ?          ?  ?   
    	 ?           ???  ?   #     ?  "       ?  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ?             ?             ?  U   ?     ?             ?             ?             ?             ?             	                                      >             F             \             m             u             ?             ?             ?             ?             ?             ?             ?                          9             A             U             c             p             ?           ?     ?    ?             ?             ?  ?  ?     ?             ?              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q     ?     ?   B  ?   U  ?     ?   i  ?   L  ?     ?     ?       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  ?  c  ?    ?  S  ?    ?  N  ?  N  ?  N  ?  K  ?  Z    N          ?  I  ?  _  ?  R  ?    ?      I  @  I  R    Z    x  R  ?  e  ?    ?    ?  N  ?  V  ?  \  ?  Y  ?  a    N  #  E  C  V  m  X  ?  a  ?  E  U  ]  o  d  ?  P  ?  a  ?  a  ?  S  ?  N  ?    ?  E  ?  D  ?          W  &  [  I  T  V    g    p  ^  {  E  ?  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  ?     ?   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     ?     ?   Q  ?     ?     ?   Q  ?     ?     ?     ?   Q  ?     ?     ?   Q  ?     ?     ?     ?     ?   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    ?  Q  ?    ?    ?    ?  Q  ?    ?    ?    ?    ?    ?  Q  ?    ?    ?  M  ?    ?  M  ?    ?    ?  Q  ?       N          Q     `       ?     ?     ?     ?     ?                                                    $              B     f     c          B     f  $                     ?   O  ?  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @   ?                    	   @       ?  ?              )             ?  ?                  %   	   @       l"  ?               8             d  ?                 4   	   @       ?"  H              K             X	                    G   	   @       <%  (               Z             ?	  ?                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       ?%  P               ?      2       }  +                 ?             ?  ?                 ?             @  (                  ?   	   @       ?%  @               ?             h                    ?   	   @       &                 ?             t                     ?             t  (                  ?             ?  $                  ?             ?  0                  ?                           @       ?   	   @       4&                                                       0          9                              9                                   <  ?     A         	              ?                                 D&  )                                                                                                                                    