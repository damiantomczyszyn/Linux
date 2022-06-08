cmd_drivers/mfd/arizona.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mfd/arizona.ko drivers/mfd/arizona.o drivers/mfd/arizona.mod.o;  true
