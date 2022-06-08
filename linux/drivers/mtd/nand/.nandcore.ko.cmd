cmd_drivers/mtd/nand/nandcore.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/nand/nandcore.ko drivers/mtd/nand/nandcore.o drivers/mtd/nand/nandcore.mod.o;  true
