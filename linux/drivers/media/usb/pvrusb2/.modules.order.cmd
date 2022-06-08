cmd_drivers/media/usb/pvrusb2/modules.order := {   echo drivers/media/usb/pvrusb2/pvrusb2.ko; :; } | awk '!x[$$0]++' - > drivers/media/usb/pvrusb2/modules.order
