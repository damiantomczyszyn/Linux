# SPDX-License-Identifier: GPL-2.0-only
config VIDEO_CX23885
	tristate "Conexant cx23885 (2388x successor) support"
	depends on DVB_CORE && VIDEO_DEV && PCI && I2C && INPUT && SND
	select SND_PCM
	select I2C_ALGOBIT
	select VIDEO_TUNER
	select VIDEO_TVEEPROM
	depends on RC_CORE
	select VIDEOBUF2_DVB
	select VIDEOBUF2_DMA_SG
	select VIDEO_CX25840
	select VIDEO_CX2341X
	select VIDEO_CS3308
	select DVB_DIB7000P if MEDIA_SUBDRV_AUTOSELECT
	select DVB_DRXK if MEDIA_SUBDRV_AUTOSELECT
	se