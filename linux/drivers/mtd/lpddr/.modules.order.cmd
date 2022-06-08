cmd_drivers/mtd/lpddr/modules.order := {   echo drivers/mtd/lpddr/qinfo_probe.ko;   echo drivers/mtd/lpddr/lpddr_cmds.ko; :; } | awk '!x[$$0]++' - > drivers/mtd/lpddr/modules.order
