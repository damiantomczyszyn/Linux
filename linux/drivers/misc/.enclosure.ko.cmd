cmd_drivers/misc/enclosure.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/enclosure.ko drivers/misc/enclosure.o drivers/misc/enclosure.mod.o;  true
