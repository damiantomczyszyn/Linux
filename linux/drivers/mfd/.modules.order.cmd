cmd_drivers/mfd/modules.order := {   echo drivers/mfd/sm501.ko;   echo drivers/mfd/bcm590xx.ko;   echo drivers/mfd/bd9571mwv.ko;   echo drivers/mfd/cros_ec_dev.ko;   echo drivers/mfd/htc-pasic3.ko;   echo drivers/mfd/ti_am335x_tscadc.ko;   echo drivers/mfd/arizona.ko;   echo drivers/mfd/arizona-i2c.ko;   echo drivers/mfd/wm8994.ko;   echo drivers/mfd/tps6105x.ko;   echo drivers/mfd/tps65010.ko;   echo drivers/mfd/tps6507x.ko;   echo drivers/mfd/mc13xxx-core.ko;   echo drivers/mfd/mc13xxx-i2c.ko;   echo drivers/mfd/mfd-core.ko;   echo drivers/mfd/ucb1400_core.ko;   echo drivers/mfd/lp3943.ko;   echo drivers/mfd/ti-lmu.ko;   echo drivers/mfd/pcf50633.ko;   echo drivers/mfd/pcf50633-adc.ko;   echo drivers/mfd/pcf50633-gpio.ko;   echo drivers/mfd/timberdale.ko;   echo drivers/mfd/intel_quark_i2c_gpio.ko;   echo drivers/mfd/lpc_sch.ko;   echo drivers/mfd/lpc_ich.ko;   echo drivers/mfd/rdc321x-southbridge.ko;   echo drivers/mfd/janz-cmodio.ko;   echo drivers/mfd/vx855.ko;   echo drivers/mfd/wl1273-core.ko;   echo drivers/mfd/si476x-core.ko;   echo drivers/mfd/cs5535-mfd.ko;   echo drivers/mfd/intel-lpss.ko;   echo drivers/mfd/intel-lpss-pci.ko;   echo drivers/mfd/intel-lpss-acpi.ko;   echo drivers/mfd/intel_pmc_bxt.ko;   echo drivers/mfd/viperboard.ko;   echo drivers/mfd/lm3533-core.ko;   echo drivers/mfd/lm3533-ctrlbank.ko;   echo drivers/mfd/retu-mfd.ko;   echo drivers/mfd/menf21bmc.ko;   echo drivers/mfd/dln2.ko;   echo drivers/mfd/rt4831.ko;   echo drivers/mfd/sky81452.ko;   echo drivers/mfd/intel_soc_pmic_bxtwc.ko;   echo drivers/mfd/mt6397.ko;   echo drivers/mfd/intel_soc_pmic_mrfld.ko;   echo drivers/mfd/intel-m10-bmc.ko;   echo drivers/mfd/atc260x-core.ko;   echo drivers/mfd/atc260x-i2c.ko; :; } | awk '!x[$$0]++' - > drivers/mfd/modules.order