cmd_drivers/mux/modules.order := {   echo drivers/mux/mux-core.ko;   echo drivers/mux/mux-adg792a.ko;   echo drivers/mux/mux-adgs1408.ko;   echo drivers/mux/mux-gpio.ko; :; } | awk '!x[$$0]++' - > drivers/mux/modules.order