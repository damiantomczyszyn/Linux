cmd_drivers/media/usb/b2c2/modules.order := {   echo drivers/media/usb/b2c2/b2c2-flexcop-usb.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/b2c2/modules.order
