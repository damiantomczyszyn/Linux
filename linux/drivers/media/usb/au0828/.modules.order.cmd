cmd_drivers/media/usb/au0828/modules.order := {   echo drivers/media/usb/au0828/au0828.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/au0828/modules.order
