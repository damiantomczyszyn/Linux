cmd_drivers/net/dsa/mv88e6xxx/modules.order := {   echo drivers/net/dsa/mv88e6xxx/mv88e6xxx.ko; :; } | awk '!x[$$0]++' - > drivers/net/dsa/mv88e6xxx/modules.order
