cmd_drivers/misc/ibmasm/modules.order := {   echo drivers/misc/ibmasm/ibmasm.ko; :; } | awk '!x[$$0]++' - > drivers/misc/ibmasm/modules.order
