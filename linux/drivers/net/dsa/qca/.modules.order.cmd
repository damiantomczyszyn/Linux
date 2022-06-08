cmd_drivers/net/dsa/qca/modules.order := {   echo drivers/net/dsa/qca/ar9331.ko; :; } | awk '!x[$$0]++' - > drivers/net/dsa/qca/modules.order
