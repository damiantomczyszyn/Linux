cmd_drivers/mmc/host/wbsd.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/wbsd.ko drivers/mmc/host/wbsd.o drivers/mmc/host/wbsd.mod.o;  true
