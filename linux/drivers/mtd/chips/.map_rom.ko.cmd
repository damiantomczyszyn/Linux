cmd_drivers/mtd/chips/map_rom.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/chips/map_rom.ko drivers/mtd/chips/map_rom.o drivers/mtd/chips/map_rom.mod.o;  true
