cmd_drivers/media/usb/stkwebcam/modules.order := {   echo drivers/media/usb/stkwebcam/stkwebcam.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/stkwebcam/modules.order
