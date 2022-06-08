cmd_drivers/media/usb/hdpvr/modules.order := {   echo drivers/media/usb/hdpvr/hdpvr.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/hdpvr/modules.order
