cmd_drivers/misc/ibmasm/ibmasm.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/ibmasm/ibmasm.ko drivers/misc/ibmasm/ibmasm.o drivers/misc/ibmasm/ibmasm.mod.o;  true
