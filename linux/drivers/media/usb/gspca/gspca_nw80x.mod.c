#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "gspca_main,videodev");

MODULE_ALIAS("usb:v046DpD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0502pD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v052BpD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v055FpD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06A5p0000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06A5pD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06A5pD800d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06BEpD001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0728pD001d*dc*dsc*dp*ic*isc*ip*in*");
