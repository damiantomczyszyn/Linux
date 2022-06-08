cmd_drivers/media/usb/siano/modules.order := {   echo drivers/media/usb/siano/smsusb.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/siano/modules.order
