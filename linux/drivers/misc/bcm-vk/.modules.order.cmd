cmd_drivers/misc/bcm-vk/modules.order := {   echo drivers/misc/bcm-vk/bcm_vk.ko; :; } | awk '!x[$$0]++' - > drivers/misc/bcm-vk/modules.order
