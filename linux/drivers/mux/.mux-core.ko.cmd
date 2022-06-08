cmd_drivers/mux/mux-core.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mux/mux-core.ko drivers/mux/mux-core.o drivers/mux/mux-core.mod.o;  true
