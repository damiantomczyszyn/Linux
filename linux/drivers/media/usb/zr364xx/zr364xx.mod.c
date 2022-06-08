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

MODULE_INFO(depends, "videodev,videobuf-vmalloc,videobuf-core");

MODULE_ALIAS("usb:v08CAp0109d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v041Ep4024d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0D64p0108d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0546p3187d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0D64p3108d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0595p4343d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0BB0p500Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0FEBp2004d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v055FpB500d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v08CAp2062d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v052Bp1A18d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04C8p0729d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04F2pA208d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0784p0040d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06D6p0034d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0A17p0062d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06D6p003Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0A17p004Ed*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v041Ep405Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v08CAp2102d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06D6p003Dd*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "4208E48B2B1459D9A11118E");
