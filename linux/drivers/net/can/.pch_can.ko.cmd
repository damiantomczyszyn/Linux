cmd_drivers/net/can/pch_can.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/pch_can.ko drivers/net/can/pch_can.o drivers/net/can/pch_can.mod.o;  true
