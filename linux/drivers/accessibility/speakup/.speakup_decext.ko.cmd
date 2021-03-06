# SPDX-License-Identifier: GPL-2.0-only
config VIDEO_CX23885
	tristate "Conexant cx23885 (2388x successor) support"
	depends on DVB_CORE && VIDEO_DEV && PCI && I2C && INPUT && SND
	select SND_PCM
	select I2C_ALGOBIT
	select VIDEO_TUNER
	select VIDEO_TVEEPROM
	depends 