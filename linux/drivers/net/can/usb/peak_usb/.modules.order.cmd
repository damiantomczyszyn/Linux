cmd_drivers/net/can/usb/peak_usb/modules.order := {   echo drivers/net/can/usb/peak_usb/peak_usb.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/usb/peak_usb/modules.order
