cmd_drivers/mmc/host/cqhci.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/cqhci.ko drivers/mmc/host/cqhci.o drivers/mmc/host/cqhci.mod.o;  true
