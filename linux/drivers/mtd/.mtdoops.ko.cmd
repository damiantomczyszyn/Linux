cmd_drivers/mtd/mtdoops.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/mtdoops.ko drivers/mtd/mtdoops.o drivers/mtd/mtdoops.mod.o;  true
