cmd_drivers/net/can/usb/etas_es58x/modules.order := {   echo drivers/net/can/usb/etas_es58x/etas_es58x.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/usb/etas_es58x/modules.order
