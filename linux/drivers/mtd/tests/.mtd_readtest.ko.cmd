cmd_drivers/mtd/tests/mtd_readtest.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/tests/mtd_readtest.ko drivers/mtd/tests/mtd_readtest.o drivers/mtd/tests/mtd_readtest.mod.o;  true