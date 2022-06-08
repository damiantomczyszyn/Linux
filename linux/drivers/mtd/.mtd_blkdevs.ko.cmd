cmd_drivers/mtd/mtd_blkdevs.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/mtd_blkdevs.ko drivers/mtd/mtd_blkdevs.o drivers/mtd/mtd_blkdevs.mod.o;  true
