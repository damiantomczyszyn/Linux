cmd_drivers/net/can/c_can/c_can.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/c_can/c_can.ko drivers/net/can/c_can/c_can.o drivers/net/can/c_can/c_can.mod.o;  true
