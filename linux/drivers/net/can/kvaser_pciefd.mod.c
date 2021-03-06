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

MODULE_INFO(depends, "can-dev");

MODULE_ALIAS("pci:v00001A07d0000000Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001A07d0000000Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001A07d0000000Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001A07d00000010sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001A07d00000011sv*sd*bc*sc*i*");
