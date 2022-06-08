cmd_drivers/misc/ti-st/modules.order := {   echo drivers/misc/ti-st/st_drv.ko; :; } | awk '!x[$$0]++' - > drivers/misc/ti-st/modules.order
