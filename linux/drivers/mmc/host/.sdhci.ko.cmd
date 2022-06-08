cmd_drivers/mmc/host/sdhci.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/sdhci.ko drivers/mmc/host/sdhci.o drivers/mmc/host/sdhci.mod.o;  true
