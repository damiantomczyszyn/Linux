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

MODULE_INFO(depends, "dvb-usb,i2c-core,stb0899,dvb-core,stb6100");

MODULE_ALIAS("usb:v13D3p3275d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0CCDp10A4d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0CCDp10ACd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v14F7p0001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v14F7p0002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0FD9p002Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0FD9p0025d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0FD9p0036d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "2F6E52086592A7ED2B1059F");
