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
#define CX23885_BOARD_TT_CT2_4500_CI           48
#define CX23885_BOARD_DVBSKY_S950              49
#define CX23885_BOARD_DVBSKY_S952              50
#define CX23885_BOARD_DVBSKY_T982              51
#define CX23885_BOARD_HAUPPAUGE_HVR5525        52
#define CX23885_BOARD_HAUPPAUGE_STARBURST      53
#define CX23885_BOARD_VIEWCAST_260E            54
#define CX23885_BOARD_VIEWCAST_460E            55
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB     56
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC    57
#define CX23885_BOARD_HAUPPAUGE_HVR1265_K4     58
#define CX23885_BOARD_HAUPPAUGE_STARBURST2     59
#define CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885 60
#define CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885 61
#define CX23885_BOARD_AVERMEDIA_CE310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	