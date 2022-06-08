cmd_drivers/mmc/core/mmc_core.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/core/mmc_core.ko drivers/mmc/core/mmc_core.o drivers/mmc/core/mmc_core.mod.o;  true
