cmd_drivers/misc/cardreader/rtsx_usb.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/cardreader/rtsx_usb.ko drivers/misc/cardreader/rtsx_usb.o drivers/misc/cardreader/rtsx_usb.mod.o;  true