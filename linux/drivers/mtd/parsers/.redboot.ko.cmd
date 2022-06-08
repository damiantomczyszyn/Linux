cmd_drivers/mtd/parsers/redboot.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/parsers/redboot.ko drivers/mtd/parsers/redboot.o drivers/mtd/parsers/redboot.mod.o;  true
