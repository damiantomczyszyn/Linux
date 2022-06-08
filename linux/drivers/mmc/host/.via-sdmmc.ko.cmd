cmd_drivers/mmc/host/via-sdmmc.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/via-sdmmc.ko drivers/mmc/host/via-sdmmc.o drivers/mmc/host/via-sdmmc.mod.o;  true
