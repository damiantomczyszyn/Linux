cmd_drivers/net/can/peak_canfd/modules.order := {   echo drivers/net/can/peak_canfd/peak_pciefd.ko; :; } | awk '!x[$$0]++' - > drivers/net/can/peak_canfd/modules.order
