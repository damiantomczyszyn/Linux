cmd_drivers/media/usb/cpia2/modules.order := {   echo drivers/media/usb/cpia2/cpia2.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/cpia2/modules.order
