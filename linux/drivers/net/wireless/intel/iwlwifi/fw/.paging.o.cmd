E310B         62

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
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* SPDX-License-Identifier: GPL-2.0-or-later */
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
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				