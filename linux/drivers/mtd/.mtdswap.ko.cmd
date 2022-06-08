cmd_drivers/mtd/mtdswap.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/mtdswap.ko drivers/mtd/mtdswap.o drivers/mtd/mtdswap.mod.o;  true
