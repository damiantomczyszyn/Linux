cmd_drivers/misc/cb710/modules.order := {   echo drivers/misc/cb710/cb710.ko; :; } | awk '!x[$$0]++' - > drivers/misc/cb710/modules.order
