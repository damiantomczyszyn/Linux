cmd_drivers/misc/eeprom/eeprom.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/misc/eeprom/eeprom.ko drivers/misc/eeprom/eeprom.o drivers/misc/eeprom/eeprom.mod.o;  true
