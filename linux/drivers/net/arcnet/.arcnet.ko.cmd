cmd_drivers/net/arcnet/arcnet.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/net/arcnet/arcnet.ko drivers/net/arcnet/arcnet.o drivers/net/arcnet/arcnet.mod.o;  true
