cmd_drivers/mfd/mfd-core.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mfd/mfd-core.ko drivers/mfd/mfd-core.o drivers/mfd/mfd-core.mod.o;  true
