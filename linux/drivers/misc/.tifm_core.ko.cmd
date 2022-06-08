cmd_drivers/misc/tifm_core.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/tifm_core.ko drivers/misc/tifm_core.o drivers/misc/tifm_core.mod.o;  true
