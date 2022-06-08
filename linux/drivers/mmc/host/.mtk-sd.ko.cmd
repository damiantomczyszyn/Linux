cmd_drivers/mmc/host/mtk-sd.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/mmc/host/mtk-sd.ko drivers/mmc/host/mtk-sd.o drivers/mmc/host/mtk-sd.mod.o;  true
