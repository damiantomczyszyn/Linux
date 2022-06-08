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

MODULE_INFO(depends, "pcmcia,mtd,chipreg");

MODULE_ALIAS("pcmcia:m*c*f01fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa547E66DCpb1FED36CDpc36EADD21pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paB569A6E5pb36EADD21pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paB569A6E5pb8BC54D2Apc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paB569A6E5pb6DF1BE3Epc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa816CC815pbD14C9DCFpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa816CC815pbA2D7DEDBpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa40ADE711pb145CEA5Cpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa40ADE711pb42064DDApc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa40ADE711pb25EE5CB0pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa40ADE711pbDF8506D8pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa54D0C69CpbAD12C29Cpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paB68968C8pb2DFB47B0pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa7ED2AD87pb675DC3FBpc7AEF3965pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paEBF91155pb805360CApc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paEBF91155pb20B6BF17pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paF9876BAFpbAD0B207Bpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paF9876BAFpbE8D884ADpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa96FD8277pb737A5B05pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa05DDCA47pbE7D67BCApc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa05DDCA47pb7BC32944pc*pd*");
