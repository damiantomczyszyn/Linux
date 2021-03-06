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

MODULE_ALIAS("usb:v0130p0130d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v041Ep4018d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0733p0110d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0AF9p0010d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0AF9p0011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v8086p0110d*dc*dsc*dp*ic*isc*ip*in*");
