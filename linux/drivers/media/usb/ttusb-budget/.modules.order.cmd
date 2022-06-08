cmd_drivers/media/usb/ttusb-budget/modules.order := {   echo drivers/media/usb/ttusb-budget/dvb-ttusb-budget.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/ttusb-budget/modules.order
