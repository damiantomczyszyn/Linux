cmd_drivers/net/can/usb/gs_usb.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/can/usb/gs_usb.ko drivers/net/can/usb/gs_usb.o drivers/net/can/usb/gs_usb.mod.o;  true
