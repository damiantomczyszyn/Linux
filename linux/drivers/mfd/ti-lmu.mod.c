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

MODULE_INFO(depends, "i2c-core");

MODULE_ALIAS("i2c:lm3631");
MODULE_ALIAS("i2c:lm3632");
MODULE_ALIAS("i2c:lm3633");
MODULE_ALIAS("i2c:lm3695");
MODULE_ALIAS("i2c:lm36274");
MODULE_ALIAS("of:N*T*Cti,lm3631");
MODULE_ALIAS("of:N*T*Cti,lm3631C*");
MODULE_ALIAS("of:N*T*Cti,lm3632");
MODULE_ALIAS("of:N*T*Cti,lm3632C*");
MODULE_ALIAS("of:N*T*Cti,lm3633");
MODULE_ALIAS("of:N*T*Cti,lm3633C*");
MODULE_ALIAS("of:N*T*Cti,lm3695");
MODULE_ALIAS("of:N*T*Cti,lm3695C*");
MODULE_ALIAS("of:N*T*Cti,lm36274");
MODULE_ALIAS("of:N*T*Cti,lm36274C*");
