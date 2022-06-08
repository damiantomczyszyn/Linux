cmd_drivers/net/can/dev/can-dev.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/dev/can-dev.ko drivers/net/can/dev/can-dev.o drivers/net/can/dev/can-dev.mod.o;  true
