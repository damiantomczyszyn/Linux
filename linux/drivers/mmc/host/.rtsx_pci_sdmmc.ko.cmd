cmd_drivers/mmc/host/rtsx_pci_sdmmc.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/rtsx_pci_sdmmc.ko drivers/mmc/host/rtsx_pci_sdmmc.o drivers/mmc/host/rtsx_pci_sdmmc.mod.o;  true