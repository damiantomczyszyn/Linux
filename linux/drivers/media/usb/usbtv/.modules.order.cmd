cmd_drivers/media/usb/usbtv/modules.order := {   echo drivers/media/usb/usbtv/usbtv.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/usbtv/modules.order
