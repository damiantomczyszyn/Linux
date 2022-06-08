cmd_drivers/mmc/host/mmc_hsq.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/mmc_hsq.ko drivers/mmc/host/mmc_hsq.o drivers/mmc/host/mmc_hsq.mod.o;  true
