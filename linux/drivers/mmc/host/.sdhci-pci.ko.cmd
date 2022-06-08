cmd_drivers/mmc/host/sdhci-pci.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/sdhci-pci.ko drivers/mmc/host/sdhci-pci.o drivers/mmc/host/sdhci-pci.mod.o;  true
