cmd_drivers/mtd/maps/map_funcs.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/maps/map_funcs.ko drivers/mtd/maps/map_funcs.o drivers/mtd/maps/map_funcs.mod.o;  true
