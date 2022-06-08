cmd_drivers/media/usb/as102/modules.order := {   echo drivers/media/usb/as102/dvb-as102.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/as102/modules.order
