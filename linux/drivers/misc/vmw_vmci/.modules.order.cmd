cmd_drivers/misc/vmw_vmci/modules.order := {   echo drivers/misc/vmw_vmci/vmw_vmci.ko; :; } | awk '!x[$$0]++' - > drivers/misc/vmw_vmci/modules.order
