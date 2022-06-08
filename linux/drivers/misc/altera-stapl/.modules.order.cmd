cmd_drivers/misc/altera-stapl/modules.order := {   echo drivers/misc/altera-stapl/altera-stapl.ko; :; } | awk '!x[$$0]++' - > drivers/misc/altera-stapl/modules.order
