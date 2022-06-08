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
  include/media/v4l2-subdev.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
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
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/media/v4l2-device.h \
  include/media/media-device.h \
  include/media/media-devnode.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \

drivers/media/i2c/tw2804.o: $(deps_drivers/media/i2c/tw2804.o)

$(deps_drivers/media/i2c/tw2804.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ude/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/segment.h \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
    $(wildcard include/config/BUG) \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
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
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
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
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/includeg&˃{��~��)Z�7֎��u�]��T����r�[Q9�A��n��3��v��Q��^1� ��Ι��<�?�����g2@������8(;OYH�I2C��@Hj��Y@�����q���t
Xp���?�fx�0���.��l�Ѡi.�gɔ� ��(5B
���D��;dqh����3G-U�@	�T�N����<����YoU�%-��X�ӸX�����W�� B�Lv���q�U�D\��JCﺹ�o!�h��8i�Y�Om�p�$��@CqW�6�-C �B��o�;�k��M����լ��Ǩ(���Y_c���3�tX��JB޽���Gn}� /��4%48��rt���r�i��O��j����>���S�u]*f	��3[y2�X��5�tĐ9b�~�Ƣ`��N�|�ēT4i���s����Z�/�Vk��|P�Ӊ��B�����`Ц�!z!��?��;�WU�J�t��󠴆)�#~*�z�ÿ�>�k�SX��������2<����2="����${�n �i^;�'��!�t�i�*��5�ݷ�s���1��B�+�v�Xl6y�s; 7.�m���Y[�vl�<�3��Ė��3B���a"���6��#D�t��w1
�frj��D?_z�}!i��%�)�e*j�Z���2�&Üߋ�6��>�h��X��3-ŏ:9yB?MQ7g��A���~�� ����C���f�x@X�tK��_��=|�N��۹_�)Ժ�6�f��yRlY���c(d1T��8��١p��� +�\T�:��<b��
%T_��-D��2�D��ino��jʰ�n�4敱�;�Hz8 #�2{-ʾ���D@�n����
����2ڏ���]V��P}�8M�s|_�c�8
\(L��2�$Q��?�3���lw�5��e:j�Q����Ue���$df�0�e�Ndr�ld���U�EI��C�9	��X�:���2@����Q	�M���`]����	��$��(�:����?H
�p&�Y��
vV�s�d�vee����D�g�爌$�da �q��7�K����\[�[�MS���K�gecُPe�ńH�
����bS!�_��7� I��x�G����߇�' �����c�e�-Ø>
�QD4��:�p�h0KV�,8�y�#&Oo(�IL��5������5@��g,�5:����dӑB)Nw�M�a���m���}�
�>g���Y����M��+Q&},\9g�P,�L�������^i
\�_G�\��6r�4=�P�b��ʨ��[�C6E��3���2�f �+/!������qͻ���l�����
��i���~%���(H���A�v����^���"	�����/`r-8�����D�N�3P�Йf������6���m�0�J�7m�̤1���&T)���sF��hs� ��r*�����u�ec�w`���YB{,�oܶ{|l��7���n,�����m��aEr�p�}�sӣ��&���Ҫz�]ճ7g�N�`%�4��}zPy.I��'�$g3��AJ��~xQ�Az����k/�$
���w���#�9,?�4 ��)��:�7
���\�֞q*��8���hYe���"�r��mor�9���!ӫ�G�.�F�	�#��<�U��B�^5LVQ���<�xu��c���<�t%'�e�A���
@��l�a*�jEA�	A6IxyPB��?�M[��Y���5��%c.L ���֑
��z?�󝨕6�I�D��nXA�מ�hzԳ���!��r���4���d.CB�>u���Z��l���ڰ�j]8���+�:Ak��}mk���V(�|�?��{.��O5t&/�S4pU�n_J�kVш6�j�mDʨ�SK�L�.Pg� �;��NF>���|n7�z��e�+Q�1wB����ڑ��_�X���p�.�(�Bq񔩾�0������E!�n��
+��Q5�x_�5��o���d\1�;-A�5��Vt�} ��0iA��p�������	��X��=)d������}2?*�di�J��b,���L[�O�},V��k�a���a�3Eh��G���M/�)�9�cE+H�m���t�5
,�oe�,����n۪I��u��~�'ځ4�>�>*�Z�����_Ad;�v��NBf�M�����~}�@�eWA�b[�?yA���ϕpG���6�����PԬ춘�e:�iJ�H�w='`�0������(���f�3�n���xj����+7����}�F��d�Q��4㦉�B��"Ob�I�T.��1�9������R���mə�F�nڵO�Q�rs�{np�b6���:P�I���`6K���E�`<�_��0
������Q<ӮL^}c�i�3�oJ���/o�U���
Һ�).:��&��� ^�����M�K��7]������8ж� �\�f�[�X�K���f=F3��U�ܵ���$cAq�Z��`;=^ѧ�z�-��М�n�CF2Y�%�S-�h&�R��F�3�iP��D�7-�3pl�7A{�Q**��&�����8y�b��W�Ի�-`�-�k:�?�|��w�$,B>uw��͵�LR�.]���t�b�A�zl��o7�l�0%�1�p��6Y.�����Զ�3$��$��0rz!���%�rP�.�:�wa^u��D`L���8l�;*����A��'`
\F����˸S0�`|A�虐�w��j���]���g��܍�pAj!вW��f���Y}}��`t���Q$L����#ɟ�zN��a=�
uq|��E�-S֮�]����u��$�a�ۀ[s@���U{خ��
��T��@�ν,zg�&�,��@} 8'<m*[�̺�,�4p���|�2%Aw�l��ę�&�؇^	��v�,��"��Y��+˶�3�l���VWx����S���+�"n�a-d٫Go���4��S�C�*��{�m6��~���bq�j�剪>7	E�r���DΚ0
``�>^]kA9�n�PK�^����T��Ϳ�T�����R�E��"W�D�E8�.��"��o)_PG9��� =�Z��C]h�>Si9b��)BD�@�@�y�s�{dMOW�t�%����/���q,��]��#�Y�YdK���O���F�1���[IY�3�Ig_t5s�H�H�XJ�GQ�v��3��xV�D$��÷��K�^��#��*[E���M��?�d�چ��"~�����(�Z?>��P�޸I��$�
���� ߱2�T��S����R���kP��=6ۖu�T��7%W�jZ=�T�֖���cַ<��"@�n�y,�
��ۚ��:$f�ę3��2/�ι���)ks�}���7ƴ/��8���-�H>
U�p(�3Dd|���L��&[qO��Gߏ֪f�ˏD_�
����h��q)��w��l�n�*}}<j[)�������Х{��Zռ��ե���8���z@��A��MY�"I��xW�]��@�"0.�Ѿ���k1��vRa3�-N��}�u�R���Z��wΠ�oNi��K�Q�Nv��^
SPs��J�����M@��5�4�
�-��ɝ_ ��}1E_.|4�{�����_-9ý�@�/b��H�q�u�JX�5�g�b��~�d9�C8��Ń�S��|��sWx��5uN6��j-���j-������Hn&�"��t��y�Fxk�+�����P����w�Y�
�- ��,��UT4\|bz�^<,p��ԇ�:�%�+X�x�۱t5��Fܣ�W���@ܲUe;{�e����c_�C������3wK{9��.7Z�s�ʏ�#��3�߾bߤ��=;҉[�(U7�l��k��h��Z��3M�ОB^���T�ր�B�z�ܘj��=" 
�-� �5d v�ȝ�r�v�����_foj%�J�0�\�:���	bى��x��Y�}�1�ޟ���&�=�Y��c�]�c�Lt9�����:.����x�п���0p�B�A��9�sᾇ����j���W�v�SI�U���KG�n.�-aj����v�o�~j��M#"�k/x�3b�%��$ګ���L�>�� ��i��=8�$Կi�󍢝ӕY�vY
	�y��Ri��;#
�����ev�Nt�{e����Mhx
'ړ���l<<����,QMl|�'g�Ao�=*�-O�Y��-�eW &w)������t敟� C�����C�`P���p�?Y���
�����չ��~J�l1�|�Ɗ�)�Ƥ��B�.^�
�f4C���<���[$^C_3�+K�[�V�S�c'�����;%
��Gw�b���T�r��<�gl�Ҩw�]�iE�*��c���,���ޤ�c��"�$�
 ����`��p��E`뿚�A���3Xh-)e=Gc�%.a'�zns$U�|W{x�9b�E е�כ�K����v�񴅄��4e��;�?�f�:*��o??RJ��͑au�}%������D�j�c�p��}��٧�%����M��vf�@;j�o��B�M}����7AuN㕷��3�Rp0�wˣ�2���	
�3�d����`粷k�g9=nn'
2�X��OA.KX��*��t���J�_^{���HV��Ե|�FE�ʊ������Bg������lGL�>:��½�Pٕ8��Xb�@�R��3��6}����Yϡ|h*��� ��ʯ�����a1~��}cV��p�lAa�Bڱ7�bM���Ϲ,�~�G�H4j5�KU���b�M�e�;����t �dP?	�T�%)�-{�Pr����,�j���$�s�5lug�䠖@s����T(ڼ��ٕm=V�ލH1�#���kR�K�}�G=͕gk��� ����"F�_Vb{�	u�+�L�Ͼ���0��z��J(ޑ���h���>�넙���ØJ{^���R����5�;3���J�j�:�b�?�s�BRw}��4/VEr]����]Q��&L�@z�w�ؿ��v�t�Q;O}S�ViRo2%$a�3ᠺ�P9 ��_쥞7�ז�dy������&�C�h�3�ز��Lh�d+�������wQ��2tUZ��˴8�y�ZU^����`��蕲���64>_�m�_�1�����5�k�`"�#Ա_��"7���S�2-T�m�˺��ȑ����/[2�uYd�R{$��b	 ��gh���W��B���˒��Mk'sU�>�#�5
�g�9��zBb(3�Q���q���F��/�BI���Z�MK�F<[�P�VWD>I�#�_�[����Ih[�^h�'K��ߟ�ߨ��4���#��}���
�I�B6d����TE)��x�L�� ,p�*�p�ew�H�)l��������พ�طk����_��)���[!%qqpʋ�������d�-H�O����;��V�� >^����p�4�Y%+9Q��k�X���N����8bU�M���R1�5C����DG��43��C�ʿ��uFG����&a����'v�U���.}�Pn2=�?�EN-�(3�x�3>���Upgi7�f �蹽{���'���\�S���hnau�����{�f$(+S�"�>�s�&���:�����$p�M�+'9B�8��.��Sd��%"}����v��ڻ���m���'��d}�2�c ���ŜA���X��p�M4�����2&�Y�y�ԥמ]%K܈i�Ą�_�~�����C|B��9J\��L�d�+wr�����>O��	�6��0r	UV��A�C�h�V���G����5�G�GI69|\.����]�$�a[6Z(vp�]�v�g
{�,}��Pi|��Y�E07S��	�#ܛT.�z�<G�`��oĢ��L�i�X����V�n*Ԙ��d�2��%���҉K�9��$�)Oq,c��w�zW<U�GS��g� ���-Ʋ���V׮U�m�j��t:���O�T�:�i0���v=>�8�lRt��|�yao��]K��?�/kJp�-b�"�!�O�� I�'Ѱ�m�ۖ���-K�����'H�@|*���Y"u�.��z[R��������Wtq!6��Uv���	n�ȔFr�@�ȏƢB����ޱ���V���=�&m���h�@��_����r'�T_�^`��K�5.q�ǉV��#0͹�g��6��9�6��dm&�B����~L م���:���+[��	�r�Ͻ��h	��EkL�������Y�\H����a<XO��������=�i
�ku{���ۍ}W՛��t�h_|S8�X"
&�{�h1Y��c�ER4C�yHw������
�Z,>�&��еv����R�Ò<��cRl��t�\��M$��a�ԧI�0��Ɖ��rC��;�Q�:��h����Գ��m���cFD�4̎�]� �ϡgzj��W /��r�r^e��SJps���L_~��,D69�''���7���U��+��8&~	�~�!���rgL�>p\�5�
SS�i=�ά��@�&w!��le�@X.�ws�$&����=��}P`� �S�#�~�C����ɛ�ب�����
��$Zk��� �t�a��#�qO��Q���&�w���@[��2�^a�\j�q�gε�[m�%�jA�t��@O�����o��bO+a��]��ƨ�#z؞���w�ŵ���&%eD	>,�[d^7��0�S��P�ˊ�j5�oJ
g~�X��G1��vrFa��mM����s���r�rI� ��܀���D�2 �4Q�l�A߲
3
CB����ϑN���}J_)����K{+
�1l��|y�-��g���	����ǳ�j�]�8|Rà�\��~�a�w�|�+�^b���-���d%���
���8�|���l��vc�&����=�R����ǱG]��$w��<]��и5���>#�+��Enw��3�֙;ͤ1��c�q�1'q���*�!�!
n�!d�01�kB�0D#륦&��bx�ױ� ����$�a���sj���[h�ݼ�j�U�}���w����.���<��bANO
G��4��y��:�X}"�����5DtU���]T�����f*p�
��xHp�s���hč">�e<��/���a�9
Q#��a��-�
#��RG!7{��&��D�:5u�y�����;�Ҽqk��� ��b�Q_M
~J� �] �tF���v�zȡ�ϹiTV7^��["�l�D�?��`��\|�}ʂR'�7�����j���^J�
�Pְ��k�W�Z��s�g���' ���N��
�pB��u��;�uIW���^Q{�-� ���o�+�%��m{��O;�-9��XڻN�Bp�9�0�`V�n���AZ+K� ��q���EX|R�@w�2�'���}`;���T�f�	��KS8��=���cv���6��җ�I�B�hq�LѲ������T���,��.��o�$A����ݭ�+	c?c�g�cq��U��*(�rW'����V="�3=.�=��C�wO;�S�et���]'���]B�@�#d��|+�#�`?��j��}�B�%��)?R�
@��#� ��_8�X���!��$�4����2?�C-�4pJ��:ZO���N��N+S��z��b��(vp�
H,O�$�R͑����R ����>g���"�XB1�K���W������P�_�;����K�M6��eV�|��&���!(��X��S<�MW�<�x���<�W�|3���_�לO��gw?]VOg�p��[�$�^T�֥4�f�5�DU�<沛�Z�
a�Q����您9��;[Jc��!U=>�\>@��D��ݮXCj�1�~)���u�S��۞�7�; SL0���I%0�3H���Y������
Hȥ�g����Z��8z�r�Ûh���\}��,�
`��:v��]_��n2J5�"��>g��T KI��t�
��l%͖V�x��	e for the next lock_list
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
	excl |= (excl & LOCKF_IRQ) << LOCK_USAGE_READ_MASK;

	return excl;
}

/*
 * Find the first pair of bit match between an original
 * usage mask and an exclusive usage mask.
 */
static int find_exclusive_match(unsigned long mask,
				unsigned long excl_mask,
				enum lock_usage_bit *bitp,
				enum lock_usage_bit *excl_bitp)
{
	int bit, excl, excl_read;

	for_each_set_bit(bit, &mask, LOCK_USED) {
		/*
		 * exclusive_bit() strips the read bit, however,
		 * LOCK_ENABLED_IRQ_*_READ may cause deadlocks too, so we need
		 * to search excl | LOCK_USAGE_READ_MASK as well.
		 */
		excl = exclusive_bit(bit);
		excl_read = excl | LOCK_USAGE_READ_MASK;
		if (excl_mask & lock_flag(excl)) {
			*bitp = bit;
			*excl_bitp = excl;
			return 0;
		} else if (excl_mask & lock_flag(excl_read)) {
			*bitp = bit;
			*excl_bitp = excl_read;
			return 0;
		}
	}
	return -1;
}

/*
 * Prove that the new dependency does not connect a hardirq-safe(-read)
 * lock with a hardirq-unsafe lock - to achieve this we search
 * the backwards-subgraph starting at <prev>, and the
 * forwards-subgraph starting at <next>:
 */
static int check_irq_usage(struct task_struct *curr, struct held_lock *prev,
			   struct held_lock *next)
{
	unsigned long usage_mask = 0, forward_mask, backward_mask;
	enum lock_usage_bit forward_bit = 0, backward_bit = 0;
	struct lock_list *target_entry1;
	struct lock_list *target_entry;
	struct lock_list this, that;
	enum bfs_result ret;

	/*
	 * Step 1: gather all hard/soft IRQs usages backward in an
	 * accumulated usage mask.
	 */
	bfs_init_rootb(&this, prev);

	ret = __bfs_backwards(&this, &usage_mask, usage_accumulate, usage_skip, NULL);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}

	usage_mask &= LOCKF_USED_IN_IRQ_ALL;
	if (!usage_mask)
		return 1;

	/*
	 * Step 2: find exclusive uses forward that match the previous
	 * backward accumulated mask.
	 */
	forward_mask = exclusive_mask(usage_mask);

	bfs_init_root(&that, next);

	ret = find_usage_forwards(&that, forward_mask, &target_entry1);
	if (bfs_error(ret)) {
		print_bfs_bug(ret);
		return 0;
	}
	if (ret == BFS_RNOMATCH)
		return 1;

	/*
	 * Step 3: we found a bad match! Now retrieve a lock from the backward
	 * list whose usage mask matches the exclusive usage mask from the
	 * lock found on the forward list.
	 *
	 * Note, we should only keep the LOCKF_ENABLED_IRQ_ALL bits, considering
	 * the follow case:
	 *
	 * When trying to add A -> B to the graph, we find that there is a
	 * hardirq-safe L, that L -> ... -> A, and another hardirq-unsafe M,
	 * that B -> ... -> M. However M is **softirq-safe**, if we us