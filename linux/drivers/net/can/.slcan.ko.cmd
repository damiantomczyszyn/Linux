cmd_drivers/net/can/slcan.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/slcan.ko drivers/net/can/slcan.o drivers/net/can/slcan.mod.o;  true
