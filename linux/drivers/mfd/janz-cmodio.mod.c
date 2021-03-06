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

MODULE_INFO(depends, "mfd-core");

MODULE_ALIAS("pci:v000010B5d00009030sv000013C3sd00000101bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d00009050sv000013C3sd00000100bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d00009030sv000013C3sd00000201bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d00009030sv000013C3sd00000202bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d00009050sv000013C3sd00000201bc*sc*i*");
MODULE_ALIAS("pci:v000010B5d00009050sv000013C3sd00000202bc*sc*i*");
