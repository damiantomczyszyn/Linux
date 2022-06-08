cmd_drivers/media/usb/stk1160/modules.order := {   echo drivers/media/usb/stk1160/stk1160.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/stk1160/modules.order
