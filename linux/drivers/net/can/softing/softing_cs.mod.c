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

MODULE_INFO(depends, "pcmcia");

MODULE_ALIAS("pcmcia:m0168c0001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0004f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0005f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0081f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0084f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0085f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0102f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0168c0105f*fn*pfn*pa*pb*pc*pd*");
