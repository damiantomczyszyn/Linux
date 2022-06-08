cmd_drivers/net/can/usb/kvaser_usb/modules.order := {   echo drivers/net/can/usb/kvaser_usb/kvaser_usb.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/usb/kvaser_usb/modules.order
