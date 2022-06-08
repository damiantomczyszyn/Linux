cmd_drivers/media/usb/gspca/gl860/modules.order := {   echo drivers/media/usb/gspca/gl860/gspca_gl860.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/gspca/gl860/modules.order
