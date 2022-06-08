cmd_drivers/mtd/chips/gen_probe.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/chips/gen_probe.ko drivers/mtd/chips/gen_probe.o drivers/mtd/chips/gen_probe.mod.o;  true
