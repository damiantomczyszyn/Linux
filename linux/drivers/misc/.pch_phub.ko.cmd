cmd_drivers/misc/pch_phub.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/pch_phub.ko drivers/misc/pch_phub.o drivers/misc/pch_phub.mod.o;  true
