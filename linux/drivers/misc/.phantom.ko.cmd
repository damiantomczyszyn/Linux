cmd_drivers/misc/phantom.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/phantom.ko drivers/misc/phantom.o drivers/misc/phantom.mod.o;  true
