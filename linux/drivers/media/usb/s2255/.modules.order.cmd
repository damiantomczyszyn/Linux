cmd_drivers/media/usb/s2255/modules.order := {   echo drivers/media/usb/s2255/s2255drv.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/s2255/modules.order
