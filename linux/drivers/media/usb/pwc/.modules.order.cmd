cmd_drivers/media/usb/pwc/modules.order := {   echo drivers/media/usb/pwc/pwc.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/pwc/modules.order
