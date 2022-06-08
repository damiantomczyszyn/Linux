cmd_drivers/mtd/ubi/ubi.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/ubi/ubi.ko drivers/mtd/ubi/ubi.o drivers/mtd/ubi/ubi.mod.o;  true
