cmd_drivers/mmc/host/ushc.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/ushc.ko drivers/mmc/host/ushc.o drivers/mmc/host/ushc.mod.o;  true
