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

MODULE_ALIAS("of:N*T*Cidt,89hpes8nt2");
MODULE_ALIAS("of:N*T*Cidt,89hpes8nt2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes12nt3");
MODULE_ALIAS("of:N*T*Cidt,89hpes12nt3C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24nt6ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes24nt6ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt8ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt8ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt8bg2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt8bg2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes12nt12g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes12nt12g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16nt16g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes16nt16g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24nt24g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes24nt24g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt24ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt24ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt24bg2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32nt24bg2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes12n3");
MODULE_ALIAS("of:N*T*Cidt,89hpes12n3C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes12n3a");
MODULE_ALIAS("of:N*T*Cidt,89hpes12n3aC*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24n3");
MODULE_ALIAS("of:N*T*Cidt,89hpes24n3C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24n3a");
MODULE_ALIAS("of:N*T*Cidt,89hpes24n3aC*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32h8");
MODULE_ALIAS("of:N*T*Cidt,89hpes32h8C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32h8g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32h8g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes48h12ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16h16");
MODULE_ALIAS("of:N*T*Cidt,89hpes16h16C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes22h16");
MODULE_ALIAS("of:N*T*Cidt,89hpes22h16C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes22h16g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes22h16g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes34h16");
MODULE_ALIAS("of:N*T*Cidt,89hpes34h16C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes34h16g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes34h16g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes64h16ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes12t3g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes12t3g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t3g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t3g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes4t4g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes4t4g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes10t4g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes10t4g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4ag2");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t4ag2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes5t5");
MODULE_ALIAS("of:N*T*Cidt,89hpes5t5C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes6t5");
MODULE_ALIAS("of:N*T*Cidt,89hpes6t5C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes8t5");
MODULE_ALIAS("of:N*T*Cidt,89hpes8t5C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes8t5a");
MODULE_ALIAS("of:N*T*Cidt,89hpes8t5aC*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t6");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t6C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes6t6g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes6t6g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t6g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes24t6g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t7");
MODULE_ALIAS("of:N*T*Cidt,89hpes16t7C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32t8");
MODULE_ALIAS("of:N*T*Cidt,89hpes32t8C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes32t8g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes32t8g2C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes48t12");
MODULE_ALIAS("of:N*T*Cidt,89hpes48t12C*");
MODULE_ALIAS("of:N*T*Cidt,89hpes48t12g2");
MODULE_ALIAS("of:N*T*Cidt,89hpes48t12g2C*");
MODULE_ALIAS("i2c:89hpes8nt2");
MODULE_ALIAS("i2c:89hpes12nt3");
MODULE_ALIAS("i2c:89hpes24nt6ag2");
MODULE_ALIAS("i2c:89hpes32nt8ag2");
MODULE_ALIAS("i2c:89hpes32nt8bg2");
MODULE_ALIAS("i2c:89hpes12nt12g2");
MODULE_ALIAS("i2c:89hpes16nt16g2");
MODULE_ALIAS("i2c:89hpes24nt24g2");
MODULE_ALIAS("i2c:89hpes32nt24ag2");
MODULE_ALIAS("i2c:89hpes32nt24bg2");
MODULE_ALIAS("i2c:89hpes12n3");
MODULE_ALIAS("i2c:89hpes12n3a");
MODULE_ALIAS("i2c:89hpes24n3");
MODULE_ALIAS("i2c:89hpes24n3a");
MODULE_ALIAS("i2c:89hpes32h8");
MODULE_ALIAS("i2c:89hpes32h8g2");
MODULE_ALIAS("i2c:89hpes48h12");
MODULE_ALIAS("i2c:89hpes48h12g2");
MODULE_ALIAS("i2c:89hpes48h12ag2");
MODULE_ALIAS("i2c:89hpes16h16");
MODULE_ALIAS("i2c:89hpes22h16");
MODULE_ALIAS("i2c:89hpes22h16g2");
MODULE_ALIAS("i2c:89hpes34h16");
MODULE_ALIAS("i2c:89hpes34h16g2");
MODULE_ALIAS("i2c:89hpes64h16");
MODULE_ALIAS("i2c:89hpes64h16g2");
MODULE_ALIAS("i2c:89hpes64h16ag2");
MODULE_ALIAS("i2c:89hpes12t3g2");
MODULE_ALIAS("i2c:89hpes24t3g2");
MODULE_ALIAS("i2c:89hpes16t4");
MODULE_ALIAS("i2c:89hpes4t4g2");
MODULE_ALIAS("i2c:89hpes10t4g2");
MODULE_ALIAS("i2c:89hpes16t4g2");
MODULE_ALIAS("i2c:89hpes16t4ag2");
MODULE_ALIAS("i2c:89hpes5t5");
MODULE_ALIAS("i2c:89hpes6t5");
MODULE_ALIAS("i2c:89hpes8t5");
MODULE_ALIAS("i2c:89hpes8t5a");
MODULE_ALIAS("i2c:89hpes24t6");
MODULE_ALIAS("i2c:89hpes6t6g2");
MODULE_ALIAS("i2c:89hpes24t6g2");
MODULE_ALIAS("i2c:89hpes16t7");
MODULE_ALIAS("i2c:89hpes32t8");
MODULE_ALIAS("i2c:89hpes32t8g2");
MODULE_ALIAS("i2c:89hpes48t12");
MODULE_ALIAS("i2c:89hpes48t12g2");
MODULE_ALIAS("i2c:24c32");
MODULE_ALIAS("i2c:24c64");
MODULE_ALIAS("i2c:24c128");
MODULE_ALIAS("i2c:24c256");
MODULE_ALIAS("i2c:24c512");

MODULE_INFO(srcversion, "2A0DD3851F858DEA6345BDA");
