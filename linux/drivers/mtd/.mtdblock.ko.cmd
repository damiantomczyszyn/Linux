cmd_drivers/mtd/mtdblock.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/mtdblock.ko drivers/mtd/mtdblock.o drivers/mtd/mtdblock.mod.o;  true
