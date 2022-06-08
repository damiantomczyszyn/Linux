cmd_drivers/mfd/lpc_ich.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mfd/lpc_ich.ko drivers/mfd/lpc_ich.o drivers/mfd/lpc_ich.mod.o;  true
