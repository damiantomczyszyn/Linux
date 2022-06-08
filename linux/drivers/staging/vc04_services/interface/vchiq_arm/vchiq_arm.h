/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci.h>
#include <linux/i2c.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dvb.h>
#include <media/rc-core.h>

#include "cx23885-reg.h"
#include "media/drv-intf/cx2341x.h"

#include <linux/mutex.h>

#define CX23885_VERSION "0.0.4"

#define UNSET (-1U)

#define CX23885_MAXBOARDS 8

/* Max number of inputs by card */
#define MAX_CX23885_INPUT 8
#define INPUT(nr) (&cx23885_boards[dev->board].input[nr])

#define BUFFER_TIMEOUT     (HZ)  /* 0.5 seconds */

#define CX23885_BOARD_NOAUTO               UNSET
#define CX23885_BOARD_UNKNOWN                  0
#define CX23885_BOARD_HAUPPAUGE_HVR1800lp      1
#define CX23885_BOARD_HAUPPAUGE_HVR1800        2
#define CX23885_BOARD_HAUPPAUGE_HVR1250        3
#define CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP   4
#define CX23885_BOARD_HAUPPAUGE_HVR1500Q       5
#define CX23885_BOARD_HAUPPAUGE_HVR1500        6
#define CX23885_BOARD_HAUPPAUGE_HVR1200        7
#define CX23885_BOARD_HAUPPAUGE_HVR1700        8
#define CX23885_BOARD_HAUPPAUGE_HVR1400        9
#define CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP 10
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP 11
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H 12
#define CX23885_BOARD_COMPRO_VIDEOMATE_E650F   13
#define CX23885_BOARD_TBS_6920                 14
#define CX23885_BOARD_TEVII_S470               15
#define CX23885_BOARD_DVBWORLD_2005            16
#define CX23885_BOARD_NETUP_DUAL_DVBS2_CI      17
#define CX23885_BOARD_HAUPPAUGE_HVR1270        18
#define CX23885_BOARD_HAUPPAUGE_HVR1275        19
#define CX23885_BOARD_HAUPPAUGE_HVR1255        20
#define CX23885_BOARD_HAUPPAUGE_HVR1210        21
#define CX23885_BOARD_MYGICA_X8506             22
#define CX23885_BOARD_MAGICPRO_PROHDTVE2       23
#define CX23885_BOARD_HAUPPAUGE_HVR1850        24
#define CX23885_BOARD_COMPRO_VIDEOMATE_E800    25
#define CX23885_BOARD_HAUPPAUGE_HVR1290        26
#define CX23885_BOARD_MYGICA_X8558PRO          27
#define CX23885_BOARD_LEADTEK_WINFAST_PXTV1200 28
#define CX23885_BOARD_GOTVIEW_X5_3D_HYBRID     29
#define CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF 30
#define CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000 31
#define CX23885_BOARD_MPX885                   32
#define CX23885_BOARD_MYGICA_X8507             33
#define CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL 34
#define CX23885_BOARD_TEVII_S471               35
#define CX23885_BOARD_HAUPPAUGE_HVR1255_22111  36
#define CX23885_BOARD_PROF_8000                37
#define CX23885_BOARD_HAUPPAUGE_HVR4400        38
#define CX23885_BOARD_AVERMEDIA_HC81R          39
#define CX23885_BOARD_TBS_6981                 40
#define CX23885_BOARD_TBS_6980                 41
#define CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200 42
#define CX23885_BOARD_HAUPPAUGE_IMPACTVCBE     43
#define CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2 44
#define CX23885_BOARD_DVBSKY_T9580             45
#define CX23885_BOARD_DVBSKY_T980C             46
#define CX23885_BOARD_DVBSKY_S950C             47
#define CX23885_BOARD_TT