cmd_drivers/mmc/core/mmc_block.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/core/mmc_block.ko drivers/mmc/core/mmc_block.o drivers/mmc/core/mmc_block.mod.o;  true
