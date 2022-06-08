cmd_drivers/mtd/ssfdc.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/ssfdc.ko drivers/mtd/ssfdc.o drivers/mtd/ssfdc.mod.o;  true
