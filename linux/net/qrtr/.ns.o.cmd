ldcard include/config/X86_INTEL_USERCOPY) \
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
  include/linux/of_graph.h \
  include/uapi/linux/v4l2-dv-timings.h \
  include/media/i2c/tvp7002.h \
  include/media/v4l2-async.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
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
  include/media/v4l2-fwnode.h \
  drivers/media/i2c/tvp7002_reg.h \

drivers/media/i2c/tvp7002.o: $(deps_drivers/media/i2c/tvp7002.o)

$(deps_drivers/media/i2c/tvp7002.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_kernel/bpf/cgroup.o := gcc -Wp,-MMD,kernel/bpf/.cgroup.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned    -DKBUILD_MODFILE='"kernel/bpf/cgroup"' -DKBUILD_BASENAME='"cgroup"' -DKBUILD_MODNAME='"cgroup"' -D__KBUILD_MODNAME=kmod_cgroup -c -o kernel/bpf/cgroup.o kernel/bpf/cgroup.c 

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
  arch�$9_q-�rz������$X�zb!��^�F���Q\�y�n80��Â�()bڬ$~^����y��\@W �27��Q�j���;&�J�x&:̌͋�����q71x`�h���UKp;ޤ�Igh d��c������2���:\K��Y��;�����9U��Q�eIri�QP�EJxj�Z��J+�^�a���G�AۡU`�R!�d4��nw�U���%��?�c����jx�kSnߐ�d�ū=���SMs�D����V|k��O6uʼ��0����O�NZ������N�q� \����sÅ�ӧ�E�rD�~ۤ�ޛ����+�Y��H����^���8���� ��U��`���*��M�!��;����vi�,�Ĝw��9{ݒյJEGp�
wLK;�eҖh9C�ޣ,�ewP̕�֍p��+��;��)z�) �T;=uT��iv:��(p�q�$産�/��/��I��o?� ��*�`X/�]����7��5���\���Au���X�q9�[�ch)}��O��0�
Z&�f0i��Z|�;�Pj���������\�	�b����5D}���r�����ܐH�f���ؚp�	u�$�,	z"Z�;�Ct�*wD�3^0��\J;�GW��5�u�~J��S��5���c�{l����ξ�m���?��ƃ,[�_��1�D����O�J�?z ��U�;0\���Z>�C�N����:ڰ�tm0`�)�Np9+awk�a�Y3~N���e8�I�[S�+^��C@��|����eb�'�������9��|���z�.j��>�L 3Y��Nl���a�� ?�u	�![��1��V�T-��|�lyw4M��$�6�_D�&�ʱ��p�W�Ko���_������";���-J ��;�:n5�k��~�G�尞>ం�����͹C�m�F�g���Է�o����t�Yj��<�J_sB)S̀���:����+�W�a�P{���m�����w&͒�̈���Z��u��-��3�G,�=VZ�e΁I�ϛ�x�'����A��!�����:<K$��q,W�� G�������.����#���njO�y$b.��K^`�9�=1r�_W�����T�lb���?��@՘��\�����ǩl�馐0�F�I�/mr�"��;4���l3;z`��JɣÊ��Ή���l��l�oS��������O�����U�aܢ����C�JL�_�M4�ߌ�qJ�`���ӕ�Y����_
)���S��������籭�k섿9�$V���f�����/9�+ɒ���p���� �mI�{�ߟ*C�JKY�K�X���� ��X�Wz�lq���:eG��.�w��>�sE 8(��聹G�K���t�>ǌ8x9u�F��xZ��,��@k�G'W<��X�G��^KGYO���;n�8���N��29��ޣt��Qᇋ �\M���EQP�6�P�K�!Iú"&�1k�v���H�v"9Zy�>�>���w�N�e0U��D�^���]��� �a*����hC�{�`����)�W�L�D�c�t���0l��,ZoB��jgzÕ׫z+.<��@#rxD_;�6�`e�P�g%��.3m�`�d�����2��8��	�B��b�x�Ii3���]]�(�s�=(z>+#��WPFH�z�ޟ�rV<�خ2�uq0��~ON���+㶼z�P�/|Z����h����Q�ˁA�lB�}H\Z�ˋP�?gN,{��F�-	-���1�L ~Xh �aO)FW{B2y���&e�.SS�j�UX���6l�:���#.�%�xf7�h��>$��z����c5�$	��*�����f|�=<�e��ؔ��N�6��oo���z�oD�&�AO+���	H�����\Z�x�Z;pW��"�+ʮ;���U������٪�*�9n�d�nU0\.#��t�=A7V!�&K��KM�(d2Z��ބO�y�P#Y�dm(��S�,��;��r�4[:|�)m{`(t�Y�]r��}8V
&�_��Lv�����Bl9-b_�zY��^-ie\GL7��ٟ*,�!���c�Չt����>CuSm��Wm���g��A���u#3=j�f��(k������(G����h5��f�R�d�Y�XR�[�T	nO�����*I�����B��4���F1l��C�g��sRO��U��W�;/�@��
Jy;�2�߄��/����a8lj���:�Â����j 0�����rFYܯ��ʝ���<1SȱF�<��"�Y�j� ��V9�aH	TY�x� �&wW��E��Sy��ж� �(�>F�]O��p�}�[g�ɹ3�t1��n�֍<�D�>��Ρa�@l�NӨ@Dc��������4ؤ�
NaY-7@Z(���s��H�y�%�52����0�z�G�c#�{خy��[���Y����<m���k��~�������_���rʶ���I�k�~n�~�S�����Ȏ�i3�튭��(7t�4�T��Dݠ"Rè4C�w�-����om=Α���ƙ���igaMgz�u>UE��RqH�yP]k�D��楉���7�;,3����ڔJ�%�+`��s��3��Xtn��}_s�;�'�]�%�H> ӟ�D�iW�`:}XG���L�M�/:�|�e5
��C#�-�ݛFo�FqDDi���c\$h�1���w~U"��A��5���r�E����$һ]#�e������Gޝ�����Ҟ��+ծ��-zΐ����aJD�HX�&�(�	��Ӵ�h!�[���0�L����x09�1��':�3��K��û�t$Æ��\����W���%��k��]-�eA�C/�g�"v��vZ�s��O
iH-��]����U�ۄ��V��CT �Hr:�"a��)j�v����[�=�q��JH�q�!�:��RkDda����C��O'�^M��jð��C�|%���A��R}����7��� h�2#,�����㘻��,�r�Y��������gf9QG�9��.R�c尫8�փ�=�k�]�@)���S"1��F��	]�
��y���"U��D���H�8����AƲ}$��%�%��_Q @�;3�p�.�=��9����oY�F���O�b����෮�w�磑����>d_����b���]}PZa ��ۧ�3FY��C�:�]k�*���C�������:y�IQsO�m	��(El�n.,߂P7K���r,�{i��t�!CMSS�Q��y]ꀡ|��@g��$�9n��+����������x�
������U�8+?�V�eI�5E_�]����+)�L�Ώ�/�$(k���b����Ŷ)á��qҨ?ٝ�����3��-q�2I�>��3r3��
#8�$�k�Z@��I�/��	1�G+���x?�C��S��Y&qV�S6q�e�%C�n�Z{:���'����"��Iʰ_l���&�>	x^*�7i�
T¸ೝ���jht��G��.��9�x�H�w�*��\A����N-<o0`+3��ŗ�!.�}U�^jtJB�I[�1����IA":$���P4�Yx�2�L�o]iI�t8p��w�+�$_'83�j����pV=����J��\��he���8	w��f'��;�姁���>�r�|DO�aE#�H���vAo�G�u��8�aڤ����c8�E{5p�J�X ��"��[��\��9�|�ظ'�-r��dR����0��ʏ0�˜;�r9��/,Jx��AԹ�R��Wp�α{�q�c��
���x�~䶬wڶ\��A
)��V�*Jv��V��?l�,|i]��n5�D��O�p�g"��� �d��T��8;�)e�Mj���R��U:�)��X�5�I�Sf��PGJ7�scD�f��́R�3�5F_��dU�[%¨*��(�����
�w�dX`��8�W8}Oq̃������^O����5m��4+F��-ˆ%���eo�\��*�,�D����Z���#����j�V9� �=�P��C��B�>}|��k�9�RH]OT)���B�[s��u3E��q\p������unu̷����"�氎c�D�tV�;G�����W[��A��
�tNd�FXL[�Uh�����b�,�e\��E�����3&����l��;�I�9�[�Z,�JB`�B���%��VQ���{�������G�Mtx̑7�-\����d�*ˎT��>8\�{B�c���K�W(bVڇGJ��U���ٰ�6�-S��l_J�ۢ�f��H�աLFrq�"�9���@vƨ��"[�Ί����#7(O��v<U#�^m�2-�'���(X�_���f��9�2��ظ!qy���Dc6>X�!o��}� T�� q3�(kW6wk��@|�8]�s���%���k�Gste�^�`�X�Mtɿ��HM���1�kI�g��9��$�Sb�O�s�A�m.!9�
e�^ť��
9f���<��L��k����P���� 2��8�I*��Y���V���T��a�J�����q�m�r�S��tƽl��������D55C��^/$���&)��#	��PMyÎ�4�Ug�Y����&s�;�]�ϵ|�[rG�׿6GtaD��kl�\ n�?:Y��B��ee�{��h.����9_��]��'�Ų�IJ���W�$]��pSO<��q����{+� �:x�I�foZ�*�����y^���`��e�?��{���Q�:���fMb��cr�(i-��>�X��j�Je ��|�GW&��{i��}sk��a=b�����O��y�`oGl�A����~.,�P�Y�F����Ȟ4>�1����[�� �|��f�Ɍ���<\˪����aO���*ì��8([�Umg�j���^�������gO��������w�%��p���z9?F��$���V�'<��ƞ��Ŵ��r�X�
V&�HC�(��ELK4|Ď2�4�*����s8�������,9Q�-+1���ة���:��#�L���N�}\�gd� :����KŒj�Xs7ޞ�J�}�_;��B)O[w�"8���\s_0�&d9�C��fiJrO7b)K�����el,��G�y���({[�څ������5����+c{��-	?�29��g�����7��Bb<쉞��4�lv����Nb����q��k�=n� �&<��5��3�,.Nxe�n�6��Ъpm~i�.�T����x���E� �����N=A�P�e����_�W\�~S�nԁ(ۤ���(�75RiW�8g�����e>s'BG\a���lۀ"��`V8�D�`��W]i�:w���=��k2��̺��>���W@5���Z���t��JĔ���ۛn's�Fo�*���g?���lI�>�Q������<+���ן	C�,��92��	_�V�1���`�r<�T�x�͆��+H$W�= ޖP2�_� e��Z��*�ab���.��D7����<��ιԎ^~gM)j�BzJ��I�l�~n���^����6�h~��oO�G��b�J6#
�'�c݃�l�� �:>X�������=�)x�� �r��mCfl>T����	fM�g�Q��J��13|"��I�@4�B�������KcBc��ۓ��|�FZ�/W���( �8V��\^l��q]k��#n����R�Kq��C����ڭ�${>��h��^,�$�ph�+�,���(�y�=v�є�'�cn�1J0�Sb����c�U(˒����=(B�&�őu[�҆�q��)��_�	Ws_,'�B�RN��ӥ�f���A�[i��'�̦�|_M�W*7�&{'�6��n˚�	X���ޒ I�y�@���{����s9HijQ�4�/�/��׿����w.��߷$v�����C��
����6Q�tf���KȲn�Q��K��⸶Q��0P�h2�]B�=a�>��x�ُ洜t�t�Q'U�ŧ{Z˩t�eVo߱:Pv��%B��Þ���G����\���%>:���. Рhgi!��=�շ�3X2�W��Z��)-_vY˭"t+�~��=��l���4�o$�{��o��O�rU�qT����G�����,EՌd4��J����{W� ��L�������o����r:����Eދl�z��j��դ�

���}�/��^��P�d��S�e`}�sk�u�ᇽE7��Nh�K�ҹۖ?����k���J�Q���4;�C�����uR.�Yyc��$�>�#paGM"����/A~H�oz�Z /�Op�T�-ˌ+�'��=�񴸸��˼&���Z�58H�\�i�)7^֞i]l�(��Q�1:(����jGɛ�����ԛ�(����l�����I\��ѿ[��V'�D����yh��P_@8�*T��oF蒄���Z�dW�{e�[�%�q	!Z��t�&��r	&T9jKڝ\��Q�d|�k=�v��-L�p�l��EL�鮣|�t��@�?�&po�y�y���:�jt8Jj�?��:�-�(�'�t�'W��U �{U��k�=��0>ŵ�t2 �<��� ��V�x`$C�*B���y^��j�{���g�lAU��:�=U�~8E�����!߉�O��>�"��Kw��?1��̰M�X�Al}r3V�lm�;�{����0^��NDb�~֏l�2���5w}�w���aF[<8o�Cqw��#b���|�5��W�q�P�0F?>��@�A_�>���*- ��g�TC�;��r�]�������M6�x�tw�7;��X��f����w����\[S���~	�$5� pI
i��3�,s��J���SHgʱ�L	1V��П%]�ʜ$�@��W�f'�Ed,Y�0�r��%'Y����4���Cz�ƃ��0��X@�RK���~�lF$e�M�C==Mo�78�͓F�|��'��*�r;m hvM�-���S��z�%�@��0�W
���=pk�1|�J��]��"���ȅ�c������>��O�r�U���o�~�m�ɒj�n�F΀�-�G}aqݒ}�$�Yh�Fi�:�J�9_,�<��Yͮ%�~�'C[s�7?�V���O`��k�΍)@˺��_�>;m؉�W�U}n��h{��o�Z�&��5���b�z���#���Sj��(Z0_����3�b��<�N��D��2����w ���u7Tp@mRΒ��c���L�T��5l�^3<8�To�{j�\�<�g�4 ?V���\s}G/�\�^_�Y���ބ�K��6�L�� Pt���uGs��=�`����&�xUtͳ�P�'��~�#��o�N�?�H"s<�l�Pt���Ve�x�:����\H�J�G�U�5e0>:Q�T�{Ё�݈X�5�j4E_�h��&���!�X��'��T��#���b�<���tw�V�Hz�OK`��%:=:D����J��L��%���L�w�S�&(A58�Sή��q�wu�#޶od���w�0�a��ڔj�u46��^UZ�q�(��꙱N+���-�v\��|hZ�4���-�.�	!G�v�P���v��9������jd��~�Fܵ��Z��ԭ��._W�3B���Le>R��J�f��N#�Y�i�Aj��Lm�N���V���s�L������H:hRe�-�0��2""�.�lL��ŐZg2��dI0	h�i�D�n O+g g\�K^���3�) ��!/���#~��� �=a~@C,�h�0[xY�S���9��f��I���A�e"��R�?|?ޱ�чE�'L�{r���=i�_R[��e�����*�X]�g�m��U���l�h$L�_xL�+��/>�	�8�j;w?3Z�l��-xEXҬ����������WyU�"�5�+�yKv�����I]W}#
�Vk�g$ߓ��� ��(����Q��-p�6�p@�� �����&;�g��S�Y0�Z�{{��Q��C�o��EtIX,bO��|��]��{P[3"
���- ��Ҧ����"^Q
r�< ���Gӂ�So�~��$��&��{��܌h��+K�1F����S�u<�5���[��1ZU�#���?"5�a�JϷ	p��q��d	fa���?��7�\�VN�y��Z�;"W�H����t�f=�#�9�"������F��-gcZX���	����lbĮ8��--� �%Ύ9Ȯ��|Ԇ1�m�z�MKȶR X�s��`|9�e�c	'f�w�iϋ�)����b��z� ��h�H0$���`ƌ����J[;%G�ZC�Xo���y��'R5Z�:�J5��[���F��O�M�=��ĹF�D��Ҍ�2}wĉ ��E�!�����4:y�0 � u���� c���SBi��*�d�_V���Nl���'��_;��)bRz`W�ͪ�}�	1�*�v-�EN$���È��&��7㎀��j�P�3��q��wwD$R��$�?Q��̴;�^���%��.l��Ja���LF}�΀6�A� u�ח���^=���?QgْL�7�j�9�F�n�50�k��%�R̬���\[�/'��:`Ƶї.���*��A�M���o*Ys�Jߒ�j{�J\���b����Td�`W���w�f���S�_�"��P�4�`��u �Y��J�i� ���N�6p7"��"x���g���p���kǴ�O-�����Y�3�@<�M���3�A���:��R/�TI�o]�����:�W8#�C�TǾ���q��eC�S�|c���У��)�~�%y�����s�ho���o�i���(��$��j�>,���X�HL.�}����Wy"�x GO���5�`��,Mi���D>Ϲ3��֦�2��;��X�zr�o�8�81��dѢ36��G�H^��K3�����t��_Te�J;&�][��˾G��F�-I�8F��0qʭ,�8�����UB�u|�C��!^m�Tޭ4�����s&������+C҃� l�����x?Ė$vJ}���A8�T6����l��D��R��Kn�p�R�V�\e"���B)��__wχ#�Mu�B�/�oԭ6���=3o��pS���*�3��̘S�@�X�O�떼hEqPr�]H���V���;Ĩn���B�*nΈ5gd}�#�O�ٹ��n�����,���:�?Ѩv���̜r}1�agC^�S$��+�l)p�S�[?�!�}<��T�Ф������2���o�3r;��7���)��yϻ��\+Y(�-,{�'@��~:	,Ɨk T�]�D@�D؞��vؑ�B63�m׿W'�2PϚ)^�8H+XᲨN��C2����گe?�Qm��,�!�$pI,�J��)͖�g�`������x*4�wXa�u>-0��H�B앥7�v<h�e2��R�b��Ԯ��k-?���KR��%Ѝ�ס�� #�����)d��y���S���	��ƈ�{������8^/Tk�_�3��/A����q2�xi���R�б��Rf���W����w^���,-`�CHk.��}�$c�5��8�[`�̸�!7u���iG�7�����[���B�q�B�G�=���k�;�uo&ڽ�=j>�'Q���L��Rhu���"��Ge&3��0N�ږ��\j�c�ʹԚ��Md�ǌ&��յ�ߒ�������O)�`��d��ʝ�$�@�c�v54U��z���;*
��{ca�
�{`IUu9֚�é���Uo�7�c���S��o�L+��Z����Ez��f��,��21e����T�5eK�Ў�;�0c�7��L}E�¶P����J:N�FQt�KR�脅��߀�ˊԵ͈��b�&�yVc��y1�W��Xo��([��C]���b�r���yp]m�(pP�3�&dվ��ݯ=�#���������)�UA_���ADL(��;�a3?�܆� s���?�Î�5Jfg��\��g��>�c�ޡ0��
rFp-��W�&ϔ�G>,=�cq�����������4bK��3!$�
�w�F�v�P[Cx>��ң_np+W�_0��~��<l��;h�r(-�R����hZ0g0su���Z��:W����>�b�M�C�"��C�����"��ܫ{m1��E�)שa�m�%� Q��_+ ���0`�`s-�+E��r��u��^ɲS�,����i��C%�p.��@\@���X���
�P�o�r� ��A��+4`�\H��
k����͛�$׍���37W���7�+pEɏ}�����E���6{�Rr&
}[�l����=�+4��E�YBD��F�X�ax��q�����~#$j�5a�f�l�Rl���t��A9쥞 �S�(q�
m�j���3�.)����"���Z�>R ����g<�Ӷ�7�({�5>$�mDy�зȫ�a�o?Jɖj���=@����<ܫ�g_V7vp������M2����DT��Q���1�KE��21��1,��n��t/�a�����ް�:�Y�\����G�KzB* =0���\O���;�D|������P;.U�l���t�u�c�� !-�W(��u�����eh��SGg@�;����>V@��$��4w�e
�>����K�l��U��z�i:fJ�@XKp]f??q����s(���lO������M�����	��;�|�bߪ���o����_9�ڊ�����?�/G��Ш�ptMYb15n��.Q�1ẳ(R�!���8#��)�|���G��z /���3�����l��
�9a�e�C��O�}8]w�[%�cw��h�]���`!��-�<3L��vҽ�1@$	�c���F	�r��:7B
�
	x
�E)�ӊ��g	��ѳK#�"`���T�h�M�r��U|?�.I�솃摁�Ǳ�L�e�J�8\Y�i=y�̬|��x8n�
�o�泒v\S>�f�R �������������K�Ep�� ����?%gt�G]$�u������x"Z���R˸}ON���T�Q�y�1�N�����"��(�0�l�n����'^eT��h��|	�������׍�ְD[�,�C�*�F.��2cu`(���)�t\ae��|��G?&ė�	�B��0H*t@ȝ�h%bO�"���������Z�����Kb�"j�+B;j�u5����o��TY���2r
j��wh i8lr��ꉤg@vQ�#�ښtA�ǣȘ��Zfu�mS��_qT���oh@)<Ǖ7^��$��w.s�Í6}�zW�_���vD��)�͛b��U��«�0��@Zϒqa�nͅ�l'����W$�8�ǝ���:��J��t��ɼ׊`��L#
}�rQ�/�
�|��m@ߑ.;N>x�ZINL���M�>Z�*�p�Yqӏ o'�Z(��C�`IF���	`�iֱ���5�OhW#�IDߵ�iO�ǭ�O�z�Ӝ���9�����+�,Ѕ��d|Z2]V	��~ɩe��ϩ(����O��d��
42
�¾��<�9!���[�[�"�C6�N6ԏ�-*��:4�C|�HPe� ��s�rP�tW�RA��ς3����<A��MZv	It�:삘3H���H	�F��''�'��x����7�#3��ԫ�$;�/Cd<n�o���c2�>��e^l�Wd��yGw�~���P��U��2�hp�	��]ʀ�#��~[2Om=���w�Ä��Nu��@q�Gˡ#��yH���A�b��^��P���PW�/}a(�}��,��Nʇ�Qjh?�Z�I_:��^=:�������W�[�@vzX����֋%:J�0�ZU��˒�T|��N� /<�3e�ۏ����0�J��ǵQ>@t���׽�������Z�o��o�|�-PQ����n�i�F�N
�h���!�N�g�ٵH��=+yu�^Ӎ�JL��Њ׹��1��T�<	�����y���9ڹw�N�!v��[�lo_���Gh��wgg幗y���S���6|˸�/�����G�ov�!�y�|�J���r���r����^���M��nQ ���	�U���hU��k`b�ѭcs���8�!����jї>�!Z4{U�r�x��p(�t�/W�L�� �޷ôo(i�ܿ���Gl�ۢټ�3�}�.,;��h��1��w�e�=�r���z%���>v�Ja�1�=S,��<�+�g?�iX
o���J���So+,V����'By'��0��ު���Ý_j��ײ=��3�1�6\7�R6Ǯ
6]�WA��.��IA��qGõ��~� �����s)K
[%��¸�^@��S3f�FfV[�bc���6�R<h�Mlo3��?A��y3͍�k:"#��?���5xg�qό.*����5��p-��Dp�Y�ϗ�U���e�]r7�S	0�4Ψ9@[���oQ���3t�#���D�C=#�o]�؋W��1���<.��q�V �:�Z�v�\8Rp����溢���)p:���Xj<&��d�2U��o�ӽ�u:(�f����ֳK;5��-���L�y���m8�[D|"������O�#������)���f��	�B95�}P��m^�x�s"�\��PESzSJ���m�䤺7�.B:�l�=[��F���IK?�P9 (#A�q~'���ٍܸ|3p��\Ӧ&�L%`z(b�3(h����*��s&K�x�ET�<��~��T b���dGr���k���l��~�8h�:z!�)�M�(�Kf@ܦ
[<�v�sZ��=I�V�)��1֠�/H�Z���8��"�Xzn���_����)א�EOu¸LƠ{ݫ�� ^�Q8��|�$�NV�]��/�@��I[��Wfaw'��_D���>�-���h�@(-y/��K�gi�n��� ����h�N��^Z҇�_�k��;��_Ý%�l<�%����3^tRM:pp}�q��j���w�Fg�x��/)��j������A���"K�4��>]�1�Զn"�C����d�xHQ�b<���;���;m:slΫ�Ji�2Cv#�8��>Ɉ�PN�X����K�֦]Qq��&�<O���tO��?c�% [=�M`m3���q467A~��b57�#�,����;�iǉ!0�+!��,�M_����ߍj�u�Y�KDVUR����z��F�@
��,	G[�ǅ:MՂ�н������'���3x|���P� +��o�ӎ��w_��d|�M�
�.ަ�ň~7Օ��԰���<� �5�mf��坍n�kx(�:7 ,�Fč��� �4o��;rU�����\N��O��4�Tă��1Ժz�
1��<`����xRJy�Q���T��]�e�C�GT1�r=a���-[.~I�z������z��%r�C��^,�g3�Z-4^��.>qR�9��h�z�N䈥m� ��6f!c���e�CY��ۧ�DPt��6����~F��%]h�!ٴ�C�ѠحG�j������!K�.>�����@�,���`��<V�0�{r�/S�}��RW=,lR�r7���6rT�]HP�;�-J"��)���a�$O#c��q>�~�m�<�>��&O����˹6���H|NsYg�"_����`[Z�����+D��=cC	��i����fh���Ю����uo������KR)u��M`�d�D ��gR��O܄� �DA���|>p�MP�`�W�'�v�՚疊	cR?e���' H �UR���T%�|je-�����`߀�!�ϱ��;�u�L���P��e��N����(f��\h�d�@u;�͵��񈀮*����8�Zv��u�;����]�_��\��Y�1��ܖ~�jG8/�3���V�-#��<zk��C�W�c�jl��D��ap<Out�gu%Pw_Ht�����	�$a�쐲�0��G��,S~��7�RE4v Y�3=ȒRl�c�}Z��x�~�Ymz`�&آ���p�i�'�� e��C�5�GLtk����bP��*�BJ�˩Y3�U���^�kSh�h���?�pG|*bJ>q��J���kO:/Q��^�BI�jmb��ߟ���,3͹�`�#�,��+]6
0�ӝ$�����^b_3.+u"ƛM:K�C�ݴ��y�|��� ���mZ��S��4r֯_��<~@�b�X: ��Ä�M�~́��u;��m;����5ZU&�P�:�@�_����kgQ!�,�3��5� M��x��{F��݇O
$�1��=�b�H����>+�h߈��7!��
���㚕�CK�n9pTWB8$�m���>l|�}E>�j�WD��k�NA�K�Am
 [�- N'JE�O	�Zu�����áƃס�,R������z��/0.���ӊ��e%�7/=�Ҋw�e 7LK_�aSʇ��S�LOːǾ�YV��NG�lR�l[{-D^�9rm����#�{ t�-�ǥ���9����G}�*��݄V�D�Z��G�c�����ڷ�ÎwXA�\�P��5��!ju��0������h3��}�f/s�.��{���H�U�P�u�b|�ˣ�H�s��c}��Z���m�ݾ==}>Y���NU�N�@���f�Zl�+��v҅QB��'տJgmj}J�����h"�%�ܜ�P�	��Wh�ĕ5V[�C�X���O�K���7@Ž?�B���y�+�1���Z�BVo�{��*�P����N�a<W�i�%����h3�'$�H�Ny��և.�5=vq\>ۦ�X�����=y��A~�m0;�8��U�"���>�����}��?����a��Й�DZaexO�Zw~�mG\�c��A�v��z�K~��t�p�����7�W��Ձb�Β��<���Rr����:�mА�m[A`������h4��1-ahp^��)��S��*Hϖ#��v��1�-�dfߩ�vӛ��Ujz��c�$����q��q���pr��s='�k����Gf/���K{G����h��uݤT��z���A�?6����DV���Z���S���x8Z�m���FG&}｜9|��V:�~���\�U�i���>yHu5(JVj|�4�9Iڋp�bF�{y;1k9�aa�p�H0H�h��Z�XF�i��8ȇzl���2�����v4O�鞚�e��]�Dy3��O{��}��Lu�
C��<}d�b�x�<pH��[A*��N�/��\�SkG$I� �2�q�B���YF�,���h��P΄&Z;��Tk?O�.�h�	��ؓZw��܌%(Z����*��7��
����}���,����t�2B�om��c���f���BX�o�<(�]�w�`*|�1)FXy3'�Aӂے�6�>ɥ�d�X(!:Z��#EE��*�;�\��E�Xr�D�$=l,Լܹ�	��v��`���&���̉G���[�a���]L�N(V:��
���6q�X��&j�=�� � ��5�?�F<��oh`$��j�g&o����(ȁq���Y�4�8�d[X9ֵ���>[�� �퇯�;���0'�f��X��y w�Z� ��6�gz1����µWD�qǿ;<R2@J)�! .����-�ƍ��	�H���ZNAC�)�缢��CcUU��;m1	�aH��2��xD�^�%^�Ū58֔X��I����I��� �"A��j����2�0�k+��<.5}|��u�<L��J0�ݩgՌ�)�I#s��Sq�>��\Cs��6q��j��HW�Y@�R�D��[{F�����F��,#n>�J���&��뺧l��E��`�/p��n��!�ŝ���$0p�G��S`���ڌ_�Ց�i1e^�X��da��."�Z�2|��L�$9�g�6U��S1@���������*�N/����ws�H#��#ob���N�k�Zn/5�c��� ���h_���UA:	�<�s�L�
�g�P���|���ȣ�BŨd{����V�� P�ڗa�h�+���3'�
���HȻ��V()٦�GӸ���s�=��b�����V8֐+�_�]-!X����B��6Jǀ��Q��ĀǋׂW������ �ǰ�(��-�_LOCK_DEPTH))
		return 0;

	class_idx = class - lock_classes;

	if (depth) { /* we're holding locks */
		hlock = curr->held_locks + depth - 1;
		if (hlock->class_idx == class_idx && nest_lock) {
			if (!references)
				references++;

			if (!hlock->references)
				hlock->references++;

			hlock->references += references;

			/* Overflow */
			if (DEBUG_LOCKS_WARN_ON(hlock->references < references))
				return 0;

			return 2;
		}
	}

	hlock = curr->held_locks + depth;
	/*
	 * Plain impossible, we just registered it and checked it weren't no
	 * NULL like.. I bet this mushroom I ate was good!
	 */
	if (DEBUG_LOCKS_WARN_ON(!class))
		return 0;
	hlock->class_idx = class_idx;
	hlock->acquire_ip = ip;
	hlock->instance = lock;
	hlock->nest_lock = nest_lock;
	hlock->irq_context = task_irq_context(curr);
	hlock->trylock = trylock;
	hlock->read = read;
	hlock->check = check;
	hlock->hardirqs_off = !!hardirqs_off;
	hlock->references = references;
#ifdef CONFIG_LOCK_STAT
	hlock->waittime_stamp = 0;
	hlock->holdtime_stamp = lockstat_clock();
#endif
	hlock->pin_count = pin_count;

	if (check_wait_context(curr, hlock))
		return 0;

	/* Initialize the lock usage bit */
	if (!mark_usage(curr, hlock, check))
		return 0;

	/*
	 * Calculate the chain hash: it's the combined hash of all the
	 * lock keys along the dependency chain. We save the hash value
	 * at every step so that we can get the current hash easily
	 * after unlock. The chain hash is then used to cache dependency
	 * results.
	 *
	 * The 'key ID' is what is the most compact key value to drive
	 * the hash, not class->key.
	 */
	/*
	 * Whoops, we did it again.. class_idx is invalid.
	 */
	if (DEBUG_LOCKS_WARN_ON(!test_bit(class_idx, lock_classes_in_use)))
		return 0;

	chain_key = curr->curr_chain_key;
	if (!depth) {
		/*
		 * How can we have a chain hash when we ain't got no keys?!
		 */
		if (DEBUG_LOCKS_WARN_ON(chain_key != INITIAL_CHAIN_KEY))
			return 0;
		chain_head = 1;
	}

	hlock->prev_chain_key = chain_key;
	if (separate_irq_context(curr, hlock)) {
		chain_key = INITIAL_CHAIN_KEY;
		chain_head = 1;
	}
	chain_key = iterate_chain_key(chain_key, hlock_id(hlock));

	if (nest_lock && !__lock_is_held(nest_lock, -1)) {
		print_lock_nested_lock_not_held(curr, hlock, ip);
		return 0;
	}

	if (!debug_locks_silent) {
		WARN_ON_ONCE(depth && !hlock_class(hlock - 1)->key);
		WARN_ON_ONCE(!hlock_class(hlock)->key);
	}

	if (!validate_chain(curr, hlock, chain_head, chain_key))
		return 0;

	curr->curr_chain_key = chain_key;
	curr->lockdep_depth++;
	check_chain_key(curr);
#ifdef CONFIG_DEBUG_LOCKDEP
	if (unlikely(!debug_locks))
		return 0;
#endif
	if (unlikely(curr->lockdep_depth >= MAX_LOCK_DEPTH)) {
		debug_locks_off();
		print_lockdep_off("BUG: MAX_LOCK_DEPTH too low!");
		printk(KERN_DEBUG "depth: %i  max: %lu!\n",
		       curr->lockdep_depth, MAX_LOCK_DEPTH);

		lockdep_print_held_locks(current);
		debug_show_all_locks();
		dump_stack();

		return 0;
	}

	if (unlikely(curr->lockdep_depth > max_lockdep_depth))
		max_lockdep_depth = curr->lockdep_depth;

	return 1;
}

static void print_unlock_imbalance_bug(struct task_struct *curr,
				       struct lockdep_map *lock,
				       unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=====================================\n");
	pr_warn("WARNING: bad unlock balance detected!\n");
	print_kernel_ident();
	pr_warn("-------------------------------------\n");
	pr_warn("%s/%d is trying to release lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no more locks to release!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static noinstr int match_held_lock(const struct held_lock *hlock,
				   const struct lockdep_map *lock)
{
	if (hlock->instance == lock)
		return 1;

	if (hlock->references) {
		const struct lock_class *class = lock->class_cache[0];

		if (!class)
			class = look_up_lock_class(lock, 0);

		/*
		 * If look_up_lock_class() failed to find a class, we're trying
		 * to test if we hold a lock that has never yet been acquired.
		 * Clearly if the lock hasn't been acquired _ever_, we're not
		 * holding it either, so report failure.
		 */
		if (!class)
			return 0;

		/*
		 * References, but not a lock we're actually ref-counting?
		 * State got messed up, follow the sites that change ->references
		 * and try to make sense of it.
		 */
		if (DEBUG_LOCKS_WARN_ON(!hlock->nest_lock))
			return 0;

		if (hlock->class_idx == class - lock_classes)
			return 1;
	}

	return 0;
}

/* @depth must not be zero */
static struct held_lock *find_held_lock(struct task_struct *curr,
					struct lockdep_map *lock,
					unsigned int depth, int *idx)
{
	struct held_lock *ret, *hlock, *prev_hlock;
	int i;

	i = depth - 1;
	hlock = curr->held_locks + i;
	ret = hlock;
	if (match_held_lock(hlock, lock))
		goto out;

	ret = NULL;
	for (i--, prev_hlock = hlock--;
	     i >= 0;
	     i--, prev_hlock = hlock--) {
		/*
		 * We must not cross into another context:
		 */
		if (prev_hlock->irq_context != hlock->irq_context) {
			ret = NULL;
			break;
		}
		if (match_held_lock(hlock, lock)) {
			ret = hlock;
			break;
		}
	}

out:
	*idx = i;
	return ret;
}

static int reacquire_held_locks(struct task_struct *curr, unsigned int depth,
				int idx, unsigned int *merged)
{
	struct held_lock *hlock;
	int first_idx = idx;

	if (DEBUG_LOCKS_WARN_ON(!irqs_disabled()))
		return 0;

	for (hlock = curr->held_locks + idx; idx < depth; idx++, hlock++) {
		switch (__lock_acquire(hlock->instance,
				    hlock_class(hlock)->subclass,
				    hlock->trylock,
				    hlock->read, hlock->check,
				    hlock->hardirqs_off,
				    hlock->nest_lock, hlock->acquire_ip,
				    hlock->references, hlock->pin_count)) {
		case 0:
			return 1;
		case 1:
			break;
		case 2:
			*merged += (idx == first_idx);
			break;
		default:
			WARN_ON(1);
			return 0;
		}
	}
	return 0;
}

static int
__lock_set_class(struct lockdep_map *lock, const char *name,
		 struct lock_class_key *key, unsigned int subclass,
		 unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	struct lock_class *class;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	lockdep_init_map_waits(lock, name, key, 0,
			       lock->wait_type_inner,
			       lock->wait_type_outer);
	class = register_lock_class(lock, subclass, 0);
	hlock->class_idx = class - lock_classes;

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged))
		return 0;
	return 1;
}

static int __lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 0;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * This function is about (re)setting the class of a held lock,
	 * yet we're not actually holding any locks. Naughty user!
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return 0;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	WARN(hlock->read, "downgrading a read lock");
	hlock->read = 1;
	hlock->acquire_ip = ip;

	if (reacquire_held_locks(curr, depth, i, &merged))
		return 0;

	/* Merging can't happen with unchanged classes.. */
	if (DEBUG_LOCKS_WARN_ON(merged))
		return 0;

	/*
	 * I took it apart and put it back together again, except now I have
	 * these 'spare' parts.. where shall I put them.
	 */
	if (DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth))
		return 0;

	return 1;
}

/*
 * Remove the lock from the list of currently held locks - this gets
 * called on mutex_unlock()/spin_unlock*() (or on a failed
 * mutex_lock_interruptible()).
 */
static int
__lock_release(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	unsigned int depth, merged = 1;
	struct held_lock *hlock;
	int i;

	if (unlikely(!debug_locks))
		return 0;

	depth = curr->lockdep_depth;
	/*
	 * So we're all set to release this lock.. wait what lock? We don't
	 * own any locks, you've been drinking again?
	 */
	if (depth <= 0) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	/*
	 * Check whether the lock exists in the current stack
	 * of held locks:
	 */
	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_unlock_imbalance_bug(curr, lock, ip);
		return 0;
	}

	if (hlock->instance == lock)
		lock_release_holdtime(hlock);

	WARN(hlock->pin_count, "releasing a pinned lock\n");

	if (hlock->references) {
		hlock->references--;
		if (hlock->references) {
			/*
			 * We had, and after removing one, still have
			 * references, the current lock stack is still
			 * valid. We're done!
			 */
			return 1;
		}
	}

	/*
	 * We have the right lock to unlock, 'hlock' points to it.
	 * Now we remove it from the stack, and add back the other
	 * entries (if any), recalculating the hash along the way:
	 */

	curr->lockdep_depth = i;
	curr->curr_chain_key = hlock->prev_chain_key;

	/*
	 * The most likely case is when the unlock is on the innermost
	 * lock. In this case, we are done!
	 */
	if (i == depth-1)
		return 1;

	if (reacquire_held_locks(curr, depth, i + 1, &merged))
		return 0;

	/*
	 * We had N bottles of beer on the wall, we drank one, but now
	 * there's not N-1 bottles of beer left on the wall...
	 * Pouring two of the bottles together is acceptable.
	 */
	DEBUG_LOCKS_WARN_ON(curr->lockdep_depth != depth - merged);

	/*
	 * Since reacquire_held_locks() would have called check_chain_key()
	 * indirectly via __lock_acquire(), we don't need to do it again
	 * on return.
	 */
	return 0;
}

static __always_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || !!hlock->read == read)
				return LOCK_STATE_HELD;

			return LOCK_STATE_NOT_HELD;
		}
	}

	return LOCK_STATE_NOT_HELD;
}

static struct pin_cookie __lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return cookie;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			/*
			 * Grab 16bits of randomness; this is sufficient to not
			 * be guessable and still allows some pin nesting in
			 * our u32 pin_count.
			 */
			cookie.val = 1 + (prandom_u32() >> 16);
			hlock->pin_count += cookie.val;
			return cookie;
		}
	}

	WARN(1, "pinning an unheld lock\n");
	return cookie;
}

static void __lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			hlock->pin_count += cookie.val;
			return;
		}
	}

	WARN(1, "pinning an unheld lock\n");
}

static void __lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	struct task_struct *curr = current;
	int i;

	if (unlikely(!debug_locks))
		return;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (WARN(!hlock->pin_count, "unpinning an unpinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 0, "pin count corrupted\n"))
				hlock->pin_count = 0;

			return;
		}
	}

	WARN(1, "unpinning an unheld lock\n");
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static noinstr void check_flags(unsigned long flags)
{
#if defined(CONFIG_PROVE_LOCKING) && defined(CONFIG_DEBUG_LOCKDEP)
	if (!debug_locks)
		return;

	/* Get the warning out..  */
	instrumentation_begin();

	if (irqs_disabled_flags(flags)) {
		if (DEBUG_LOCKS_WARN_ON(lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-off.\n");
		}
	} else {
		if (DEBUG_LOCKS_WARN_ON(!lockdep_hardirqs_enabled())) {
			printk("possible reason: unannotated irqs-on.\n");
		}
	}

#ifndef CONFIG_PREEMPT_RT
	/*
	 * We dont accurately track softirq state in e.g.
	 * hardirq contexts (such as on 4KSTACKS), so only
	 * check if not in hardirq contexts:
	 */
	if (!hardirq_count()) {
		if (softirq_count()) {
			/* like the above, but with softirqs */
			DEBUG_LOCKS_WARN_ON(current->softirqs_enabled);
		} else {
			/* lick the above, does it taste good? */
			DEBUG_LOCKS_WARN_ON(!current->softirqs_enabled);
		}
	}
#endif

	if (!debug_locks)
		print_irqtrace_events(current);

	instrumentation_end();
#endif
}

void lock_set_class(struct lockdep_map *lock, const char *name,
		    struct lock_class_key *key, unsigned int subclass,
		    unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_set_class(lock, name, key, subclass, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_set_class);

void lock_downgrade(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	lockdep_recursion_inc();
	check_flags(flags);
	if (__lock_downgrade(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_downgrade);

/* NMI context !!! */
static void verify_lock_unused(struct lockdep_map *lock, struct held_lock *hlock, int subclass)
{
#ifdef CONFIG_PROVE_LOCKING
	struct lock_class *class = look_up_lock_class(lock, subclass);
	unsigned long mask = LOCKF_USED;

	/* if it doesn't have a class (yet), it certainly hasn't been used yet */
	if (!class)
		return;

	/*
	 * READ locks only conflict with USED, such that if we only ever use
	 * READ locks, there is no deadlock possible -- RCU.
	 */
	if (!hlock->read)
		mask |= LOCKF_USED_READ;

	if (!(class->usage_mask & mask))
		return;

	hlock->class_idx = class - lock_classes;

	print_usage_bug(current, hlock, LOCK_USED, LOCK_USAGE_STATES);
#endif
}

static bool lockdep_nmi(void)
{
	if (raw_cpu_read(lockdep_recursion))
		return false;

	if (!in_nmi())
		return false;

	return true;
}

/*
 * read_lock() is recursive if:
 * 1. We force lockdep think this way in selftests or
 * 2. The implementation is not queued read/write lock or
 * 3. The locker is at an in_interrupt() context.
 */
bool read_lock_is_recursive(void)
{
	return force_read_lock_recursive ||
	       !IS_ENABLED(CONFIG_QUEUED_RWLOCKS) ||
	       in_interrupt();
}
EXPORT_SYMBOL_GPL(read_lock_is_recursive);

/*
 * We are not always called with irqs disabled - do that here,
 * and also avoid lockdep recursion:
 */
void lock_acquire(struct lockdep_map *lock, unsigned int subclass,
			  int trylock, int read, int check,
			  struct lockdep_map *nest_lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquire(lock, subclass, trylock, read, check, nest_lock, ip);

	if (!debug_locks)
		return;

	if (unlikely(!lockdep_enabled())) {
		/* XXX allow trylock from NMI ?!? */
		if (lockdep_nmi() && !trylock) {
			struct held_lock hlock;

			hlock.acquire_ip = ip;
			hlock.instance = lock;
			hlock.nest_lock = nest_lock;
			hlock.irq_context = 2; // XXX
			hlock.trylock = trylock;
			hlock.read = read;
			hlock.check = check;
			hlock.hardirqs_off = true;
			hlock.references = 0;

			verify_lock_unused(lock, &hlock, subclass);
		}
		return;
	}

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_acquire(lock, subclass, trylock, read, check,
		       irqs_disabled_flags(flags), nest_lock, ip, 0, 0);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquire);

void lock_release(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_release(lock, ip);

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	if (__lock_release(lock, ip))
		check_chain_key(current);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_release);

noinstr int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = LOCK_STATE_NOT_HELD;

	/*
	 * Avoid false negative lockdep_assert_held() and
	 * lockdep_assert_not_held().
	 */
	if (unlikely(!lockdep_enabled()))
		return LOCK_STATE_UNKNOWN;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	ret = __lock_is_held(lock, read);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);

struct pin_cookie lock_pin_lock(struct lockdep_map *lock)
{
	struct pin_cookie cookie = NIL_COOKIE;
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return cookie;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	cookie = __lock_pin_lock(lock);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);

	return cookie;
}
EXPORT_SYMBOL_GPL(lock_pin_lock);

void lock_repin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_repin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_repin_lock);

void lock_unpin_lock(struct lockdep_map *lock, struct pin_cookie cookie)
{
	unsigned long flags;

	if (unlikely(!lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);

	lockdep_recursion_inc();
	__lock_unpin_lock(lock, cookie);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_unpin_lock);

#ifdef CONFIG_LOCK_STAT
static void print_lock_contention_bug(struct task_struct *curr,
				      struct lockdep_map *lock,
				      unsigned long ip)
{
	if (!debug_locks_off())
		return;
	if (debug_locks_silent)
		return;

	pr_warn("\n");
	pr_warn("=================================\n");
	pr_warn("WARNING: bad contention detected!\n");
	print_kernel_ident();
	pr_warn("---------------------------------\n");
	pr_warn("%s/%d is trying to contend lock (",
		curr->comm, task_pid_nr(curr));
	print_lockdep_cache(lock);
	pr_cont(") at:\n");
	print_ip_sym(KERN_WARNING, ip);
	pr_warn("but there are no locks held!\n");
	pr_warn("\nother info that might help us debug this:\n");
	lockdep_print_held_locks(curr);

	pr_warn("\nstack backtrace:\n");
	dump_stack();
}

static void
__lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	int i, contention_point, contending_point;

	depth = curr->lockdep_depth;
	/*
	 * Whee, we contended on this lock, except it seems we're not
	 * actually trying to acquire anything much at all..
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, ip);
		return;
	}

	if (hlock->instance != lock)
		return;

	hlock->waittime_stamp = lockstat_clock();

	contention_point = lock_point(hlock_class(hlock)->contention_point, ip);
	contending_point = lock_point(hlock_class(hlock)->contending_point,
				      lock->ip);

	stats = get_lock_stats(hlock_class(hlock));
	if (contention_point < LOCKSTAT_POINTS)
		stats->contention_point[contention_point]++;
	if (contending_point < LOCKSTAT_POINTS)
		stats->contending_point[contending_point]++;
	if (lock->cpu != smp_processor_id())
		stats->bounces[bounce_contended + !!hlock->read]++;
}

static void
__lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	struct task_struct *curr = current;
	struct held_lock *hlock;
	struct lock_class_stats *stats;
	unsigned int depth;
	u64 now, waittime = 0;
	int i, cpu;

	depth = curr->lockdep_depth;
	/*
	 * Yay, we acquired ownership of this lock we didn't try to
	 * acquire, how the heck did that happen?
	 */
	if (DEBUG_LOCKS_WARN_ON(!depth))
		return;

	hlock = find_held_lock(curr, lock, depth, &i);
	if (!hlock) {
		print_lock_contention_bug(curr, lock, _RET_IP_);
		return;
	}

	if (hlock->instance != lock)
		return;

	cpu = smp_processor_id();
	if (hlock->waittime_stamp) {
		now = lockstat_clock();
		waittime = now - hlock->waittime_stamp;
		hlock->holdtime_stamp = now;
	}

	stats = get_lock_stats(hlock_class(hlock));
	if (waittime) {
		if (hlock->read)
			lock_time_inc(&stats->read_waittime, waittime);
		else
			lock_time_inc(&stats->write_waittime, waittime);
	}
	if (lock->cpu != cpu)
		stats->bounces[bounce_acquired + !!hlock->read]++;

	lock->cpu = cpu;
	lock->ip = ip;
}

void lock_contended(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_contended(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_contended(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_contended);

void lock_acquired(struct lockdep_map *lock, unsigned long ip)
{
	unsigned long flags;

	trace_lock_acquired(lock, ip);

	if (unlikely(!lock_stat || !lockdep_enabled()))
		return;

	raw_local_irq_save(flags);
	check_flags(flags);
	lockdep_recursion_inc();
	__lock_acquired(lock, ip);
	lockdep_recursion_finish();
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL_GPL(lock_acquired);
#endif

/*
 * Used by the testsuite, sanitize the validator state
 * after a simulated failure:
 */

void lockdep_reset(void)
{
	unsigned long flags;
	int i;

	raw_local_irq_save(flags);
	lockdep_init_task(current);
	memset(current->held_locks, 0, MAX_LOCK_DEPTH*sizeof(struct held_lock));
	nr_hardirq_chains = 0;
	nr_softirq_chains = 0;
	nr_process_chains = 0;
	debug_locks = 1;
	for (i = 0; i < CHAINHASH_SIZE; i++)
		INIT_HLIST_HEAD(chainhash_table + i);
	raw_local_irq_restore(flags);
}

/* Remove a class from a lock chain. Must be called with the graph lock held. */
static void remove_class_from_lock_chain(struct pending_free *pf,
					 struct lock_chain *chain,
					 struct lock_class *class)
{
#ifdef CONFIG_PROVE_LOCKING
	int i;

	for (i = chain->base; i < chain->base + chain->depth; i++) {
		if (chain_hlock_class_idx(chain_hlocks[i]) != class - lock_classes)
			continue;
		/*
		 * Each lock class occurs at most once in a lock chain so once
		 * we found a match we can break out of this loop.
		 */
		goto free_lock_chain;
	}
	/* Since the chain has not been modified, return. */
	return;

free_lock_chain:
	free_chain_hlocks(chain->base, chain->depth);
	/* Overwrite the chain key for concurrent RCU readers. */
	WRITE_ONCE(chain->chain_key, INITIAL_CHAIN_KEY);
	dec_chains(chain->irq_context);

	/*
	 * Note: calling hlist_del_rcu() from inside a
	 * hlist_for_each_entry_rcu() loop is safe.
	 */
	hlist_del_rcu(&chain->entry);
	__set_bit(chain - lock_chains, pf->lock_chains_being_freed);
	nr_zapped_lock_chains++;
#endif
}

/* Must be called with the graph lock held. */
static void remove_class_from_lock_chains(struct pending_free *pf,
					  struct lock_class *class)
{
	struct lock_chain *chain;
	struct hlist_head *head;
	int i;

	for (i = 0; i < ARRAY_SIZE(chainhash_table); i++) {
		head = chainhash_table + i;
		hlist_for_each_entry_rcu(chain, head, entry) {
			remove_class_from_lock_chain(pf, chain, class);
		}
	}
}

/*
 * Remove all references to a lock class. The caller must hold the graph lock.
 */
static void zap_class(struct pending_free *pf, struct lock_class *class)
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
		hlist_for_