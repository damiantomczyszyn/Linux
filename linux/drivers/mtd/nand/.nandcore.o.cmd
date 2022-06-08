cmd_drivers/mtd/nand/nandcore.o := ld -m elf_i386   -r -o drivers/mtd/nand/nandcore.o drivers/mtd/nand/core.o drivers/mtd/nand/bbt.o drivers/mtd/nand/ecc.o drivers/mtd/nand/ecc-sw-hamming.o
