cmd_drivers/media/usb/zr364xx/modules.order := {   echo drivers/media/usb/zr364xx/zr364xx.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/zr364xx/modules.order
