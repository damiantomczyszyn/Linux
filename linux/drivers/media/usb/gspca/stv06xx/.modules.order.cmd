cmd_drivers/media/usb/gspca/stv06xx/modules.order := {   echo drivers/media/usb/gspca/stv06xx/gspca_stv06xx.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/gspca/stv06xx/modules.order
