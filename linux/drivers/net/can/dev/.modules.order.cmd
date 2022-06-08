cmd_drivers/net/can/dev/modules.order := {   echo drivers/net/can/dev/can-dev.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/dev/modules.order
