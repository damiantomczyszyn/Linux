cmd_drivers/net/can/vxcan.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/vxcan.ko drivers/net/can/vxcan.o drivers/net/can/vxcan.mod.o;  true
