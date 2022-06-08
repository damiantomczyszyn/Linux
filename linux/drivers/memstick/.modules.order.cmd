cmd_drivers/memstick/modules.order := {   cat drivers/memstick/core/modules.order;   cat drivers/memstick/host/modules.order; :; } | awk '!x[$$0]++' - > drivers/memstick/modules.order
