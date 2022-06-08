cmd_drivers/net/dsa/hirschmann/modules.order := {   echo drivers/net/dsa/hirschmann/hellcreek_sw.ko; :; } | awk '!x[$$0]++' - > drivers/net/dsa/hirschmann/modules.order
