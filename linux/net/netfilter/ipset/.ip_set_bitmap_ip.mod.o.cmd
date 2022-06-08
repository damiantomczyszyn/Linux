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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   E������������5wDf���>��{i����Ԩ_`��x��X�k��
?X�a-@Ώ���oO��5j�a��&D@,0�:-�M���Y#�"TV�����oYYfJ�-����@�,�cr�~�T�&��m�C���&#j�P�x��9M��C��p���VEC�{z�k2h��[wK��Pa�?�r��4*׵�>4���HGX߫7\����\�ͪ��mE�:�����TQ�tt�n�r���E���ś�ok#�k���b�EY�
ٰT����o�|����Ԓ̵7��ȋ����d�_��Ț��6��&h^��n�I� ����k��羑t�{��2Ӿ�@�\ HW{��+����Z�$��r�"ƒ{��(s��5<Ǌ���/4�U�`���^���L�f�w��ٶ$��Ѽ0��&2�\ �h(d��H��l�vM�oT�g����V�2�x�4��{��U�֌�|4p{��J`��N�tT�����G Jx�=@ c��@rh{�j	-�v{�ڐ����OX��e��e��0ѹl��5!����I��ڌ�� �a�qz1%��%�y��Cw�L�6©��Μ�L�ʻ��[��c����~.���*�M�z+ձ���U��:>������*tPhi��Q�(ᖛ z�/�D�@޷/���}���I~rmW��/��D@S@:Qp����<CJu�[��F�]D��R@y�&��&
�f���T �gJ�j@3E�v^�)C�X��/��lʨT�KN3jf�ͪ���a�?\i�g��6,�=|Y����S�g�����pI��Wnoݬ�e]1��,k)��]%��p�:]H����ĪbM� ��Z��З��yPZ���+zg@t�K�٘1���B���`�
 �/��P-��l��I�H<�ے1��\��jE���k������8�ޅ����쭢U��c�E�ٲ?b,񗊱��,n�Y��J��$>M�2rW"��3��_`����8Y+�T6-w� ���B@Z�zq��)�����0�����}�'���ׂ�z�h��a�1"�W7m.���h�q�S9���A�t��_�N�R�2����S�7��p����:���i�ZCH�a����yz0�^m��p�u#X$g4�"U�Ї��Cu�!xcٔC�^��>UH�����R݀�/�/��i�/���LT�DہK��ؔ��U5UC���j��p��-�l��`����6�?�r#�����chQ[�jL��"j2~����>H1)qJ$�U��Q~Q��.�Be��?�7�J��G�y����.�r�U�0�)���l��)S���;�@��$���������'�cR1o#_��9^��z22�*��Pk`l���*���м#|'�YJ�SV����L8+�B�	Ӟ�+���;0����m��w���M��L�8u��K��>S%(S7k�.� "�w�gY�7��'�0!y�å�L���"�/���zϡ����MU�D���$���"�Q����+59���-� ������F�����y y�l�cjg�Թy�v�"�&�D��3�T�2�P��L7�p��5��q)m��yS��ʬl��!�/O�,N��Zp���Pҋc�^Q~��M�Zͬm5��n�ڏ⒛�Ω�ˡ�Z��P�\�*��tɣ�X�D��(i�G^�ѕ"�֧��N�����J���K�6}�R#:�u�騒q��Qm�̩���@FMg���pM��7P*e�2*a�mAk~Oj;y\q���;��I�N]Ă��[o^�F��\b3"���W����r	����ӑK9��kgt�0ԐJx���$�խ���&�CqP�%��7��i���J�ox��1�����*�sA0�����<	vw��t �S�|�����A㇘u�2s�{���zJ�⽈ɹ6f?sg�6���wQ�R�ك�� ���
�~���@((%Om�F�8�5�c�`�2=�K��hMT��l�5l92A��L�-��f�҃v����(�4����C>_x�AB��j� ���~O�@���|�b�B���fvqx%#��T��W��N���e��������}�Qs��g�����tٳ�������Q�+�_�'�i;o�Μ�}�J�=�*�a-��(K�	��"�6��g�ë��_��#,�9'mJ����1���dC��=JfI�t�bV'�������hh��(S��o�
�]��HD��9&��lx��Gr1R�$����y��>3�\��(� �B/6����Λ� �'a�L5��(��r�jG��M�Dj�1���e���6B/��F ���_���1����%%iU
D�15$en����׋�ꁶ����ٿ�f�B�O����a[�!5őh܄�vI�M�eo����<VY`�5��w �*Vm}%�V�\7sL�xf߽�^LfhB�51n�(2���8u�����ۨ��P�.y���VVM��[�A�M�V@��c��sӸ�wj6�����J���X��w"�;�z�~�"��(�������@����I�n��ʘ�B�Q��kT�gz3�t�nP!)�|�8��4]A`j:�(�����d��
����V��H�mZ0򩪋��0CZ�H�i��B�z�u;�R��6���R^�$Cv�?C{�"���|�6bQt\v>���]!�e��@�N���$�����w�>t��P�L������9
ԡ�L|�S[��bZ�����v��э��l�:����m�)�( ���S�� �<c9�����������q3��:+�!��P)io��ѫ�"t1)Հ��.	^�%����ۃG�b+���m�d?�s�;�;,�S��N�{qD�T�J}WXuڄ!�eŘmm1��AMh9�7=�~�2�gu����G�P4�'� S���9YҀ�����??&6B�� p��=�:����f�B�v ��m<e�1^[��V�ݡ~��O3�����gyv��K��J�NYfHyG��⩥��e����jg�n����ܐ���h�1���l�x������S�;�Uρ?�B��%�{�י���˻� 9[N��>u��T� �\y ��pttf�W@���7�t�7�4��$�	1�c��r���>Fm��A�^����cP9Lޣ�r�l���/�h4�+)3`�FX�yU�d��*Z���H���0K�.j�D�TW��z7T8ap����Q�H��Th}�AO&���Ry�s'X�%-	��|j	"}�F�y���=����&v��<'�#���K}�oٙZ7��UJڷ,Yiu߱���Y��e�d���'��[%c��k�z�L��0�uw��r�����e�������7�|����'ĭ`P���ݜq*h�f�$���?�B'L�O׺~���iUK�k3��YF�e+�sN�e��7j?)8�l90��C�v���㺃c>��%ϻ����޴ζ�?���"��G#���U�@��!Y�2���ayE�A.n��0�٪�7bL��"�ah]�����DM�we���"*�G��U�#����� �v�����b����Iݭ�8��7$���J)��(��tM-v*:G�|�Wx�"���K�j�`��[�ַ�n"U�9]��Ȯv0��6�\E�X�R�(\Gŋ~������`7%�أ�D��Ι�U1vK�]GU�y�X�Ɇ++/A5QS�y)
��n:C����f��5=�N�`�?�~��
=/�b�L����mv�Y��k L`�0��/��3H[ѹݹ0��b��M5V0������j��|�'t��;��@���>˫q
N�I4 �}����aAD�FpǪ�6^�^��rg,m���ɜ�G��Mm����䅵��pf�B��c�k�6��X�m��6y\�O飡Q�������>����>,n��Z���c�_�/�5��!6.��vO�m�?Ip�9=��K��b2ؖ*�⯯��$�� /W����f�~LC| �Y�h��L�33�7G9�7�9/�鳎��H��u������*{<A��cYL@��m�S�e�&?����������'S����W�6��[�k����3��&��}�0���1���V'6�}��-؈��}Xi�=[�{�������e{-/����L���������c����U��	&u�4K8�.�n.�����e6F-0,�yP9y~��<R4�\�"C-���]�t@��)9���,�׭��ɴ:.fM�c�1G��<�%�/�U"g��Q?�f�;���#��5VWC}����^�+�N;�Զ�]b �ձ�iS��g=�=��+6e��x�\�wp0#����E���8�N���J[V�(��yy�0Gj2��n^\��G� �.�׺W��T�4�-���j�X��j;���#�L�Jȗ|��Nd5#ϗ�B��Z�yo��k���K/���
	3�3fAgw�����o�����qTJb���|�R9tR���0��p浯ח��-ծ��9V"�v素������4��rΗ��w���,�Q6ָǯ�6Q�f���B#LՌ��7<���L����	��o��Ϥ�b�"W��{r�,T��I�9��?�AQ�w�Ved��o=׈,�i��*ϫⳔ��
6~�^�~NkIk�iţ�Xk�%�%�)�+�ʞ����Jr�6�}�Q'ވ�H���ۂ\�H,)��S��gP�C��e�L��oE��sV�eΐG�H�T��ϻ�Y�G�&C��6@�e�23�Q��/I���h�G�C �Qa��I (zP�޼�J��k�ɛ�´��m?�S�!�[���-M�Eʯr�c���e�_��X�	G �'ZIڲz�]�^���z�"A�Ea�L�g7"�y ���v�'12�
ڔ�j[/=V���,���1l[�g�aC�A���)i��v�V���y�g���Q��(���6��<�i�1M�CQΖ=�P�+�K��=]�R�|�(T�:*٤$m�n,�gZ0ձ���,�P53.�'~��5����M���y�}���両0Z�~ʯg*��'xڣP�	��ȁ��"ǁ�~U� ��f�s���qq��uך��:~%����}q'��� ��*�5=�<�Y��`�h��3��	��=��^t
1�]dysS� �"��W?��Y�C�x�L4��6�Ġ��e��
��X�:�K��A��K�(i {.zSC&(���l�MU #��-1+�������E�9F?�i+YEBEL����*v�,Ƨ:���qXJ��<�΀ZNd�HHcp�^����Q|��m�@Lx�˾h {L)����-��>1��(6��|�(_��!�eIHz���`���H�������|n�j\��m9������|�;�ȥ������'MY�#�Gb��*:ߜ'�s$uk�xM;��ܷ���N��1�d޽���x���h1��*ཀྵ����-k��)A�i�ܿD� :�q����}w��xb��S]��r$��Zx������K�O-ԓ���({ƣ;(�Ⱦz|;Y��v�Fޏ"������@8��S�� ��`���ɻ!�A��4]�_���
������5?����Ws��qX4Cw��+���F���oa���+�#S��K���z��e��A�!�*��ɤ�V'�J=�ͬ�6VF�h[��
s����F> 7�u���ta��,M������ص
�y��md����Ƭ��ߖ7�MV9%����v�F�a4��&�����γ��8����3bk��-���yw���i��l�A����ޙ.fzgE��?K�	�}��Ź���W��o-@���G����O�U0h~�z�-�E�u�*�=�NA �Zx~�z�m�+�Y0	o�W�;���ت���'��q������=��N�&�E�ê(��~G���?����5��L��u�
�3a� ��C}A��N'BU�q��M�"���?��Ӱ��p���?jMR�+al���$P�g]t�Rܨ� �X�!�ߥl`X��p��J���T[-毄���	������5�쐔�}"�5��0
T�����Z�͋y��q�\=y_M��|��hg�([ml�B+����܍�>�,q����H�/���H�E�u��ݨ�+�Ø���|�+8Z�J�z���<�Tf�hY�Ma��8����rL.2��i�lS$��U��i�����E{�w�	�����K�V�4JqsƓ;��0@QV�ա��G �B-�~q�X�=>�2%HE��R}ϭ��=.Ӣ�ݓ7�z�Π�=
��R|��mXw���7��E��o�],\��έ� �&�[�j�	.R�
L�^�� ���ܖK�-��
�ǲi�O	S,�%FM�Cf��bg|��gL�L��bɽ7o�C�x�e��Ɂ�TH�j����^���O����fn��f�SP��p�l`w)�`�d�EC�Ef�Fd����§�n���X���{T$-���^��JS�+�==��?�4`W���Z��`[������53�sR��{VW_C��-�zK������Q_�X�����$��R ;>�`٧縮@ �#Ǐ-��Wf� �o�fj�$�Y��\�B��bY�P˫\�٘��Ui0�>k�쐧���H�����"U^����?0>�?��>����"5�D��k~��z���J���` t�����OI|�0��ei-Z�����%;�$g}U�5q)��C"���f(���5���ll^��^<��i�%�d��O`,5H���X�G�|DRQ��;�M��D��:��� vB�uV|���2��*����^R�[t|��d7G�)�����9x�ʋ��k��d�&;G�'�!ҟ�R��3�9*\��~o�۾��5��M�|���Q�e&Y{�<^��g '�>�q[��eH\a�,�y����V�|BC��?��r��] �uK_�8�x��X�!�L�6�QS����9�N��d��ZE���\9���9�4KHXI([?�P��je���P����#�!�36Է����mʔx#0P��W��%��.'�/K>=L��eHL$$�R;�R 5�lKI�$|�8��CwD7��uz�ix6�Q[d��߽��9�<𺐿�n:m���&Uנ'E�n�ް8v/=)U3������
��X�(��p����5gu�`bB.�7ݙ],9������Z%�[�k}�]��0��nrļ�SLd]� ���b�᝱�o�L�u��A�,[�v�=�z]�':��
���UET�m �0e`����"GR���&C