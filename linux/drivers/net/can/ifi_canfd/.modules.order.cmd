cmd_drivers/net/can/ifi_canfd/modules.order := {   echo drivers/net/can/ifi_canfd/ifi_canfd.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/ifi_canfd/modules.order
