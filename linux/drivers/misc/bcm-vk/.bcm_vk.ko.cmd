cmd_drivers/misc/bcm-vk/bcm_vk.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/bcm-vk/bcm_vk.ko drivers/misc/bcm-vk/bcm_vk.o drivers/misc/bcm-vk/bcm_vk.mod.o;  true
