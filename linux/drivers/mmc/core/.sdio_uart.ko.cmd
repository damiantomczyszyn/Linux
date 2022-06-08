cmd_drivers/mmc/core/sdio_uart.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/core/sdio_uart.ko drivers/mmc/core/sdio_uart.o drivers/mmc/core/sdio_uart.mod.o;  true
