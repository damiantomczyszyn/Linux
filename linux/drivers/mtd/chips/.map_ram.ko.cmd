cmd_drivers/mtd/chips/map_ram.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/chips/map_ram.ko drivers/mtd/chips/map_ram.o drivers/mtd/chips/map_ram.mod.o;  true
