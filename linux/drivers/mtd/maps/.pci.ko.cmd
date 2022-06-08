cmd_drivers/mtd/maps/pci.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mtd/maps/pci.ko drivers/mtd/maps/pci.o drivers/mtd/maps/pci.mod.o;  true
