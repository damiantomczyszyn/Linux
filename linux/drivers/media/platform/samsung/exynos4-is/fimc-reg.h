g/KMAP_LOCAL) \
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ELF                            4     (   srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                      cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Linux                Linux        GCC: (GNU) 11.2.0           GNU  ?        ?                                  ??       #     &   #   +     :   N   `     P   ?        h   ?   	     }   ?   
     ?   ?   7     ?            ?           ?            ?              ?               cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module ?     ?     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4                    9             @             @       5   	   @       l                 S             @  0                  _      0       p                   h              ?                     x             ?  (                                ?  ?               	              ?  ?                                |  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                            4     (   srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                      cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Linux                Linux        GCC: (GNU) 11.2.0           GNU  ?        ?                                  ??       #     &   #   +     :   N   `     P   ?        h   ?   	     }   ?   
     ?   ?   7     ?            ?           ?            ?              ?               cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module ?     ?     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4                    9             @             @       5   	   @       l                 S             @  0                  _      0       p                   h              ?                     x             ?  (                                ?  ?               	              ?  ?                                |  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          cmd_drivers/media/pci/cx18/cx18.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-bran