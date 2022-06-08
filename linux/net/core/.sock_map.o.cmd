rd include/config/ALTERNATE_USER_ADDRESS_SPACE) \
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
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw9903.o: $(deps_drivers/media/i2c/tw9903.o)

$(deps_drivers/media/i2c/tw9903.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
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
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
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
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
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
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux�w�u��0P��W�%S�1�@��tg�#���h�A��2vojr�6�=�X�����t8��M��&@�s)tEO,�5tro�e�Ca�]|�á���_��N�Dy�Dga���$8p�$D2�*켞IH��ǡ :P������Ǯ�{ݓ���8].���2�Q�F�l��m3LB��,1=�Ȕ��D��Ï�'����u˝m���0���S������$�u��d<wȎ�as����֥c��M��_'^���
G��������k/Nq�O�l�Pj��k��^�e�3��4��6�}&8��h���Ԃ�L�q{!`�yB"����r����=�U $��T��)��pK��J�DQ/(�v��7� o����Qv�Wq�]}�3U����4~�|�8��3�h�os����ׯo��%G�R`��k��� ��~M�d^�A܊6�U���"ԭ�r�7���x�4O1��sQ�PE�\<��aUΩ���I��k��ܟ���UI� �'>��,6��Z{/��t����ukc�Z�ˌ���s��9X�b��ܺ����ܡȄ��z��*B\�E땿�t`d��IzPH���ƴ����>H[;X݀S���4���)���}B�u�Y�p��������;<^���F3J��HiŞ��(ەs&��/��V b���I�a�mJ�?�(9Ӟ�+��v˾��J�O��<k7��W�
i<�>�NB݉�����gsL3z׬.��ͫjBd-�$w��|��!-	���ܴ�A#۠ ��c���6�N���|���J�_j�y;��:�^8��ݜΦ���S`	,|2h�,���$T�X���?D�譒44��r���(����1?b�ǳ�0�U�|����:ݷ���׿�׉��g`g�%�>����ڹ�-��]��ⱋ�Cx��i�L��W��}\&��Nz��6&=Ф�R�jp�]a���HT� �(v4b��/jR�m�U�.�Ro{t�\?V(t*j�K���^��^R�����$�2��G��s�s��=�
�wn�m/�S�0�� �O1?�Z)�nk#A	<*k�ېo�ںY�W�!�q��jj[���4=�@E������ ��6��D0��_�(phs����C�Z�mH=v�j��SP ���c�b�ǫ�^fVBQ�	���/M1&|ı���U�66�� ��rj���k�~�����x�3��"�K����r���I���fb�v��a��K�NC7�-(t�m<�ئ��_=�Е��d׍�Lj��(����ɑ)��Ho����+a�jX�����QE��i�8� '&k���+��H�������+���i2D^W���a����b ?=U�O�U���ꁁ���*�����m�}y>49 P���4��� ܁Ҁ�Dh�M�<C��?���������#  �V0~� {5��p�y�]c����JA��.5�c����z�-`尺@/��a���"��ɛ�����%c�f�sσ�؆?m!�AȚ����
��3
O|������ɵ�3��w�ysԺ#bpՓ��=�l�X��%�3���S��B��	�Dl~Ab�hK� 8�8����#eT[(MG ?�C�KR��R	�2��r�:��8�V�aT�ܼ^ �ܙ�N/�BI"��\��4s�����UJ�������Rih���X��\�J[lە�D�D{�]�~�T{�����Bs�-�|?7�;�^K�fkLX����3ч��	g=�e�8�"�vs�d��_~-�s�oA�X�'^Ńm���E��^\]B�>��V��p�>��5��N"���$}`�0�#%��">4,{{�5�1��B�����.�����*�.A�}c��k�y�)���ƪ�hU���6�9�t��u2+N��V:Z�����f�D�@�6�	.�k��8��߯u7��p���Mpi�oNO:_���1�O�?#�#|����t�~⼻SU�|7�����V'�;�$�����e�5æǜ�����G콓)�TB"����쩞P�&V�],*B�	�&�d��v���EƮ�֓9�\N��XT/�+�=I�O=��� d�%�z���7�</#F0�0��Y��X�D�+��)�<�I)��i�"U�܄�b֝�gQٺhєw��KH�U�V�_w%�r@��Y_�O�6�����.���H������^���Zn�?'��oKx�j���]TVƁs��x��;�-%.���d��ϡ&yʃ�j%�� � 2����M��*xS�ؑJ];����p=ME���lM�Z�Wǃ�Ɇ��}Eeo�PY�aaL��qŻW�L�I�>�R�J�͇0ϒ���6`9>E�4J�x��,���>�,�s�1d�O4����� &3Wj����Qt�],�,m#����U�Y���Ai���dGF������{�k��;+x�ќ���z٘�I���X��K�����IϢ��+�?usM�6����b������*�@���
MW��[�(���'А�*,��u�|����$X�<�Έ��=�j��蔘�LB���郭��uK�� ����k�8�M�������3'0�qk-��iS��9PtV
^�hJF1��#'��x�LsG��p���M?2��<$2�TW�.��=���wLF�u�0��<�$�=�Td��K�*p��P&~�g�����-)��Z�
����ے�ȬY2�#�gN�/��&�8<(�|�p%&f�K�?�>���ဳ�mh���#&�l�A B��,9�����ٗ޵p"�������xK��x�~.S�}aZmb�XN6(mr��-��͝�Dѭ��x�b�O�Vy��?� q�X%��;'@��"�)IƱpiP*�A�C�p�G��C�\��	�i;M�x>���v0��oR���+�N���cB��ʄ�u/>�0�m)���v�jt�^��8��bFG���jC������DV���a�2զ��z�
ͳ�dr�R�z��k�kr��Q�ڕ�^�a,�r�k.�a���Uf�&��W�~��z;����tO��{�BA�ǋ�=����5ߢ'�%i�Q�1d��q)o�ĭ��h���������x-#�-��կ��LI�0�c��|��L|�����QĴn�Ev������odPŗz��*�I��"�k���oHES��?^P��'���V��� �g$j+Dz���T���+B]=���f۔���}�wNz�*ڨ�؇!�"vWh�$�[��(����N�-�حY���'�n�Tt*�Yy4�}f0.9���l���;�^�]K�hl(����m��]�nJѸw�X���۔�@��������P�ˢ�Hl.J?s�,����3w�w���s%K%�׆�е����}�vG�h�琢�V?�!�ai��se3��\Q��Ac�,Z ��J"F�J
�"�����}�vq��:�p�(,:����x��j�fSp��]0@�U��a����_����л�%4�Dtq����;�W.[;h����U%�4V:.V�z��=O��xau������KEd�?1G�C5����o��"Y��!p����qd��?�v��MG�x�!d����E$�s�ڃV��մ+����z�_�^SɄM��h�7���d ��j͙Q[�!BCͰ/r��T`55����yc���>q.r{�3ן~A����`1��yh���>1��Ѕ>�_diR�ʀ*�|Q��hj��-r��� ��fk��܂�x��K�ʙ�0#��aeGF�_��]|+4w;�W������<Fz��~��uf/�H!������+洡��Q}~�X��yۂ>���8os���A��F���7.&D�=E�s� ��*��4��y��w#9�,��DK���g���p1��5�*�)���^�`FΤF&�X���Rޑ�,�������J5�/�MR%{�`؊ńo�Dp$�ѹ�G) ��+ �����[nl��Vh���j�sK� ~��S�e��:E+D��s`w3XV7U�g���$�����;/?ͮ�IC�@�D#�=���-m�iܽ�~�>��r	S�����\;�o�Sdn�Rg���D sK�.N"��Kj�/���S@L���(��ZfmK�+�TR�t>KM���>o��g���赉FԷ!B�~�v�T"KV��%��O��.�gP�TW�Qxu3���Iq�s�+��a0����b����t侼x�A���^`�ѱ�,S�@g� ��=�t~�=�t����ݓ^�P��
��?L�Yh����s���Ƶ�)FG
����3��Xu��J5�:N�����P\�B*^b<�%�Kr�\ �-r{���?�%a��n��|^~>@�pr-���_�Yg�R����Q��M§o;��%���\O*# ��Si��qKyל eE��U ������Ð�3��;��pr��v��5���Y�қ�w`�6�N���;��0�܌�/!F��Y��P�Zc�DK�LJ��,��S�6����ka��)D@��P�p��:>����8+����b;�&3o���$��}�ՈE;��ҝ��$�7Y�MNQ<J���o�)��k�ҲY���3A���%�/;��|���?����E���]~�篼V�rM<�a$U7������p9�B<#����M�\��`a�41q�"���CR寲Ļ�����I�#���
4���$S�ҢU/�p56�P��i}���\1���L�t���J�˿h7R���NŅ%��W��ۤ��/=��ǺV����B��*v+ ����L�#�__E��$�,��()ԭa�����Ƨ��=2>��m\�s l2t��g0�ĝD��G�H�Y��mޱޱ���~�q����S�ܽ���[��~��R�=�$��Q<����� �*�?O�/�#{^ܧi���;�BK �����]`y�w%٣u̮�"�7��A� �b�j��Ճ@����mLG�]�a-�s�	�=794�hխ䨧����mϭDY�o���}n8o�=�� 	�i�I��3�����S�g��j��K��8��M��%>�%[�G;�(�7��С�Ä�F��~��,zi��6@����������,��<��O��߹���lfJ^�;w�G�|Q�\���O��X3c����@;����:��e5ԩ�-��|��`B����`z�S���xd
����s��(����򨁙ÿ�3`��������Ƣ��QCJ� 9�9��Q�&�	 @��K^m�4�8
_tB����9 &��CY��>��(�4hQeU����|��&�Ix��3$�3��.��M�/y��`���y�]�m$vvb�^:�3�c�c��y(�`�bS�_�� �?��Q��E�_dA��1U���ӊ���f�/�2"|�؇����Tr@&�%��B�S*����+Al��d�u�+e�k{�=�z����F}�t���-�t������^
�\�c�� Y�_{��ޒ�C _l���@֐#4�w-��(';&��a�3�����"H�>0<��r��E��3�^����}����E��m׶�4�9�4ţ��r#�Aic:G\�A�0]��������h�i��G%�63qg���-�bx��2��(s}7�����t��ukhH��}���^:V�@����1�~m2��8F�3��D%$j��0I��u_[�Zlם!�pӚ\�r��~�^�j�!Fkī#�V�����
�K�1��VS�-����l2j�J�耨ZinZ���0�a��;/��`V�5��XPHN���ag;�K���ۂ��<��%\��o6�����ddb�X�I<��+Ӗ�,,+/��^����t�y���nٷ��"u��B��^���2�y�C��?Zc��������V��cA��y�UzOa��h"�3Čqj �}&L1�I����0��Iu,�C���)ӥ��q'm��j��9\������� ����.��M.o���ퟬ���je:e	�o�ײ�7?�t�����G��c�O�4�i�;h&V�
*o�,^(�].r�u<�:3�#��87��t0�(��|�Ń�����]����5^=� ��kA ��T�4NҪ��)˳���g���n.z�ʖ�=�>d��"VfXށ�
�_!�05�5LЛ�L�=o�\.��,T2�O7ѯ�35v۸*������h�����_�l fR��k_T�2�����k�Hגj��'jnC���*�th�0)�<s��������~|�͐~#�V�yh��nK�2���=e�&��ޙ�����Y�7��K��:���Z�\*��P���VS�8?#�*δ:�C��6���u�)�>LLټ h�m�9�+��Bl�w�	�q��F������%(<���a�Lt8���*7��ؓ,i�tռY��*��-�6g@��ȣ'���6ovw���R"�dn@�n��G�NY���)�����3��?-\u+�B�},�M�L���ۯ��rJ��з���,��r���O	�mƫX\�w�n3�x؜�('��R�/�QW��T-<�!A�z��)W����퍳*�z�5��gf�5��@2r�G���,(�ʆ���M��@�9$&�6ϩ�T�/l/G�$�4o!���{�Oj�[35��?C}��|���H9��l�Emvfi(�P)��{���E|W9?���F3���՛	#M7
9ġ���uZ�_��sG����h@��������H'ƗF�/,�fW-��HC��ۧ�F��o�Uꡑ�ű�Y�~ �zL2ʄT��`5⍖�:��,�yݍ7��)���$-i��*1�JK���`� ��}I1��l�c&9r��
�5U��O��G%�k2Zƺ�����R�rt��0`���
���@����;v|V�p�PWV��a-�5|��?��%�)$N"=5C���N����q�����،1��Y	�҉�[��?c���%��#f�t����PxHO�Ua�P��hT%�`Fws�
!U� R���Cm��Ȑ���f���h��z�.���,��'��*�0)���p�*�Y��9[M�|*�s�d-�@�p����b7ԭ�gیNZdX+������m�(e�`���_P�!�q?1�Aa[hD���3r����l�%Kw\�?��i䡸�:�7𨈙�c��t]t0���2���H��J0g�_�.�!�;�%}����?/��˴�N�B���R��ѹ���AӞء��� l��-�p�׾4���S�Bj�Y���"�������K��AР�Y���νO�S�?7�r�9��R1\`���'f����J�d׏��G�1�� ���ĠY{I�w�Ds�]�ayN��x�(?ĺ�X8�ыk��xLC��3��P؟ipI���=�	#��v  �͎5ڴ�|��`�yŶgԍU$�VzG��E2��״)���tW�.F�h����A��GqΛ�%�S�����kū��O	 jx�j<[��x5O��#���R �UQ���S<���6_���#�Lz#ʧ�g��D6^#/ve䒂�Zi'\�e��!����Y%���z@e����\�*����|�����2�_�0�3�1ĺF0F9���촓�%�V�P�+e�8k�t��D�өN���烛�>iS�QnF��g5�<���1��%p���B�ͮ�i�8�d���� M)P��:Z���P�C��1j.�WrSu�� Ԓ?�~!�(8#��j�w]A���)�Xƫ�eQ�F��h�0;�OS,����V&���Dt�,T�L�NI��[�{��8-�TD��:KnM*@,���!'�g�*�h�<m�E3�i1&w���	����$[����*:1��Gb��)���-)�&�Z��#�X윮	[Bu�YL>H*��]������,��@�?S���Ϻ�1�_��u%����F��l;B���[��:�sw����`�Z�H�6y�GP�ֵ�q�N	xO������D��{���v#/��B��>�o�9�Z�tW7��}��N,G�����ڟǎ��KL����*�?��G��T�(��������"J�#7�x����:/nx����.!���{BJ��#D�3"��X������ƻa��61W3�58���!�@�=`oM�ׄ �R�&��^%|(TPZ���*C��G5��2L*(1��A�D@�݉kY�Ҿu�ЮQM��\Ty갃��a��dO��ԇ��W�îR�&y�Q�{�D�>Tm�H��K߃�ј@? ��i�<,B"��	��o��퉔����C��9�@��j�'h�m��t���{i`����<I���b�yɹMbޫN� ��;@��U~��Q�)+�l[����5�҂9�$B�� k���.P��,�e�ޯLT�FQj���i�M
�
̣�u:�d �y�%�0v���;�Amd��>s�N�n��^�N��pY�5zk`
��Ρj��Oo�3u�������{���;��C�q +}C�ܡںz�a�KVݯ���Ev I_�rx�ÉBI*�b�P�#�`�
!6�ھ��{]�ql�g�Me+�'V�3p�9�2�N��g}��p. }�
���+_��(/�?�T��َ�z0�%5�dʢ�m���5�;���h�8����P��ޥ�f�A���0gS�8���vw���#F��gC`�̪F�ED�'d�ϓ�^�5΋��/���Ԩ#��_�l�xb�\�����U�Ї�!�k[���������t ��$)(��H6�`��Hi�)�U��s�u�d�Ҵ�Mw}L�e��s?X��F̒�BϬ�O���R(�Ѕ�jw�l�Ƈ��xٛ�g�C��-��J���������Xp}����4I��g�;���!�76���8ʿ8ݿ+�:@�'�oH�=���,O��$7j�������:�r��]+N[�~8���W�d.*2��, ���h�h�
�vl�G!y������7Ꮶ]`j�?�|к[�,��螧10���%��ZVQ���-�R�9rC[jl���|=�Ⱥ�6ךy?�(���:�:	h�+�.��0�K��}��;�P�0	��hБ��1���ٱ@���8ҋ�(��Ґ���e&�Ƴ��lqH�3��#���Yj��t��/���e��?� |R��Mӵ�w�ƣE��A�SK�P1h�W9xa��{J%�.�ø~�9i�����"U�\�³��N���$�8��|�6�#���ɴ�\gxy�:��/��<�k�?�L�9b��ٯ�'���Q��|�1�a�[j��d����~ZB�.��[�uL�A<d��-.��6m��!�Կ�kE�BC-�:�U��
��)g0��v�mO{h�B|�GP�_�dh����^��O1�l�4J�v�I���;��P%��TC���Uf�%)�"�}k\�|��X.����PCLt
S������7��}�͚i
�����:(����,k�����.i��xZ�|����a�d���*�;����OӌqH�2~�>B�K��:��h9��%M����F�Ȋ:Ƹ�M!`�'t�DE�kR�?� UPa��:)Do����N@J�,<Z��6bciM�vZo�c�,Ib�V��!��8h�����_]�j��&!��u��R,�o�yY��Y�3�F�S���c�$����)[Q4�ɁM��yH�0����2�P�Қ���2F�cWR'3qG	&<">�}�c��C/�Ku�<��T�ĭ�V�Ǝ�ӇB�5�j��(3ݍW����1Zd۬�[#������ZT	�Pн
�_�GVkO����.��et�5�<\T^��F�oN��B���r�W�p�LXY��S�R��7�|��1�rP o�W����+-搮�<�Y:jC�7�6zG�4�-���C�c1`ύ�C�f9@�\Hœn��v��R������:�����
M��KΑ���$M�%X�O;�6�D#?+��}��z������{q��a�}u���.������ֈ-�h[
��JOYRd�XO��~��S���O�X��e��!
_�Q�����f�/:��PɫZ�%�����sQ�C�q��Ғ����ӓ��i/CE��c':0�m�?�
v坡��.��]A���r���w�1T>��à��[�5_�yŇ�+}?V����4uf����5���[�5���w`b��ؘ���}���$,�6� �m��3�ϛ=-�S�{��J/�#�<-�q�K�R *���!�v��;YO��01�~ӹ��5w%(��M�PRG�����7���(�~;S�����å&`���m�VO:�As�b�%f��!-4�cV�Ny�"\Ps�&�-~�c�M��y�k�2X`��1E�����9�������<��:���G I�:K5����*4	ᖝfx�k�;Zĭ�z�����6�������-��-��Z�&�>ؕ6 �����JxH��-��utX�}P'nSj1��Ơ�T)~gQ�wH��x	����u��	���k��Cd���EW6���Z�%6�jR�c�v�l���[Rϒ���}ԇ�%G|/I�B�tP��q�P���!�'5�u������i=;gs�J��%A���(4�гm?I}:����[N�HX�4�`�lZlLL��������=q#0��ߌ�!OG��F;�������aG�g���S��N�C�P�����đ�sN�}��+�C�(u�
�"#٢5Y8�?�ۘzEJ�w�aD �k{	0HDX�ٸ)�g����C차��7
o� r(Y8�������ھE���:���k��j�ޛOI�i���Q-vYF�b�>:h��%��%v��C�oe��"k�١��8wXR�*�@��>��En�ԍd���0b�z�;���CZ�*�$07�Z��U`�[�j��t��� \�ʪ�t젬l٦Ƕiҩlx��s�'#�Pu-C7�'�3��LB���hVn���̔R�ѹ��7��	�����
�*t��j�)$�p��T����$�Z��Q
���NR�?�%�HۊHwŬ�4�ŭ�l��>B�bQ��P������V
��G�Dj���gĤc��oJ�{Yz�7��� �-��3s�G��wΝy���BK��g��S��?�>E�7ݍt�����[��Tb�+�v��LZH�t8����w��'E� �mTOqfnj����A���?�g��6+����v4�B�C
�vr]�V�p���vE��%��/�i�aS1 ���������m�]��e[��yC���X��ŷ��KQ�Pk����f�SP�DZL`k�
hb����r�x/�������x�e^����D9��]�*�ЛO�kU��P���Ѓj�xVa��k��r���?��B�}ɨ�3²@�bW9)~��������Q� ���4����O�4V�*r��%�)ʨ�&&1s��^f�8�1���s�='�c�)6Y�Ls�M��@P(����'�Yx"!��rZ��7 �?�b/�U8&A��pɣm���p���D���,ē\��>o��ʾ�s�P.ԙʴ=�|w��ݓ�=~D�T��f�����R�kcM����4 �.�_�\�ަ{8HQ�o�h��u	o�s][������z�,	6<�z3>ԅ�O��l���P�|3rWE4G�:wyE]o�Q$��1=�+��s�l�)�<oa�-!���B�,�5l��b�{O���Ոi�����y8��uh�M�V�����5=p�ҕ��l5������t�y�',A�4��j�j@>$�Fm����V�_iq@�9�[~�*^���+�|v4�S�m�z�� �p��Z�4�D�$���T���e�wb�|�+uX��������9�
l�T.*#'<j:�� cRLE�*A�H���^�R-��4F��9[�Q��kC�� x9 ^C&��a���k<5.):�.�x}m��y��$�u}o� �%���g���Y�,C�~^4h����uc.�3� �,f2�*?s�W��{�|�և�moH;z!{j�L�YZ���|_P���V�E':�������5m}�xq�i�
����m"bϟ��Ӝ;�E��C6�!�=e��ه�F���	�|&�B�_q���<peK?�o*�փ��U�G�bA�m��@����p�}L�Ho��O��3�����2g+�S�9/XA�4"��ׂ/F��-�zb"��o�|��uI��
�������o�I ã�d��.;��L���aG޼����x�$5Qv]Y-y��4�>��D��>MXBy'
(͘6Q���_3U��� �m��~���G�����ݻ
�-Ƅ�>�'�W���ҿm�|�/��Iq��yi��f0ϐ)hg�^lzG~���ii(��[�ӓ��W.�WX��GP+0��9�"�!��s�#�B� ��0�/h��扑N�����-�&B�M� J*2��ᦾ�a�t�(�+d�9����Kƻ�)"���m�9%1m�O���"������pI���*8��J�߈e+[��:s��bʯq��8YPs[�fbw����6ٻ���X�0����H��I�y�_k�cp:�d�Bz�l�]}��M��� w��}yh�mtpT�X!�L6�I'�-S�}uy���A3��	�2���):��8;8d#G�
Ĝ���׉]r+⻁����A'�2����B�Z�QU��+ZJKc��E�H)�Oڠ%~������g�^���	f�88"��dML@�{�<Y���T ��ӧ�ʸ�Yν����q�D~�Ւ��h�zƔjR��/GU�܆�_ +o����3F�|ӂ�6^=c����&r��%������}WI���zj_�)-�]�C��!�v}F�������|�p����c�#/ulG�욫� Aމ޾?^*"��+B�+50I}G���`9��`�L��`�cP0��s�ѥ���1�Z��r���1k����ax%�e<|�q+��r���/�C2����dѝU)�*-i��Ms �j���J��
����9<�_����N�V%e絛p���"�ri����F��ne��k�G~�2efҲJ��6�\�uaBD���18d/���0����g$�#GQ�bKj�a�k���R�+��s/#F,?;�i��Ϙ��ȦD;P+�����ӄ{V'@r�c����������������Y1R7���T�me�(��H�A!�0I�"[u�-ǯPҕ����p����j!F�JPĤ�\�Jth��uZ�*G���ݏ��j��o�VP�b)�xR]��8
�*��s���Jq�t�m��/�X<6|��)T?(v���k���X�(M3�u�5
=ݪ���R�&��u�۠��:�F#�y_U%���.�~��Dҍ�+�K��m��s:���fW�Q�@ƾ�K��D���Sꤑ�5~0��*!�ml'ͼ�X�$���P"(|U��us�W�d��Ꜧ�ԁ:"��.ؤL��c%�䢱�G�v*���u"���V��ۆ���Z�iҔLlC�-
�7:�����
{��z P�.[�=Шf��)����n�)����N�����8ԝ�,mFR����tXրe֏x�m	���jtܘ���'X�\D�g�+3j5�o����QE����J���fc��#$�ueR��t�-QD��l� �Ę8��̫n����ˆ�U$N��JhKd��*����*��E�叩w�ͅ�ŉ�}9�h�Gș3Nm��3҄x:ϝ]Aҍ��C������E�ӥ�;�5٩˔w��@\١�(��>�m�\z�J@��EYHE�F��l�I�h,�����3T*�bԙ	�*��-�͝��7������wu��b�+�^�caڸ�0D�X�MT�y�&	x�G����8����H�*���B�q��!=��l�h|ѥ9D�4:�
�H���~/C~D�\F^dzn`9���D�0��D��d���YL��Y =lg�۷5���Wz9�{!l*�l"�� p���bQ�>����=<g
��%�]I�'����ҕm��$Z٘�tP��!��	i�&�J�J�V��^���~���
C���#�獐�B���$q/fEj��Y��������}���N�Sش�+�8�4�C��^�h���V��9+G�Qt!����7'�����!�w�ch'��������4�J�����9�����0h�|�z_�L�EO�|�K��� A�kޔ�!�Cm�ZrF3os���j ��f���$�%�2O��r�2���5m�zw�K:oʦ���l�N��6���æ;Er�<"����ʙ_	��-�����|��)W�xrh���� ��k�d�k�;��X(T=��+�x���݋�Iד�ܨ�)e�^C��ã�����X�&��rWpE|_�Q\*AE�_�����U8�ְm.��_�mӬ��6j@2�A��-�2��^�A�(�#�ƌ�c���w��,C>F���%��'�+���ҝ���MPr��ֹE����}�2���kk'ɇ��Zz�JВ�z�ljU�� ��䙉Ĺ�xx�k�N�E�J90)��vU3Y �)�o(�_[�~�F0L���ɭ��O�[�\C�>|�Z���^�jV
��8sZV���D�kjԖ�x�.4�Vfj�����g{}�\r����I(\���2�6r�*�;Y=#y�}ˉ�l�wU*�{���1-kh�js%��,����ɘ2�������E �7�s�NQ�ksq���&p'ݍ)�n;�Zu`�ā_o����t� �*���/&�͒ =vN/�
��s8dݶ1F~O�iF`�.�o�`�JA[8���p�����R��)j��Ee�*b�_X�?"̯�m�͵ye4a�,���t�ӵ���+f�v��\Bk��2�����:�����J$5��icms��H&��W��ӎ�`�G�^I-C���YP�!3%��G��Q鋽Q�F@��	��]��XSlpG�w/;�Ă����^����,��
H�}��gB%*<�\�c۰�X����m����a�g�g�3?�HwL��\u�on���/{1�߱���Ų��,�mW^��j��O���E�-������� ԟ*<�B� V�cm��Ir���-@�'[��*��A���~qՍe����*���?�!���w�B-�|V_d�	̂ӂ�{��Pf���L��<,$.�%�p�L�z�,�E��ˡ<V��~~�bM��L�<��-�B�@r�J�$_K�W������$�(s_�{G�M"��.lN�	�ɲ�dg5TT.I��(�{�K]��� �9�'y��.�.�L�J5�6���&xg�9A(H������dL�o�Ju����v�G�8E�ƈ�0����zr(a�0NBu��f�#K�0�:���>Ӡλ�;$�XK��W9@I�De[]v]r�`��[	a;�=�=r}��SqƔ���~�]��ٵG%������B�ޙ�)�O�CB�W��:.���A�-�p#F�'��ou{4���,�t���Ć����O'�"�;��U��nU� �P������_�Y���F�8�s�ɣ=f;��$x�6�q�l���v�`���Q�E� 0[e2$���p�.R���(���A�+�0=�-�N M;�2����m�B�����o��i�Q�����EU����oǰYn}3�H-��|�i(��T'�#t�wG�+k�q�h����v�����.���	`���y	��#�]U^��ة�
�,�����W�kg����2ד�}����aG��2:�#�M�m�@G�p�T��T?��g�4.�7[=�2��f_2*qN.9~��4X�+y���q��ψ�.������fr�<�'	���	O�b���\�p�߀!]�y&�>�C?VD��:��˄�J�`Ev��.�*u�c���·v̄�k�'ㆴ0>�+n��W#�S��x�`��M��^ �Dp��p�D�����1	�aV}&���c\9��0�a�R�)�A��2%��u�xBE��FL� J�������5.a��l�W��r�g;
CW��C��Q�����桫q�0��a~�uw�����R%�2���"��6_�7Iw,N�B�զ�!� �&7q��G�=�y�!V�-�j�$~��S矌st_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;
	int i, j;

	/*
	 * The caller must hold the graph lock, ensure we've got IRQs
	 * disabled to make this an IRQ-safe lock.. for recursion reasons
	 * lockdep won't complain about its own locking errors.
	 */
	if (lockdep_assert_locked())
		return 0;

	chain = alloc_lock_chain();
	if (!chain) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAINS too low!");
		dump_stack();
		return 0;
	}
	chain->chain_key = chain_key;
	chain->irq_context = hlock->irq_context;
	i = get_first_held_lock(curr, hlock);
	chain->depth = curr->lockdep_depth + 1 - i;

	BUILD_BUG_ON((1UL << 24) <= ARRAY_SIZE(chain_hlocks));
	BUILD_BUG_ON((1UL << 6)  <= ARRAY_SIZE(curr->held_locks));
	BUILD_BUG_ON((1UL << 8*sizeof(chain_hlocks[0])) <= ARRAY_SIZE(lock_classes));

	j = alloc_chain_hlocks(chain->depth);
	if (j < 0) {
		if (!debug_locks_off_graph_unlock())
			return 0;

		print_lockdep_off("BUG: MAX_LOCKDEP_CHAIN_HLOCKS too low!");
		dump_stack();
		return 0;
	}

	chain->base = j;
	for (j = 0; j < chain->depth - 1; j++, i++) {
		int lock_id = hlock_id(curr->held_locks + i);

		chain_hlocks[chain->base + j] = lock_id;
	}
	chain_hlocks[chain->base + j] = hlock_id(hlock);
	hlist_add_head_rcu(&chain->entry, hash_head);
	debug_atomic_inc(chain_lookup_misses);
	inc_chains(chain->irq_context);

	return 1;
}

/*
 * Look up a dependency chain. Must be called with either the graph lock or
 * the RCU read lock held.
 */
static inline struct lock_chain *lookup_chain_cache(u64 chain_key)
{
	struct hlist_head *hash_head = chainhashentry(chain_key);
	struct lock_chain *chain;

	hlist_for_each_entry_rcu(chain, hash_head, entry) {
		if (READ_ONCE(chain->chain_key) == chain_key) {
			debug_atomic_inc(chain_lookup_hits);
			return chain;
		}
	}
	return NULL;
}

/*
 * If the key is not present yet in dependency chain cache then
 * add it and return 1 - in this case the new dependency chain is
 * validated. If the key is already hashed, return 0.
 * (On return with 1 graph_lock is held.)
 */
static inline int lookup_chain_cache_add(struct task_struct *curr,
					 struct held_lock *hlock,
					 u64 chain_key)
{
	struct lock_class *class = hlock_class(hlock);
	struct lock_chain *chain = lookup_chain_cache(chain_key);

	if (chain) {
cache_hit:
		if (!check_no_collision(curr, hlock, chain))
			return 0;

		if (very_verbose(class)) {
			printk("\nhash chain already cached, key: "
					"%016Lx tail class: [%px] %s\n",
					(unsigned long long)chain_key,
					class->key, class->name);
		}

		return 0;
	}

	if (very_verbose(class)) {
		printk("\nnew hash chain, key: %016Lx tail class: [%px] %s\n",
			(unsigned long long)chain_key, class->key, class->name);
	}

	if (!graph_lock())
		return 0;

	/*
	 * We have to walk the chain again locked - to avoid duplicates:
	 */
	chain = lookup_chain_cache(chain_key);
	if (chain) {
		graph_unlock();
		goto cache_hit;
	}

	if (!add_chain_cache(curr, hlock, chain_key))
		return 0;

	return 1;
}

static int validate_chain(struct task_struct *curr,
			  struct held_lock *hlock,
			  int chain_head, u64 chain_key)
{
	/*
	 * Trylock needs to maintain the stack of held locks, but it
	 * does not add new dependencies, because trylock can be done
	 * in any order.
	 *
	 * We look up the chain_key and do the O(N^2) check and update of
	 * the dependencies only if this is a new dependency chain.
	 * (If lookup_chain_cache_add() return with 1 it acquires
	 * graph_lock for us)
	 */
	if (!hlock->trylock && hlock->check &&
	    lookup_chain_cache_add(curr, hlock, chain_key)) {
		/*
		 * Check whether last held lock:
		 *
		 * - is irq-safe, if this lock is irq-unsafe
		 * - is softirq-safe, if this lock is hardirq-unsafe
		 *
		 * And check whether the new lock's dependency graph
		 * could lead back to the previous lock:
		 *
		 * - within the current held-lock stack
		 * - across our accumulated lock dependency records
		 *
		 * any of these scenarios could lead to a deadlock.
		 */
		/*
		 * The simple case: does the current hold the same lock
		 * already?
		 */
		int ret = check_deadlock(curr, hlock);

		if (!ret)
			return 0;
		/*
		 * Add dependency only if this lock is not the head
		 * of the chain, and if the new lock introduces no more
		 * lock dependency (because we already hold a lock with the
		 * same lock class) nor deadlock (because the nest_lock
		 * serializes nesting locks), see the comments for
		 * check_deadlock().
		 */
		if (!chain_head && ret != 2) {
			if (!check_prevs_add(curr, hlock))
				return 0;
		}

		graph_unlock();
	} else {
		/* after lookup_chain_cache_add(): */
		if (unlikely(!debug_locks))
			return 0;
	}

	return 1;
}
#else
static inline int validate_chain(struct task_struct *curr,
				 struct held_lock *hlock,
				 int chain_head, u64 chain_key)
{
	return 1;
}

static void init_chain_block_buckets(void)	{ }
#endif /* CONFIG_PROVE_LOCKING */

/*
 * We are building curr_chain_key incrementally, so double-check
 * it from scratch, to make sure that it's done correctly:
 */
static void check_chain_key(struct task_struct *curr)
{
#ifdef CONFIG_DEBUG_LOCKDEP
	struct held_lock *hlock, *prev_hlock = NULL;
	unsigned int i;
	u64 chain_key = INITIAL_CHAIN_KEY;

	for (i = 0; i < curr->lockdep_depth; i++) {
		hlock = curr->held_locks + i;
		if (chain_key != hlock->prev_chain_key) {
			debug_locks_off();
			/*
			 * We got mighty confused, our chain keys don't match
			 * with what we expect, someone trample on our task state?
			 */
			WARN(1, "hm#1, depth: %u [%u], %016Lx != %016Lx\n",
				curr->lockdep_depth, i,
				(unsigned long long)chain_key,
				(unsigned long long)hlock->prev_chain_key);
			return;
		}

		/*
		 * hlock->class_idx can't go beyond MAX_LOCKDEP_KEYS, but is
		 * it registered lock class index?
		 */
		if (DEBUG_LOCKS_WARN_ON(!test_bit(hlock->class_idx, lock_classes_in_use)))
			return;

		if (prev_hlock && (prev_hlock->irq_context !=
							hlock->irq_context))
			chain_key = INITIAL_CHAIN_KEY;
		chain_key = iterate_chain_key(chain_key, hlock_id(hlock));
		prev_hlock = hlock;
	}
	if (chain_key != curr->curr_chain_key) {
		debug_locks_off();
		/*
		 * More smoking hash instead of calculating it, damn see these
		 * numbers float.. I bet that a pink elephant stepped on my memory.
		 */
		WARN(1, "hm#2, depth: %u [%u], %016Lx != %016Lx\n",
			curr->lockdep_depth, i,
			(unsigned long long)chain_key,
			(unsigned long long)curr->curr_chain_key);
	}
#endif
}

#ifdef CONFIG_PROVE_LOCKING
static int mark_lock(struct task_struct *curr, struct held_lock *this,
		     enum lock_usage_bit new_bit);

static void print_usage_bug_scenario(struct held_lock *lock)
{
	struct lock_class *class = hlock_class(lock);

	printk(" Possible unsafe locking scenario:\n\n");
	printk("       CPU0\n");
	printk("       ----\n");
	printk("  lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("  <Interrupt>\n");
	printk("    lock(");
	__print_lock_name(class);
	printk(KERN_CONT ");\n");
	printk("\n *** DEADLOCK ***\n\n");
}

static void
print_usage_bug(struct task_struct *curr, struct held_lock *this,
		enum lock_usage_bit prev_bit, enum lock_usage_bit new_bit)
{
	if (!debug_locks_off() || debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("================================\n");
	pr_warn("WARNING: inconsistent lock state\n");
	print_kernel_ident();
	pr_warn("--------------------------------\n");

	pr_warn("inconsistent {%s} -> {%s} usage.\n",
		usage_str[prev_bit], usage_str[new_bit]);

	pr_warn("%s/%d [HC%u[%lu]:SC%u[%lu]:HE%u:SE%u] takes:\n",
		curr->comm, task_pid_nr(curr),
		lockdep_hardirq_context(), hardirq_count() >> HARDIRQ_SHIFT,
		lockdep_softirq_context(curr), softirq_count() >> SOFTIRQ_SHIFT,
		lockdep_hardirqs_enabled(),
		lockdep_softirqs_enabled(curr));
	print_lock(this);

	pr_warn("{%s} state was registered at:\n", usage_str[prev_bit]);
	print_lock_trace(hlock_class(this)->usage_traces[prev_bit], 1);

	print_irqtrace_events(curr);
	pr_warn("\nother info that might help us debug this:\n");
	print_usage_bug_scenario(this);

	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
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
		lock->class_cache[i] 