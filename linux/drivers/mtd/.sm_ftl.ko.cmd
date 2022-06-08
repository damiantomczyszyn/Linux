cmd_drivers/mtd/sm_ftl.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/sm_ftl.ko drivers/mtd/sm_ftl.o drivers/mtd/sm_ftl.mod.o;  true
