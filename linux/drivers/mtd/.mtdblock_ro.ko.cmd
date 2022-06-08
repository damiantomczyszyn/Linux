cmd_drivers/mtd/mtdblock_ro.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/mtdblock_ro.ko drivers/mtd/mtdblock_ro.o drivers/mtd/mtdblock_ro.mod.o;  true
