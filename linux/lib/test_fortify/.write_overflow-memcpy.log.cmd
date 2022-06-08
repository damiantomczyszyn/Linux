nclude/config/GENERIC_CLOCKEVENTS_BROADCAST) \
  include/linux/clockchips.h \
    $(wildcard include/config/ARCH_HAS_TICK_BROADCAST) \
  include/linux/clocksource.h \
    $(wildcard include/config/ARCH_CLOCKSOURCE_DATA) \
    $(wildcard include/config/GENERIC_GETTIMEOFDAY) \
    $(wildcard include/config/CLOCKSOURCE_WATCHDOG) \
    $(wildcard include/config/ARCH_CLOCKSOURCE_INIT) \
    $(wildcard include/config/TIMER_PROBE) \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  arch/x86/include/asm/clocksource.h \
  arch/x86/include/asm/vdso/clocksource.h \
  include/vdso/clocksource.h \
  kernel/irq/internals.h \
    $(wildcard include/config/GENERIC_IRQ_CHIP) \
  include/linux/kernel_stat.h \
  include/linux/pm_runtime.h \
    $(wildcard include/config/PM) \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $