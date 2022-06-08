cmd_drivers/net/bonding/modules.order := {   echo drivers/net/bonding/bonding.ko; :; } | awk '!x[$$0]++' - > drivers/net/bonding/modules.order
