cmd_drivers/media/usb/gspca/m5602/modules.order := {   echo drivers/media/usb/gspca/m5602/gspca_m5602.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/gspca/m5602/modules.order
