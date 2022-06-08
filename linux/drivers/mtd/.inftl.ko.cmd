cmd_drivers/mtd/inftl.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/inftl.ko drivers/mtd/inftl.o drivers/mtd/inftl.mod.o;  true
