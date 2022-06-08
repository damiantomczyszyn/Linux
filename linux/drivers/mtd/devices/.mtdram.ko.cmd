cmd_drivers/mtd/devices/mtdram.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/devices/mtdram.ko drivers/mtd/devices/mtdram.o drivers/mtd/devices/mtdram.mod.o;  true
