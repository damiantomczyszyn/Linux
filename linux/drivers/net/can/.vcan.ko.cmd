cmd_drivers/net/can/vcan.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/vcan.ko drivers/net/can/vcan.o drivers/net/can/vcan.mod.o;  true
