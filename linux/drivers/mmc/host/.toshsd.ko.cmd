cmd_drivers/mmc/host/toshsd.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/toshsd.ko drivers/mmc/host/toshsd.o drivers/mmc/host/toshsd.mod.o;  true
