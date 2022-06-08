cmd_drivers/mtd/devices/phram.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/devices/phram.ko drivers/mtd/devices/phram.o drivers/mtd/devices/phram.mod.o;  true
