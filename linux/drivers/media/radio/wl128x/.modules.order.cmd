cmd_drivers/media/radio/wl128x/modules.order := {   echo drivers/media/radio/wl128x/fm_drv.ko; :; } | awk '!x[$$0]++' - > drivers/media/radio/wl128x/modules.order
