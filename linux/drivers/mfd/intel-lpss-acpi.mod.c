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

MODULE_INFO(depends, "intel-lpss");

MODULE_ALIAS("acpi*:INT3440:*");
MODULE_ALIAS("acpi*:INT3441:*");
MODULE_ALIAS("acpi*:INT3442:*");
MODULE_ALIAS("acpi*:INT3443:*");
MODULE_ALIAS("acpi*:INT3444:*");
MODULE_ALIAS("acpi*:INT3445:*");
MODULE_ALIAS("acpi*:INT3446:*");
MODULE_ALIAS("acpi*:INT3447:*");
MODULE_ALIAS("acpi*:INT3448:*");
MODULE_ALIAS("acpi*:INT3449:*");
MODULE_ALIAS("acpi*:INT344A:*");
MODULE_ALIAS("acpi*:INT34B0:*");
MODULE_ALIAS("acpi*:INT34B1:*");
MODULE_ALIAS("acpi*:INT34B2:*");
MODULE_ALIAS("acpi*:INT34B3:*");
MODULE_ALIAS("acpi*:INT34B4:*");
MODULE_ALIAS("acpi*:INT34B5:*");
MODULE_ALIAS("acpi*:INT34B6:*");
MODULE_ALIAS("acpi*:INT34B7:*");
MODULE_ALIAS("acpi*:INT34B8:*");
MODULE_ALIAS("acpi*:INT34B9:*");
MODULE_ALIAS("acpi*:INT34BA:*");
MODULE_ALIAS("acpi*:INT34BC:*");
MODULE_ALIAS("acpi*:80860AAC:*");
MODULE_ALIAS("acpi*:80860ABC:*");
MODULE_ALIAS("acpi*:80860AC2:*");
MODULE_ALIAS("acpi*:80865AAC:*");
MODULE_ALIAS("acpi*:80865ABC:*");
MODULE_ALIAS("acpi*:80865AC2:*");
