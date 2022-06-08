cmd_drivers/mtd/maps/physmap.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/maps/physmap.ko drivers/mtd/maps/physmap.o drivers/mtd/maps/physmap.mod.o;  true
