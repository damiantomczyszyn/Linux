cmd_drivers/memstick/host/rtsx_pci_ms.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/memstick/host/rtsx_pci_ms.ko drivers/memstick/host/rtsx_pci_ms.o drivers/memstick/host/rtsx_pci_ms.mod.o;  true