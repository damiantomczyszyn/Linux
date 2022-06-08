e        *video_dev;
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
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // SPDX-License-Identifier: GPL-2.0-or-later
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
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23888_IR_H_
#define _CX23888_IR_H_
int cx23888_ir_probe(struct cx23885_dev *dev);
int cx23888_ir_remove(struct cx23885_dev *dev);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later

/*
 * netup-eeprom.c
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#
#include "cx23885.h"
#include "netup-eeprom.h"

#define EEPROM_I2C_ADDR 0x50

int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr)
{
	int ret;
	unsigned char buf[2];

	/* Read from EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &buf[0],
			.len	= 1
		}, {
			.addr	= EEPROM_I2C_ADDR,
			.flags	= I2C_M_RD,
			.buf	= &buf[1],
			.len	= 1
		}

	};

	buf[0] = addr;
	buf[1] = 0x0;

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		pr_err("eeprom i2c read error, status=%d\n", ret);
		return -1;
	}

	return buf[1];
};

int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data)
{
	int ret;
	unsigned char bufw[2];

	/* Write into EEPROM */
	struct i2c_msg msg[] = {
		{
			.addr	= EEPROM_I2C_ADDR,
			.flags	= 0,
			.buf	= &bufw[0],
			.len	= 2
		}
	};

	bufw[0] = addr;
	bufw[1] = data;

	ret = i2c_transfer(i2c_adap, msg, 1);

	if (ret != 1) {
		pr_err("eeprom i2c write error, status=%d\n", ret);
		return -1;
	}

	mdelay(10); /* prophylactic delay, datasheet write cycle time = 5 ms */
	return 0;
};

void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo)
{
	int i, j;

	cinfo->rev =  netup_eeprom_read(i2c_adap, 63);

	for (i = 64, j = 0; i < 70; i++, j++)
		cinfo->port[0].mac[j] =  netup_eeprom_read(i2c_adap, i);

	for (i = 70, j = 0; i < 76; i++, j++)
		cinfo->port[1].mac[j] =  netup_eeprom_read(i2c_adap, i);
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-eeprom.h
 *
 * 24LC02 EEPROM driver in conjunction with NetUP Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef NETUP_EEPROM_H
#define NETUP_EEPROM_H

struct netup_port_info {
	u8 mac[6];/* card MAC address */
};

struct netup_card_info {
	struct netup_port_info port[2];/* ports - 1,2 */
	u8 rev;/* card revision */
};

extern int netup_eeprom_read(struct i2c_adapter *i2c_adap, u8 addr);
extern int netup_eeprom_write(struct i2c_adapter *i2c_adap, u8 addr, u8 data);
extern void netup_get_card_info(struct i2c_adapter *i2c_adap,
				struct netup_card_info *cinfo);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // SPDX-License-Identifier: GPL-2.0-or-later
/*
 * netup-init.c
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "netup-init.h"

static void i2c_av_write(struct i2c_adapter *i2c, u16 reg, u8 val)
{
	int ret;
	u8 buf[3];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 3
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static void i2c_av_write4(struct i2c_adapter *i2c, u16 reg, u32 val)
{
	int ret;
	u8 buf[6];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 6
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val & 0xff;
	buf[3] = (val >> 8) & 0xff;
	buf[4] = (val >> 16) & 0xff;
	buf[5] = val >> 24;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);
}

static u8 i2c_av_read(struct i2c_adapter *i2c, u16 reg)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg = {
		.addr	= 0x88 >> 1,
		.flags	= 0,
		.buf	= buf,
		.len	= 2
	};

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c write error!\n", __func__);

	msg.flags = I2C_M_RD;
	msg.len = 1;

	ret = i2c_transfer(i2c, &msg, 1);

	if (ret != 1)
		pr_err("%s: i2c read error!\n", __func__);

	return buf[0];
}

static void i2c_av_and_or(struct i2c_adapter *i2c, u16 reg, unsigned and_mask,
								u8 or_value)
{
	i2c_av_write(i2c, reg, (i2c_av_read(i2c, reg) & and_mask) | or_value);
}
/* set 27MHz on AUX_CLK */
void netup_initialize(struct cx23885_dev *dev)
{
	struct cx23885_i2c *i2c_bus = &dev->i2c_bus[2];
	struct i2c_adapter *i2c = &i2c_bus->i2c_adap;

	/* Stop microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x00);

	/* Aux PLL frac for 27 MHz */
	i2c_av_write4(i2c, 0x114, 0xea0eb3);

	/* Aux PLL int for 27 MHz */
	i2c_av_write4(i2c, 0x110, 0x090319);

	/* start microcontroller */
	i2c_av_and_or(i2c, 0x803, ~0x10, 0x10);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * netup-init.h
 *
 * NetUP Dual DVB-S2 CI driver
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */
extern void netup_initialize(struct cx23885_dev *dev);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ¬2     4     (            GNU  À       À          èüÿÿÿWV‰Æ‰Ğ‰ÊS‹Ü   ‹|$‹™  ƒãıƒø‹Ü   Àƒàş  	Ø‰  ‹†Ü      ‹ 1Û€ä_…Ò•ÃÁã	Ã‹†Ü   ‰˜   ‰Ø‰ÚÁøƒà…ÿEø‹†Ü   €Î ‰   ¸Ç  èüÿÿÿ‹†Ü   ‰˜   ‰ø[^_ÃfèüÿÿÿS‰ÃÃ¼  ƒìHd¡    ‰D$D‰àèüÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Õˆ ‡6  =Óˆ ‡   =Q… ‡   =O… ‡   =í„ ‡
  =ë„ ‡   =Wu „   ‡î  =9K „   ‡İ  =ÅØ „   ‡Ì  =»Ø „   ‡»  =ÙN „   ‡ª  =L „   ‡™  =o> „   ‡ˆ  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Ó6 „   ‡D  =É6 „   ‡3  =6 „   ‡"  =ı4 „   ‡  =™4 „   ‡   ='3 „   =Ã2 ‡ê   =¤2 ‡?   =»0 „   ‡Î   =û, „   ‡½   =ñ, „   ‡¬   =Ó, „   ‡›   =É, „   ‡Š   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=¿ „   w,=¼ „   w=a „   w=V  ‡üÿÿÿ=ğU  wPSh    èüÿÿÿƒÄé   éo   ´&    v èüÿÿÿQÿUWVSƒú†Ó   ‹8‰Î…É…í   ‹¸  ƒùwƒùws[‰ğ^_]Ãƒù„Í   ƒù,„®   wåƒù*u¸ûÿÿÿ½   ‹—Ü   Â  ‹
!È‰»È   ¶    ¸X‰A èüÿÿÿƒëuñ‹‡Ü     ‹€  	Å‰*[‰ğ^_]Ãº   Óâ÷Â`2 u€æu@ƒù…pÿÿÿ¸ıÿÿÿ½   ëv ƒéƒá÷„uÿÿÿéPÿÿÿ´&    1ö[‰ğ^_]Ã´&    ‹hƒıtƒı…(ÿÿÿéCÿÿÿ‹P‰øèüÿÿÿ‰ğ[^_]Ã¸şÿÿÿé2ÿÿÿ´&    èüÿÿÿS‰Ã‹€¸  ƒø„	  wwƒø	„#  †   ƒø„'  †Q  ƒøt†  ƒø„½  ¹   ‰Øº `  èüÿÿÿº `  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº @  èüÿÿÿ¸d   [éüÿÿÿ´&    ƒø)‡  ƒø'‡  ƒø!„Ã  †G  ƒø&„r  ƒø'…é  ‹ƒÜ   (  ‹ƒâş‰‹ƒÜ     ‹Ê   ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âşÿşÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âüÿüÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âıÿıÿ‰‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿûÿ‰‹ƒÜ     ‹€    ‰¸<   [éüÿÿÿ´&    fƒøvPƒø„î   ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   ‰Øèüÿÿÿ¸d   [éüÿÿÿƒø$…[  ¹   ‰Øº`  èüÿÿÿ‰Øº`  èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   [éüÿÿÿv ƒø„/  v]ƒøuT‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâû‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãfƒøt£ƒøuò‹ƒÜ     ‹€    ‰[Ãv ƒø„  †  ƒø†hÿÿÿƒøu·‹ƒÜ   º6   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ      ‹ƒÊ‰¸È   èüÿÿÿ‹ƒÜ      ‹€æ÷‰¸È   èüÿÿÿ‹ƒÜ      ‹€   €Ì‰¸È   [éüÿÿÿ´&    ƒø„·  †É   ƒøt	ƒø…ÿÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãƒø
…¸şÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâğ‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ã¶    ƒø„aşÿÿƒø„nÿÿÿƒø…Aşÿÿ¹   ‰Øº À  èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   èüÿÿÿ¸   [éüÿÿÿt& ƒø1„÷  †   ƒø5w\ƒø3wc‹ƒÜ   º7   ‰(  ¹   ‰Øº  èüÿÿÿº  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº  [éüÿÿÿfƒø%„Œıÿÿé>ıÿÿfƒè8ƒø‡0ıÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   é=üÿÿƒø-„cÿÿÿ†ª   ƒø1„çüÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âıÿşÿ‰º7   ‹ƒÜ   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ   º Ã  ‰   1Ò‹ƒÜ   ‰  [Ãƒø*„éûÿÿƒø,„|ıÿÿ[Ã¶    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âùÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹ƒÊ‰º7   ‹ƒÜ   ‰(  º P  ‹ƒÜ   ‰$  º   ‹ƒÜ   éXÿÿÿ´&    ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº   [éüÿÿÿt& ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿébüÿÿf¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   éIúÿÿ´&    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âûÿüÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  é-şÿÿ´&    t& èüÿÿÿUWVS‰Ãƒì0‹‹¸  d¡    ‰D$,1Àƒù„w  vEƒéƒùw¸   Óà© ãud¨…×  ƒùt71ö‹D$,d+    …±  ƒÄ0‰ğ[^_]Ãt& ƒù„§   1öƒùvĞƒùuh    1öjèüÿÿÿXZë¹´&    fƒùu«‹5   …ötŸº   ‰Øèüÿÿÿ‰ƒ¨  …À„?  ‹ƒ”  …À„wÿÿÿ‹Pd‹…Ò„jÿÿÿ‹Z…Û„_ÿÿÿ‹    ¹   …Ò„æ  ‹r…ö„Û  º   èüÿÿÿ1öé2ÿÿÿf‹   …É„ ÿÿÿº   ‰Øèüÿÿÿ‰ƒ¨  …À„À  ‹ƒ”  …À„øşÿÿ‹Pd‹…Ò„ëşÿÿ‹Z…Û„àşÿÿ‹    ¹    …Ò„x  ‹r…ö„m  º   èüÿÿÿ1öé³şÿÿv ‰Øèüÿÿÿ‰Æ…À…Ÿşÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‹ƒ”  …À„}şÿÿ‹Pd‹…Ò„pşÿÿ‹Z…Û„eşÿÿ‹    ¹   …Ò„İ  ‹z…ÿ„Ò  º   èüÿÿÿé:şÿÿ´&    v ‰Øèüÿÿÿ‰Æ…À…şÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‰Á‹ƒ”  …ÀtA‹Pd‹…Òt8‹z…ÿt1‹    ¹    …Ò„@  ‹j…í„5  º   èüÿÿÿ‹‹¨  v …É„¶ıÿÿ‹Ad‹@…À„Ú   ‹x…ÿ„Ï   ¡   ‰â…À„Ô   ‹h…í„É   ‰Èèüÿÿÿ‹‹¨  ÆD$ ÆD$
 ÆD$…É„_ıÿÿ‹Ad‹@…Àtw‹x…ÿtp¡   ‰â…À„¸   ‹h…í„­   ‰Èèüÿÿÿ‹‹¨  ÆD$
…É„ıÿÿ‹Ad‹@…À„ıÿÿ‹X…Û„ıÿÿ¡   ‰â…ÀtN‹x…ÿtG‰Èèüÿÿÿéåüÿÿt& ÆD$
ë½´&    fÆD$ ÆD$
 ÆD$é[ÿÿÿ‰Èèüÿÿÿ‹‹¨  é2ÿÿÿ‰Èèüÿÿÿéüÿÿº   èüÿÿÿ‹‹¨  éÉşÿÿ‰Èèüÿÿÿ‹‹¨  éNÿÿÿº   èüÿÿÿéhüÿÿº   1öèüÿÿÿéWüÿÿº   èüÿÿÿéFüÿÿ¾íÿÿÿé>üÿÿèüÿÿÿ´&    fèüÿÿÿ‹ˆ¸  ƒù3wpS‰Ãƒùvƒé¸   Óà© ãu¨u3[Ãƒùt
ƒùt&ƒùuî‰Øº   èüÿÿÿÇƒ¨      [Ã´&    f‰Øº   èüÿÿÿ‰ØèüÿÿÿÇƒ¨      [ÃÃ´&    ´&    èüÿÿÿ‹ˆ¸  ƒù3wƒùvƒéº   Óâ÷Â ãuƒâu,Ãv ƒùt
ƒùtƒùuí‹¨  …Òtãº   éüÿÿÿt& ‹ˆ¨  …ÉtÊº   éüÿÿÿt& èüÿÿÿWVS‰Ãƒì‹»`  d¡    ‰D$1À…ÿ„9  ‹ƒ¸  ‰Âƒø	t‡æ  ƒø„¬  Hûƒù‡1  ‰Âƒø&†  ƒø2„µ  †ç  ƒø5„f  †Ø  ƒø;„ˆ  †¶  HÄƒù‡\  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     ƒø"‡½  Jú¸   Óà©ÅXß„³  j “
  ¹9   CjDèüÿÿÿ‰ƒ”  ZY…À„Ô  “ğ   ‰˜   ‹ƒ”  Ç€      ‹ƒ”  …À„ª  ‹Pd‹…Ò„  ‹R…Ò„’  ‹    …É„|  ‹I…É„q  èüÿÿÿ‹“¸  éo  ƒøt3†Û   Höƒù‡X  Çƒ     Çƒ      Çƒ$     v ÇƒØ     ÇƒÜ     Çƒà     ƒú"‡¶  ƒú‡ğşÿÿBÿƒø†ùşÿÿƒú„"  ƒú„  ‹D$d+    …3  ƒÄ[^_ÃfHÊƒù‡%şÿÿ¾P   ƒô  ¹   º    f‰³ö  èüÿÿÿ‹»`  …ÿ„
  ‹ƒ¸  ééıÿÿƒø…€  ÇƒØ    ‹ƒÜ   º   ÇƒÜ     Çƒà     Çƒä      Çƒè  ÌG ‰„ Çƒ     ‹“¸  Çƒ      Çƒ$     éûşÿÿt& èüÿÿÿv ‹“¸  ƒú6…Á   j “ü   C¹A   jAèüÿÿÿ‹“¸  _Xƒú…Şşÿÿ‹D$d+    …  ƒÄ‰Ø[^_éüÿÿÿHÈƒù†Jıÿÿé¡  fƒø3…§   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     BÜƒø‡Mÿÿÿ¹ùş}£Á‚>ıÿÿƒú6„?ÿÿÿƒú7…Uÿÿÿj »ü   s¹A   j@‰ú‰ğèüÿÿÿ‰ú¹A   ‰ğj jAèüÿÿÿ‹“¸  ƒÄéÿÿÿt& ƒø4…æ  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     éKÿÿÿ¶    ƒø)wKƒø'…  ÇƒØ     ÇƒÜ     Çƒà     Çƒ    Çƒ      Çƒ$     éNüÿÿƒø-„°şÿÿv,ƒø2„;  ÇƒØ     ÇƒÜ     Çƒà     éøûÿÿƒø,„·üÿÿé
  v ƒø‡×   ƒø„ö  †€üÿÿƒø„fıÿÿ†	  ƒø…×   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é­üÿÿ´&    ¾P   º    ƒl  ¹   f‰³n  èüÿÿÿ‹ƒ¸  ‰Âƒø†®   HÜƒù‡³úÿÿ¾   Óæ‰ñ÷Æ… ó…H  á   …müÿÿƒø&‡’úÿÿHâ¾   Óæ‰ñöÁ¨…×şÿÿƒá…­úÿÿƒø&„¡  Çƒ     ‰ÂÇƒ      Çƒ$     éÂûÿÿt& ƒø„rúÿÿvHêƒù†…şÿÿë¾´&    …vşÿÿë¯…Àt«‰Á¾   Óæ‰ñ÷Æ†<……   á`  u]ƒø……şÿÿ‹‹`  …É…uÿÿÿ€=    „tKºà   ‰ØèÑçÿÿ‹ƒ¸  ‰Âéµùÿÿt& ƒú…müÿÿ‹5   …ö„Oûÿÿé1úÿÿt& ‹“`  …Ò…ùÿÿº    ‰Øè†çÿÿé|ûÿÿ‹‹`  …Ét™éşÿÿÇƒØ     ÇƒÜ     Çƒà     éóúÿÿt& ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é”ùÿÿ´&    ÇƒØ     ‹ƒÜ   ÇƒÜ     Çƒà       Çƒ     Çƒ      Çƒ$     ‹Ê  ‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹ƒâı‰¹   º'  ¸è  1öèüÿÿÿv ‹ƒÜ     ‹ƒâø‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹:‰ğ‰ñÁø÷Ñ¶€    ƒáÓø‰Áƒá‰ÈƒÈ÷Ğ!ø	ÈƒÈ‰¹   º'  ¸è  ƒÆèüÿÿÿƒşHu†‹ƒÜ     ‹ƒÊ‰‹“¸  éOùÿÿ´&    fÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é5ùÿÿ´&    ÇƒØ     ÇƒÜ     Çƒà     éùÿÿƒ»`   „{ıÿÿéD÷ÿÿ‰Ø‰\$ÇD$H   ÇD$    èüÿÿÿƒü   º    èüÿÿÿ¡ŒF  …À„6  é1  èüÿÿÿèüÿÿÿ‹P(Ç   ‹‚ì  ¯‚ğ  ‹’è  ¯B‹T$Áè‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ãƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®ø  ø  ‰Âƒx  9étS‹«ˆ  M    ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ‹E‹‹  ƒ=x  ‰H‡ø  ƒÄ‰ø[^_]éüÿÿÿt& ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ƒ=x  ‡Ö  ƒÄ‰ø[^_]éüÿÿÿfèüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâî‰½¸  ø  ‰øèüÿÿÿ‰Æ‹…ø  9Ãt@v ‹…ø  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…ø  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿVSèüÿÿÿ‹°(  ‹FƒÆ9Æt$X´‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Æuà1À[^Ã´&    t& èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwPÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u§¸êÿÿÿëæ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwH‹…ÀuK‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u¯¸êÿÿÿëæèüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwX‹…Àu[F¹   ºª  ƒÇèüÿÿÿ‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  uŸ¸êÿÿÿëæèüÿÿÿS‰Ëèüÿÿÿƒ;‹(  uH‹Cƒèƒøw=ÇC    ÇC    ÇCĞ  ‹‚x  % ù  ƒøÀƒà`à  ‰C1À[Ã´&    f¸êÿÿÿ[Ã´&    fèüÿÿÿV‰ÎS‹\$èüÿÿÿ‹€(  ‹€x  %ÿÿ ƒşu(ƒøÒƒâÕƒÂ6ƒøÀ‰ƒàÏƒÀ;‰C1À[^Ã´&    ¸êÿÿÿëî´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…  ‹†x  ‹–|  ‰1À‰S[^Ã´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹(  ‹‚ì  ‰C‹Šğ  ‰K‹²ô  ‰s‹²è  ‹6‰s‹’è  ¯BÇC   Áè¯È‰C1À‰K[^Ãt& èüÿÿÿ‹…Àu¡ÔI  ‰A,1ÀÃ´&    v ¸êÿÿÿÃ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿU1ÒWVS‰Ãƒì‹ ‹p(‰Øèüÿÿÿ‹k‰Ç‹†è  ‹@¯†ì  Áè‰ƒ€  ‰Â…í…¼   ¯–ğ  …Ò…  ‹–ô  ƒú„À   wTƒú„+  ƒú…  ‹®Ì   ÿ¶ğ  “„  j P‰èj jÿ‹èüÿÿÿƒÄƒ=x  ‡6  1ÀƒÄ[^_]Ã¶    ƒú„  ƒú…J  ‹ğ  ‹®Ì   “„  ÑéQ¯Èj P‰èj Q‹èüÿÿÿƒÄë¥´&    v ¯–ğ  9ST‚  ‰SP‹–ô  ƒú…@ÿÿÿ‹x  ÷†x   ù  ti¸    ‹‰  …É„´   …Ò…p  Ç$    ‰Á‹–Ì   «„  ‰T$‹–ğ  ÑêR‰êQQPÿt$‹‹D$èüÿÿÿƒÄé	ÿÿÿ¶    ‹®Ì   ÿ¶ğ  “„  j Pjÿj ‹‰èèüÿÿÿƒÄéÙşÿÿ¶    ‹ğ  ‹®Ì   “„  ÑéQ¯Èj PQëÉ´&    v ƒÄ¸êÿÿÿ[^_]Ãv …Ò…’  ‰$‰Á1ÀéIÿÿÿ¸êÿÿÿéŠşÿÿèüÿÿÿVS‰Ëèüÿÿÿ‹5ÔI  ‹€(  9s…­   ‹€x  ‹S% ù  ƒøÀƒà`à  …Òt^ƒúwyƒú…   º   j s¹Ğ  j PC‰Sº0   j P‰ğjèüÿÿÿ‹C¯ØI  ÇC   ƒÄÁè‰C¯C‰C1À[^Ã‰Áº   Ñé9Kw§º   ‰Èë´&    fJüƒù¹   CÑë…¸êÿÿÿ[^Ã´&    Ñè‰Á‰Èéjÿÿÿt& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìpd¡    ‰D$l‰Øèüÿÿÿ¹   ‹€(  |$ÇD$   ‰$1Àƒ=x  ó«‡´  ‰é‰ò‰Øè·şÿÿ‰D$‰Á…À…U  ‹<$‹Ÿ   …Û…]  ‹—`  …Ò…O  ‹‡Ä  …À…A  ¡ÔI  9E¸ÔI  EÁƒ=x  ‰‡è  ‹E‰‡ì  ‹U‰—ğ  ‹M‰ô  ‡Ğ  ‹E‹]0ÇD$$   ‹<$‹u‹M‹U‰D$(‰D$‹E(ƒÇ‰t$ f‰D$0f‰D$‹E,‰L$,f‰D$2f‰D$·Ãf‰\$4‹‰T$‰\$ƒëL;|$tYt& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ‹D$(‹T$‹t$ ‹L$,‰D$·D$0f‰D$·D$2f‰D$·D$4‰U‹T$‰E0‹$‰U·T$‰u‰U(·T$‰M‰U,‹€ô  ‰E‹D$ld+    u‹D$ƒÄp[^_]ÃÇD$ğÿÿÿëİèüÿÿÿ´&    t& èüÿÿÿUWVS‰ËèüÿÿÿºĞ  ¹   ‹°(  ‰Øèüÿÿÿº    i†¸    ‹¸    ‰øèüÿÿÿƒøÿ„ù  k‰Áƒø tƒÁƒù ‡ï  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hØ  PèüÿÿÿÇCT …‹†¸  ƒÄƒø4tƒè8ƒøwÇCT …1À[^_]Ãt& ƒ¾€  uä[1À^_]ÃèüÿÿÿW‰×V‰ÆS‹‹@P‰V‰ƒôşÿÿèüÿÿÿ‰ƒœıÿÿ‰“ ıÿÿƒ=x  ‡  ³ˆıÿÿ‹‹C‰B‰‰ğº   Ç   ÇC"  [^_éüÿÿÿ´&    fèüÿÿÿWVS‰Ã¡x  …À…+  ‹ƒúwrs{1ÀÇC    ƒçü‰ñÇF,    )ùƒÁ0Áéó«‹<•À   º    ‰øèüÿÿÿ‰Áƒøÿ„Q  ƒø tƒÁƒù ‡G  ‰ú‰ğèüÿÿÿÇC$   1À[^_Ã´&    ¸êÿÿÿëí´&    fèüÿÿÿ‰ÈéTÿÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹(  kŠl  i‚¸    ‹„,   ¹   ƒèƒøv‹Šp  ‰¡x  …À…[  ‰Ø[éûşÿÿ´&    t& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒì¡x  …À…x  èüÿÿÿ‰Â…Àt]‰Ç¹Í   Có¥‰‚¬  ƒ  Ç‚      ‰‚0  iƒ¸    Uÿ°    ‚À  hà  j ‰T$Pèüÿÿÿ‹T$ƒÄ‰š(  ƒÄ‰Ğ[^_]Ã´&    ´&    èüÿÿÿV‹x  S‹X(‹³ø  …Ò…”  ‹ƒÜ   @  ‹ƒâî‰‹“d  ‹Nÿv‰Øèüÿÿÿ‹ƒÜ   º   ‰0  Çƒ       ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ‰X1À[^Ãt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…°  1À[Ãv èüÿÿÿ‰Ğ‰Ê‹9Èt	éìüÿÿt& Ã´&    èüÿÿÿUWV‰ÆS‰Ë¹   ƒìd‰$|$d¡    ‰D$`1ÀÇD$   ó«‹=x  ÇD$   …ÿ…6  ‹†|  ‹$3–x  1Ø	Ğ„ÿ   ‹   …É…  ‹–`  …Ò…ú   ‹†Ä  …À…ì   ‹$~‰|  ‰İÇ†ì  Ğ  ‰†x  % ù  Ç†ô     ƒøÀƒà`à  ‰†ğ  ‹FX´9Ç„‡   ‰t$v ‹Cd‹@…Àt‹p…öt‹$‰é‰Øèüÿÿÿ‹CLX´9ÇuÙ‹t$‹ğ  ‹–ô  ‹F‹¶ì  ‰L$‰t$X´‰T$9Çt-t& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ1À‹T$`d+    u8ƒÄd[^_]Ã¸ğÿÿÿëä‰×‰Ğ‰ÚèüÿÿÿPWh˜  h˜  h0  èüÿÿÿƒÄé¡şÿÿèüÿÿÿt& èüÿÿÿWVS‹t$‹|$èüÿÿÿ‹˜(  ¡x  …À…Í  ‰ò‰ù‰Ø[^_éşÿÿ´&    v èüÿÿÿS
  ƒìd‹    ‰\$1ÛˆT$º   f‰T$T$ˆL$¹   ‰T$‰âÇ$L   èüÿÿÿ‹T$d+    uƒÄ[Ãèüÿÿÿèüÿÿÿƒì 
  d‹    ‰L$1ÉˆT$T$¹   ‰T$º   f‰T$T$f‰L$¹   ‰T$‰âÆD$ Ç$L   ÇD$L  èüÿÿÿƒø…é  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    v èüÿÿÿWVS‰Ãƒìd¡    ‰D$1À¡x  …À…ÿ  ƒú„©   ƒú…ˆ   º   ‰ØèüÿÿÿƒÈƒÈ |$T$¹   ³
  ˆD$f‰L$‰ğ¹   ÆD$ÇD$L   ‰|$èüÿÿÿ¸   ¹   f‰D$¸   T$f‰D$‰ğÇD$L   ‰|$èüÿÿÿ¡x  …À…$  ‹D$d+    uƒÄ[^_Ãt& º   ‰Øèüÿÿÿƒà÷é[ÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‹-x  WV‰ÖS‰Ã…í…o  ‹“¸  ‰³l  Bê‰Ñƒø†›   ƒú!„’   ‹ƒ”  …À„Ä   ‹Hd‹y…ÿ„&  ‹?…ÿ„  kî‹   iÒ  ‹”0   …É„^  ‹)…í„T  j 1Éèüÿÿÿ_‹“¸  ƒú„‹   ƒú „‚   ƒúvlBßƒøw
¹Iè#£Árk[^_]Ã¶    kşiÂ  ƒ¼,   …Wÿÿÿº   ‰Øèüÿÿÿ‹“¸  ‹ƒ”  ‰Ñ…À…Cÿÿÿ´&    ƒú t}ƒútxƒúw”ƒúv¡¸ £Ğs—f‹ƒ”  …ÀtV‹Hd‹I…ÉtL‹y…ÿtEkî‹   iÒ  ‹”4   …Étc‹i…ít\j 1Éèüÿÿÿ‹‹¸  Zëfƒú t¸ƒú… ÿÿÿë­‰Ñköº   iÉ  ‹„4   ƒøtƒø…ÿÿÿº   ‰Ø[^_]é;ıÿÿv j 1Éèüÿÿÿ‹‹¸  Xë¸¶    j 1ÉèüÿÿÿYé§şÿÿèüÿÿÿS‰Ãƒì¡x  …À…¯  kÒiƒ¸    ƒ¼4   tƒÄ‰Øº   [éÑüÿÿƒÄ‰Øº   [éÀüÿÿèüÿÿÿV‰ÎSèüÿÿÿ‹˜(  k“l  iƒ¸    ‹„,   ƒèƒøv,‹ƒúw=¡x  …À…Ô  ‰“p  ‰ØƒÂèhüÿÿ1À[^Ãv ƒ>¸êÿÿÿº    [DÂ^Ãt& ¸êÿÿÿëÛèüÿÿÿUWV‰ÖS‰Ãƒì‹x  …É…ó  ‹>ƒÿ‡ı   kÇi“¸    ‹”,   …Ò„á   ÇF$   i“¸    ‹„,   º    ‹,…@  F‰$‰èèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡  ‹$‰êèüÿÿÿÇF0÷r k×ÇF4    iƒ¸    ‹„,   º   ƒèƒøwÇF$   º   ‰V(9»l  t1ÀƒÄ[^_]Ãt& ‹CƒÃx´9ÃtåƒÆ8‹Gd‹@…Àt‹H$…Ét	‰ò‰øèüÿÿÿ‹GLx´9ÃuÜë¼¸êÿÿÿë·v èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…#  ‰Ú‰ğ[^é¤şÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…?  1À[Ãv èüÿÿÿVS‰Ëèüÿÿÿ‹x  ‹°(  …Ò…\  ƒûw9kÓi†¸    ‹„,   …Àt!‰Ú‰ğèhûÿÿ‰ğ‰ÚèOıÿÿ1À[^Ã´&    v ¸êÿÿÿëì´&    fèüÿÿÿë‰´&    fèüÿÿÿUWVS‰Ëƒì$d‹    ‰T$ 1Òèüÿÿÿ‹°(  ‹†¸  ƒø:wƒø#‡Î   Pìƒâû„â   ƒè8ƒø‡6  ‹…Ò…<  ‹Cº		˜ n‰†Ü  F\èüÿÿÿ‰Ç…À„A  èüÿÿÿ‰D$…À„  ‹Fp´9Å„8  f‹Fd‹@…Àt‹H…Ét	‰Ú‰ğèüÿÿÿ‹FLp´9ÅuÜ¸d   èüÿÿÿ‹D$Ç$    …À…ˆ  ‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿé]  v PÜ¹ q £Ñrƒø4„4ÿÿÿé#ÿÿÿt& ‹K‹†x  ÇD$   ‹–|  ÇD$   ‰Ü  ‰D$F\‰T$º		˜ ‰L$èüÿÿÿ‰D$Ç$   …Àtèüÿÿÿ‰$…À„k  ‹=x  …ÿ…«  †,  º   èüÿÿÿ…À„í   ‹@‰D$‹†¸  Pìƒâû„ä   ƒè$ƒøwº q £Â‚Î   ‹D$…À„Õ  ‹ˆÔ  …É„Õ  ‹FnX´9Åt?´&    ‹Cd‹@…Àt‹x…ÿt‹–x  ‹|  ‰Øèüÿÿÿ‹CLX´9ÅuÒ‹D$‹ˆÔ  ‹D$T$èüÿÿÿ¸d   èüÿÿÿ‹$…É„ş   Ç$    ‹D$ d+    …  ‹$ƒÄ$[^_]Ãv ƒ¾€  …½ıÿÿv Ç$êÿÿÿëÈ´&    †ø  ‰D$é#ÿÿÿ´&    f‹FÇD$   p´9Å…Êıÿÿ¸d   èüÿÿÿÇ$    ë€´&    ‹|$‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿéjşÿÿt& ‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿ‹Fp´9Å…Pıÿÿ¸d   èüÿÿÿé‚ıÿÿ‹|$‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿéáşÿÿèüÿÿÿv èüÿÿÿéfüÿÿ¶    èüÿÿÿWV‰ÆS‹€Ü      ‹ …Â„¡   ‹†Ü   ‰Ó‰$  ÷Â  t+÷Â   …  öÇ…†   öÇtƒ=x  ‡ë  t& 1ÿöÃu‰Ú‰ğèüÿÿÿ[^ø_Ã´&    v ¾¸  ‰øèüÿÿÿ‹†Ü   ‹ˆ   ‹–ø  †ø  9Ğt‰Êèàïÿÿ‰ø¿   èüÿÿÿë©f[1À^_Ã´&    v ƒ=x  ‡L  1ÿöÃtƒëš´&    v èüÿÿÿ‹x  S‰Ã…Ò…h  ‰Øº   èüÿÿÿ‹ƒä  …Àt‹ğ  ƒâuYèüÿÿÿÇƒä      ‹ƒà  …Àt‹ğ  ƒâu%èüÿÿÿÇƒà      ‹ƒ0  …Àt,‰Ø[éüÿÿÿt& èüÿÿÿëÙ´&    fèüÿÿÿë¥´&    f[Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì@‹x  d¡    ‰D$<1À…É…@  ¿@  ¾ F  ¹Í   º  ó¥¸   ‰Í¹   èüÿÿÿ¸ÔI  º   =ÔI  YUYVÇƒx     EÅÇƒ|      ‰ƒè  ƒø  ‰ƒø  ‰ƒü  ƒX  ‰ƒX  ‰ƒ\  ‰ØÇƒô     Çƒì  Ğ  Çƒğ  à  èüÿÿÿƒ»€  t‹ƒˆ  ƒø†ÿ  ³  »  ‰ğèüÿÿÿ‹‹|  ‹“x  ‰Øèüÿÿÿ1Ò‰Øèàôÿÿ1Ò‰ØèÇöÿÿ‰ğèüÿÿÿ‹ƒÌ   Çƒ     Çƒ     ˆ   ‰›,  ‰ƒ  ‰øÇƒ<     Çƒ@     Çƒ4     Çƒ      Çƒ$      Çƒ8      ‰³  èüÿÿÿ‰Â…Àˆ  ‹ƒÌ   ‰›Œ  «d  Çƒd     ˆ   ‰³x  ‰ƒl  ‰èÇƒh     Çƒœ     Çƒ      Çƒ”     Çƒ€      Çƒ„      Çƒ˜      èüÿÿÿ‰Â…Àˆ–   ¹K  º F  ‰Øè=îÿÿ‰ƒà  ‰¸¸  ‹ƒà  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡  ‹ƒà  ˆä      ‹“Œ  ‹ƒà  ³¼  ‹• J  ‹à   ÿ21Òjèüÿÿÿ‰ÂXY…Ò‰„  é\  ´&    ‰Ø‰$èüÿÿÿ‹$‹D$<d+    …†  ƒÄ@‰Ğ[^_]Ã¶“„  s„Ò…^  ¸   èüÿÿÿ¹(  i“ˆ  ˆ  P‰ğj ”ü   èüÿÿÿ_]‰Æ…ö„¹ıÿÿ‹ƒ€  ÇD$    ÇD$    ‰D$‰ğÇD$    ÇD$   èüÿÿÿf‰D$‹FdÇD$    ‹@…Àt*‹H$…Ét#¡   T$…À„   ‹x$…ÿ„•  ‰ğèüÿÿÿ‹ƒ¸  ƒø„ï   ƒø*„æ   ƒø'…&ıÿÿ‹ƒ€  ÇD$(    ÇD$,    ‰D$D$ ÇD$0    ÇD$4    ÇD$8    ÇD$ <  ÇD$$@   ‰D$‹Fd‹@…À„Îüÿÿ‹H(…É„Ãüÿÿ¡   T$…À„!  ‹x(…ÿ„  ‰ğèüÿÿÿé›üÿÿ´&    v iÀˆ  j ¹(  R„ü   ‰Â‰ğèüÿÿÿ‰ÆXZé©şÿÿ´&    fƒ»€  …Ùıÿÿéäıÿÿ¶    ‹“€  ÇD$(    ÇD$,    ‰T$T$ ÇD$0    ÇD$4    ÇD$8    ÇD$ .  ÇD$$@   ‰T$‹Vd‹R…Ò„Âşÿÿ‹J(…É„·şÿÿ¡   T$…Àt0‹x(…ÿt)‰ğèüÿÿÿ‹ƒ¸  é‘şÿÿ´&    ‰ğèüÿÿÿéfşÿÿt& ‰ğèüÿÿÿ‹ƒ¸  éhşÿÿ¶    ‰ğèüÿÿÿé…ûÿÿèüÿÿÿfffffffèüÿÿÿ‹@(‹T$‹€x  Ç   % ù  ƒøÀ%€C   ‡  ‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã«x  ƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀX  ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‹†X  9Átt‹ƒˆ  ‰L$   ‹†\  ‰$‰øèüÿÿÿ‹L$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿ‹$‹“  ƒ=|  ‹@‰P‡ı  ƒÄ[^_]Ã´&    f‰ø‰$èüÿÿÿ‹$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿƒ=|  ‡â  ƒÄ[^_]Ã´&    t& èüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâİ‰½¸  X  ‰øèüÿÿÿ‰Æ‹…X  9Ãt@v ‹…X  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…X  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿW1ÒVS‰Ã‹ ‹p(‰Øèüÿÿÿ‹S‰Á‹†x  % ù  ƒøÀƒàƒÀ…Òu	[¸êÿÿÿ^_Ãiø@  ‹ST9úrêG×‰SP“„  ‹Ì   PiÀ   j h   P‰Øj ‹	èüÿÿÿ1ÀƒÄ[^_Ã´&    ´&    èüÿÿÿV‹|  S‹X(‹³X  …Ò…  ‹ƒd  ÿv¹   P,‰Øèüÿÿÿ‹ƒÜ   º   ‰ˆ  ‹ƒÜ   ‰4  Çƒ`      ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ"‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ"‰X1À[^Ã¶    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCÀü›ÇC   ÇCGREYÇC    ÇC$    ‹€x  öÄùt'ÇC
   ÇC  ÇC   ÇC    1À[Ã´&    ©ÿÿ tîÇC   1ÀÇC?  ÇC   ÇC    [ÃèüÿÿÿƒâtFVS‰Ã¡|  …À…4  ³¸  ‰ğèüÿÿÿ‹ƒÜ   ‹ˆ$  “X  ‰Øèüÿÿÿ‰ğèüÿÿÿ¸   [^Ã1ÀÃffffffèüÿÿÿV°è   S‰Ã‰ğèüÿÿÿ‰Â‹ƒÜ   ‹˜  ‰ğèüÿÿÿ‰Ø[^ÃèüÿÿÿUWV‰ÎS‰Óƒì€|$4 ‹T$ tÇ    pƒÀÇ@ø    Ç@ü    ‰ÇƒúÿtÊ € €ƒÇ‰‹T$,…Ò„­   ‹D$0Ç$    …À•D$¶    ‹K‹l$$…öt-‰Øë´&    f)Îèüÿÿÿ…ö„  ‹H9ñvê‹l$$‰Ã)ñõ‹$…Àti€|$ tb1Ò÷t$0ƒúWÀ%     ;L$$rRD$$   ‰s‰wÇG    ‰×‹t$(ƒ$‹$î9D$,…kÿÿÿƒÄ‰ø[^_]Ã´&    f¸   W;L$$s®	È‰T$   ‰s‰Ø‰wÇG    +kèüÿÿÿ‹T$‰Ã‹@9Åv5‰Á‰Ø‰Óv É   ƒÃ‰Kô‹P‰SøÇCü    +hèüÿÿÿ‹H9érØ‰Ú‰Ã‰èz   ‰‹CÇB    ‰BéDÿÿÿ¶    ‹H‹l$$‰Ãéêşÿÿ´&    v èüÿÿÿWVS‹ˆÌ   ‰Ã‹‘\  ‹X  ‹±P  ‹¹T  ‰Ñ	Át
ƒÀƒÒ )ğúR‰ù‰òP¸    èüÿÿÿğÿ”ÀZY„Àt|‹ƒ¸  iĞ  ƒº   tiĞ  ‹’   ƒú„‚   ƒú„Ÿ   iÀ  ‹€   ƒø„¤   ƒøt7ƒ
  èüÿÿÿƒ|  èüÿÿÿƒô   èüÿÿÿ‹ƒÜ   [^_éüÿÿÿ[^_Ãt& ‰ØèüÿÿÿëÀ´&    ‰Øèüÿÿÿ‹ƒ¸  éoÿÿÿ¶    ƒÜ  èüÿÿÿ‹ƒ¸  iĞ  ‹’   ƒú…aÿÿÿ‰Øèüÿÿÿ‹ƒ¸  éOÿÿÿ´&    ƒ   èüÿÿÿiƒ¸    ‹€   é<ÿÿÿèüÿÿÿ…Àt7S‹X`ú v@túv@t[Ã9ƒ¤  uõ‹[éüÿÿÿ9ƒ¤  uå‹[éüÿÿÿÃ´&    ´&    èüÿÿÿö€4  t*S‹˜Ü   ‹‹  ‹˜Ü   ‹“”  …Ét…Òu
[Ã´&    Ã‹˜Ü   ‰‹  ‹˜Ü   ‰“”  ‹˜Ü   Ã  ‹‹˜Ü   Ã ‹‹˜Ü   Ã  ‹‹˜Ü   Ã ‹RQhè
  ‹€Ì   ˆ   PèüÿÿÿƒÄ[Ã´&    v èüÿÿÿUW¿   V‰ÆSƒì‰$ëM´&    ‹„  …Ò…Ô  ‹‹Cƒï‰B‰º   ‰èÇ   ÇC"  èüÿÿÿ·V‹$)Ğ…À~O…ÿ~K‹9ğtE‹èüÿÿÿ‰“ ıÿÿ«ˆıÿÿ‰ƒœıÿÿ‹FP‰V‰ƒôşÿÿ‹F·Ğ;$u‚ƒ=„  v‡é÷  t& ƒÄ[^_]Ã´&    èüÿÿÿWV1öS‰Ã‹€Ü   ‰°   ‹ƒÜ   ‰°   ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°   »è   ‰øèüÿÿÿ‰Â‹ƒÜ   ˆ  ‹‰1‰øèüÿÿÿ‹ƒÜ   ‰°   ‹ƒÜ   ‰°0  ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°P  ‹ƒÜ   ‰°`  [^_Ã´&    ´&    èüÿÿÿWV‰ÆS‹¸È   _ü‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèøşÿÿ‹†L  ‰Úèüÿÿÿ‰ğèüÿÿÿ‰Øè½ûÿÿGXèüÿÿÿ‰øèüÿÿÿ‰Ø[^_éüÿÿÿt& èüÿÿÿW¸è   V‰ÖS‰Ã‰øèüÿÿÿ	³ä   ‰Â‹ƒÜ   ˆ  ‹€  	ğ‰[‰ø^_éüÿÿÿ´&    ´&    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â#ä   t‹†Ü   ˆ  ‹€  	Ø‰[‰ø^_éüÿÿÿ´&    ¶    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â‹†Ü   ˆ  ‹€  ÷Ó!Ã‰[‰ø^_éüÿÿÿv èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‹Ü   ‰Â‰Ø÷Ğ™  !†ä   ‹‰  !È‰[‰ø^_éüÿÿÿ´&    t& èüÿÿÿUW‰×VS‰Ãƒì‹r¡„  …ö„  …À…8	  ‹GƒÁ1Òƒáø‰D$‹G‰L$‰$÷ñº   9ĞFĞ‰T$ƒø†>  ‹ƒÜ   º  p‰Pº   ‹ƒÜ   ‰P1Ò‹ƒÜ   ‰P1íÇD$    ‹t$‹L$9$ro‰|$1Éfƒ=„  ‡]	  ‰ğƒàüƒÜ   ‰$‹D$‹x‹$ï‰8FƒàüƒÜ   ‰FƒàüƒÜ   ‰FƒàüƒÜ   ‰ƒÁƒÆl$9L$w‹|$‹G‹W(ƒàüƒÜ   …Ò…  ‹T$(‰‹G1ÒƒÀƒàüƒÜ   ‰‹G‹L$ƒÀƒàüƒÜ   ‰‹G‹t$ƒÀÁæƒàüÁîƒÜ   ‰0‹G‹WƒÀƒàüƒÜ   ‰‹G(…À‹G„÷   ƒÀº  €ƒàüƒÜ   ‰º   1Ét& ‹GĞƒàüƒÜ   ‰ƒÂƒúPuè‹G‹WƒàüƒÜ   ‰‹G‹L$ƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰0‹T$‹G ÁêƒàüƒÜ   ƒê‰ƒ=„  ‡	  ƒÄ1À[^_]Ãº   ‰éøşÿÿt& …À…	  ‹G1ÒƒàüƒÜ   ‰‹GƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰‹G ƒàüƒÜ   ‰ƒÄ1À[^_]Ãt& ƒÀº   ƒàüƒÜ   ‰éÿÿÿ¶    èüÿÿÿUWV‰ÆS‰Óâ  ‹8…€  öÃt`ƒ=„  ‡k  ®´   ‰èèüÿÿÿ‹†¸   ƒàü‡Ü   ‹†˜   èÚùÿÿ‰èèüÿÿÿ‹†è   ƒàü‡Ü   ‰¸   [^_]Ã´&    …ÛuÙ[1À^_]Ãt& èüÿÿÿUhW‰×V‰Æ‰èSèüÿÿÿ‹FV9Ât/X´ë´&    v ‹CLX´9Ât9»   uî‰èèüÿÿÿ‰Ø[^_]Ã‰è1Ûèüÿÿÿ‰Ø[^_]ÃèüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j èŸôÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èqôÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿUˆ   WV‰ÎS‰Ó‹|$‹T$‹l$j KhÀ  ¯×ÁêTRÁâ‰èüÿÿÿZY‰C…Àt:1Ò…í”Â1ÉR‰òUWj ÿt$$jÿèİóÿÿ1Ò‰C+CƒÄƒÀ;w[‰Ğ^_]Ãt& ºôÿÿÿëì´&    èüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j è/óÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èóÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿS‹š„  ‹Šˆ  ‹€Ì   j ÿ²”  ÿ²  ˆ   ‰ÚèüÿÿÿƒÄ[Ã´&    fèüÿÿÿU‰ÕW‰ÏV‰ÆSƒì‹„  ‹…É…q  ‰Øè×õÿÿ‹†À   ƒàüƒÜ   ‹‹–ô   ÷Ò!Ê‰kV,‹¤   ‰Ø“d  ÿ·  èüÿÿÿƒ=„  Z‡²  ‹†Ä   ‹–¤   ƒàüƒÜ   ‰iƒ¸        ‹PöÂu‹@¨„Ÿ  ƒú„  ¸¼ èüÿÿÿ‹†ì   …Àtƒàü‹–  ƒÜ   ‰‹†È   ‹–  ƒàüƒÜ   ‰‹†à   ‹–   ƒàüƒÜ   ‰‹†Ü   ‹–  ƒàüƒÜ   ‰‹†Ì   ‹–ü   ƒàüƒÜ   ‰¸¼ èüÿÿÿ‹†¼   º   ƒàüƒÜ   ‰ÇE    ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàü‹»Ü   ‰‡L  ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàû‹»Ü   ‰‡L  ‹ƒ¸  iÀ  ƒ¸   uE‹ƒÜ   L  ‹ ƒàş‹»Ü   ƒÈ
‰‡L  ‹ƒÜ   H  ‹Ê  €‰ºE ‹ƒÜ   ‰,  ‹ƒh  =u  t-w  ƒø‡,  ¡„  …À…–  ‰Øèºóÿÿ‹†ä   ƒàüƒÜ   ‹–ø   ‰‹†À   ƒàüƒÜ   ‹–ô   ‰»è   ‰Øè{óÿÿ‰ø‹®ğ   èüÿÿÿ	«ä   ‰Â‰øèüÿÿÿ‰øèüÿÿÿ‰Â‹ƒä   …Àut‰øèüÿÿÿ‰Øè>óÿÿ‹ƒÜ      ‹ƒÊ ‰‰Øè%óÿÿiƒ¸    ƒ¸   tbƒ=„  wi‰øèüÿÿÿ‰Â‹ƒÜ     ‹ ‰øèüÿÿÿ‰Øèåòÿÿ1ÀƒÄ[^_]Ãv ‹‹Ü   Á  ‹)	è‰éuÿÿÿ1Ò‰Øèüÿÿÿéeıÿÿfº   ‰ØèüÿÿÿëéÊ  ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì‹º¤   ‹*¯º¨   1Òèüÿÿÿ‹„  ‰Â…É…!  ‹C…ÀuC¸êÿÿÿ…ÿu(‹…Ì   j Ã„  ÿ¶¨   ÿ¶¤   ‹
‰Úèüÿÿÿ1ÀƒÄƒÄ[^_]Ã´&    v ;{Tw‰{Pë¼¸êÿÿÿëİ´&    ´&    èüÿÿÿUWV‰ÆS‰Óƒì‹8‹ƒ  ‹’ˆ  ƒÀÇ¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®˜   ˜   ‰Âƒx  9étT‹«ˆ  M    ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹E‹‹  ‰H¡„  …À…l  ƒÄ‰ø[^_]éüÿÿÿt& ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹„  …É…E  ƒÄ‰ø[^_]éüÿÿÿèüÿÿÿUW‰ÇVSƒì‹„  …Ò…“  ‹0‹‡ä   ƒàü†Ü   ‹‹—ø   ÷Ò!Ê‰‹‡À   ƒàü†Ü   ‹‹—ô   ÷Ò!Ê‰»   v ¸X‰A èüÿÿÿƒëuñ»d   ë´&    v …Àt,¸X‰A èüÿÿÿƒët‹†Ü   ‹  ‹†Ü   ”  ‹ …ÒuĞi†¸    ƒ¸   t}‡´   Ÿ˜   ‰$èüÿÿÿ‰D$ë?v ‹·˜   ‹‹F®ˆıÿÿ‰B‰‰èº   Ç   ÇF"  èüÿÿÿ¡„  …À…Õ  ‹‡˜   9Øuº‹T$‹$ƒÄ[^_]éüÿÿÿ´&    ‹†Ü   L  ‹ ƒàõ‹Ü   ƒÈ‰L  ‹‡ì   1Òƒàü†Ü   ‰‹‡Ì   º   ƒàü†Ü   ‰i†¸    ƒ¸   …$ÿÿÿ1Ò‰ğèüÿÿÿéÿÿÿv èüÿÿÿ…Ò„ƒ   UWV‰ÖS‰Ã‹€”  ƒàüƒÜ   ‹8ƒ=„  wd÷Æ 3 …û  ÷Æ   t.ƒ=„  ‡G  «  ‰èèüÿÿÿƒt  ‰úè†ïÿÿ‰èèüÿÿÿ‹ƒÄ  ƒàüƒÜ   ‰0¸   [^_]Ãt& 1ÀÃ‹ƒÀ  ƒàüƒÜ   ‹ WPRhÔ  hD  èüÿÿÿƒÄéqÿÿÿfèüÿÿÿUWVS‰Óƒì<d¡    ‰D$8‹‚Ü   ‹¨  ºè   ‰øèüÿÿÿ‰Â‹ƒÜ   ‹°  ‰øèüÿÿÿ‰è!ğ‰$„%  ‹ƒÜ   $  ‹ ‰D$‹ƒÜ      ‹ ‰D$‹ƒÜ   T  ‹ ‰D$‹ƒÜ   P  ‹ ‰D$‹ƒÜ   ‹ˆ4  ‹ƒÜ   0  ‹ ‰D$‹ƒÜ   D  ‹ ‰D$‹ƒÜ   @  ‹ ‰D$‹ƒÜ   ‹   ‹ƒÜ      ‹ ‰D$ ‹ƒ”  ƒàüƒÜ   ‹ ‰D$$‹ƒØ  ƒàüƒÜ   ‹ ƒ=„  ‰D$(‡x  ‰è%   ‰D$÷Å€„ò   ÷Å   tƒ=„  ‡K  ÷Å   tƒ=„  ‡o  ÷Å   tƒ=„  ‡  ÷Å   tƒ=„  ‡'  ÷Å   tƒ=„  ‡ß  ÷Å   …Ú  ÷Å   tƒ=„  ‡>  ÷Å   „Ò  ƒ=„  ‡  ÷Å   „  ƒ=„  ‡ü  ÷Å  € u÷Å   tƒ=„  ‡Ø  ÷Å   „ú  ƒ=„  ‡´  iƒ¸    1ö‹€  ƒø„2  ƒø„a  …Ét"iƒ¸    ‹€   ƒø„Ì  ƒø„S  ‹D$…Àt"iƒ¸    ‹€   ƒø„º  ƒø„9  ‹L$…É…-  ‹T$…Ò…‘  ‹D$…Àt[‹ƒ¨  ÆD$7 …ÀtL‹Pd‹…ÒtC‹J$‰L$…Ét8‹    …Ò„x  ‹J$‰L$…É„i  T$7‰Ñ‰ê‹l$èüÿÿÿ€|$7 tƒÆ÷$   …Ñ   …öu-Ç$    ‹D$8d+    …9  ‹$ƒÄ<[^_]Ã´&    v ‹ƒÜ   ‹<$‰¸  Ç$   ëÂ´&    ÷Å  €„Ëşÿÿ‰ê‰Ø‰L$èüÿÿÿ‹L$‰Æiƒ¸    ‹€  éšşÿÿt& ÷Å  € „“şÿÿ‰Ø‰L$èüÿÿÿ‹L$Æé}şÿÿ¶    ‹T$‰Øèüÿÿÿ‹T$Æ…Ò„ÆşÿÿëUt& ‰øèüÿÿÿ‰Â‹ƒÜ     ‹áÿÿÿ÷‰‰øƒÆèüÿÿÿ‹˜  ¸    ‹    èüÿÿÿ…ö„íşÿÿéÿÿÿt& ‹L$‹T$‰ØèüÿÿÿÆ‹D$…À„µşÿÿéUşÿÿ¶    ƒ=„  †µşÿÿé_  ¶    ƒ=„  ‡“  ÷Å   ….ıÿÿ÷Å   „/ıÿÿƒ=„  †"ıÿÿéš  ´&    f‰Ê‰ØèüÿÿÿÆéıÿÿ‹T$‰ØèüÿÿÿÆéµıÿÿ¶    ÷Å  € „ıÿÿƒ=„  †	ıÿÿé»  ´&    v ‹D$…À„ıÿÿƒ=„  †úüÿÿév  fƒÜ  ‰Êè³ïÿÿÆé)ıÿÿt& ‹T$ƒ   è™ïÿÿÆé9ıÿÿf‰êL$7‹l$èüÿÿÿé”ıÿÿèüÿÿÿ´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
	È‰÷Ãøÿ t>i†¸    ƒ¸   „_  ƒ¸   „_  ‹†Ü      ‹
‰ØÁè·À	È‰ã  ø …w  [^Ã´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
÷Ğ!È‰÷Ãøÿ t@i†¸    ƒ¸   „  ƒ¸   „  ‹†Ü      ‹
‰ØÁè·À÷Ğ!È‰ã  ø …§  [^Ãv èüÿÿÿV‰ÆS‰Óƒâuh÷Ãøÿ u‰Ø%  ø …¿  [^Ã´&    i€¸    ƒ¸   „Ù  ƒ¸   „Ù  ‹†Ü   ‹   ‰Ø[^Áè!ĞÁà%øÿ Ã¶    ‹€Ü   ‹€  Áè!Ø[^ƒàÃèüÿÿÿW‰ÏV‰ÆS‰Óƒât ‹€Ü     ‰ØÁà%   …Éts‹
	È‰÷Ãøÿ tFi†¸    ƒ¸   „ñ  ƒ¸   „ñ  ‹–Ü   Áë·Û‚$  …ÿt‹’$  ÷Ó!Ó‰[^_Ãv ‹’$  	Ó‰[^_Ã´&    v ‹
÷Ğ!È‰ë‰¶    èüÿÿÿU¹8  ºÀ  W‰ÇVSƒì¡4   èüÿÿÿ…À„š  ‰Ã¡`N  …À„Ç   º   ƒøtM1É¸"  ºQ  1öèüÿÿÿ…Àu%ƒÆƒş„š   ‹õ	  ‹õ 	  1Éèüÿÿÿ…ÀtÛèüÿÿÿº   ´&    ¶ƒ4  kƒàş	Ğ‰êˆƒ4  ‡ˆ   ‰D$èüÿÿÿ‰Æ…Àx/j C\1Éº   ‰D$èüÿÿÿ‹³˜   X…öt,‹$èüÿÿÿ‰èèüÿÿÿ‰ØèüÿÿÿƒÄ‰ğ[^_]Ãt& 1ÒëŒt& ‹$‰»Ì   ¾ûÿÿÿÇƒ˜  àÿÿÿ‰C@ƒœ  ‰ƒœ  ‰ƒ   ƒ°  ‰ƒ°  ‰ƒ´  ƒÄ  ‰ƒÄ  ‰ƒÈ  ‰øÇƒ¤      Çƒ¬  àÿÿÿÇƒ¸      ÇƒÀ  àÿÿÿÇƒÌ      ÇC< E  èüÿÿÿ‰D$…À…7ÿÿÿƒ  ¹€  º°  Çƒè       Çƒ¸      èüÿÿÿƒ¤  ¹€  º»  èüÿÿÿğÿ¡€  ‹¼  ‰L$‰ƒŒ  PPhË  Q‰€  èüÿÿÿ‹ƒÌ   ƒÄ·@"f=€ˆ„§  f=Rˆt¸u  ¹ ?«º L  ‰‹ğ   ‹„  ‰ƒh  ‰“d  …É…'  ‹ƒŒ  ‹…@N  ;    ‚(  Çƒ¸  ÿÿÿÿ‹    ‹t$ëf‹“¸  ƒÆƒúÿuR9Ît1‹ƒÌ   ·õ    f9P$uÚ·õ   f9P&uÌ‹õ   ‰“¸  ëÃÇƒ¸      ‰Øèüÿÿÿ‹ƒÌ   ‹“¸  fx"Rˆ„Ù  iÊ  ‹‰(   …Ét‰‹ğ   ƒú+„Ÿ  ‹P³è   ¶Rt‰“Ô   ‹@Áèƒà‰ƒØ   ‰ğèüÿÿÿ‹ä      ‰Â‰ğèüÿÿÿ‹ƒ¸  Çƒø       ‰›ô   iĞ  ‰›|  Çƒl    Çƒh    Çƒp     Çƒt    Çƒx    Çƒd     Çƒ€     Çƒô	    Çƒğ	    Çƒø	     Çƒü	    Çƒ 
    Çƒì	     Çƒ
     ‰›
  Çƒ|    Çƒx    Çƒ€     Çƒ„    Çƒˆ    Çƒt     ‹’   ƒâıƒú„ü  iÀ  ‹€   ƒàıƒø„™  ‹‹Ì   ‹‘\  ‹X  ‰T$‹‘T  ‰D$‹P  ‰T$‹T$‰D$‹D$‰Ñ	ÁtƒÀƒÒ +D$T$‰D$‰T$j ¸    ÿt$ÿt$ÿt$‹T$$‹L$(èüÿÿÿƒÄ…À„  ‹³Ì   ‹T  ‹†P  ‹–\  ‰L$‹X  ƒÁ)Á–X  ¾    ‹T$DÎèüÿÿÿ‹“Œ  ¹•  ‰ƒÜ   ‰ƒà   ‹ƒ¸  9•@N  º£  EÊ‹“Ì   QPiÀ  ÿ°    ·B&P·B$Pÿt$$hP  èüÿÿÿ¡„  ƒÄ…À„[  éD  ´&    ‰“¸  ƒúÿ„Óüÿÿ‹ƒÌ   é6ıÿÿfx&7q…UıÿÿÇƒğ   @x}éFıÿÿƒú9„É  ƒú8…ıÿÿÇƒ¸  <   ‹Ø@  …Ò„ıÿÿ‰“ğ   éıÿÿ¡„  …À…	  ƒ¸  ‰›   ¹€  ºõ  ‰ƒ¸  ‰ƒ¼  ƒ4  Çƒ     Çƒ    Çƒ(      Çƒ,  ÀG ÇƒÔ      Çƒ$     èüÿÿÿƒ,  Çƒ°      ‰ƒ,  ‰ƒ0  ‹ƒ0  …Àu
Çƒ0     ÇƒØ    ÇƒÜ  0 Çƒà  @ Çƒä  P Çƒè  T Çƒì  X Çƒğ  \ Çƒô  ` Çƒø  d Çƒü  h Çƒ   l Çƒ      Çƒ  @  Çƒ  D  Çƒ(     Çƒ     éıÿÿ‹„  …Ò…Q  ƒt  ‰›Ü  ¹€  ºõ  ‰ƒt  ‰ƒx  ƒğ  ÇƒĞ     ÇƒÔ    Çƒä      Çƒè  ÀG Çƒ      Çƒà     èüÿÿÿƒè  Çƒl      ‰ƒè  ‰ƒì  ‹ƒì  …Àu
Çƒì     Çƒ”    ‹ƒ¸  Çƒ˜  4 Çƒœ  @ Çƒ   P Çƒ¤  T Çƒ¨  X Çƒ¬  \ Çƒ°  ` Çƒ´  d Çƒ¸  h Çƒ¼  l ÇƒÈ  D ÇƒÀ  0  ÇƒÄ  4  Çƒä     ÇƒÌ     é²ûÿÿÇƒ¸  =   ‹ìA  …Ò„\úÿÿé;ıÿÿ¾ôÿÿÿé$øÿÿ¸x  ¹€ğúº K  é]ùÿÿfffffffèüÿÿÿ¸	 ÿÃt& èüÿÿÿV‰ÆS»@   ë¸à èüÿÿÿƒët)‹F‹‹€x  ƒàü‚Ü   ‹ ¨u×¸   [^Ã´&    1À[^Ãv èüÿÿÿU‰ÕWVSƒì‹x‰D$¡Œ  ‹…Àt	öÁ„"  ·Ef…À„Â   ‹Œ  …Û…ê!  ‰ë1ö‰Õ‰Úë1f‹‡€  ƒàü…Ü   ‹‹Bˆ0¡Œ  …À…o"  ·BƒÆ9ğvm‹Ÿp  ƒè‰$‰ÙË  É  9ğ‹‡|  FÙ·
ƒàü…Ü   Áá‰‹‡t  ƒàü…Ü   ‰‹D$èáşÿÿ‹$…À…vÿÿÿ¡Œ  …À…   ¸ûÿÿÿƒÄ[^_]Ãt& ‹‡|  ·M ƒàüÁá‚Ü   ‰‹‡t  ƒàü‚Ü   ‹—p  ƒÊ‰‹D$è}şÿÿ…Àt°‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t‹5Œ  …ö…Ì!  ƒÄ1À[^_]Ã¸úÿÿÿéuÿÿÿé¿"  t& èüÿÿÿU‰ÕW‰ÏVSƒì‹p‰D$¡Œ  ‰L$‹·M…ÿ„b  …À…#  ‹|  ·E ‹ºÜ   ƒãüÁàßf…É„‰  ‹]¶	Ø‰D$‹†p  fƒù„-    ‰$‹D$‰‹†„  ƒàü‚Ü   ‰‹†t  ‹$‰T$ƒàü‚Ü   ‰‹D$è}ıÿÿ…À„  ‹Œ  …Ò‹T$…a#  ·Eƒøù   ‰é¿   ‰Õ‰Êëv´&    fË  ‹†|  ‰$‹T$ƒàü…Ü   ‰‹†„  ƒàü…Ü   ‰‹†t  ƒàü…Ü   ‰‹D$è÷üÿÿ…À„   ‹Œ  ‹$…É…¤#  ·BƒÇ9ø~v‹Jƒè‹p  ¶99ø{ÿÿÿ‰ØË    ƒ|$ DØégÿÿÿ…À„şÿÿé9#  v ‰Á  É  ƒ|$ EÈ‰$é¾şÿÿv ‹Œ  …É…õ"  ¸ûÿÿÿƒÄ[^_]Ãt& ‰‹†t  ƒàü‚Ü   ‹–p  ƒÊ‰‹D$è+üÿÿ…ÀtÅ‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t¡Œ  …À…×"  ƒÄ1À[^_]Ã¸úÿÿÿë´&    t& èüÿÿÿU‰ÕW‰ÇV‰ÎSƒì‹Œ  …Ò…Ú#  1Û…ö<é•   t& C‰$9ğ}D‰Áö@t· f9tJ1É‰øèYıÿÿ…Àx1ƒÃ9Ş~^[¡Œ  ÁáT …À…÷#  öBt±1É‰øè¨ûÿÿ…ÀyÏƒÄ[^_]Ãt& ‰Ë·I‰øèıÿÿ…Àxã‰Ú¹   ‰øèyûÿÿ‹$ë›t& ƒÄ‰ğ[^_]Ã¶    èüÿÿÿUWVS‰ÃƒìH‹3d¡    ‰D$D1À¡Œ  …À…ï$  ‰òk1À¹œ   ‰ïó«»x  ¹}   ÇC    ÇC   Çƒ  cx23Çƒ  885 ‰|$ó«º¼  º    Çƒ|  cx23Çƒ€  885 Çƒ„  inteÇƒˆ  rnal‹†Ì   ˆ   ‰Chƒ  ‰$‰øèüÿÿÿ‰Áƒø†¨   ƒø0…å$  ‹$‰úèüÿÿÿF‰[‰ƒ„   ‰èèüÿÿÿ‹ƒl  ‰«  …À…Ì$  ‹=Œ  …ÿ…$  ¡ˆ  …À…-$  t$1À¹   º!	  ‰÷ó«¹	   ‰ğèüÿÿÿ‰ò¹Ì	  ‰èh    èüÿÿÿ‹ƒl  Z‹T$Dd+    u&ƒÄH[^_]Ãt& ƒø0„XÿÿÿƒÁƒù0†Lÿÿÿé%  èüÿÿÿèüÿÿÿƒÀèüÿÿÿ1ÀÃèüÿÿÿS‰Ãƒì‹Œ  d¡    ‰D$1À…É…%  ƒú¸D  º   ¹   f‰D$”À€f‰T$T$ˆD$D$‰D$ƒ
  ÇD$D  è&ıÿÿ‹D$d+    uƒÄ[Ãèüÿÿÿfffffffèüÿÿÿ‹@(Ç€¤   ğ  Ç€¨       Ç   ‹ˆ¤   ¯ˆ¨   ‰È‹L$‰1ÀÇ    Ãèüÿÿÿ‹€$  ‹@ ‹ ‹€Ü      ƒút …Òtº   ‰1ÀÃfº   ‰1ÀÃ¶    º   ‰1ÀÃ¶    èüÿÿÿS1É‹˜4  …Ò”Áº   Q1É‹[èüÿÿÿZ[Ã´&    v èüÿÿÿ1ÀÃ´&    èüÿÿÿS‹ˆà  …Étèüÿÿÿ‰Ã…Àˆ?%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸<  ‰X  ‹Ğ  Ç€<      …Òt‰Ãèüÿÿÿ‰»<  ‰Æ…ÀˆU%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‹ˆÜ  …Étèüÿÿÿ‰Ã…Àˆk%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸X  ‰<  ‹Ğ  Ç€X      …Òt‰Ãèüÿÿÿ‰»X  ‰Æ…Àˆ%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‰Ó‹”   Hƒú~2‰Èèüÿÿÿ…Àt‹@…Àt‹ˆ  …Ét‰Ú[éüÿÿÿ¶    [Ã¶    º   ‰ÈèüÿÿÿëÇ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹@(éüÿÿÿv èüÿÿÿ‹@(‹˜   Šˆıÿÿ˜   èüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿ‰Â‹ ‹@(‹ éüÿÿÿ´&    ¶    èüÿÿÿ‹‹R(éüÿÿÿèüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº 0  ‹h ‹} ‰øèüÿÿÿƒût!ƒût|…ÛtH‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& èüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº   ‹h ‹} ‰øèüÿÿÿƒût!ƒût\…Ût8‹@  ‰Ú‰ğèüÿÿÿ1À[^_]Ãf‰øº   èüÿÿÿº   ‰øèüÿÿÿëÎ¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë®¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë¶    èüÿÿÿVS‰Ã‹€$  ‹p ‹‹¸  ƒøw9ƒøw9ƒøuƒ»@  º    ‰Èt?èüÿÿÿ‹–<  …Òt(‰Ø[^éüÿÿÿ´&    ƒø!uáº   ‰ÈëÓ´&    v 1À[^Ãv èüÿÿÿë¿èüÿÿÿUW‰ÏVSƒì‹0‰$‹\$®¤  ‰èèüÿÿÿ‹†Ü   º   ‰$  ‹†Ü      ‰ø¶À€ÌÇ‰‹†Ü      ‹€æû‰‰ù‹†Ü   ¶ıÏ Ë  ‰¸   ‹†Ü      ‹€æ÷‰€<$ ‹†Ü   „Ò   ºÿ  ‰$  ‹†Ü      ‹€æş‰‹†Ü      ‹
€<$À% Àÿÿ-@  !È‰‹=    ¡    ƒÇ9Çy&é   t& ¸2   ¹   ºd   èüÿÿÿ¡    9Çx‹†Ü   ‹˜   öÇuÒ¶Û‹†Ü      ‹€   €ÌÃ‰‹D$‰‰èèüÿÿÿ1À€<$ u‹D$‹ Áøƒà‡ƒÄ[^_]Ã´&    ¶Û€ÏÃ‰˜   é(ÿÿÿ1Ûë¤v èüÿÿÿUWVS‰Ãƒì‹5    ‹€Ü   ‰$‹|$ƒÆ   ‹ ‹‹Ü   …ÿ„—   Á$  ‹)Íÿ   ‰)‰Á€äı€Í…ÒEÁ‹‹Ü   €äò‰Â€Ì€Î…ÿEÂ‰   ët& ‹    9Öx¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €Î‰¶À…ÿEøƒÄ[^‰ø_]Ã´&    f©$  ‰é‹m å ÿÿÿ‰)Š$é^ÿÿÿt& èüÿÿÿVpº   S‰Ã‰ğèüÿÿÿ…Àt‹@…Àt‰ğèüÿÿÿ‹³8  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³4  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³0  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³,  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹‹¸  ƒøt!ƒøu
‹S‰ÈèüÿÿÿÇƒ      1À[^Ãv ‰Øèüÿÿÿ1ÀÇƒ      [^ÃfèüÿÿÿS‹tY  ‹@(‰˜€  ‹xY  ‰˜„  Ç   ¡tY  ¯xY  ‹L$‰¡|Y  [‰1ÀÃ´&    t& èüÿÿÿ‹…ÀuÇA,MPEG1ÀÃv ¸êÿÿÿÃfèüÿÿÿ‰Â‹ ‹@(Ü  éüÿÿÿ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹‹R(ÂÜ  éüÿÿÿ´&    t& èüÿÿÿUWVSƒì(d‹    ‰T$$1Òèüÿÿÿ‹°(  †¼  nPhI  j"|$Wèüÿÿÿ‹FƒÄ9Åt'X´t& ‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Åuà†Ô  ‰úèüÿÿÿ‹D$$d+    u
ƒÄ(1À[^_]Ãèüÿÿÿ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  tLÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  t<‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹|  ‹€x  ‰S‰1À[Ã´&    èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…©k  1À[ÃèüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCMPEGÇC    ‹€  ¯„  ÇC    ‰S‹ˆ  ‰S‹Œ  ÇC   ‰S‹¤  …Ò…Ìk  1À[Ã´&    ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  th‹…Òu[F¹   ºN  ƒÇèüÿÿÿ‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ¡¤  …À…ïk  1À[^_Ã¸êÿÿÿëõ¸êÿÿÿëîv èüÿÿÿéüÿÿÿ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡¤  …À…l  ‰Ú‰ğ[^éüÿÿÿt& èüÿÿÿUWVS‹l$1Û‹|$èüÿÿÿº   ‹°(  1Àë´&    f›‹…ä  ‹…è  !ê!ø	ĞuƒÃƒûuß¸êÿÿÿ[^_]Ãt& ‰ê‰ù‰ğèüÿÿÿ…Àuç›Áá‘à  ‹‰à  ‰  ‹J‰   ‹J‰$  ‹J‹R‰(  ‰–,  [^_]ÃèüÿÿÿUWVS‰Ëèüÿÿÿº   ‹°(  ¾¼  ‰øèüÿÿÿƒø‡1l  ‰ÁƒøtƒÁƒù‡'l  ‰ú‰Økèüÿÿÿ‹†Ü  º    i€¸    ‹¸    ‰øèüÿÿÿ‰Áƒøÿ„1l  ƒø tƒÁƒù ‡'l  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hr  Pèüÿÿÿ¸ …º …ÇCT …ƒÄƒ¾€  DÂ‰CT[1À^_]Ã´&    t& èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…;l  1À[Ãèüÿÿÿƒ=¤  S‰Ã‡Zl  ‹ƒÜ   º7   ‰(  º   ‹ƒÜ   ‰$  º à  ‹ƒÜ   ‰   [Ã¶    èüÿÿÿWV¾   S‰Ã‰Ğ‰Ê‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ X  ‰¾   ‹»Ü   €Íø‰   ¶Î‹³Ü   ‰ÏÏ Y  ‰¾   ‹»Ü   €Íù‰   ‰Ñ‹³Ü   Áé¶É‰ÏÏ Z  ‰¾   ‹»Ü   €Íú‰   Áê‹»Ü   ‰ÖÎ [  ‰·   ‹»Ü   €Îû‰—   ¶Ğ‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹“Ü   ·À‰‚   º^  ‹ƒÜ   ‰   ºş  ‹ƒÜ   ‰   ‹5    ƒÆë¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  ¶Ò‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹»Ü   ·À‰‡   º ^  ‹ƒÜ   ‰   º ş  ‹ƒÜ   ‰   ‹5    ƒÆë ´&    f¡    9Æˆ[  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º X  ‹ƒÜ   ‰   º ˜  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ Y  ‹ƒÜ   ‰°   ¾ ™  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Z  ‹«Ü   ‰µ   ¾ š  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‹«Ü   ‰   ‹«Ü   µ   ½ [  ‰.‹«Ü   µ   ½ ›  ‰.‹«Ü   ‹µ   Áæ¶Ò	ò‹t$Áæ·ö	Ö‹“Ü   ‰Š   Áà%  ÿ 	ğ‰Â‹$‰ƒÄ‰ø[^_]Ã´&    ¿ÿÿÿÿé¹şÿÿ´&    ´&    èüÿÿÿWV¾   S‰Ã‰È‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ P  ‰¾   ‹»Ü   €Íğ‰   ¶Ì‹³Ü   ‰ÏÏ Q  ‰¾   ‹»Ü   €Íñ‰   ‰Á‹³Ü   Áé¶É‰ÏÏ R  ‰¾   ‹»Ü   €Íò‰   Áè‹»Ü   ‰ÆÎ S  ‰·   ‹»Ü   €Ìó‰‡   ‰Ğ‹»Ü   Áèƒà?‰ÆÎ@T  ‰·   ‹»Ü   @ô  ‰‡   ¶Æ‹»Ü   ‰ÆÎ U  ‰·   ‹»Ü   €Ìõ‰‡   ¶Ò‹ƒÜ   ‰Ñ€ÍV‰ˆ   ‹ƒÜ   €Îö‰   ‹5    ƒÆët& ¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  Áê‹»Ü   ƒâ?‰ÖÎ T  ‰·   ‹»Ü   €Îô‰—   ¶Ô‹»Ü   ‰ÖÎ U  ‰·   ‹»Ü   €Îõ‰—   ¶À‹»Ü   ‰Á€ÍV‰   ‹»Ü   €Ìö‰‡   ‹5    ƒÆë´&    ¡    9Æˆc  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º S  ‹ƒÜ   ‰   º “  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ R  ‹ƒÜ   ‰°   ¾ ’  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Q  ‹«Ü   ‰µ   ¾ ‘  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‰D$‹«Ü   ‰   ‹«Ü   µ   ½ P  ‰.‹«Ü   µ   ½   ‰.‹«Ü   ‹µ   ‰ğÁâ¶ğ	ò‹t$Áææ  ÿ 	Ö‹“Ü   ‰Š   ‹D$Áà·Ğ‹$	ò‰ƒÄ‰ø[^_]Ã¶    ¿ÿÿÿÿé±şÿÿ´&    ´&    èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìd¡    ‰D$1Àƒ=¤  ÇD$    ‹|$(‡!m  ‹ƒ¼  ‰áPü‰Øèüÿÿÿ‹$=xV4…ıl  ‹“¼  L$‰Øèüÿÿÿ‹D$…À…Ùl  ‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹ƒ¼  ‰ñ1öP‰Øèüÿÿÿ‹ƒ¼  ¹ô  P‰Øèüÿÿÿ…í~5t& ‹ƒ¼  ‹·T‰Øèüÿÿÿƒ=¤  ‡¾l  ƒÆ9õuØƒşv ‹ƒ¼  1ÉT‰ØƒÆèüÿÿÿƒşuå‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹5    ƒÆ
ë¡    9Æˆ«l  ¸Æ§  èüÿÿÿ‹“¼  L$‰ØèüÿÿÿöD$tÑ‹T$$‹l$$1ö…Ò~-v ‹ƒ¼  ‰ùT‰Øèüÿÿÿƒ=¤  ‡‘l  ƒÆƒÇ9õuÖ‹ƒ¼  L$P‰Øèüÿÿÿƒ=¤  ‡vl  ‹“¼  1É‰Øèüÿÿÿ‹D$‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    èüÿÿÿUWVSƒìHd¡    ‰D$D1À‹t$\‹|$dƒ=¤  ‡gm  \$l…ÿt{‰4$D$pl$‰ê¸v ‹pü‰ÃƒÀƒÂ‰rü9Èuî‹4$U‰ù‰ğÿt$l‹T$hè¡ıÿÿZY‹t$h…öt‹‹L$ƒ|$h‰
u	‹S‹L$‰
‹T$Dd+    uƒÄH[^_]Ã´&    v l$ë§èüÿÿÿt& èüÿÿÿéFıÿÿ¶    èüÿÿÿUWVS‰Ãƒì,‹¤  ‰T$d¡    ‰D$(1À…É…ëm  j j h€   SèõşÿÿƒÄ…À‰;  ƒ=¤  ‡Ån  ÇD$    ÇD$    L$º   ‰Øèüÿÿÿº  ‰Ø|$‰ùèüÿÿÿ¹íÿÿÿºX  ‰Øèüÿÿÿ¹ÿÿÿÿºT  ‰Æ‰Øèüÿÿÿ¹  €ºø  	Æ‰Øèüÿÿÿ¹   ºü  	Æ‰Øèüÿÿÿ1Éºd   	Æ‰Øèüÿÿÿ	ğ…¯n  ‹ƒÌ   º‚  ˆˆ   D$èüÿÿÿ…À…n  ‹D$‹úÀ …n  ‹@8§  ‰Át8h/  èüÿÿÿ‹D$èüÿÿÿXhD  hh0  èüÿÿÿXZÇD$ÿÿÿÿéô  ¶    xf»Uªu¿ƒ=¤  ‡.  ‰$1í1ö‰Ëë	´&    ‰Í‹«‹$‰ê)Î‰L$ƒîèüÿÿÿ…À…Òm  ‹D$M‹ Áè9ÈwĞ‹¤  ‹$…É…]o  …íx%v L$‰ê‰Øèüÿÿÿ…À…Do  ‹D$tƒísŞ…ö…+o  ‹D$èüÿÿÿ¡¤  …À…o  ¹ÿÿÿÿºT  ‰Øèüÿÿÿ‹L$º   ‰Æ‰Øèüÿÿÿ‹L$º  	Æ‰ØèüÿÿÿºX  	ÆL$‰Øèüÿÿÿ‹L$ºX  	Æ‰Øƒáèèüÿÿÿ¹ @  º   	Æ‰Øèüÿÿÿ¹ @  º  	Æ‰Øèüÿÿÿº   	ÆL$‰Øèüÿÿÿ‰ùº  ‰Øèüÿÿÿ…öˆo  ƒ=¤  ÇD$xV4ÇD$xV4ÇD$ 4xVÇD$$V4x‡so  1ö1íëv 1öƒÅıÀ „¥o  ‰ù‰ê‰Øèüÿÿÿ‹D$9D´uÚƒÆƒşuÔ¡¤  ƒÅ…À…ºm  ‰«¼  j j h€   SèíûÿÿƒÄ…ÀˆQ  Wjj hÄ   SèÒûÿÿƒÄ…Àˆ¢m  ‹=¤  …ÿ…‡m  ¸È   èüÿÿÿ‹5¤  …ö…rn  ‹ƒ   ¾   ‰ß% ù  ƒøÀƒà`à  ‰ƒŒ  ÿ³ˆ  Pj jh‘   Sèfûÿÿ‹ƒˆ  ƒÄf‰ƒÈ  ‹ƒŒ  f‰ƒÊ  1À÷ƒ   ÿÿ •Àf‰ƒÌ  ƒÀ  èüÿÿÿjjj jhÜ   Sèûÿÿjjj jhÜ   Sèûÿÿ¸<   ƒÄ0èüÿÿÿhğ   hğ   j jhÖ   SèÜúÿÿj j j j j j j j j j j j j jh×   SèµúÿÿƒÄXj@jhÀ€` hU‘U‘jjh ½ j jhÈ   SèŒúÿÿƒÄ,´&    fFí1Ûj ƒøj –Ãj SVj jh·   Wè^úÿÿ‰ğƒÆƒÄ$   €j j j SPj jh·   Wè:úÿÿƒÄ$ƒşu²j j jhÚ   Wè!úÿÿ¸<   èüÿÿÿj j hÍ   Wèúÿÿ¸<   ƒÄ$èüÿÿÿ¹€   ºH  ‰øèüÿÿÿ‹L$…Éu"‹D$(d+    u?‹D$ƒÄ,[^_]Ã´&    fj j j jh   Wè­ùÿÿ¸
   èüÿÿÿƒÄÇD$    ë´èüÿÿÿéÎo  é(n  ´&    èüÿÿÿUº   WVSƒì‹h(‰èt  è/úÿÿ‰Æ…Àty½¸  ‰øèüÿÿÿ‰$‹…t  9ØtD´&    ‹…t  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…t  9ÃuÃ‹$‰øèüÿÿÿ‰ğƒÄ[^_]Ã´&    ‹…t  ‰Úˆˆıÿÿ…Ü  èüÿÿÿ‰ğƒÄ[^_]Ã´&    ¶    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë$ƒÄ‰ğ[º  ^éïÿÿèüÿÿÿ´&    ´&    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë‰ğº  ÷Ñ#$ƒÄ[^é²îÿÿèüÿÿÿ´&    ¶    èüÿÿÿW‰ÏV‰ÆS‰Óº   ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ·Ë…ÿt&$‹D$d+    uƒÄ‰ğº   [^_éLîÿÿt& ÷Ñ#$ëÖèüÿÿÿ´&    t& èüÿÿÿƒìd‹    ‰T$1ÒT$ÇD$    Ç$    RT$Rjj hÆ   PèC÷ÿÿ¡¤  ƒÄ…À…æo  ‹D$d+    uƒÄÃèüÿÿÿ´&    ¶    èüÿÿÿS‹X(j j jj jh‚   Sèòöÿÿ¸ô  èüÿÿÿ‰ØèüÿÿÿƒÜ  ƒÄ[éüÿÿÿfèüÿÿÿS‰Ã¡¤  …À…
p  ‹ƒÄ  …Àt%‹ğ  ƒâu&èüÿÿÿƒÔ  èüÿÿÿÇƒÄ      [Ã´&    v èüÿÿÿëØ´&    fèüÿÿÿUWVS‰Ãƒì¡¤  …À…op  iƒ¸    ƒ¸   …N  Çƒ(      ³À  º2   Çƒ,      ‰ğÇƒ  •  Çƒ      Çƒ$      ÇƒŒ  à  Çƒˆ  Ğ  ÇƒÄ     èüÿÿÿ‰Å…ÀtƒÄ‰è[^_]Ãt& ‰›D  1Ò‰ğ»Œ  @  ÇƒH  p‡  ”Âèüÿÿÿ“Ô  C\1Éj èüÿÿÿ‹ƒÜ  ‰D$X¡¤  …À…‹p  èüÿÿÿ‰Â…Àt^‰Ç¾@V  ¹Í   ó¥hœ  ‹ƒÜ  i€¸    ÿ°    ‚À  h¡  j ‰T$Pèüÿÿÿ‹D$‹T$ƒÄÇ‚      ƒÀ‰‚¬  ‹ƒÌ   »  ‰“Ä  ³È  ÇƒÈ     ˆ   ‰›ğ  ‰ƒĞ  ‰ğÇƒÌ     Çƒ      Çƒ     Çƒø     Çƒä     Çƒè      Çƒü      ‰»Ü  èüÿÿÿ…Àˆ   ‹ƒÄ  ‰˜(  ‹ƒÄ  ‰¸0  ‹ƒÄ  ‰°¸  ‹ƒÄ  Ç€ä     ƒ»€  t‹ƒÄ  ˆä      ‹ƒÄ  ¹ÿÿÿÿ‹à   ÿ21ÒjèüÿÿÿY_‰Æƒ¼  …ö‰&p  éSp  ƒÄ‰Å[‰è^_]Ãt& ½íÿÿÿéşÿÿfffèüÿÿÿSğºp’Â¶ÒÁâğºp’Ã‰ÑƒÉ„ÛEÑğºp’Ã‰ÑƒÉ„ÛEÑğºp sƒÊ‹H(…Ét-¬  [éüÿÿÿ´&    …Òuâ[Ã´&    v èüÿÿÿğºp Ãt& èüÿÿÿ‹H`öÂtğ€‰¸  öÂtğ€‰¸  öÂtğ€‰¸  ƒâtğ€‰¸  ‘¨  9  t‰Ñ‹    ¸    éüÿÿÿt& ‰Ğéÿÿÿ´&    fèüÿÿÿƒâ‹H`tğ€‰Ì  9  t‹    Á¼  ¸    éüÿÿÿt& ğº±Ì   ÃfffffffèüÿÿÿWVS˜hæÿÿƒì‹@üd‹    ‰T$1ÒÆD$ …Àt6‹Pd‹…Òt-‹r$…öt&‹    L$…Òtx‹z$…ÿtqº   èüÿÿÿ€|$ u<º   ‰Øèüÿÿÿº   ƒÈ€¶È‰Øèüÿÿÿº#   ‰Øèüÿÿÿº#   ƒÈ€¶È‰Øèüÿÿÿ‰Øº   èüÿÿÿ‹D$d+    uƒÄ[^_Ãt& º   èüÿÿÿëèüÿÿÿfffffffèüÿÿÿVSƒì0‹€h  d‹    ‰T$,1Ò‹…Û„)  ‹ƒ¨  …À„  ÇƒØ      ‹Pd‹R…Ò„³   ‹J…É„¨   ‹   …Ò„Ê  ‹r…ö„¿  ‰âèüÿÿÿ‹‹¸  ‹ƒ¨  ƒù†{  ƒé!ƒùw#º   Óâ÷Âğ…0  â‚  …€   t& …À„È   ‹Pd‹R…Ò„º   ‹J…É„¯   ‹   …Ò„Y  ‹Z…Û„N  ‰âèüÿÿÿ1ÀéŠ   ‹‹¸  ƒù†™   ƒé!ƒùw¯»   Óã‰Ù÷Ãğ…¬   á‚  t’ÇD$ è€  ÇD$$ø§  ÇD$ ] ÇD$    ÇD$  ÇD$¦ß¼ ÇD$”  ÇD$!   ÆD$…À…?ÿÿÿ´&    1À‹T$,d+    …¿   ƒÄ0[^Ã´&    ƒù†ÿÿÿ»   Óãã uƒù„kÿÿÿ…Ò…ışÿÿëµt& ÇD$    ÇD$  ÇD$ÕÜ2 ÇD$ ÆD$é¶şÿÿ¶    ƒù†§şÿÿº   Óââ u¸ƒù…şÿÿéÿÿÿf‰âèüÿÿÿé<şÿÿt& ‰âèüÿÿÿé<ÿÿÿt& ¸íÿÿÿé0ÿÿÿèüÿÿÿèüÿÿÿU‰ÅW1ÿVSì  ‹Ô  ‰T$d¡    ‰„$  1À‹…¨  ÇD$    …Àtz‹Pd‹R…Òtp‹2…ötj‹   …É„„   ‹   ‹	‰$…ÉtuL$Q¹   ‹t$T$èüÿÿÿZ‹D$‰ÆÁît1ÿ‹CTüƒÇèüÿÿÿ9şuí‹D$¿   …À…xÿÿÿt& €|$ uA‰ø„Àu1‹„$  d+    u^Ä  [^_]ÃL$Q¹   T$èüÿÿÿXë‹CèüÿÿÿëÅ‹[T$ÇD$    ÇD$    ‰ØÆD$èüÿÿÿÆƒ0  ‰Øèüÿÿÿë’èüÿÿÿ´&    èüÿÿÿWVS‰Ãƒì0d¡    ‰D$,‹ƒ¨  …À„ü   ÇƒØ     ‹Pd‹R…Òt'‹J…Ét ‹   …Ò„
  ‹r…ö„ÿ   ‰âèüÿÿÿ€|$
 ‹5   …   ´&    1ÀÆD$
f‰D$‹ƒ¨  …Àtr‹Pd‹R…Ò„¡   ‹J…Ét4‹   …Ò„—   ‹z…ÿ„Œ   ‰âèüÿÿÿ‹ƒ¨  …Àtp‹Pd‹R…Òtf‹J…Ét_‰â…ötT‹~…ÿtMèüÿÿÿ¶D$
„À„{ÿÿÿv ƒ˜  èüÿÿÿƒ¬  èüÿÿÿƒÀ  èüÿÿÿ‹D$,d+    u6ƒÄ0[^_Ãv èüÿÿÿ¶D$
ë±t& ‰âèüÿÿÿéoÿÿÿt& ‰âèüÿÿÿéüşÿÿèüÿÿÿ´&    èüÿÿÿ‹€h  ‹ …Àtézşÿÿ´&    v Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì0‹‹¨  d¡    ‰D$,1À…Ét…Òt‹ƒ¸  ƒøw-ƒø‡  t& ‹D$,d+    …Ó  ƒÄ0[^_]Ã´&    ƒè!ƒøwØ¾ƒñ£ÆsÎ‰Öƒæƒâ„Ü   ‹Ad‹@…À„ú   ‹x…ÿ„ï   ¡   ‰â…À„0  ‹h…í„%  ‰Èèüÿÿÿ‹‹¨  ‹ƒØ  ÆD$ …À•D$
…É…À   …ö…  ‹ƒØ  ÆD$…À‹ƒ¨  •D$
…À„>ÿÿÿ‹Pd‹R…Ò„0ÿÿÿ‹J…É„%ÿÿÿ‹   …Ò„ß   ‹Z…Û„Ô   ‰âèüÿÿÿé ÿÿÿ¾€£Æƒòşÿÿ‰Öƒæƒâ…$ÿÿÿ…ö„Üşÿÿ‹D$,d+    …¯   ƒÄ0‰Ø1Ò[^_]é›ûÿÿv ‹ƒØ  ÆD$ …À•D$
‹Ad‹@…À„2ÿÿÿ‹x…ÿ„'ÿÿÿ¡   ‰â…ÀtZ‹h…ítS‰Èèüÿÿÿé	ÿÿÿ´&    ‰Èèüÿÿÿ‹‹¨  éÖşÿÿ¶    º   ‰Øè$ûÿÿéáşÿÿ´&    ‰âèüÿÿÿé,şÿÿt& ‰Èèüÿÿÿé¶şÿÿèüÿÿÿ´&    èüÿÿÿUWVS‹ˆ¨  …É„Ä  ‹ˆ¸  ‰Æƒù"„¸  †Ú  ƒù0„I  †‹  ¿Á  ƒù3‡e  t& ¡   ¹   ºÀ  èüÿÿÿ‰Å…À„|  ‰0i†¸    ÿ°    hÖ  hÀ  èüÿÿÿ‰EƒÄ…À„^  ‹–Ì   ‹‚´   …À„§  Phæ  hÀ  èüÿÿÿ‰EƒÄ…À„%  ¸   èüÿÿÿ‰Ã…À„í  ‰E‹Eº   ‰ƒØ  ‹Ef‰“æ  ‰ƒÜ  ¸   f‰ƒà  ‹†Ì   ·P$f…Ò…E  ·@ f‰ƒâ  ‹†Ì   ·@"f‰ƒä  ‹†Ì   ‰»ì  ˆ   ‰«h  ‰C$‰ØÇƒ4  üÿ¿Çƒ8      Çƒà  ğ”  Çƒä   š  Çƒè  ñ  ‰®Ô  èüÿÿÿ‰Ç…À…î   [‰ø^_]Ã´&    ¿íÿÿÿƒù:uè¿g  éŒşÿÿt& ƒù)w#¿¶  ƒù'‡uşÿÿ¿íÿÿÿ[‰ø^_]Ã´&    fƒé-ƒùwã¿Á  éNşÿÿ¶    ƒùw3¿íÿÿÿƒùv‹¸   Óà© u•ƒù…tÿÿÿ¿  éşÿÿ´&    ¿œ  ƒù!„şÿÿ¿íÿÿÿë‹v ‹‚ˆ   éNşÿÿt& f‰“â  ‹†Ì   ·@&éºşÿÿf¿Ë  éÆıÿÿ¶    ‰ğè™ùÿÿ‰ØÇ†Ô      èüÿÿÿ‹Eèüÿÿÿ‹Eèüÿÿÿ‰èèüÿÿÿ‰ø[^_]Ã´&    v ¿ôÿÿÿëÑ´&    f¿t  éfıÿÿ¿ôÿÿÿé®şÿÿ¿ôÿÿÿëµ¿ôÿÿÿë¶´&    ´&    èüÿÿÿS‰Ãèùÿÿ‹ƒÔ  …Àt9‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  èüÿÿÿÇƒÔ      [Ãfffffffèüÿÿÿ‰Â‰È‹’”   ‹’À   ‹’Ü   Š  ‹’  ƒÊ ‰‹T$‰1ÀÃ´&    t& èüÿÿÿWVS‹°”   ‰Ó¾P  ‰øèüÿÿÿ‹–$  ‰‹–(  ‰S‹–,  ‰S‹–0  ‰S‹–4  ‰S‹–8  ‰S‹–<  ‰S‹–@  ‰S‹–D  ‰S ‹–H  ‰S$‹†L  ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿWVS‹°”   ‰Ó¾ğ   ‰øèüÿÿÿ‹–Ä   ‰‹–È   ‰S‹–Ì   ‰S‹–Ğ   ‰S‹–Ô   ‰S‹–Ø   ‰S‹–Ü   ‰S‹–à   ‰S‹–ä   ‰S ‹–è   ‰S$‹†ì   ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸P  VS‹°À   ‰Ã‰øèüÿÿÿ‹†Ü     ‹ƒâß‰‹†Ü      ‹âıÿÿ‰‹†Ü      ‹ƒâß‰ºÿÿ  ‹†Ü   ‰  Æƒ.  ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸ğ   V‰ÆS‹˜À   ‰øèüÿÿÿ‹ƒÜ     ‹ƒâì‰‹ƒÜ      ‹â¿şÿÿ‰‹ƒÜ      ‹ƒâï‰‹ƒÜ      ‹ƒâó‰1Ò‹ƒÜ   ‰  ºÿÿ  ‹ƒÜ   ‰  Æ†Î   ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿUW‰ÇVSì„   ‰D$‰L$d¡    ‰„$€   ‹‡”   ‹°À   ‰D$‹†Ü      ‹ ‰D$ ‹†Ü   ‹¸  ‹†Ü   ‹˜  ‰Ø‰Ú‰ıÆ ƒàƒâ ƒå ‰D$‰Øƒà‰$‰øƒà‰D$‰øƒàƒ=¨  ‰D$‡ìp  …ít…Ò…¶  !û‹|$‰Ø‹$ƒàˆD$…Û‹D$•Â…ÿ¨   •À!Â‹D$ˆT$…À„p  ‹L$1Û…É„b  ‹D$  ‰$¶    ‹†Ü   @  ‹ ‰Ââÿÿıÿ‰TÜ@ƒÃ©   t=ƒûuØ‰èèüÿÿÿ¹@   ‰Ç‹$T$@èüÿÿÿ‰ú‰Ã‰èèüÿÿÿƒû@„ê   éõq  t& ‰èÁãèüÿÿÿ‰Ù‰Ç‹$T$@èüÿÿÿ‰ú‰$‰èèüÿÿÿ;$…õq  ‹D$ÇD$<    Æ €|$ …r  €|$ ¸ÿşÿÿt*ƒL$<‹|$ ‹Ü   !ø‰   ‹†Ü   ‰¸   ‹D$Æ ‰èèüÿÿÿ‹L$‰Â‹  +  =ÿ  vƒL$<‰èèüÿÿÿ‹\$<…Ût‹L$…É…©   ‹„$€   d+    …Ä   Ä„   1À[^_]Ã1Ûé¹şÿÿ´&    f€|$ tU1Ûéşÿÿ‹†Ü     ‹ƒâß‰‹D$ÇD$<   …Àt ‹@`…Àt‹h8…ít‹D$L$<ºv@èüÿÿÿ‹D$Æ éüıÿÿ€|$ ÇD$<    „ ÿÿÿéQr  t& ‹|$‹G`…À„Hÿÿÿ‹X8…Û„=ÿÿÿL$<º v@‰øèüÿÿÿé(ÿÿÿèüÿÿÿt& èüÿÿÿU‰ÕWVSƒì$‹”   ‰D$ ‹‚  ‹š  ‰D$ƒáøu‹D$8Ç     ƒÄ$1À[^_]Ã´&    º   ‰L$‰ø‰$èüÿÿÿ‹$‹L$‰Æ‚  ‰êèüÿÿÿ‰ò‰$‰øèüÿÿÿ‹$‹|$8‰Èƒàø‰‰ÈÁèt¤·ÛDÅ ƒÃ‰D$‰$Áû‰\$éë   ‰ğ‹\$Áèƒà‰D$…Ûtƒt$‹D$ƒàˆD$1ÛÇD$    ·ö1ÿ‹L$‹$¤÷Áæ‰ğ‰ş‰ÏƒÈ¯ò¯ø‰D$÷$$ş¿è  òiòè  ÷çò1ö‰Ñ‰Ç…Òt‰Öº÷ÚKÑî‰ğ÷â‰ĞÁêÁè‰ÖkÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‰òƒùvƒÀº ¡ =çhÍw
ºÓMb÷âÁêÁã‰U 
\$ƒ=¨  ÇE    ˆ]‡wr  ƒÅ;l$„œşÿÿ‹u ‰ğ%ÿÿ =ÿÿ … ÿÿÿÆD$ »   ÇD$   ÇD$    éÿÿÿèüÿÿÿUWVSƒì$‹°”   €z
 ‹®À   …Î  ‹B‰Ó‰D$…À…  †P  ‰$èüÿÿÿ¶C
ˆ†.  ÇC    Ç†(      Ç   Ç†$     ‹…Ü     ‹ƒâß‰‹…Ü      ‹âıÿÿ‰¶K‹…Ü   Áá   ‹1Èƒàß1È‰¶Cˆ†/  „À„å   ‹K1ÒÁá‰ÈÑè€ù7÷ñ‰Á=   ‡M  ƒø†¬  @ÿ‰Ï‰D$Áç‰D$¸üŸ÷é‰D$‰øÑè‰T$ 1Ò€ù7÷÷iÑ   ¹÷ÚKƒÂÑê‰Ç‰Ğ÷áÁê‰T$‹…Ü   ‹L$‰ˆ  ‰{ºY·Ñ‰¾4  kKdÁ8  ‰È÷â¸   Áê	ùq  ƒÒÿ9ÂGĞ‹…Ü   ‰  ƒÂ‹D$kÒdƒÂÁê‰S‰–8  ‹T$ é  v ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ñ‰Ç…Òt‰ÈºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Á‰ø‰Ê¿è  ÷÷‰Ñ‹T$ùó  vƒÀƒÒ 1É‰D$‰×…Òt"¹@  ‰Ğ÷á‰ø)ĞÑè‰ÊÁê‰Ğ‰ÑÁà)Ğ)Ç‰ú‹D$¿ÿÿ ÷÷‰Ê¿   1É9Ç‰Ï×‚¨  ¿   9ÇÑƒ  ƒÀÿ‰ÁƒÒÿ‰D$‰ÇƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹…Ü   ‰¸  ‹D$‹T$ÇD$    ‰Ñ‰Ç…Òt‰Ğº÷ÚKÑè÷â‰ĞÁêÁè‰T$kÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‹T$ƒùvƒÀ‰C‰†0  ‹D$‰†d  ‹D$‰C(‰†L  ‹…Ü      ‹€æ÷‰¶K‹…Ü   Áá   ‹1È€äï1È‰¶Cˆ†=  ¶K‹…Ü   Áá   ‹1È€ä1È‰¶Cˆ†<  ¶C	ˆ†-  ¶Cˆ†,  „ÀtC€{	 t‹…Ü     ‹ƒÊ ‰‹…Ü   €{ ¿€  D|$   ‹€   1ø%ıÿÿ1ø‰‹$èüÿÿÿ1ÀƒÄ$[^_]ÃfƒÄ$‰ğ[^_]éöÿÿfÇD$÷J ¿ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  é|şÿÿ¶    ÇD$÷J ¿3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  éÓüÿÿ¶    ÇD$”   ¿   ÇD$   ÇD$0ø?ÇD$    éşÿÿ¶    ÇD$”   ¿Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   éküÿÿ¶    ¸Úÿÿÿéÿÿÿ¶    èüÿÿÿUWVSƒì$‹¨”   €z
 ‹½À   …  ‹B‰Ó‰D$…À…}  …ğ   ‰$èüÿÿÿ¶C
ˆ…Î   ÇC    Ç…È       Ç   Ç…Ä      ‹‡Ü     ‹ƒâì‰‹‡Ü      ‹â¿şÿÿ‰¶K‹‡Ü   Áá   ‹1Èƒàï1È‰¶Cˆ…Ï   „À„5  ‹s1ÒÁæ‰ğÑè€ù7÷ö‰Á=   ‡  ƒø†Œ  @ÿ‰Î‰D$Áæ‰D$¸üŸ÷é‰D$‰ğÑè‰T$ 1Ò€ù7÷öiÑ   ¹÷ÚKƒÂÑê‰Æ‰Ğ÷áÁê‰T$‹‡Ü   ‹L$‰ˆ  ‰sºËk(¯‰µÔ   ÇC2   Ç…Ø   2   ‹KÁáq	‰ğ÷â)ÖÑîò1öÁê9S sQ
¾ÍÌÌÌ‰Ğ÷æ¾   ÁêƒÁ‰S ºOìÄN‰È÷âÁê9S$vº«ªªª‰ÈƒÎ÷âÁê‰S$‹‡Ü      ‹1ğƒàü1ğ‰‹C ‹T$ ‰…ä   ‹C$‰…è   ‹D$é  t& ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ö‰Á…Òt‰ğºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Æ‰È‰ò¹è  ÷ñ‰Ö‹T$şó  vƒÀƒÒ 1ö‰D$‰Ñ…Òt"‰Èº@  ÷â‰È)ĞÑè4‰òÁê‰Ğ‰ÖÁà)Ğ)Á‰Ê‹D$¹ÿÿ ÷ñ‰ò1É¾   9Æ‰ÎÖ‚˜  ¾   9ÆÑƒ‘  ƒÀÿ‰ÁƒÒÿ‰D$‰ÆƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹‡Ü   ‰°  ‹D$‹T$1ö‰D$‰Ñ…Òt‰Öº÷ÚKÑî‰ğ÷â‰Ğ‰ÖÁèÁîkÀ6)Á‰Ê‹D$¹6   ÷ñ‰Ñ‰òƒùvƒÀ‰C‰…Ğ   ‹D$‰…  kK6‹·Ü   Áô  ùÿÿç‡.  ùŸ  †²  ‰ÈºÓMb÷â¸÷ÚK‰ÑÁéiÑè  ƒÂÑê÷âÁê‰  ‹D$‰S‰•à   ‰C(‰…ì   ‹‡Ü      ‹€æû‰‹‡Ü      ‹ƒÊ‰¶Cˆ…Ü   ‰…  ¶C	ˆ…Í   ¶Cˆ…Ì   „À…«   ‹$èüÿÿÿ1ÀƒÄ$[^_]Ã´&    fƒÄ$‰è[^_]érñÿÿfÇD$÷J ¾ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  éŒşÿÿ¶    ÇD$÷J ¾3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  é“üÿÿ¶    º«„ ¹ÿÿ  éòşÿÿ´&    fµ   ‰ğèüÿÿÿÇ…      Ç…      ‰Â‰ğèüÿÿÿ€{	 t‹‡Ü     ‹ƒÊ‰€{ ¸@  DD$‰Ã‹‡Ü      ‹€   1Ø%¿şÿÿ1Ø‰éáşÿÿt& 1Ò1Ééhşÿÿ´&    ÇD$”   ¾   ÇD$   ÇD$0ø?ÇD$    é„ıÿÿ¶    ÇD$”   ¾Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   é‹ûÿÿ¶    ¸Úÿÿÿéeşÿÿ¶    èüÿÿÿUWVS‰Ãƒì$‹€”   ‹€À   ‹¸Ü   ‹·   ‹¸Ü   ‹  ‰L$‹¸Ü   ‹  ‰L$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹€Ü   ‹€  ƒÃp‰D$½f  ¿^  Sh  èüÿÿÿ÷Æ   XZ¸W  º[  DÂPSh3  èüÿÿÿ‰ğ‰éƒÄƒàEÏ‰$QSh<3  èüÿÿÿ‰êƒÄ÷Æ@   E×RSh46  èüÿÿÿ‰ñƒáÿ4   ShĞ7  èüÿÿÿºz  ¸o  ƒÄ÷Æ @  DÂPShl3  èüÿÿÿº”  ¸Š  ƒÄ÷Æ   DÂPShœ3  èüÿÿÿº¹  ¸©  ·|$ƒÄ÷Æ    DÂPShÌ3  èüÿÿÿ‹L$ƒÄ…É„(s  écv  t& èüÿÿÿU¹h  ºÀ  WV‰ÆSƒì0d¡    ‰D$,1À¡$   èüÿÿÿ…À„Ø  ¸  º   ‰Ã¹   Ç€       ‰øhÀ  èüÿÿÿZ…À…™  ‰³À   º`  ‰Øèüÿÿÿ†¼  ‰›”   PCph7  j PèüÿÿÿF‰ÚÇƒ      èüÿÿÿ‰ÅƒÄ…À…	  ‹†Ü   ‰¨  ¹¨  ƒğ   ºA  èüÿÿÿ‰è¹
   t$Ç$   ‰÷ó«‹CdÆD$
ÇD$ ÇD$ ¸ˆ  ÇD$$ˆ  ‹@ÆD$ÇD$ Œ  …Àt(‹H…Ét!¡   ‰â…À„¨   ‹x…ÿ„   ‰ØèüÿÿÿƒP  ¹¨  ºX  ‰÷èüÿÿÿ1À¹
   ó«‹CdÇD$   Ç$   ‹@ÆD$
ÆD$ÇD$ Œ  …Àt2‹H…Ét+¡   ‰â…ÀtI‹p…ötB‰Øèüÿÿÿë´&    f‰øèüÿÿÿ‹D$,d+    uAƒÄ0‰è[^_]Ãf‰Øèüÿÿÿé^ÿÿÿt& ‰ØèüÿÿÿëÎ´&    ‰Ø½ôÿÿÿèüÿÿÿë¹½ôÿÿÿë²èüÿÿÿ¶    èüÿÿÿVº   Sèüÿÿÿ…Àt<‰Ã‹€”   èìÿÿ‹ƒ”   è“ëÿÿ‹³”   ‰Øèüÿÿÿ†  èüÿÿÿ‰ğèüÿÿÿ1À[^Ã¸íÿÿÿëöffffèüÿÿÿS‰ÃfÁÂ¹   ƒìd¡    ‰D$1À¸   f‰T$‰âf‰D$D$‰D$‰ØÇ$D   èüÿÿÿƒø…Jw  ¹   ‰â‰ØÇD$  èüÿÿÿƒø…4w  ¶D$‹T$d+    uƒÄ[Ãèüÿÿÿt& èüÿÿÿVS˜
  ‰Øƒìd‹    ‰T$1Òº  èGÿÿÿ¹   º  ƒàïf‰L$t$¹   f‰T$‰âˆD$‰ØÇ$D   ‰t$èüÿÿÿƒø…¢w  ¹   ¸ê   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$³èüÿÿÿƒø…Œw  ¹   ¸	   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$èüÿÿÿƒø…vw  º  ‰Øè~şÿÿ¹   ƒÈf‰L$‰â¹   ‰t$¾  ˆD$‰ØÇ$D   f‰t$èüÿÿÿƒø…`w  ‹D$d+    uƒÄ[^Ãèüÿÿÿffffèüÿÿÿ1ÀÃ´&    èüÿÿÿU¶ÒWV‰ÆS‰ËƒìPd‹    ‰L$L1É‹D$hf‰$1Éf‰L$‹|$dPˆ\$f‰T$T$‰T$ƒøs4…Àup¹   ‰â‰ğèüÿÿÿƒøu}1À‹T$Ld+    …‚   ƒÄP[^_]Ãf‹l$L$)Í‰T$‹Tü)ï‰T	è‰úƒàüƒørªƒàü1ÿ‹,:‰,9ƒÇ9Çróë–¶    ¶ˆL$¨t…·Tşf‰Tévÿÿÿ¶    ‹°  …Ò…¸w  ¸ÿÿÿÿémÿÿÿèüÿÿÿv èüÿÿÿWVSƒì‹p,d‹    ‰L$1ÉÆD$€…Òu|¸¬Ä  èüÿÿÿ¶VL‹FH1Éj|$Wè½şÿÿY‰ÃX…Ûu5¸¬Ä  èüÿÿÿ¶VL‹FH1ÉÆD$ jWè•şÿÿ¸è  èüÿÿÿ1Ò‰ğèüÿÿÿXZ‹D$d+    uƒÄ‰Ø[^_Ã´&    v »êÿÿÿëÙèüÿÿÿt& èüÿÿÿV‰ÎS‰Óƒì d‹    ‰T$1Ò¶ÓL$‰t$f‰T$f‰T$‹T$,‰L$¹   f‰L$¹   f‰T$T$ÆD$ ÇD$   èüÿÿÿƒøu)¡°  …À…Üw  1À‹T$d+    u"ƒÄ [^Ã´&    ‹°  …Ò…x  ¸ÿÿÿÿëÑèüÿÿÿ¶    èüÿÿÿS‰Ã1Òƒì(d¡    ‰D$$1ÀC¬èüÿÿÿ¡    9Cx‹Cü…Àt‹D$$d+    uOƒÄ([Ãt& ¶Sø‹Côj!L$èöşÿÿY‰Â¡    è  ‰C…ÒuÄ¡°  …À….x  ¶D$ÁàÁøƒà‰Cüë¤èüÿÿÿ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹D$,‰L$‰$‹D$4‰D$d¡    ‰D$‹Fd‹…Ò…Ş  ¶ù9~lte¶VL‹FHjL$è^şÿÿ‰ÂX…Òt‹D$d+    …¶  ƒÄ‰Ğ[^_]Ãt& ¶D$¶VLƒàó
D$ˆD$‹FHjL$Q1ÉècüÿÿY]‰Â…Àu´‰~l«¤  ‰èèüÿÿÿ‹ƒÜ   º   ‰$  ‹ƒÜ      ¶D$0€ÌÇ‰‹ƒÜ      ‹€æû‰‹ƒÜ      ‹D$0¶Ä€ÌË‰‹ƒÜ      ‹€æ÷‰€<$ …Ş   ‹ƒÜ      ¶D$€ÌÃ‰‹ƒÜ      ‹¿ÿıÿÿ€~L@¹ÿşÿÿEÏ!Ê‰‹ƒÜ      ‹
€<$À% Àÿÿ-@  !È‰‹=    ƒÇë´&    f‹    9×x¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €ÎÃ‰¶Ğ‰è‰T$èüÿÿÿ¡°  ‹T$…À…Xx  1À€<$ DĞé_şÿÿ¶    ‹ƒÜ   ºÿ  ‰$  é"ÿÿÿºêÿÿÿé9şÿÿèüÿÿÿ´&    t& èüÿÿÿS¶\$¶É‹@,SQ¹   j è±ıÿÿƒÄ[Ã´&    t& èüÿÿÿ¶É‹@,j Q¹   jè†ıÿÿƒÄÃfèüÿÿÿS¶\$‹@,SQ1Éj ègıÿÿƒÄ[Ãfèüÿÿÿ‹@,j Q1ÉjèLıÿÿƒÄÃ´&    èüÿÿÿWVS‰ÓƒìT‹p,d‹    ‰T$P1Ò…Û…‹   ¶VL‹FHjL$è¹ûÿÿ¶VL¶D$¹   ‹~HÇD$
   ƒÈ`f‰T$T$ˆD$‰T$T$ˆD$‰øÆD$ èüÿÿÿYƒøu‰Ø‹T$Pd+    u*ƒÄT[^_Ãt& ‹°  …Ò…Åx  »ÿÿÿÿëÑv ¸êÿÿÿëÉèüÿÿÿt& èüÿÿÿVSƒìP‹p,d‹    ‰\$L1Û…Ò…®   …Éur»   ¸   8^htF‹°  …É…çx  ¶VL‹FH¹   ˆ\$ÇD$   f‰$T$‰T$‰âÆD$èüÿÿÿƒøuHˆ^h‹FP‹T$Ld+    uLƒÄP[^Ã´&    ƒ=¬  ÛƒãüƒÃƒ=¬  ÀƒàüƒÀéuÿÿÿt& ‹°  …Òt±é	y  ¸êÿÿÿë§èüÿÿÿt& èüÿÿÿ‹°  V‰ÆS‰Ó…É…ey  1À÷Ã  €t÷Ã  € uEã   u¸   [^Ãt& ‹8  ‹    ¸    ƒÁTèüÿÿÿ¡°  …ÀtÑé(y  ´&    f‹†ô  ‹    HT¸    èüÿÿÿ‹°  …Ò…Iy  ã   t”ëŸ´&    ´&    èüÿÿÿUW¿31  V‰ÆS»   ƒì(‹-°  d¡    ‰D$$1Àƒ=¬  ÇD$    ÀÇD$ D  ƒàüÇD$
    ƒÀÇD$  D ÇD$    ÇD$   f‰\$ÆD$ ˆD$ÇD$  f‰|$"…í…ºy  ¡   ¹t   ºÀ  èüÿÿÿ‰Ã…À„y  ‰†  ‹Fƒø„T  ƒø…  ÆCLAºA   ‹ƒ=¬  Ç    ÇC    ¸ü   À1ÉÇC    ƒàü‰{HƒÀÇC    ˆChÇC    ÇC    ÇC     ÇC$    ÇC(    ‰[,‰sdj"D$P‰øè±öÿÿ¶SL¹   ‰Ç‹CHjl$&Uè—öÿÿ¶SL¹   	Ç‹CHjUèöÿÿƒÄ	Çuzj1ÉF(‰ÚèüÿÿÿY‰Ç…ÀueCXÇCTàÿÿÿKT‹    ‰CX‰C\¸    ÇC` º  èüÿÿÿ‹°  …Ò…Öy  ‹D$$d+    uNƒÄ(‰ø[^_]Ã¶    ¶SLéåşÿÿ´&    ¡°  …À…y  ‰Øèüÿÿÿë½´&    v ÆCL@º@   é°şÿÿèüÿÿÿƒ=°   ¿ôÿÿÿtÌéy  ´&    v èüÿÿÿ…Àt/S‹˜  …Ût‹C,…Àt‰Øèüÿÿÿ‰Ø[éüÿÿÿ¶    [Ã¶    Ãfffèüÿÿÿƒì d‹    ‰L$1É¹   ˆT$‰âf‰L$L$‰L$¹   f‰L$L$‰L$¹   Ç$P   ÇD$P  ÆD$ èüÿÿÿƒø…òy  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    ´&    èüÿÿÿSƒìd‹    ‰\$1Û»   ˆT$‰âf‰\$\$ˆL$¹   Ç$P   ‰\$èüÿÿÿƒø…z  »
   ´&    ¸X‰A èüÿÿÿƒëuñ1À‹T$d+    uƒÄ[ÃèüÿÿÿèüÿÿÿW‰ÇV‰Öº?   S»@   èüÿÿÿˆFf‰Ú‰øèüÿÿÿˆDÀƒÃƒûFuëv ‰Ú‰øèüÿÿÿˆDÀƒÃƒûLuë[^_ÃfffffffèüÿÿÿW¿   V1öS‰Ã´&    ¹   º   ‰Øöèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ	Æ‰ø,‰ÇuŒ‰ğ[^_Ã´&    ´&    èüÿÿÿW¿   V‰ÖS‰ÃëEt& èüÿÿÿ¸R÷ öèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ‰ø,‰ÇtX¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿº   ‰Ø¹   èüÿÿÿ‰ğº   „À‰Ø‰wÿÿÿèüÿÿÿérÿÿÿt& [^_Ã´&    t& èüÿÿÿW¿8  VSƒìd‹    ‰L$1Éf‰<$ÆD$ƒú„I  ƒú„p  …Ò„P  ‹€$  L$º   ‹@ ‹‰à´&    v PƒÀ9Áuö÷Ú¹   ‰ØˆT$º   èüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºà   ‰Øè2şÿÿ¸   èüÿÿÿº   |$t$ë´&    f¶ƒÇ‰Øèşÿÿ9şuï¹   º   ‰Ø1öèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿv ¸   ƒÆèüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ„Àt‰ğ<vÏ‰ğ<„z  ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºá   ‰ØèRıÿÿ¸   èüÿÿÿ‰Øè¡üÿÿº   8Ğ‰×Fø1öv ‰Øè‰üÿÿ‰ğ‰ùƒÆ8Èuî¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿ‰Øº   èüÿÿÿ1À‹T$d+    uNƒÄ[^_Ã¶    »  ÆD$f‰\$é´ıÿÿt& ¾  ÆD$ f‰t$éœıÿÿt& 1ÉÆD$ f‰L$é‡ıÿÿèüÿÿÿfèüÿÿÿ1ÀÃ´&    èüÿÿÿ‹P‹H|‹R‹AP·Òƒè!Ğ¯ALÃfèüÿÿÿ‹@|4  ‰Ğéüÿÿÿ´&    t& èüÿÿÿUWV‰ÖSƒì‹X{‰øèüÿÿÿ…ö„=  ½êÿÿÿƒş…  ‹3‹k ‹†d  ‰D$  ‰$¡´  ƒÀƒø‡7z  ‹†Ü   @  ‹ƒâî‰ÿu‹M ‹T$‹èüÿÿÿ‹†Ü   ‹U ‰P  º   ‹†Ü   ‰X  º   ‹†Ü   ‰0  ¡´  ÇC    ZƒÀƒø‡  ‹†Ü   º ‰P  ºÿÿÿÿ‹†Ü   ‰T  ‹†Ü     ‹*‹‹ˆä   ‰ÈƒÈ÷Ğ!è	ÈƒÈ‰‹†Ü      ‹ƒÊ ‰‹†Ü   @  ‹€@  ƒÈ‰‹5´  …ö…‹   1í‰øèüÿÿÿ‰èƒÄ[^_]Ã´&    ¡´  ‹3ƒÀƒø‡Uz  ‹†Ü   @  ‹ƒâî‰‹†Ü     ‹ƒâ÷‰‹†Ü   P  ‹âşïşÿ‰¡´  …Àt‹–d  ‹1íÂ  èüÿÿÿéxÿÿÿ‹$‹1íèüÿÿÿéfÿÿÿ‹D$‹S‹M ‹€  ¯ÊƒÀƒàü†Ü   ‹ ÑèQRPÿu ‹¼  Ph<;  èüÿÿÿƒÄé¢şÿÿ´&    t& èüÿÿÿUWVS‹h‹X|…í„mz  ‰Ç¹   1Ò‰Øèüÿÿÿ‰Æ…Àˆ¯   ‰}$1À»ø   ¹   ó«Æƒ)  1öÇƒø    Çƒü      Çƒ       Çƒ  €   Çƒ  €»  Çƒ  €»  Çƒ     Çƒ     Çƒ    Çƒ!    ‹E ‹€d  ‹€  =   tÁèƒàø‰ƒ  ‰ƒ   [‰ğ^_]Ã¡´  ƒÀƒøvëé‚z  ´&    ´&    èüÿÿÿ‹P…Ò„°   VS‰Ã¡´  ƒÀƒø‡œz  ‹s ‹N …Éua‹FèüÿÿÿÇF    ‹FèüÿÿÿÇF    ‹S ‹C‹J‹rj ÿrˆ   ÿr‰òèüÿÿÿ‹C èüÿÿÿÇC     ƒÄÇC    [^Ã´&    ‹V‹N$‹Cj jˆ   èüÿÿÿÇF     X‹s Zéwÿÿÿ´&    ´&    ´&    èüÿÿÿS‰Ã‹@|‹€4  …Àt‹Cèÿÿÿ‹C|Ç€4      1À[ÃfèüÿÿÿUW‰ÇVS‰Óƒì‹p‹@|‹€4  …Àt‰ğèÙşÿÿ‹G|Ç€4      ‹ƒL  ‰F‹“X  ¯Â‰V‰F…À„Ô  Bÿ…Ğ…Ñ  ¡   ¹(   ºÀ  èüÿÿÿ‰Ã…À„µ  ‹F‰‹F‰^ ˆÿ  ‰Íá ğÿÿ‰È‰$Áíèüÿÿÿ‹$…À‰C„ˆ  ‹´  ƒÂƒú‡µz  1Òèüÿÿÿ‰k$Dm Áàèüÿÿÿ‰C…À„  ‹S$èüÿÿÿ‹C$…ÀtU1íë#´&    f	ÂÇA    ƒÅ‰ÇA   9k$v.‰èÁàCèüÿÿÿ…À„±   ‹KTm Ñ‹ƒâ¨t¾t& ‹n ‹F‹M$‹Uj ˆ   jèüÿÿÿ‰E Y]…À„Üz  ‹K‹FjSÿvÿvèüÿÿÿ‰ÂƒÄ…Àxu‹CÇ   q‹C‹S‰P‹CÇ@    ‹V ‹G|‹R‰4  ‹G|‹V‰@  ‹G|1ÒÇ€8      Ç€<      ë7‹CèüÿÿÿÇC    ‹CèüÿÿÿÇC    ºôÿÿÿ‰Ø‰$èüÿÿÿÇF     ‹$ƒÄ‰Ğ[^_]Ã´&    ¶    ºôÿÿÿëŞ¡´  ºôÿÿÿƒÀƒøv¸é÷z  èüÿÿÿ1ÀÃ´&    èüÿÿÿUWVS…Êts‹¨0  ‰Æ‰Ó‹€Ü   ‰T  ÷Â   …{  öÇuƒã¿   uR[‰ø^_]Ãv ¡´  ƒÀƒø‡V{  ‹†Ü   º   ‰0  ¿   [^‰ø_]Ã´&    v 1ÿ[^‰ø_]Ã´&    ‹†Ü   ‹€   ‰E‹E$èüÿÿÿ‰ø[^_]ÃfèüÿÿÿVS‰Ãƒìd¡    ‰D$1À¡¸  …À…+  ‹ƒd  ‹€  …À„p{  ‹ƒÌ   T1Éºÿÿÿÿj(h    ˆ   èüÿÿÿƒÄ…Àˆ¡{  ‹$T$1É‹°ˆ  ‰‹ƒÌ   ÇF    ‰F‹$‰FRº  jj èüÿÿÿƒÄ…Àˆ°   ‹D$¹   º  Ã¼  ‰°¸  ƒÀXèüÿÿÿ‹D$¹   º   èüÿÿÿ‹$¹   º)  ƒÀèüÿÿÿh1  ‹D$ƒÀ$Pèüÿÿÿ‹D$SP$ƒÀDRhB  Pèüÿÿÿ‹D$èüÿÿÿƒÄ…Àx-ƒ=´  ÿ…ˆ{  ‹D$d+    u ƒÄ‰ğ[^Ã´&    1öëà‹$èüÿÿÿé¡{  èüÿÿÿ´&    v èüÿÿÿ‹€0  ‹@éüÿÿÿˆ«©ÿÿƒù‡  ºÓê€â„  ÿt$,Sh8   èüÿÿÿƒÄ‹D$Dd+    u=ƒÄH[Ãˆ[Íşÿº @Óê€â„  ëÄˆıæşÿº Óê€â„  ë¬ˆªÿÿë”èüÿÿÿèüÿÿÿVS˜¼  ‹€Ì   ƒx$ uSSSSShd   èüÿÿÿƒÄëSSSShD  èüÿÿÿƒÄS1öh  èüÿÿÿXZiÆ  ÿ°    VFSh\  èüÿÿÿƒÄƒş?uß[^ÃQ¾êÿÿÿh   h€  èüÿÿÿƒÄém  €=    Yt}h   h¨  èüÿÿÿ‹ƒ¸  _Y‰Âé  ¢   ¶   ¸"   ¾   €úEğVRh  èüÿÿÿ‹‹Ì   ‰òD$Áˆ   èüÿÿÿƒÄ…ÀuP‹$D$èüÿÿÿ‹$èüÿÿÿéB  ÿ5#   iƒ¸    ÿ°    ƒ¼  Phè  èüÿÿÿ‹ƒ¸  ƒÄ‰Âé  Vh<  èüÿÿÿ[^ë­‰$ÿsSh4  h  èüÿÿÿ‹T$ƒÄé‚  ‰$ÿsSh4  hP  èüÿÿÿ‹T$ƒÄé‚  hŒ   hŒ   hµ  èüÿÿÿƒÄé=   ‹†è  ÿ³  ÿ0ÿpÿ¶ğ  ÿ¶ì  ÿsSh  h(  èüÿÿÿ1ÀƒÄ$é¶!  h  h  hÄ  èüÿÿÿ‹ƒ€  ƒÄé`"  h  h  h  èüÿÿÿ‹ƒ€  ƒÄé#  ht   ht   hµ  èüÿÿÿƒÄéj$  QRPht   ht   hx  èüÿÿÿƒÄéî$  ¸0  èüÿÿÿ¸(  èüÿÿÿÿv³ˆıÿÿWÿ³ŒıÿÿVh¬  h°  èüÿÿÿƒÄé'  hÌ   hÌ   hµ  èüÿÿÿƒÄéc'  ¸0  èüÿÿÿ¸(  èüÿÿÿQhä   hä   hä  èüÿÿÿƒÄéI(  h`   h`   hµ  èüÿÿÿƒÄé{(  h  h  hµ  èüÿÿÿƒÄé)  Ph  h  h  èüÿÿÿƒÄéÕ)  hœ   hœ   hµ  èüÿÿÿƒÄéÔ+  h€  hè  èüÿÿÿXZé·,  R‰T$hÔ  hÔ  hd  èüÿÿÿ‹T$ƒÄé-  h¼  1öhŒ  èüÿÿÿXZƒ=x   uƒÆƒş$uïéœ-  ‰ò‰Øèüÿÿÿ¶ÀPVh¼  h°  èüÿÿÿƒÄëÑi€¸    kÒ„    ÿpDÿp@ÿp<ÿp8ÿp0Vhì  hì  hØ  èüÿÿÿƒÄ$é÷-  R‰T$h¨  h¨  hä  èüÿÿÿ‹T$ƒÄéä/  Rhø   hø   hÿ  èüÿÿÿ‹ƒÄéd0  hl  hl  hµ  èüÿÿÿƒÄéº0  ¸0  èüÿÿÿ¸(  èüÿÿÿh¬   h¬   hµ  èüÿÿÿƒÄéí1  Ph  h  h  èüÿÿÿƒÄé%2  Sh   h   hÿ  èüÿÿÿƒÄƒû†S2  ƒ=x   „Œ2  h   h   h(  èüÿÿÿ¸êÿÿÿƒÄé2  ÿt$ÿt$ÿ3ÿt$hŒ  hŒ  hL  èüÿÿÿƒÄé?4  hŒ  hˆ  èüÿÿÿ[^éù4  h   hx  h\	  èüÿÿÿƒÄé¬6  ƒ=x  vh   hx  h´  èüÿÿÿƒÄ†¼  Phè  èüÿÿÿ‹–d  ‰ğèüÿÿÿXZé6  h   hx  h	  èüÿÿÿƒÄé–6  h\  h\  hµ  èüÿÿÿƒÄéR7  ‹“à  ‹‚  …À„Ü   PVhÄ	  èüÿÿÿ¹Q  º@  ‰Øè\(  ‰ƒä  ƒÄ‰¨¸  ‹ƒä  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡”   ‹ƒä  ˆä      ‹ƒä  ‹“Œ  ‹• J  ‹à   ÿ2º   jèüÿÿÿY_…Ày‰$Vhğ	  èüÿÿÿXZ‹$é¤:  hD  hD  hµ  èüÿÿÿƒÄé8  ‰$Vh˜	  èüÿÿÿ]X‹$é¤:  ‹‚è   éÿÿÿƒ»€  ‹ƒä  „oÿÿÿˆä      ‹ƒä  éZÿÿÿ‹“ä  ‹‚  …Àt#PVh
  èüÿÿÿ‰Øèüÿÿÿ1Ò‰ƒ0  ƒÄé±:  ‹‚è   ëÕÿsSh¸  hD
  èüÿÿÿƒÄék>  ÿsSh¸  h|
  èüÿÿÿƒÄék>  h   h   hĞ  èüÿÿÿƒÄé*@  hÈ  hÈ  h¸
  èüÿÿÿƒÄégA  èüÿÿÿUW‰ÇÁïVS‰Ã‹½   …Àu¸é  P¾   ½   Shñ  èüÿÿÿƒÄ‰è‰ñÓà…Øtÿ4µ  hÿ  èüÿÿÿXZNƒşuİãÿ  Sh  èüÿÿÿ‹½€  ZY…Àu¸   [^_]ÃPÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  Pÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  ÿ2hx  hx  hL  èüÿÿÿƒÄédK  ‰$ÿ2hx  hx  hx  èüÿÿÿ‹L$ƒÄéTI  ‰$‹L$‹AèPVhx  hx  hÔ  èüÿÿÿ1Ò‹L$ƒÄéÙI  ÿt$ÿt$ÿ7ÿ³h  hx  hü  èüÿÿÿƒÄéBK  èüÿÿÿU1íW‰ÇV°¼  S‰Óƒìÿ2Vh8  èüÿÿÿƒÄ‹Cèƒàü‡Ü   ‹ PƒÅÿµ<  Vhh  èüÿÿÿƒÄƒı8uÓ1í‹CD¨8ƒàü‡Ü   ‹‰$UEVh  èüÿÿÿ‹D$èşÿÿƒÄƒıuÍ‰4$‰ù1í‰ß‹G­    ‰L$ØƒàüÜ   ‹0U_S»   ÿt$h  èüÿÿÿ‰ğuèÌıÿÿ‰Â‰l$‹l$ƒÄ‡×9ûs1‹B‰T$°ƒàü…Ü   ‹ SCPVFÿt$h´  èüÿÿÿ‹T$ƒÄëË‰é‹l$‡úÕƒı†lÿÿÿ‰û‹4$‰Ï‹C‹SÂRPVhà  èüÿÿÿ‹CP`RPVh  èüÿÿÿ‹Cƒàü‡Ü   ‹ ƒÄ PVh0  èüÿÿÿ‹Cƒàü‡Ü   ‹ PVhT  èüÿÿÿ‹C ƒàü‡Ü   ‹ PVhx  èüÿÿÿ‹C$ƒàü‡Ü   ‹ ƒÄ$PVhœ  èüÿÿÿƒÄ[^_]Ã÷Ã   t ƒ=„  vh   h¨  hÀ  èüÿÿÿƒÄ÷Ã   t$ƒ=„  †Ç   h   h¨  hğ  èüÿÿÿƒÄ‰İå   öÇt ƒ=„  v;h   h¨  h   èüÿÿÿƒÄ…ít ƒ=„  vh   h¨  hP  èüÿÿÿƒÄ‡¼  Ph€  èüÿÿÿ‹†À   ƒàü‡Ü   ‹‹–ô   ÷Ò!Ê‰kV,‰ø—d  èüÿÿÿXZé)L  jh¨  h¨  èüÿÿÿƒÄéùK  ‰İå   öÇu”ékÿÿÿhd  hd  h”  èüÿÿÿƒÄé‹Q  kV,‰Ø“d  èüÿÿÿ‹¼  ‰$ÿ·  ÿ·ˆ  Ph   èüÿÿÿ1À‰\$ƒÄ‰t$‰Æ‹‡„  Áè9Æƒ¹   V»   ÿt$h.  èüÿÿÿµ    ‰D$‹‡ˆ  ‹°èûÿÿVƒÄ‰t$‰Ö‰l$‰ı‰Ç‹…ˆ  9ß†e  SƒÃÿ4°VƒÆÿt$h0  èüÿÿÿƒÄëÔÿ³ô  ÿ³ğ  ÿ³ì  hd  hd  hÔ  èüÿÿÿƒÄénO  PRhd  h\  èüÿÿÿ¸êÿÿÿƒÄéiR  ‹\$‹t$é½O  ‹.hL  hL  h˜  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü      ‹ PhL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‰èè¬A  PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   P  ‹ PhL  hL  h4  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @  ‹ PhL  hL  hl  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   `  ‹ PhL  hL  h¤  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @ ‹ PhL  hL  hÜ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   L  ‹ PhL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ,  ‹ PhL  hL  hL  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   d  ‹ PhL  hL  h„  èüÿÿÿƒÄƒ=„   „CR  ‹†¸   ƒàü…Ü   ‹ Pÿ¶¸   hL  hL  h¼  èüÿÿÿƒÄƒ=„   „CR  ‹†¼   ƒàü…Ü   ‹ Pÿ¶¼   hL  hL  hğ  èüÿÿÿƒÄƒ=„   „CR  ‹†À   ƒàü…Ü   ‹ Pÿ¶À   hL  hL  h$  èüÿÿÿ‹†ì   ‹„  ƒÄ…À…~  …Ò„CR  ‹†Ä   ƒàü…Ü   ‹ Pÿ¶Ä   hL  hL  hŒ  èüÿÿÿƒÄƒ=„   „CR  ‹†È   ƒàü…Ü   ‹ Pÿ¶È   hL  hL  hÀ  èüÿÿÿƒÄƒ=„   „CR  ‹†Ì   ƒàü…Ü   ‹ Pÿ¶Ì   hL  hL  hô  èüÿÿÿƒÄƒ=„   „CR  ‹†Ğ   ƒàü…Ü   ‹ Pÿ¶Ğ   hL  hL  h(  èüÿÿÿƒÄƒ=„   „CR  ‹†Ô   ƒàü…Ü   ‹ Pÿ¶Ô   hL  hL  h\  èüÿÿÿƒÄƒ=„   „CR  ‹†Ø   ƒàü…Ü   ‹ Pÿ¶Ø   hL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹†Ü   ƒàü…Ü   ‹ Pÿ¶Ü   hL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‹†à   ƒàü…Ü   ‹ Pÿ¶à   hL  hL  hø  èüÿÿÿƒÄƒ=„   „CR  ‹†ä   ƒàü…Ü   ‹ Pÿ¶ä   hL  hL  h,  èüÿÿÿƒÄƒ=„   „CR  ‹†è   ƒàü…Ü   ‹ Pÿ¶è   hL  hL  h`  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü     ‹ PhL  hL  h”  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   8  ‹ PhL  hL  hÈ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   <  ‹ PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   H  ‹ PhL  hL  h0  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ‹€L  PhL  hL  hd  èüÿÿÿƒÄéCR  ‹\$‰ù‹t$‰ï‹l$<   pu‹\$‹t$é½O  Îéùÿÿ…Ò„CR  ƒàü…Ü   ‹ Pÿ¶ì   hL  hL  hX  èüÿÿÿ‹„  ƒÄéFüÿÿ‰$Sh8  h8  hF  èüÿÿÿ‹T$ƒÄéòR  ‰$h$  ÿsSh$  h¼  èüÿÿÿ‹T$ƒÄéT  ‰$h$  ÿsSh$  hè  èüÿÿÿ‹T$ƒÄéT  h  h  h]  èüÿÿÿ‹7ƒÄƒ=„   „zT  hø  hø  h]  èüÿÿÿƒÄézT  ÿvhr  ÿ¶ŒıÿÿUhä  h  èüÿÿÿƒÄéxU  ƒ¼  VPh€  èüÿÿÿƒÄ÷Æ   tƒ=„   thÔ  h¼  èüÿÿÿ]X÷Æ   tƒ=„   thÔ  hè  èüÿÿÿY_÷Æ   tƒ=„   „ô   hÔ  h  èüÿÿÿXZ‰÷ç    ÷Æ   tƒ=„   t/hÔ  hD  èüÿÿÿY]…ÿtƒ=„   thÔ  hl  èüÿÿÿXZ‰÷ç   ÷Æ   tƒ=„   t/hÔ  h˜  èüÿÿÿ]X…ÿtƒ=„   thÔ  hÀ  èüÿÿÿY_‹ƒœ  ƒàüƒÜ   ‹‹“Ğ  ÷Ò!Ê‰k“ä  ,‰Ø“d  èüÿÿÿ‰Øèüÿÿÿé}V  hÔ  hì  èüÿÿÿXZé\V  ‰÷ç    ÷Æ   …Qÿÿÿé.ÿÿÿ‰L$h   h¸  h   èüÿÿÿ‹L$$ƒÄéÿX  ‰L$jh¸  hÄ  èüÿÿÿ‹L$$ƒÄé°X  ‰L$h  € h¸  h  èüÿÿÿ‹L$$ƒÄéÑX  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄévX  ‰L$h   h¸  h¼  èüÿÿÿ‹L$$ƒÄéLX  ‰L$h   h¸  hè  èüÿÿÿ‹L$$ƒÄéaX  ‰L$h   h¸  hd  èüÿÿÿ‹L$$ƒÄé"X  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄé7X  ‰L$jh¸  h@  èüÿÿÿ‹L$$ƒÄé‚X  ‰L$h   h¸  ht  èüÿÿÿ‹L$$ƒÄéì[  ‰L$h   h¸  hH  èüÿÿÿ‹L$$ƒÄéæX  ‰L$jh¸  hğ  èüÿÿÿ‹L$$ƒÄéÄ[  ‰L$jh¸  h˜  èüÿÿÿ‹L$$ƒÄéu[  ‰L$jh¸  hl  èüÿÿÿ‹L$$ƒÄé—X  VUh¸  h  èüÿÿÿƒÄéZ  ‰T$0‰L$,VUh¸  h  èüÿÿÿ‹L$<‹T$@ƒÄƒ=„  †öW  Rÿt$ÿt$h¸  hP  èüÿÿÿ‹L$@ƒÄƒ=„  †öW  ‰L$ÿt$ ÿt$ÿt$h¸  h”  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$$ÿt$Q‰L$h¸  hÜ  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$(ÿt$ ÿt$h¸  h   èüÿÿÿ‹L$ ƒÄéöW  †¼  PhÌ  èüÿÿÿYXé¬\  Æ¼  Vhy  èüÿÿÿXZéÒ\  †¼  Phü  èüÿÿÿYXé.]  Æ¼  Vhy  èüÿÿÿXZéV]  †¼  Phy  èüÿÿÿX1ÀZé]  †¼  Ph4  èüÿÿÿYXé°]  †¼  Phl  èüÿÿÿXZéK^  jh€  h€  h×  èüÿÿÿƒÄé³d  Ph”  h”  hœ  èüÿÿÿƒÄéû`  hl  hl  h]  èüÿÿÿƒÄ»h  u  u‹ƒÜ    ‹ƒâï‰‰ØèLE  ‹ƒ¸  iĞ  ‹²   ‰³€  ¶Š    ˆ‹„  ˆL$‹Š$   ‰L$‰‹ˆ  ‹Š   Â    ƒ=„   ‰‹Œ  ¶R!ˆ“  tVÿt$¶D$PVh”  h”  h”  èüÿÿÿƒÄƒ=„   t%¶ƒ  Pÿ³Œ  h”  h”  hÜ  èüÿÿÿƒÄ‹ƒ¸  iÀ  ƒ¸   …(  ‰Øèüÿÿÿƒ=„   th\  h\  h]  èüÿÿÿƒÄ‰Øè¼F  ‹ƒÜ     ƒÈÿ‰‹‹Ü   ‰$  ‹‹Ü   ‰4  ‹‹Ü   ‰D  ‹‹Ü   ‰T  ‹‹Ü   ‰d  ‹ƒÜ   H  ‹â   €‰º P ‹ƒÜ   ‰L  ‰ØèLE  ¸d   èüÿÿÿ‹“d  ¹@  ‰Øj èüÿÿÿ‹ƒd  ¹€   j P,‰Øèüÿÿÿ‹ƒd  ¹ğ  j PX‰Øèüÿÿÿ‹ƒd  ¹€   j „   ‰Øèüÿÿÿ‹ƒd  ¹€   j °   ‰Øèüÿÿÿ‹ƒd  ¹ğ  j Ü   ‰Øèüÿÿÿ‹ƒd  ¹€   j   ‰Øèüÿÿÿ‹ƒd  ¹€   j 4  ‰Øèüÿÿÿ‹ƒd  ¹€   ƒÄ j `  ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øè¬A  ‰ØèLE  ƒô   èüÿÿÿƒ|  èüÿÿÿƒ
  èüÿÿÿ‰Øèüÿÿÿ‹Cp´XC‰D$FL;D$„Ÿ   ‹Fd‹@…Àt‹…Òt‰ğèüÿÿÿ‹vLƒîLëÔjh€  h€  h×  èüÿÿÿƒÄéÿe  ƒ¸   „ËıÿÿéÍıÿÿ‹ƒÌ   ¾êÿÿÿÿ°T  ÿ°P  ‹|$WhÜ  èüÿÿÿ‹ƒÌ   ·P&R·@$PWh  èüÿÿÿƒ-€  ƒÄ éx_  ‰Øèüÿÿÿƒ»¸  7u1Éº   ‰Øèüÿÿÿ‹ƒ¸  iĞ  ƒº   u"‰Øèüÿÿÿ…Àyh”  h  èüÿÿÿY^‹ƒ¸  iĞ  ‹Š   Â    ƒù…ù  ‹B…Àt‰ƒì  ƒÜ  èüÿÿÿ…À‰ş  h”  h\  èüÿÿÿX‹ƒ¸  ZiÀ  ‹       ƒú…Ø  ‹@…Àt‰ƒ0  ƒ   èüÿÿÿ…Àyh”  hÌ  èüÿÿÿXZ‹ƒÜ   ‹  ¶Â€ú„Á  €âğuCƒø„Ü  ƒøt5ƒø„ì  w*ƒø„’  ƒøtƒø„¢  w…À„¢  ƒè„£  ÿ³ì   h@  h4  èüÿÿÿ‹ƒì   ƒÄ…À„c  Ph@  hh  èüÿÿÿƒÄ‹ƒ¸  iĞ  ƒº   ~‹ƒÜ    ‹€æş‰‹ƒ¸  ƒø„:  ƒø#„1  ¶G,‹Ñ   º   ˆƒĞ   ‰øèüÿÿÿ¶ƒÑ   ‹—P  ‹T  ¶³Ğ   ‰D$‹‡L  ‰D$‹‡´   …À„  QRÿt$ÿt$ VPÿt$(hÄ  èüÿÿÿ‰øƒÄ èüÿÿÿ‹D$ƒÊÿ1Éèüÿÿÿ‰Æ…À„à   ÿt$h   èüÿÿÿXZéx_  ƒù…5şÿÿ‰Øèüÿÿÿ…Àyh”  h˜  èüÿÿÿY^‹ƒ¸  éşÿÿƒú…Lşÿÿ‰Øèüÿÿÿ…À‰=şÿÿh”  h   èüÿÿÿY^é'şÿÿ¸±   ‰ƒì   éşÿÿj h@  h”  èüÿÿÿƒÄé—şÿÿ¸À   ëÕ‹ƒÜ    ‹€æş‰é¸şÿÿ‹ƒÌ   ºĞ   fx"€ˆ¸¥   DÂë£‹‡ˆ   éäşÿÿ‹‡L  S1ÉºĞV  ÿt$h€   èüÿÿÿƒÄ…Ày+‰D$ÿ·L  ÿt$h@   èüÿÿÿ‰ØèœC  ‹t$ƒÄéx_  ‹ƒ¸  ƒøtƒøuº  € ‰Øèüÿÿÿëº  €‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿé_  ‹ƒÌ   ºÀ   fx"€ˆ¸¤   DÂéïşÿÿ¸°   éåşÿÿ¸    éÛşÿÿ¸¡   éÑşÿÿh°	  h°	  h¸   èüÿÿÿ1ÀƒÄéØh  ‰$…ÉtXh°	  hk  èüÿÿÿ·EZY‹$f…À…0h  1ÀéØh  ·E‰L$‰$Ph°	  h°	  h”   èüÿÿÿ‹T$‹L$ƒÄéh  ·E ÀƒÀPh°	  h  èüÿÿÿ·E‹T$ƒÄëš‹B‰$¶0Ph°	  h  èüÿÿÿ‹T$ƒÄ€ã…`h  ƒ=Œ   „`h  h°	  h¶  èüÿÿÿY[‹$é`h  jûhÍ  èüÿÿÿX¸ûÿÿÿZéØh  h 	  h 	  h¸   èüÿÿÿ1ÀƒÄé?k  jûhÍ  èüÿÿÿX¸ûÿÿÿZé?k  ‰$ÿt$Qh 	  h 	  hÜ   èüÿÿÿ·M‹T$ƒÄé¢i  ‰$Qh 	  h 	  h”   èüÿÿÿ·M‹T$ƒÄé¢i  ‹E‰T$¶ P·E ÀPhâ  èüÿÿÿ‹T$ƒÄö$…9j  ‰$hò  èüÿÿÿX‹$é9j  ‹B‰$¶8Phø  èüÿÿÿ€ãXZ‹$…¾j  hò  èüÿÿÿ[‹$é¾j  QhÀ	  hÀ	  h!  èüÿÿÿƒÄéÜk  ·B‰L$P·‰T$PVhÀ	  hÀ	  h8!  èüÿÿÿ‹T$‹L$ƒÄé5l  ¾¼  ÿs1öWh	  èüÿÿÿƒÄj‹D$1Éf‰³z  T$èüÿÿÿY…ÀyEƒÆş€   uØ‹ƒl  …À…ãm  é©m  ¾¼  ÿsWhĞ	  h˜!  èüÿÿÿƒÄƒ=ˆ   tÈë‹‹µ 
  …ÀtPVWhÈ!  èüÿÿÿƒÄë¸ 	  ëçÆ¼  ÿsVh "  èüÿÿÿƒÄë‰¸	  èüÿÿÿÿshĞ	  hĞ	  ht!  èüÿÿÿ‹ƒÄé·l  ¸˜	  èüÿÿÿR‰T$h€	  h€	  h,"  èüÿÿÿ‹T$ƒÄéQn  hˆ  h 	  èüÿÿÿXZé­o  hp  h 	  èüÿÿÿXZéıo  hX  h 	  èüÿÿÿXZé-p  h@  h 	  èüÿÿÿXZé}p  èüÿÿÿUº   W¿êÿÿÿVS‰ÃìL  ‹3d¡    ‰„$H  1ÀCèüÿÿÿ…À„g  ‹–¸  ƒú„Ô   wVƒútzƒú…½   †ü   º  èüÿÿÿ‹SK@BÿkÀ‹°  ‰s@f‹€”  f‰CDQRhx"  èüÿÿÿ‰ØèüÿÿÿƒÄëuƒú0wƒú-‡  „”   ë_ƒê11ÿƒú†„   é×  ƒ{uF¹   T$H†l  fÇ†n  P èüÿÿÿ„$è   Ph°"  èüÿÿÿ‹„$ğ   ‰C@‹„$ô   f‰CDY[1ÿé„  S(€  ‰4$1ÿ‰T$‹S‰D$‰àÇD$u  èüÿÿÿéX  ƒ{wÇ¹   T$H†l  fÇ†n  P èüÿÿÿ‹C”Ä   RPi†¸    ÿ°    hĞ"  èüÿÿÿ‹C„Ä  ƒÄ‹f‹@‰S@f‰CDéeÿÿÿl$C(¹   ‰\$‰$‰ï1Àº·	  ó«¹   ‰è¿íÿÿÿÇD$s  èüÿÿÿfÇD$&@ ‰d$,Ujèüÿÿÿ‰ê†ü   èüÿÿÿ‰ÅXZ…í„   ı ğÿÿ‡   ‹ET…Àtz‹@èüÿÿÿ„Àu	‰èèüÿÿÿëe1ÿƒ{‰«8  uW¹   T$H†l  fÇ†n  P èüÿÿÿ„$  Pi†¸    ÿ°    hø"  èüÿÿÿ‹„$  ‰C@‹„$  ƒÄf‰CD‹„$H  d+    tèüÿÿÿÄL  ‰ø[^_]ÃèüÿÿÿUº   WVSì  ‰D$d¡    ‰„$   ‹D$ÇD$    ÇD$    ‹ ‰$‹D$ƒÀ‰D$èüÿÿÿ‰Å¸êÿÿÿ…í„›A  ‹$¼  ‰E‰D$‹D$Ç€”       Ç€  p  ‹$‹€¸  ƒø„ğ  ‡â   ƒøwvƒø‡Ü  ƒø„
  w9ƒø„ó  wƒø„ì  ƒø„…  é<  ƒø„Ã  ƒø„­  éş;  ƒø
„#  ƒø„=  ƒø†×
  ƒø	„Y  éÕ;  ƒøw2ƒø‡  ƒø„õ  ƒø„X  ƒø„t  ƒø„í  é;  ƒø„  wƒø„ì  ƒø„;  é|;  ƒø„û  éø  ƒø)w}ƒø'‡¸  ƒø!„İ  w0ƒø„t  wƒø„Ï  ƒø„†  é3;  ƒø„å  é%;  ƒø$„~  wƒø"„”  ƒø#„g  é;  ƒø%„D   ƒø&„#!  éì:  ƒø2„û*  wBƒø/„¯)  w ƒø-„R%  ƒø.„/(  ƒø,„  é¶:  ƒø0„(  ƒø1„)%  éŸ:  ƒø5„˜#  wƒø3„ª,  ƒø4„$.  é}:  ƒø:„9  wƒø8„Ã1  ƒø9…`:  é˜5  ƒø;„ô-  ƒø<„£1  ƒø=…@:  éx5  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét7‹$°ü   ¸V  ‰òèüÿÿÿ‰Ã…À…3  ¸»	  1öèüÿÿÿÇE    éê9  h#  1ö1ÛèüÿÿÿÇE    XéÏ9  ¸
  èüÿÿÿ‰Á…Àuh
  jèüÿÿÿ¸
  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ÀS  èüÿÿÿ‰Ã…Àu3¸
  1öèüÿÿÿÇE    ép9  hx#  1ö1ÛèüÿÿÿÇE    XéU9  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…Àt,‹$º`   ˆ„  ‹EhäS  èüÿÿÿ[…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿY‹<$‹Eƒ¿¸  t	1ö1ÛéÕ8  ‹Ô   ‹|$1ö1Û‰—<  Ç€Ô   s  é²8  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸pU  èüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    éS8  hØ#  1ö1ÛèüÿÿÿÇE    Xé88  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ_‰ÃX…Ût,‹$º`   ˆ„  ‹EhT  èüÿÿÿ^…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿ[‹$hT  º`   1ö1Ûˆ„  ø  èüÿÿÿYé©7  ƒ=   ¸»	  …Ñ  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ÔU  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    é=7  h#  1ö1ÛèüÿÿÿÇE    _é"7  ‰E¸v
  èüÿÿÿ‰Ã…Àuh…
  jèüÿÿÿ¸v
  èüÿÿÿY^‰Ã…Àt,‹$¹B   „  ‹Eh<U  èüÿÿÿZ…Àu¸v
  èüÿÿÿëh$  èüÿÿÿX¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…À„®   ‹$ˆ„  ‹EhT  º`   èüÿÿÿ[…À…†ıÿÿ¸*
  1ö1ÛèüÿÿÿéQ6  ‰E¸v
  èüÿÿÿ‰Ã…À„˜  ‹$¹B   „  ‹Eh<U  èüÿÿÿZ…Àu
¸v
  èüÿÿÿ¸*
  èüÿÿÿ‰Ã…À…Œ  h:
  jèüÿÿÿ¸*
  èüÿÿÿY^‰Ã…À…j  h¨#  1ö1ÛèüÿÿÿYéÆ5  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét7‹$°ü   ¸V  ‰òèüÿÿÿ‰Ã…À…¯   ¸»	  1öèüÿÿÿÇE    éf5  h#  1ö1ÛèüÿÿÿÇE    _éK5  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ[^‰Á…Àt3‹$°ü   ¸ÈU  ‰òèüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    éê4  h#  1ö1ÛèüÿÿÿÇE    YéÏ4  ‰]¸à	  èüÿÿÿ‰Ã…Àuhî	  jèüÿÿÿ¸à	  èüÿÿÿ_‰ÃX…Ût-‹E¹¸U  j ‰òèüÿÿÿ[…À…¦ûÿÿ¸à	  1ö1Ûèüÿÿÿéq4  hH#  1ö1ÛèüÿÿÿYé]4  ¸›
  èüÿÿÿ‰Ã…Àuh«
  jèüÿÿÿ¸›
  èüÿÿÿ‰ÃXZ…Ût8‹$º   °ü   ¸ U  ‰ñèüÿÿÿ‰Ã…Àu3¸›
  1öèüÿÿÿÇE    é÷3  h8$  1ö1ÛèüÿÿÿÇE    XéÜ3  ‰E¸Â
  èüÿÿÿ‰Ã…ÀuhÖ
  jèüÿÿÿ¸Â
  èüÿÿÿ_‰ÃX…Ût-‹E‰òj@¹a   èüÿÿÿ^‰Ã…À…±úÿÿ¸Â
  1öèüÿÿÿé~3  hh$  1ö1ÛèüÿÿÿYéj3  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸”U  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    é3  h#  1ö1ÛèüÿÿÿÇE    Xéğ2  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àtj‹$¹\U  „  ‹Eèüÿÿÿ…À…Ãùÿÿ¸ñ
  1ö1Ûèüÿÿÿé2  ‰E¸ñ
  èüÿÿÿ‰Ã…À…Š  hÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ‰ÃXZ…Û…h  hœ$  1ö1ÛèüÿÿÿYéA2  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸¼U  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    éâ1  h#  1ö1ÛèüÿÿÿÇE    _éÇ1  ‰E‹$Ç„$Œ       „  Æ„$Œ   a‰„$ˆ   ¸  Ç„$       èüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿYë‹ˆØ  …Ét
ºÔN  èüÿÿÿ1ö1Ûé 1  ¸7  èüÿÿÿ‰Á…ÀuhG  jèüÿÿÿ¸7  èüÿÿÿ‰ÁXZ…Ét5‹$ü   ¸ôU  èüÿÿÿ‰Ã…À…„úÿÿ¸7  1öèüÿÿÿÇE    é½0  hü$  1ö1ÛèüÿÿÿÇE    _é¢0  h…
  jèüÿÿÿ¸v
  èüÿÿÿY^‰Ã…À…Fúÿÿh$  èüÿÿÿ_é`úÿÿ‹$ˆ„  ‹EhtT  éãùÿÿ¸7  èüÿÿÿ‰Á…ÀuhG  jèüÿÿÿ¸7  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸àU  èüÿÿÿ‰Ã…Àu3¸7  1öèüÿÿÿÇE    éû/  hü$  1ö1ÛèüÿÿÿÇE    Xéà/  ‰E¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…À„Ñùÿÿ‹$ˆ„  ‹EhXT  éùÿÿ¸^  èüÿÿÿ‰Â…Àuhn  jèüÿÿÿ¸^  èüÿÿÿ‰ÂXY…Òtl„$ˆ   èüÿÿÿ…À…Ì0  ¸^  èüÿÿÿ¸íÿÿÿé74  ‹D$‹@‰D$Xÿ¸^  èüÿÿÿ‰Â…À…Š  hn  jèüÿÿÿ¸^  èüÿÿÿ^_‰Â…À…h  h,%  èüÿÿÿ¸íÿÿÿ_éŞ3  ‹D$‹@‰D$pÿ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét.iÆˆ  ‹<$”ü   ¸ˆU  èüÿÿÿ…À…úûÿÿ¸»	  èüÿÿÿëh#  èüÿÿÿXÇE    ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ[_‰Á…Àt8iÆˆ  ‹<$”ü   ¸|U  èüÿÿÿ…À…Œûÿÿ¸Q
  èüÿÿÿÇE    é&õÿÿhØ#  èüÿÿÿÇE    Yéõÿÿiöˆ  ‹$¹HU  ”0ü   ‹Eèüÿÿÿ…À…êôÿÿé"ûÿÿ‹D$‹@‰D$Xÿ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ^_‰Á…Àt:iÛˆ  ‹$´ü   ¸S  ‰òèüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    éM-  h\%  1ö1ÛèüÿÿÿÇE    Yé2-  ‰E¸  Ç„$Œ       Ç„$       ‰´$ˆ   Æ„$Œ   aèüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿXë‹ˆØ  …Ét
ºœN  èüÿÿÿ1ö1Ûé“,  „$ˆ   èüÿÿÿ…À…a0  é'ıÿÿ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ[^‰Á…Àt1‹$ü   ¸S  èüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    é,  h\%  1ö1ÛèüÿÿÿÇE    Yé ,  ‰E‹$Ç„$Œ       „  Æ„$Œ   a‰„$ˆ   ¸  Ç„$       èüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿ_ë‹ˆØ  …Ét
º€N  èüÿÿÿ1ö1ÛéY+  ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ[^‰Á…Àt1‹$ü   ¸S  èüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    éú*  h\%  1ö1ÛèüÿÿÿÇE    Yéß*  ‰E¸Ï  Ç„$ˆ   a †Ç„$Œ   Ğ  èüÿÿÿ‰Ã…Àuhİ  jèüÿÿÿ¸Ï  èüÿÿÿ‰ÃXZ…Ût,‹$Œ$ˆ   „  ‹Eèüÿÿÿ…À…;.  ¸Ï  èüÿÿÿëhè%  èüÿÿÿXÿt$h&  èüÿÿÿ_]é+  ¸ò  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ò  èüÿÿÿ[^‰Á…Àt1‹$„  ¸R  èüÿÿÿ‰Ã…Àu3¸ò  1öèüÿÿÿÇE    éí)  h@&  1ö1ÛèüÿÿÿÇE    YéÒ)  ‰E1ö1ÛÇ€      é¼)  ‹D$‹@ƒøtƒøtu1ö1Ûé¢)  ¸  èüÿÿÿ‰Á…Àuh&  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét%‹$„  ¸ŒR  èüÿÿÿ‰Ã…Àu¸  èüÿÿÿëhp&  1Ûèüÿÿÿ_‰]1ö1Ûé6)  ¸  èüÿÿÿ‰Á…Àuh&  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt%‹$„  ¸ŒR  èüÿÿÿ‰Ã…Àu¸  èüÿÿÿëhp&  1ÛèüÿÿÿY‰]1ö1ÛéÊ(  ¸<  èüÿÿÿ‰Á…ÀuhJ  jèüÿÿÿ¸<  èüÿÿÿ‰ÁXZ…Ét3‹$°„  ¸€R  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éi(  h &  1ö1ÛèüÿÿÿÇE    XéN(  ‰E¸_  èüÿÿÿ‰Ã…Àuhm  jèüÿÿÿ¸_  èüÿÿÿ_‰ÃX…Ût‹E‰ñºhR  èüÿÿÿ…Àu¸_  èüÿÿÿëhĞ&  èüÿÿÿY‹E1ö1ÛÇ€      éİ'  ¸ò  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ò  èüÿÿÿ‰ÁXZ…Ét%‹$„  ¸XR  èüÿÿÿ‰Ã…Àu¸ò  èüÿÿÿëh@&  1ÛèüÿÿÿX‰]1ö1Ûéq'  ‹D$‹@ƒøtƒø„+  é¬ıÿÿ¸‚  èüÿÿÿ‰Ã…Àuh‘  jèüÿÿÿ¸‚  èüÿÿÿ^_‰Ã…Àt5‹$1É°ü   ¸ÀR  ‰òèüÿÿÿ‰Ã…Àu3¸‚  1öèüÿÿÿÇE    éô&  h '  1ö1ÛèüÿÿÿÇE    YéÙ&  ‰E¸§  èüÿÿÿ‰Ã…Àuh¶  jèüÿÿÿ¸§  èüÿÿÿ‰ÃXZ…Ûtc‹E‰ñº¬R  èüÿÿÿ…À…È(  ¸§  1ö1Ûèüÿÿÿé~&  ‰E¸§  èüÿÿÿ‰Ã…À…»   h¶  jèüÿÿÿ¸§  èüÿÿÿ‰ÃXZ…Û…™   h0'  1ö1ÛèüÿÿÿXé1&  ¸‚  èüÿÿÿ‰Ã…Àuh‘  jèüÿÿÿ¸‚  èüÿÿÿ^_‰Ã…Àt<‹$¹   °ü   ¸ÀR  ‰òèüÿÿÿ‰Ã…À…aÿÿÿ¸‚  1öèüÿÿÿÇE    éÇ%  h '  1ö1ÛèüÿÿÿÇE    Yé¬%  ‹E‰ñº R  èüÿÿÿ…À…#(  éÿşÿÿ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸@R  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é1%  h'  1ö1ÛèüÿÿÿÇE    Xé%  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹,R  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  é$  ¸1  èüÿÿÿ‰Á…ÀuhA  jèüÿÿÿ¸1  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸$R  èüÿÿÿ‰Ã…Àu3¸1  1öèüÿÿÿÇE    é/$  hÀ'  1ö1ÛèüÿÿÿÇE    Xé$  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹R  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  éŒ#  ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸Q  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é-#  h'  1ö1ÛèüÿÿÿÇE    Xé#  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹ Q  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  éŠ"  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸pU  ‰òèüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    é)"  hØ#  1ö1ÛèüÿÿÿÇE    Xé"  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ_‰ÃX…Ût%‹E‰ñhT  º`   èüÿÿÿ^…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿ[‹$hT  º`   1ö1Ûˆ„  ø  èüÿÿÿYé†!  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸pU  ‰òèüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    é%!  hØ#  1ö1ÛèüÿÿÿÇE    Xé
!  ‰E¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿY_‰Ã…À„ûêÿÿ‹E‰ñhT  éOêÿÿ‹D$‹@ƒøtƒø„)  é÷ÿÿ¸X  èüÿÿÿ‰Á…Àuhh  jèüÿÿÿ¸X  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸ìP  ‰òèüÿÿÿ‰Ã…Àu3¸X  1öèüÿÿÿÇE    éK   hğ'  1ö1ÛèüÿÿÿÇE    Xé0   ‰E¸  èüÿÿÿ‰Ã…Àuh  jèüÿÿÿ¸  èüÿÿÿ_‰ÃX…Ûtc‹E¹èP  ‰òèüÿÿÿ…À…
çÿÿ¸  1ö1ÛèüÿÿÿéÕ  ‰E¸  èüÿÿÿ‰Ã…À…¶   h  jèüÿÿÿ¸  èüÿÿÿY_‰Ã…À…”   h (  1ö1ÛèüÿÿÿYéˆ  ¸X  èüÿÿÿ‰Á…Àuhh  jèüÿÿÿ¸X  èüÿÿÿ‰ÁXZ…Ét7‹$°„  ¸ÔP  ‰òèüÿÿÿ‰Ã…À…fÿÿÿ¸X  1öèüÿÿÿÇE    é#  hğ'  1ö1ÛèüÿÿÿÇE    Xé  ‹E¹ĞP  ‰òèüÿÿÿ…À…æÿÿéÿÿÿ‹D$ƒx‡  ¸¤  èüÿÿÿ‰Á…Àuh¶  jèüÿÿÿ¸¤  èüÿÿÿ‰ÁXZ…ÉtE‹$¸ü   ‹D$‰ú‹@‰D$HkÀ P  èüÿÿÿ‰Ã…Àu9¸¤  ¾   èüÿÿÿÇE    ék  hP(  ¾   1ÛèüÿÿÿÇE    XéM  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^‰ÃX…Ût{‹D$‰ú‹@Hÿ‰D$‹EkÉÁ`P  èüÿÿÿ…À…Õ   ¸ñ
  èüÿÿÿé²  ‰CÇ€     ¸ñ
  èüÿÿÿ‰Æ…À…S!  hÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿY‰ÆX…ö…1!  hœ$  èüÿÿÿYé_  ‹D$‹@ƒøtƒø„&  éÌóÿÿ¸ú  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ú  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ P  èüÿÿÿ‰Ã…Àu3¸ú  1öèüÿÿÿÇE    é  h¸(  1ö1ÛèüÿÿÿÇE    Xéı  ‰E¸  èüÿÿÿ‰Ã…Àuh'  jèüÿÿÿ¸  èüÿÿÿ^_‰Ã…Àtf‹$º P  ˆ„  ‹Eèüÿÿÿ…À…Ğãÿÿ¸  èüÿÿÿém  ‰E¸  èüÿÿÿ‰Ã…À…°   h'  jèüÿÿÿ¸  èüÿÿÿ_‰ÃX…Û…   hä(  èüÿÿÿYé$  ¸ú  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ú  èüÿÿÿ‰ÁXZ…Ét5‹$ü   ¸8P  èüÿÿÿ‰Ã…À…lÿÿÿ¸ú  1öèüÿÿÿÇE    éó  h¸(  1ö1ÛèüÿÿÿÇE    XéØ  ‹$ºP  ˆ„  ‹Eèüÿÿÿ…À…Üâÿÿéÿÿÿ¸<  èüÿÿÿ‰Á…ÀuhJ  jèüÿÿÿ¸<  èüÿÿÿ[^‰Á…Àt3‹$°„  ¸€R  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éT  h &  1ö1ÛèüÿÿÿÇE    Yé9  ‰E¸_  èüÿÿÿ‰Ã…Àuhm  jèüÿÿÿ¸_  èüÿÿÿ‰ÃXZ…Ût*‹E‰ñºhR  èüÿÿÿ‰Ã…À…âÿÿ¸_  1öèüÿÿÿéŞ  hĞ&  1ö1ÛèüÿÿÿXéÊ  ¸<  èüÿÿÿ‰Ã…ÀuhK  jèüÿÿÿ¸<  èüÿÿÿ^_‰Ã…Àt8‹$¹   °ü   ¸ Q  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éd  h)  1ö1ÛèüÿÿÿÇE    YéI  ‰E¸a  èüÿÿÿ‰Ã…Àuhp  jèüÿÿÿ¸a  èüÿÿÿ‰ÃXZ…Ût&‹E‰ñº€Q  èüÿÿÿ…À…Í  ¸a  èüÿÿÿéÀ  hD)  èüÿÿÿXé°  ‹D$Ç„$ˆ   ` iÇ„$Œ   @   ‹@ÇD$d    Ç„$   T  Ç„$”       ƒøtƒø„J  éy  œ$È   1À¹   º†  ‰ßó«¹   ‰Øèüÿÿÿ„$ˆ   fÇ„$Ş    ‰„$ä   h—  jèüÿÿÿ‹D$‰Ú°ü   ‰ğèüÿÿÿY‰ÇX…ÿ„ü  ÿ ğÿÿ‡ğ  ‹GT…À„å  ‹@èüÿÿÿ„À‰øu
èüÿÿÿéÍ  ‹”$”   èüÿÿÿ¹   º   ‰E‹D$‰¸,  ‹E‰ß‰D$d1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş    D$d‰„$ä   h   jèüÿÿÿ‰Ú‰ğèüÿÿÿ‰ÃXZ…Û„[  û ğÿÿ‡O  ‹CT…À„D  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé,  ‹D$‰˜4  é4  T$@1Àt$<¹   œ$È   ‰×Uó«‰ß¹   ‰T$<ó«º¦  ‰Ø¹   ÆD$@!ÇD$D $ô èüÿÿÿfÇ„$Ş   d ‰´$ä   Sjèüÿÿÿ‹D$‰Úü   èüÿÿÿ_‰ÃX…Û„   û ğÿÿ‡”  ‹CT…À„‰  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿéq  ‹D$‰˜,  ‹E…ÀtwÇ€      ¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿY^‰Ã…Àt/‹$º`   ˆ„  ‹Eh<T  èüÿÿÿZ…Àu¸*
  èüÿÿÿéù  h¨#  èüÿÿÿ]éé  1ö1Ûé  œ$È   1À¹   º†  ‰ßÇ„$ˆ   ` ió«¹   ‰ØÇ„$Œ   @   Ç„$   T  Ç„$”       ÇD$d    èüÿÿÿ„$ˆ   fÇ„$Ş    ‰„$ä   h—  jèüÿÿÿ‹D$‰Ú°ü   ‰ğèüÿÿÿY_‰Ã…À„F  = ğÿÿ‡;  ‹@T…À„0  ‹@èüÿÿÿ„À‰Øu
èüÿÿÿé  ‹”$”   èüÿÿÿ¹   º   ‰E‹D$‰˜,  ‹Eœ$È   ‰ß‰D$d1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş    D$d‰„$ä   h   jèüÿÿÿ‰Ú‰ğèüÿÿÿ‰ÃXZ…Û„Ÿ  û ğÿÿ‡“  ‹CT…À„ˆ  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿép  ‹D$1ö‰˜4  1Ûé  ‹D$‹@‰D$ƒøtƒø„b  éÆëÿÿ¸­  èüÿÿÿ‰Ã…Àuh¾  jèüÿÿÿ¸­  èüÿÿÿ^_‰Ã…Àt5‹$L$ „  ¸ø  èüÿÿÿ‰Ã…Àu3¸­  1öèüÿÿÿÇE    é  ht)  1ö1ÛèüÿÿÿÇE    Yéó  t$L‰E1À¹   ‰÷œ$È   ó«‰ß¹   ‹UÇD$`    ó«¹   ‰Ø‰T$XºÖ  èüÿÿÿfÇ„$Ş   ` ‰´$ä   Sjèüÿÿÿ‹D$(‰Úèüÿÿÿ‰ÃXZ…Û„O  û ğÿÿ‡C  ‹CT…À„8  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé   ‹E‹|$1ö‹ì  ‰è   ‹E‹€  ‰‡@  ‹EÇ€  `r  ‰Ÿ0  1Ûé  T$-1À¹   ÆD$,‰×œ$È   t$$ºİ  óªD$ ‰ß‰D$(E‰D$$1À‰L$¹   ó«¹   ‰Øèüÿÿÿ‰´$ä   fÇ„$Ş   d Sjèüÿÿÿ‹D$‰Úü   èüÿÿÿY_‰Æ…À„_  = ğÿÿ‡T  ‹@T…À„I  ‹@èüÿÿÿ„Àu‰ğèüÿÿÿé1  ‹D$T$4‹L$‰×ºä  ‰°,  ‹D$t$0ó«‹E‰ß¹   ÆD$9‰D$0‹D$ó«¹   ‰ØèüÿÿÿfÇ„$Ş   ` ‰´$ä   Sjèüÿÿÿ‹D$(‰Úèüÿÿÿ‰ÃXZ…Û„¹  û ğÿÿ‡­  ‹CT…À„¢  ‹@èüÿÿÿ„Àu‰ØèüÿÿÿéŠ  ‹D$1ö‰˜0  1Ûé©  T$-1À¹   ÆD$,‰×œ$È   t$$ºİ  óªD$ ‰ß‰D$(E‰D$$1À‰L$¹   ó«¹   ‰Øèüÿÿÿ‰´$ä   fÇ„$Ş   d Sjèüÿÿÿ‹D$‰Ú„  èüÿÿÿY_‰Æ…À„ô  = ğÿÿ‡é  ‹@T…À„Ş  ‹@èüÿÿÿ„Àu‰ğèüÿÿÿéÆ  ‹D$T$4¹   ‰×ºä  ‰°,  ‹D$t$0ó«‹E‰ß¹   ÆD$9‰D$0‹D$ó«¹   ‰ØèüÿÿÿfÇ„$Ş   ` ‰´$ä   Sjèüÿÿÿ‹D$(‰Úèüÿÿÿ‰ÃXZ…Û„M  û ğÿÿ‡A  ‹CT…À„6  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé  ‹D$1ö‰˜0  1Ûé=  ¸­  èüÿÿÿ‰Ã…Àuh¾  jèüÿÿÿ¸­  èüÿÿÿ^_‰Ã…Àt5‹$L$ „  ¸à  èüÿÿÿ‰Ã…Àu3¸­  1öèüÿÿÿÇE    éÚ  ht)  1ö1ÛèüÿÿÿÇE    Yé¿  t$L‰E1À¹   ‰÷œ$È   ó«‰ß¹   ‹UÇD$`    ó«¹   ‰Ø‰T$XºÖ  èüÿÿÿfÇ„$Ş   ` ‰´$ä   Sjèüÿÿÿ‹D$(‰Úèüÿÿÿ‰ÃXZ…Û„  û ğÿÿ‡  ‹CT…À„  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿéì  ‹E1ö‹ì  ‰è   ‹D$‰˜0  1Ûéü  T$h1À¹   ‰×ó«‹D$fÇD$h! ÇD$dÀü›‹@fÇD$y™ƒøtƒøt/1Àé·  ‹$€L$xÇD$l   ÇD$p€>  °|  ÇD$`r  ë‹$ÇD$p w ÇD$€q  °ô   œ$È   1À¹   ºë  ‰ßó«¹
   ‰ØèüÿÿÿfÇ„$Ş   h D$d‰„$ä   SjèüÿÿÿF‰ÚèüÿÿÿY_‰Æ…À„ö  = ğÿÿ‡ë  ‹@T…À„à  ‹@èüÿÿÿ„Àu‰ğèüÿÿÿéÈ  ‹D$‹”$€   ‰°,  ‰ğèüÿÿÿ‹T$|‰D$ ‰ğèüÿÿÿ¹   ºÖ  t$L‰E1À‰÷ó«‰ß¹   ‹EÇD$`    ‰D$X1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş   ` ‰´$ä   Sjèüÿÿÿ‹D$(‰Úèüÿÿÿ‰ÃXZ…Û„4  û ğÿÿ‡(  ‹CT…À„  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé  ‹E‹|$1ö‹ì  ‰è   ‹E‹€  ‰‡@  ‹|$‹E‰¸  ‹D$‰˜0  1Ûéõ  ‹D$ÇD$,    ‹@ƒøt1öƒøu‹$ÆD$,°ô   ë‹$ÆD$,°|  D$ ¹   ºİ  œ$È   ‰D$(E‰D$$‰ß1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş   d D$$‰„$ä   SjèüÿÿÿF‰Úèüÿÿÿ_‰ÆX…ö„%  ş ğÿÿ‡  ‹FT…À„  ‹@èüÿÿÿ„Àu‰ğèüÿÿÿéö  ‹D$T$4¹   ‰×ºä  ‰°,  1Àt$0ó«‹E‰ß¹   ÆD$9‰D$01Àó«¹   ‰Øèüÿÿÿ‰´$ä   fÇ„$Ş   ` Sjèüÿÿÿ‹D$(‰ÚèüÿÿÿY^‰Ã…À„  = ğÿÿ‡v  ‹@T…À„k  ‹@èüÿÿÿ„Àu‰ØèüÿÿÿéS  ‹D$1ö‰˜0  1Ûér  ‹D$‹@‰D$ƒøtƒø„Ë  é©âÿÿ¸­  èüÿÿÿ‰Ã…Àuh¾  jèüÿÿÿ¸­  èüÿÿÿ‰ÃXZ…Ût7‹$L$ °ü   ¸È  ‰òèüÿÿÿ‰Ã…Àu3¸­  1öèüÿÿÿÇE    éï  ht)  1ö1ÛèüÿÿÿÇE    XéÔ  œ$È   ‰E¹   º   ‰D$‰ß1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş    D$‰„$ä   h   jèüÿÿÿ‰ğ‰Úèüÿÿÿ_‰ÆX…ö„B  ş ğÿÿ‡6  ‹FT…À„+  ‹@èüÿÿÿ„Àu‰ğèüÿÿÿé  ‹D$‰ß¹   ºõ  ‰°4  ‹E‰D$1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş   ! D$‰„$ä   Sh   jèüÿÿÿ‹D$,‰Úèüÿÿÿ‰ÃƒÄ…À„¬  = ğÿÿ‡¡  ‹@T…À„–  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé~  ‹D$1ö‰˜0  ‹E1Û‹ì  ‰è   é
  T$-1À¹   ÆD$,‰×´$È   \$$ºİ  óªD$ ‰÷‰D$(E‰D$$1À‰L$¹   ó«¹   ‰ğèüÿÿÿ‰œ$ä   fÇ„$Ş   d Vh   jèüÿÿÿ‹D$‰òü   èüÿÿÿ‰ÃƒÄ…À„Ó
  = ğÿÿ‡È
  ‹@T…À„½
  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé¥
  ‹D$T$4‹L$‰×ºä  ‰˜,  ‹D$ó«‹E‰÷¹   ÆD$9‰D$0‹D$ó«¹   ‰ğèüÿÿÿfÇ„$Ş   ` D$0‰„$ä   Vh   jèüÿÿÿ‹D$‰ò„  èüÿÿÿ‰ÆƒÄ…Àt= ğÿÿw‹@T…Àu%‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      éó	  ‹@èüÿÿÿ„Àu,‰ğèüÿÿÿ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      é»	  ‹D$¹7   1Û‰°0  ‹$‰°$  ‹}  ·$  ‰Çó¥1öéº  ‹|$ÿwPh   h¨)  èüÿÿÿ‹D$‹@‰D$ ƒÄƒøtƒø…ÜŞÿÿé÷  T$-1À¹   ÆD$,‰×´$È   \$$ºİ  óªD$ ‰÷‰D$(E‰D$$1À‰L$¹   ó«¹   ‰ğèüÿÿÿ‰œ$ä   fÇ„$Ş   d Vh   jèüÿÿÿ‹D$‰òü   èüÿÿÿ‰ÃƒÄ…À„Ç  = ğÿÿ‡¼  ‹@T…À„±  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé™  ‹D$T$4¹   ‰×ºä  ‰˜,  ‹D$ó«‹E‰÷¹   ÆD$9‰D$0‹D$ó«¹   ‰ğèüÿÿÿfÇ„$Ş   ` D$0‰„$ä   Vh   jèüÿÿÿ‹D$‰ò„  èüÿÿÿ‰ÆƒÄ…Àt= ğÿÿw‹@T…Àu%‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      éæ  ‹@èüÿÿÿ„Àu,‰ğèüÿÿÿ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      é®  ‹D$‰°0  ‹$ƒ¸¸  8…èÍÿÿh   hÌ)  èüÿÿÿ‹D$¹7   ‰°$  ‹}  ·$  ‰Çó¥1öY[1Ûé‹  T$-1À¹   ÆD$,‰×´$È   \$$ºİ  óªD$ ‰÷‰D$(E‰D$$1À‰L$¹   ó«¹   ‰ğèüÿÿÿ‰œ$ä   fÇ„$Ş   f Vh   jèüÿÿÿ‹D$‰òü   èüÿÿÿ‰ÃƒÄ…À„Ğ  = ğÿÿ‡Å  ‹@T…À„º  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé¢  ‹D$T$4‹L$‰×ºä  ‰˜,  ‹D$ó«‹E‰÷¹   ÆD$9‰D$0‹D$ó«¹   ‰ğèüÿÿÿfÇ„$Ş   b D$0‰„$ä   Vh   jèüÿÿÿ‹D$‰ò„  èüÿÿÿ‰ÆƒÄ…Àt= ğÿÿw‹@T…Àu%‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      éğ  ‹@èüÿÿÿ„Àu,‰ğèüÿÿÿ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      é¸  ‹D$1Û‰°0  1öé×  ‹|$ÿwPh   h¨)  èüÿÿÿ‹D$ƒÄ‹@ƒøtƒø…ıÚÿÿé·  ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸`Q  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    éD  hø)  1ö1ÛèüÿÿÿÇE    Xé)  T$4‰E¹   1À‰\$0‰×œ$È   t$0ó«‰ß¹   ºä  ó«¹   ‰ØfÇD$8èüÿÿÿfÇ„$Ş   ` ‰´$ä   Sh   jèüÿÿÿ‹D$‰Ú„  èüÿÿÿ‰ÃƒÄ…Àt= ğÿÿw‹@T…Àu'¡T   ‹@èüÿÿÿ1Àèüÿÿÿ‹D$Ç€,      éI  ‹@èüÿÿÿ„Àu.‰Øèüÿÿÿ¡T   ‹@èüÿÿÿ1Àèüÿÿÿ‹D$Ç€,      é  ‹D$‰˜0  ‹$ƒ¸¸  9…IÊÿÿh   h,*  èüÿÿÿ‹D$¹7   ‰˜$  ‹}  1Û·$  ‰Çó¥1ö_Xéì  ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt1‹$ü   ¸@Q  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é  hø)  1ö1ÛèüÿÿÿÇE    Yér  T$4‰E¹   1À‰\$0‰×œ$È   t$0ó«‰ß¹   ºä  ó«¹   ‰ØfÇD$8èüÿÿÿfÇ„$Ş   b ‰´$ä   Sh   jèüÿÿÿ‹D$‰Ú„  èüÿÿÿ‰ÃƒÄ…Àt= ğÿÿw‹@T…Àu'¡T   ‹@èüÿÿÿ1Àèüÿÿÿ‹D$Ç€,      é’  ‹@èüÿÿÿ„Àu.‰Øèüÿÿÿ¡T   ‹@èüÿÿÿ1Àèüÿÿÿ‹D$Ç€,      éX  ‹D$1ö‰˜0  1Ûéw  ‹D$ƒx…‹Èÿÿ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸àO  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é
  hø)  1ö1ÛèüÿÿÿÇE    Xéï   T$4‰E¹   1À‰\$0‰×œ$È   t$0ó«‰ß¹   ºä  ó«¹   ‰ØfÇD$8èüÿÿÿfÇ„$Ş   ` ‰´$ä   Sh   jèüÿÿÿ‹D$‰Ú„  èüÿÿÿ‰ÃƒÄ…À„@  = ğÿÿ‡5  ‹@T…À„*  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé  ‹D$¹7   ‰˜0  ‹$‰˜$  ‹}  1Û·$  ‰Çó¥1öëÿt$1ÛhX*  èüÿÿÿ^1ö_‹E…Àt
…Ûtƒ{ uÿt$h *  èüÿÿÿY[é¨   Ç€      …Ût‹CÇ€      ‹$‹@‰D$x´‹$XGL9Øt‹Gd‹@…Àt‹…Òt‰øèüÿÿÿ‹LƒïLëÚ‹E‹  …ÒtèüÿÿÿVº    hV  j ‹D$‹€Ì   ˆ   P‹L$‹D$èüÿÿÿƒÄ…Àu‹D$èu¿ÿÿ…À„lïÿÿ‹D$‹˜4  …Ût ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€4      ‹D$‹˜0  …Ût ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€0      ‹D$‹˜,  …Ût ‹CT‹@èüÿÿÿ‰Øèüÿÿÿ‹D$Ç€,      ‹D$Ç€      ‹D$èüÿÿÿ¸êÿÿÿé  ‹$¹ S  º   ‹œ$Ä   ü   èüÿÿÿ‰E…À„€Åÿÿ‹$ÇD$h    ÇD$l    „  ÆD$hd‰D$d¸  èüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt‹ET$dèüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿYë‹ˆØ  …Ét
º¸N  èüÿÿÿ1ö1ÛéÉıÿÿ¸á  èüÿÿÿ‰Ã…ÀuhÌ  jèüÿÿÿ¸á  èüÿÿÿ_‰ÃX…ÛtW‹Ej	j ¹À   ‰òèüÿÿÿZY…À… Äÿÿ¸á  èüÿÿÿë9¸á  èüÿÿÿ‰Ã…Àu=hÌ  jèüÿÿÿ¸á  èüÿÿÿY_‰Ã…Àuh`'  èüÿÿÿXhï  1ö1ÛèüÿÿÿXé%ıÿÿ‹Ej
ëˆ‹E‹À  èüÿÿÿ‹D$º   èüÿÿÿ‰Ã…À„Æıÿÿ¸Ï  èüÿÿÿ‰Á…Àuhá  jèüÿÿÿ¸Ï  èüÿÿÿ‰ÁXZ…Ét>‹D$‰ú‹@‰D$HkÀ P  èüÿÿÿ…À…ÃŞÿÿ¸Ï  ¾   èüÿÿÿÇC    éŒüÿÿh„(  ¾   èüÿÿÿÇC    Xépüÿÿ‹D$‰ú‹@Hÿ‰D$‹CkÉÁ`P  èüÿÿÿ¾   …À…BüÿÿéJŞÿÿ1ö1Ûé4üÿÿ‹E1ö1ÛÇ€   o  éüÿÿiÛˆ  ‹$º   h O  ¹   ‹´$´   œü   ‰ØèüÿÿÿZ…ÀyhŒ%  èüÿÿÿ¸íÿÿÿYéÏ   ‰Ø¹ O  ‹œ$Ä   º€   èüÿÿÿ‰E‰Ã…À„ÔÂÿÿ¹À  º@   „$ˆ   èüÿÿÿ‰ƒ4  ‹]¸ôÿÿÿƒ»4   t}¹   1Ò‰Ø‹œ$¤   èüÿÿÿ‰Æ¸ª  èüÿÿÿ‰Ã…Àuh¹  jèüÿÿÿ¸ª  èüÿÿÿ‰ÃXZ…Ût(‹E¹ O  ‰òèüÿÿÿ…À…KÂÿÿ¸ª  èüÿÿÿ¸íÿÿÿëh¸%  èüÿÿÿ¸íÿÿÿ_‹”$   d+    tèüÿÿÿÄ  [^_]ÃèüÿÿÿU½   WVS‰Ã{ƒì‹0ÿ°  h°  hĞ*  èüÿÿÿ†¼  ‰D$ƒÄ9«  ‚¶   ‰ê‰øèüÿÿÿ…Àuh°  hü*  èüÿÿÿºôÿÿÿ[^é½   ‰ê‰øèüÿÿÿ…À„§   ‰D$ÿ4$h +  èüÿÿÿ‹T$†  ÇB   ÇB   ÇBL   ÇBP   ‰Z<ÇBD   ÇB0   ÇB4    ÇBH    ‰B(‹†Ì   ˆ   ‰BBèüÿÿÿ‰ÂXY…Òx5Eé>ÿÿÿ‰Øèå¼ÿÿ‰Â…Àt"P‰D$h°  hH+  èüÿÿÿ‹T$ƒÄëºêÿÿÿƒÄ‰Ğ[^_]Ãÿ²Œ  ÿ²ˆ  h¨  hx+  èüÿÿÿƒÄé¸y  ÿ°Œ  ÿ°ˆ  hx  h¬+  èüÿÿÿƒÄéz  ÿv$hä  hÜ+  èüÿÿÿ1ÀƒÄé·z  hÀ  hÀ  hY  èüÿÿÿƒÄé{  ¸Ü  èüÿÿÿ¸Ô  èüÿÿÿjÿsÿsh  h,  èüÿÿÿƒÄé}  hd  hd  hY  èüÿÿÿƒÄé!}  ÿt$h$  h”-  èüÿÿÿƒÄéw†  ÿ7Vh$  hl-  èüÿÿÿƒÄéL†  hD-  èüÿÿÿXƒÈÿéŠ†  ÿ4·Vh$  h-  èüÿÿÿƒÄé…  V€ƒú\vtºz  RPhÜ,  èüÿÿÿƒÈÿƒÄéŠ†  V€ƒú\vGºz  RPht,  èüÿÿÿƒÈÿƒÄéŠ†  B€ƒø\v5¸z  PVh$  h$  hH,  èüÿÿÿƒÄéó„  ‹•€  ëµ‹•€  ëˆ‹…€  ëÇÿt$`h8  h8  h¸-  èüÿÿÿƒÄéÙ†  ÿt$hD  hŒ1  èüÿÿÿƒÄéóŠ  hH1  èüÿÿÿ]ÇD$ÿÿÿÿéÊŒ  Uh  hŒ0  èüÿÿÿƒÄéªŠ  ht/  èüÿÿÿ‹D$èüÿÿÿ[é¸ˆ  hD  hD  hY  èüÿÿÿƒÄé¨‡  hÀ RhÌ.  èüÿÿÿ‹D$èüÿÿÿƒÄé¸ˆ  h,  hH/  èüÿÿÿ‹D$^]‹‹HÁê…òˆ  ƒ=¤   „z‰  h,  h /  èüÿÿÿXZ‹D$éz‰  hô  hô  hY  èüÿÿÿƒÄé‹  h‚  h4.  èüÿÿÿhp.  èüÿÿÿƒÄé¸ˆ  h,  h.  èüÿÿÿXZé¸ˆ  hD  hD  hà-  èüÿÿÿÇD$    ÇD$    ƒÄƒ=¤  †ß‡  h,  h,  hY  èüÿÿÿƒÄéß‡  h,  h80  èüÿÿÿ]XéŒ‰  hø/  èüÿÿÿ‹D$èüÿÿÿXé¸ˆ  hÌ/  èüÿÿÿ‹D$èüÿÿÿXé¸ˆ  h,  h /  èüÿÿÿXZéE‰  h  h  hY  èüÿÿÿƒÄécŠ  h,  h.  èüÿÿÿY^é6Š  hÀ0  èüÿÿÿhD  hğ0  èüÿÿÿƒÄÇD$ÿÿÿÿéÊŒ  h1  èüÿÿÿXÇD$ÿÿÿÿéÊŒ  ÿt$ÿt$hH  hH  hÄ1  èüÿÿÿƒÄéy  h  h  hY  èüÿÿÿƒÄéñ  ‹‹Ä  ‹‘  …Òt1RPh 2  èüÿÿÿ1Ò‰Øè|‡  ƒÄéÖ  P‰õhô1  èüÿÿÿXZéÖ  ‹‘è   ëÇhô  hô  hY  èüÿÿÿƒÄéW  h`  h`  hY  èüÿÿÿƒÄé4‘  èüÿÿÿ·Àºÿÿ V1ö@S÷ê‰Ñ‰Ã…Òt¾6   ‰Ğ1Ò÷ö‰Ñ‰Æ‰Ø‰Ê»6   ÷ó‰Ñ‰òƒùvƒÀ[^Ã¹ı  öÃ¸ù  ‰T$8DÁöÃ‰D$$¸  DÁƒ<$ ‰D$(¸  DÁöÃ‰D$,¸	  DÁƒ|$ ‰D$0¸  DÁ…Ò‰D$4¸  DÁ‹L$ƒÁp‰L$ÿt$$ÿt$,ÿt$4ÿt$<ÿt$DPQhL2  èüÿÿÿ¹ı  ‹T$XƒÄ ƒ=¨  †è¢  ƒ|$ ¸  ‰T$,DÁ÷Ç   ‰Â¸  DÁƒ|$ ‰D$$¸  DÁ…í‰D$(¸!  DÁRÿt$(ÿt$0Pÿt$ht2  èüÿÿÿ‹T$DƒÄéè¢  ‹D$ÇD$<   Æ ‹D$ƒÀpPh”2  èüÿÿÿXZéê£  ‹D$ƒL$<ƒÀpPhÀ2  èüÿÿÿ_X€|$ t
¸¿şÿÿé¤  ¸¿ÿÿÿé¤  ‹D$ÇD$<   ƒÀpPhÀ2  èüÿÿÿX¸¿ÿÿÿZé¤  ƒ|$ tFƒ|$ tk‹D$ h?  hR  XpRShì2  èüÿÿÿƒÄƒ=¨  †Ú¦  Sh%  èüÿÿÿXZéÚ¦  ƒ|$ t8hQ  hR  R‹D$,ƒÀpPhì2  èüÿÿÿƒÄéÚ¦  ‹D$ h?  hK  Xpë“hQ  hK  R‹D$,ƒÀpPhì2  èüÿÿÿƒÄéÚ¦  o¸üŸ÷í‰Å…Ò„Ò  ¹6   ‰Ğ1Ò÷ñ‰Á‰è½6   ÷õ‰Õ‰Ê‰Ñ‰ÂƒıvƒÂƒÑ ‰ø‰$·l$‰L$è/ıÿÿ‹$‹L$QRPShx4  èüÿÿÿƒÄ…í„}  h^  Shh7  èüÿÿÿiÅè  ¹6   1ÒƒÀ÷ñ1ÒPE÷ñPSh˜7  èüÿÿÿƒÄ½[  öD$¿W  ‰éEÏQSh´4  èüÿÿÿ¹^  ºf  ƒÄöD$EÑRShä4  èüÿÿÿ‰êƒÄöD$E×RSh5  èüÿÿÿ¹^  ºf  ƒÄöD$EÑRShD5  èüÿÿÿ‰êƒÄöD$E×RSht5  èüÿÿÿ‰èƒÄöD$EÇPSh¤5  èüÿÿÿ¹^  ¸f  ƒÄöD$EÁPShÔ5  èüÿÿÿSh   èüÿÿÿ‰èƒÄ÷Æ   EÇPSh3  èüÿÿÿ‰ğ¹^  ºf  ƒÄƒà EÑ‰$RSh6  èüÿÿÿ¹^  ºf  ƒÄ÷Æ€   EÑRSh46  èüÿÿÿºÈ  ¸Š  ƒÄ÷Æ   DÂPShœ3  èüÿÿÿƒÄ÷Æ €  DıWShd6  èüÿÿÿºó  ¸Ú  ·l$ ƒÄæ   DÂuPSh”6  èüÿÿÿƒÄƒ<$ t5‰ñõ€ù71ÒÁá÷ñPShÄ6  èüÿÿÿ‹D$$ƒàƒÀPShø6  èüÿÿÿƒÄ¸üŸ÷î‰Á…Ò„p  ¾6   ‰Ğ1Ò÷ö‰Æ‰È¿6   ÷÷‰Ñ‰ò‰Æ‰×ƒùvƒÆƒ× ‰èèºúÿÿW¿[  V¾W  PSh,7  èüÿÿÿ‹l$ ‰øƒÄ÷Å   EÆPSht5  èüÿÿÿƒÄƒå D÷VSh¤5  èüÿÿÿºf  ¸^  ƒÄöD$ DÂPShÔ5  èüÿÿÿ1ÀƒÄ0[^_]Ão1Ò‰éí€ù7Áá÷ñPShü3  èüÿÿÿ‰ğƒàƒÄƒèƒøveÇD$   Ç$   ‹D$¹   +$¯ÍƒÀ¯Å‰ÊÑê‰D$ ‰Ğ1Ò€ù7÷ñ1ÒP‹L$$‰ÈÑè€ù7÷ñPÿt$ÿt$Sh04  èüÿÿÿƒÄé4üÿÿ‹…  ‹…   ‰$‰D$ë“1Éé4üÿÿhf  Shh7  èüÿÿÿƒÄé¤üÿÿ1öé–şÿÿhğ  h$8  èüÿÿÿXZé;¶  hğ  h 8  èüÿÿÿYXé¶  hà  h 8  èüÿÿÿXZé‹·  hü  h 8  èüÿÿÿXZéB·  hü  h 8  èüÿÿÿXZé·  hà  h 8  èüÿÿÿXZéÆ¶  ¶ÛPSh`  h`  hD8  èüÿÿÿƒÈÿƒÄé!¸  ¶¶ÛP¶D$PShp  hp  hÄ8  èüÿÿÿ1ÀƒÄéâ¹  P¶D$Php  hp  h„8  èüÿÿÿƒÈÿƒÄéâ¹  ¶D$PP¶Cøj Ph  h  h9  èüÿÿÿƒÄé—º  €<$ ¶NLuH¾±  €|$¸¨  ¶\$EÆ¾µ  ‰T$SPÿt$8QVh€  h€  hd9  èüÿÿÿ‹T$$ƒÄ é¨¼  ¾¨  €|$¸±  ‰ÓDÆ¾¬  ë¹PƒËÿj h`  h`  hD8  èüÿÿÿƒÄéú½  P¶FLPh4  h4  h¤9  èüÿÿÿƒÄé¾  Pjh`  h`  hD8  èüÿÿÿƒÄé·¾  hH  hH  h:  èüÿÿÿ¸   ƒÄéT¿  hH  hH  hä9  èüÿÿÿƒÄéG¿  hH  hH  h»  èüÿÿÿƒÄé5¿  Wh$  h$  hx:  èüÿÿÿƒÄéÉÁ  h$  h$  h$:  èüÿÿÿƒÄé¼Á  h$  h$  hÏ  èüÿÿÿƒÄéXÀ  h$  h$  hT:  èüÿÿÿƒÄéÁ  Ph¬:  èüÿÿÿXƒÈÿZé·Â  PhÜ:  èüÿÿÿXƒÈÿZéEÃ  h¨  h;  èüÿÿÿX¸   ZézÇ  †¼  hØ  Phâ  èüÿÿÿƒÄé…È  †¼  Ph˜;  èüÿÿÿZYéŸÉ  hÄ;  ¾íÿÿÿèüÿÿÿYé4Ë  ‹E ¼  Ph<  èüÿÿÿXZé4Ë  ‹¼  Phh<  èüÿÿÿ^XéË  Q‰L$P‹¼  PhÄ<  èüÿÿÿ‹C‹L$ƒÄéÍ  hÀ  hü<  èüÿÿÿXZºôÿÿÿé_Î  ‰$U‹¼  Ph<  èüÿÿÿ‹T$ƒÄé_Î  †¼  Ph(=  èüÿÿÿ‹†Ü   @  ‹ƒâî‰‹†d    ‰ğèüÿÿÿY_éëÎ  ‹E ¼  PhT=  èüÿÿÿXZéÏ  hğ  h€=  èüÿÿÿ^1öXé Ğ  ‹¼  PhÌ=  èüÿÿÿY[é Ğ  hğ  1öh >  èüÿÿÿXZé Ğ  èüÿÿÿh  hd   èüÿÿÿ¹  X¸`J  Zº    éüÿÿÿ¸`J  éüÿÿÿ                            àf3eŞ   viewcast_eeprom cx23885_tuner_callback  P   ?   enable_885_ir   netup_card_rev      cx23885_vdev_init   vidioc_s_fmt_vid_cap    vidioc_g_std    vidioc_s_std    vidioc_enum_input   U  d  s  cx23885_query_audinput  vidioc_g_audinput   vidioc_s_audinput   buffer_prepare  cx23885_start_video_dma buffer_queue    cx23885_video_register  cx23885_video_unregister    cx23885_video_irq   cx23885_set_freq_via_ops    cx23885_audio_mux   cx23885_flatiron_dump   cx23885_flatiron_mux    cx23885_video_mux   cx23885_set_input   cx23885_get_input   strnlen strscpy             v    Œ  —  ¢  ª  ª  ´  ½  Á  cx23885_enum_input  cx23885_flatiron_read   cx23885_set_tvnorm  cx23885_video_wakeup                                &  À                           `                                                        $                                                      0#                                                                                                          °+      Ğ1  °)   2                                              ğ'   (   0                      À  P                                          À  @  °  @6          `                                                                                                                                            !  ğ        )  Ğ  Ğ      vid_limit   irq_debug   video_debug vbi_nr                 J  video_nr                   J                  cx23885_start_vbi_dma   buffer_queue    cx23885_vbi_irq         `=                  €?  `?      @  Ğ>   =      vbi_debug   vbibufs                             cx23885_dev_checkrevision   cx23885_reset   cx23885_pci_quirks  cx23885_init_tsport cx23885_dev_setup   cx23885_irq_ts  cx23885_irq cx23885_wakeup  cx23885_irq_417 do_cancel_buffers   cx23885_stop_dma    cx23885_cancel_buffers  cx23885_buf_queue   cx23885_buf_prepare cx23885_tsport_reg_dump cx23885_start_dma                                                                          "  )  .  3  6  9  <  ?  B  E  J  O  S      W  ]          b      i  n  s  x  ~  †            –  £  °  ¹  Â  Ê  Ò  İ  è  ò  ü      $  cx23885_sram_channel_setup              ñ  Rˆ  ÿÿÿÿÿÿÿÿ                ñ  €ˆ  ÿÿÿÿÿÿÿÿ                                                "  Q  "  #  "    "      #Z  card                  @N  debug   dma_reset_workaround                                cx23885_av_clk  strnlen strscpy i2c_sendbytes   i2c_readbytes   i2c_xfer    k şÿcx23885_i2c_register                                                            *	  3	                  =	                                          F	                                                                                                                                                          N	  N	  U	  ]	  e	                              m	              v	                                                              }	  T"          ’	                                                                                                              Àk              g          i2c_scan    i2c_debug               stb6100_set_frequency   stb6100_get_frequency   stb6100_set_bandwidth   stb6100_get_bandwidth   dvb_register    cx23885_dvb_register    i   Àü›!  €>    ™    h   Àü›!  '    ™   h   Àü›!  €>    ™                   Àn                  pq  Pq       q  q  ğp      adapter_nr                V  alt_tuner       ©  ±  ¿  z  z  z  z  z  z  Ì  z  Ù  z  æ  z  ñ  z     z  z  z    z    z  /  z  @  z  T  z  i  z  {  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  “  z     z  °  z  À  z  z  z  z  z  Õ  İ  é  ù      '  :  R  i  {  z  z  Œ  z     ®  ¼  Ê  á  õ  z      z    cx23885_codec_settings  cx23885_find_mailbox    §  f»Uªcx23885_load_firmware   cx23885_initialize_codec    cx23885_video_dev_alloc vidioc_g_fmt_vid_cap    vidioc_s_fmt_vid_cap    vidioc_try_fmt_vid_cap  vidioc_enum_input   strnlen strscpy vidioc_g_tuner  cx23885_417_register    cx23885_417_unregister  cx23885_mbox_func   cx23885_api_cmd cx23885_417_check_encoder   cx23885_mc417_init          Ğ{  0w                          Ày                                                      À|                                                      py                                                                                                      @y   {      ğz  àz  Ğz                                                                                                                              @z  Ğx  Px  0z          °w                                                                                                                          àv                  w  pw      0     Pw                      •                                    (                 /  à               6                 <                 B                 H                 O                 V    @             ^    2             v4l_debug   mpeglinesize    mpeglines   mpegbufs                                              f  o  o  o  x  o  o  o  …  o  o  o  ‘                                 €          P¥       ¬  ĞŸ      §          À±                                  P¢          ir_888_debug    i2c_av_write    i2c_av_read i2c_av_write4   netup_read_ci_status    netup_ci_init   netup_ci_set_irq    netup_ci_slot_status    netup_write_i2c netup_read_i2c  netup_ci_op_cam ci_irq_enable   ci_dbg  f300_xfer               cx23885_alsa_dma_map    cx23885_start_audio_dma cx23885_audio_register                          `Ê  àÇ      `Ì  0Ì  °Î  0È      ğÇ                  È          audio_debug disable_analog_audio         €   @  p     @  Ğ  @    Ğ  Ğ  `  °  @  À  P  À      `   À   ğ   !  0#   $  &  à&  P'  ğ'   (  `(   )  °)  à)   *  °+  ğ+  P,  à,  à-  Ğ/   0   0  Ğ1   2  02   2  °2  @6  P6  @7  ğ7  `=   =  Ğ>  `?  €?  @  À@  PA  °A  T  àA   C   E  PE   F  ÀF   G   H  PH   H  àH  0I  ¸	  ĞK  `L  ÀL   M  0N  O  PO  ÀR  `S  `T  V  ĞV  `\  à\  `]  ğ]   ^      g   g  ğg  pi  Àk  l   n  0n  Àn   o  Po  €o  o  Ào  p  @p  p  ğp  q   q  Pq  pq  €q  `r  s  ›%  `(  s  u  xj   v  àv  0w  Pw  pw  w  °w  Px  Ğx  @y  py  Ày  0z  @z  Ğz  àz  ğz   {  Ğ{  À|  }  `}  à~    ‚  À„  °†  p‡  €‡  0     `  À  0     à  @  Ğ’  @“  P“  À“  ”  ğ”  P—  ˜   š  0š  `œ  pŸ  ĞŸ         «p  0¡  °¡  P¢  P¥   §   ¬  À±  P³  pµ  Ğµ  `¶  °·  À·  À¸  p¹   º  Àº  ğ¼   ½  @½  `½  €½  @¾   ¿  Ğ¿  Â  PÂ  àÂ  `Ã  ÀÃ  `Ä   Å  àÇ  ğÇ  È  0È  `Ê  `Ë  0Ì  `Ì  °Î  ÀÎ  pÏ  àĞ     4cx23885: %s: warning: unknown hauppauge model #%d
    6cx23885: %s: hauppauge eeprom: model=%d
  6cx23885: %s: Board has no valid PCIe Subsystem ID and can't
%s: be autodetected. Pass card=<n> insmod option
%s: to workaround that. Redirect complaints to the
%s: vendor of the TV card.  Best regards,
%s:         -- tux
 6cx23885: %s: Your board isn't known (yet) to the driver.
%s: Try to pick one of the existing card configs via
%s: card=<n> insmod option.  Updating to the latest
%s: version might help as well.
    6cx23885: %s: Here is a list of valid choices for the card=<n> insmod option:
 6cx23885: %s:    card=%d -> %s
    3cx23885: %s(): Unknown command 0x%x.
 6cx23885: %s() eeprom records are undefined, no serial number
 6cx23885: %s: card '%s' sn# MM%d
  6cx23885: NetUP card rev=0x%x fw_filename=%s
  3cx23885: did not find the firmware file '%s'. You can use <kernel_dir>/scripts/get_dvb_firmware to get the firmware.  DViCO FusionHDTV7 Dual Express  DViCO FusionHDTV DVB-T Dual Express Leadtek Winfast PxDVR3200 H XC4000  Hauppauge WinTV-HVR4400/HVR5500 AVerTV Hybrid Express Slim HC81R    DViCO FusionHDTV DVB-T Dual Express2    Technotrend TT-budget CT2-4500 CI   Hauppauge WinTV-HVR-1265(161111)    Hauppauge WinTV-QuadHD-DVB(885) Hauppauge WinTV-QuadHD-ATSC(885)    7cx23885: %s: video:[%p/%d] buffer_queue - first active
   7cx23885: %s: video:[%p/%d] buffer_queue - append to active
   7cx23885: %s: video:%s() Creating TFF/NTSC risc
   7cx23885: %s: video:%s() Creating BFF/PAL/SECAM risc
  drivers/media/pci/cx23885/cx23885-video.c   7cx23885: %s: video:[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx
    7cx23885: %s: video:%s() width=%d height=%d field=%d
  7cx23885: %s: video:[%p/%d] wakeup reg=%d buf=%d
  7cx23885: %s: video:%s(input=%d)
  7cx23885: %s: video:%s() returns %d
   7cx23885: %s: video:%s(norm = 0x%08x) name: [%s]
  7cx23885: %s: video:%s(input = %d)
    7cx23885: %s: video:Flatiron dump
 7cx23885: %s: video:FI[%02x] = %02x
   7cx23885: %s: video:%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]
    7cx23885: %s: video:%s() -EINVAL
  7cx23885: %s: video:%s() frequency=%d tuner=%d std=0x%llx
 3cx23885: %s() No analog tuner, aborting
  7cx23885: %s: video: (VID_BC_MSK_OPC_ERR 0x%08x)
  4cx23885: %s: video risc op code error
    7cx23885: %s: video: (VID_BC_MSK_SYNC 0x%08x) video lines miss-match
  7cx23885: %s: video: (VID_BC_MSK_OF 0x%08x) fifo overflow
 6cx23885: %s: can't register video device
 6cx23885: %s: registered device %s [v4l2]
 6cx23885: %s: can't register vbi device
   6cx23885: %s: registered device %s
    7cx23885: %s: vbi:[%p/%d] vbi_queue - first active
    7cx23885: %s: vbi:[%p/%d] buffer_queue - append to active
 7cx23885: %s: vbi:%s() VID_BC_MSK_VBI_RISCI1
  dma in progress detected 0x%08x 0x%08x, clearing
   7cx23885: %s: [%p/%d] wakeup reg=%d buf=%d
    7cx23885: %s: %s() Erasing channel [%s]
   7cx23885: %s: %s() Configuring channel [%s]
   drivers/media/pci/cx23885/cx23885-core.c    7cx23885: %s: %s() 0x%08x <- 0x%08x
   7cx23885: %s: [bridge %d] sram setup %s: bpl=%d lines=%d
  4cx23885: %s: %s - dma channel status dump
    4cx23885: %s:   cmds: %-15s: 0x%08x
   4cx23885: %s:   (0x%08x) iq %x:    4cx23885: %s:   iq %x: 0x%08x [ arg #%d ]
 4cx23885: %s: fifo: 0x%08x -> 0x%x
    4cx23885: %s: ctrl: 0x%08x -> 0x%x
    4cx23885: %s:   ptr1_reg: 0x%08x
  4cx23885: %s:   ptr2_reg: 0x%08x
  4cx23885: %s:   cnt1_reg: 0x%08x
  4cx23885: %s:   cnt2_reg: 0x%08x
  7cx23885: %s:  (VID_BC_MSK_OPC_ERR 0x%08x)
    7cx23885: %s:  (VID_BC_MSK_BAD_PKT 0x%08x)
    7cx23885: %s:  (VID_BC_MSK_SYNC    0x%08x)
    7cx23885: %s:  (VID_BC_MSK_OF      0x%08x)
    3cx23885: %s: mpeg risc op code error
 7cx23885: %s:  (RISCI1            0x%08x)
 7cx23885: %s: %s() w: %d, h: %d, f: %d
    6cx23885: %s: risc disasm: %p [dma=0x%08lx]
   6cx23885: %s:   %04d: 0x%08x [ arg #%d ]
  3cx23885: %s() Unsupported .portb/c (0x%08x)/(0x%08x)
 7cx23885: %s: %s() enabling TS int's and DMA
  7cx23885: %s: %s() DEV_CNTRL2               0x%08X
    7cx23885: %s: %s() PCI_INT_MSK              0x%08X
    7cx23885: %s: %s() AUD_INT_INT_MSK          0x%08X
    7cx23885: %s: %s() AUD_INT_DMA_CTL          0x%08X
    7cx23885: %s: %s() AUD_EXT_INT_MSK          0x%08X
    7cx23885: %s: %s() AUD_EXT_DMA_CTL          0x%08X
    7cx23885: %s: %s() PAD_CTRL                 0x%08X
    7cx23885: %s: %s() ALT_PIN_OUT_SEL          0x%08X
    7cx23885: %s: %s() GPIO2                    0x%08X
    7cx23885: %s: %s() gpcnt(0x%08X)          0x%08X
  7cx23885: %s: %s() gpcnt_ctl(0x%08X)      0x%08x
  7cx23885: %s: %s() dma_ctl(0x%08X)        0x%08x
  7cx23885: %s: %s() src_sel(0x%08X)        0x%08x
  7cx23885: %s: %s() lngth(0x%08X)          0x%08x
  7cx23885: %s: %s() hw_sop_ctrl(0x%08X)    0x%08x
  7cx23885: %s: %s() gen_ctrl(0x%08X)       0x%08x
  7cx23885: %s: %s() bd_pkt_status(0x%08X)  0x%08x
  7cx23885: %s: %s() sop_status(0x%08X)     0x%08x
  7cx23885: %s: %s() fifo_ovfl_stat(0x%08X) 0x%08x
  7cx23885: %s: %s() vld_misc(0x%08X)       0x%08x
  7cx23885: %s: %s() ts_clk_en(0x%08X)      0x%08x
  7cx23885: %s: %s() ts_int_msk(0x%08X)     0x%08x
  7cx23885: %s: %s() ts_int_status(0x%08X)  0x%08x
  7cx23885: %s: %s() PCI_INT_STAT           0x%08X
  7cx23885: %s: %s() VID_B_INT_MSTAT        0x%08X
  7cx23885: %s: %s() VID_B_INT_SSTAT        0x%08X
  7cx23885: %s: %s() VID_C_INT_MSTAT        0x%08X
  7cx23885: %s: %s() VID_C_INT_SSTAT        0x%08X
  7cx23885: %s: %s() Register Dump
  7cx23885: %s: [%p/%d] %s - first active
   7cx23885: %s: [%p/%d] %s - append to active
   7cx23885: %s: [%p/%d] %s - dma=0x%08lx
    7cx23885: %s: status: 0x%08x  mask: 0x%08x count: 0x%x
    3cx23885: %s: V4L mpeg risc op code error, status = 0x%x
  7cx23885: %s:         VID_B_MSK_BAD_PKT
   7cx23885: %s:         VID_B_MSK_OPC_ERR
   7cx23885: %s:         VID_B_MSK_VBI_OPC_ERR
   7cx23885: %s:         VID_B_MSK_SYNC
  7cx23885: %s:         VID_B_MSK_VBI_SYNC
  7cx23885: %s:         VID_B_MSK_OF
    7cx23885: %s:         VID_B_MSK_VBI_OF
    7cx23885: %s:         VID_B_MSK_RISCI1
    7cx23885: %s: pci_status: 0x%08x  pci_mask: 0x%08x
    7cx23885: %s: vida_status: 0x%08x vida_mask: 0x%08x count: 0x%x
   7cx23885: %s: audint_status: 0x%08x audint_mask: 0x%08x count: 0x%x
   7cx23885: %s: ts1_status: 0x%08x  ts1_mask: 0x%08x count: 0x%x
    7cx23885: %s: ts2_status: 0x%08x  ts2_mask: 0x%08x count: 0x%x
    7cx23885: %s:  (PCI_MSK_RISC_RD   0x%08x)
 7cx23885: %s:  (PCI_MSK_RISC_WR   0x%08x)
 7cx23885: %s:  (PCI_MSK_AL_RD     0x%08x)
 7cx23885: %s:  (PCI_MSK_AL_WR     0x%08x)
 7cx23885: %s:  (PCI_MSK_APB_DMA   0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_C     0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_B     0x%08x)
 7cx23885: %s:  (PCI_MSK_VID_A     0x%08x)
 7cx23885: %s:  (PCI_MSK_AUD_INT   0x%08x)
 7cx23885: %s:  (PCI_MSK_AUD_EXT   0x%08x)
 7cx23885: %s:  (PCI_MSK_GPIO0     0x%08x)
 7cx23885: %s:  (PCI_MSK_GPIO1     0x%08x)
 7cx23885: %s:  (PCI_MSK_AV_CORE   0x%08x)
 7cx23885: %s:  (PCI_MSK_IR        0x%08x)
 3cx23885: %s: Setting GPIO on encoder ports
   3cx23885: %s: Clearing GPIO moving on encoder ports
   3cx23885: %s: Reading GPIO moving on encoder ports
    3cx23885: %s: Enabling GPIO on encoder ports
  7cx23885: %s: %s() Memory configured for PCIe bridge type %d
  3cx23885: %s: can't get MMIO memory @ 0x%llx
  3cx23885: CORE %s No more PCIe resources for subsystem: %04x:%04x
 6cx23885: CORE %s: subsystem: %04x:%04x, board: %s [card=%d,%s]
   7cx23885: %s: %s() tuner_type = 0x%x tuner_addr = 0x%x tuner_bus = %d
 7cx23885: %s: %s() radio_type = 0x%x radio_addr = 0x%x
    3cx23885: %s() Failed to register analog video adapters on VID_A
  3cx23885: %s() Failed to register dvb adapters on VID_B
   3cx23885: %s() Failed to register 417 on VID_B
    3cx23885: %s() Failed to register dvb on VID_C
    3cx23885: %s() Failed to register 417 on VID_C
    3cx23885: %s() New hardware revision found 0x%x
   6cx23885: %s() Hardware revision = 0x%02x
 3cx23885: %s() Hardware revision unknown 0x%x
 6cx23885: %s/0: found at %s, rev: %d, irq: %d, latency: %d, mmio: 0x%llx
  3cx23885: %s/0: Oops: no 32bit PCI DMA ???
    3cx23885: %s: can't get IRQ %d
    6cx23885: cx23885 driver version %s loaded
    7cx23885: %s: i2c:%s(msg->len=%d)
 7cx23885: %s: i2c:%s() returns 0
  7cx23885: %s: i2c:%s(msg->wlen=%d, nextmsg->rlen=%d)
  7cx23885: %s: i2c:%s(num = %d)
    7cx23885: %s: i2c:%s(num = %d) addr = 0x%02x  len = 0x%x
  7cx23885: %s: i2c:%s(bus = %d)
    7cx23885: %s: i2c:%s: i2c bus %d registered
   6cx23885: %s: i2c scan: found device @ 0x%04x  [%s]
   4cx23885: %s: i2c bus %d register FAILED
  7cx23885: %s: i2c:%s(enabled = %d)
    tuner/mt2131/tda8275/xc5000/xc3028  6cx23885: NetUP Dual DVB-S2 CI card port%d MAC=%pM
    6cx23885: TeVii S470 MAC= %pM
 6cx23885: %s port %d MAC address: %pM
 6cx23885: %s MAC address: %pM
 3DVB: Unable to find symbol s5h1409_attach()
  3DVB: Unable to find symbol mt2131_attach()
   3DVB: Unable to find symbol lgdt3305_attach()
 3DVB: Unable to find symbol tda18271_attach()
 3DVB: Unable to find symbol s5h1411_attach()
  3DVB: Unable to find symbol tda829x_attach()
  3DVB: Unable to find symbol lgdt330x_attach()
 3DVB: Unable to find symbol simple_tuner_attach()
 3DVB: Unable to find symbol xc5000_attach()
   3DVB: Unable to find symbol xc2028_attach()
   3DVB: Unable to find symbol tda10048_attach()
 3DVB: Unable to find symbol dib7000p_attach()
 3DVB: Unable to find symbol zl10353_attach()
  4cx23885: Unable to enumerate dib7000p
    3DVB: Unable to find symbol dib0070_attach()
  3DVB: Unable to find symbol xc4000_attach()
   3cx23885: %s/2: xc4000 attach failed
  3DVB: Unable to find symbol cx24116_attach()
  3DVB: Unable to find symbol cx24117_attach()
  3DVB: Unable to find symbol ds3000_attach()
   3DVB: Unable to find symbol ts2020_attach()
   3DVB: Unable to find symbol stv0900_attach()
  3DVB: Unable to find symbol stv6110_attach()
  3DVB: Unable to find symbol lnbh24_attach()
   3DVB: Unable to find symbol lgs8gxx_attach()
  3DVB: Unable to find symbol mb86a20s_attach()
 3DVB: Unable to find symbol atbm8830_attach()
 3DVB: Unable to find symbol max2165_attach()
  3DVB: Unable to find symbol stv0367ter_attach()
   3DVB: Unable to find symbol stv0367cab_attach()
   3DVB: Unable to find symbol drxk_attach()
 3DVB: Unable to find symbol mt2063_attach()
   3DVB: Unable to find symbol stv090x_attach()
  3DVB: Unable to find symbol stb6100_attach()
  3DVB: Unable to find symbol m88ds3103_attach()
    6cx23885: %s(): board=%d port=%d
  6cx23885: %s(): QUADHD_DVB analog setup
   3DVB: Unable to find symbol lgdt3306a_attach()
    6cx23885: %s(): QUADHD_ATSC analog setup
  6cx23885: %s: The frontend of your DVB/ATSC card  isn't supported yet
 3cx23885: %s: frontend initialization failed
  6cx23885: %s() allocating %d frontend(s)
  3cx23885: %s() failed to alloc
    6cx23885: %s: cx23885 based dvb card
  3cx23885: %s() dvb_register failed err = %d
   7cx23885: %s: 417:VIDIOC_TRY_FMT: w: %d, h: %d
    7cx23885: %s: 417:VIDIOC_G_FMT: w: %d, h: %d
  7cx23885: %s: 417:VIDIOC_G_TUNER: tuner type %d
   7cx23885: %s: 417:VIDIOC_S_FMT: w: %d, h: %d, f: %d
   7cx23885: %s: 417:%s: command(0x%X) = %s
  3cx23885: Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s
    3cx23885: ERROR: Mailbox appears to be in use (%x), cmd = %s
  7cx23885: %s: 417:API Input %d = %d
   3cx23885: ERROR: API Mailbox timeout
  7cx23885: %s: 417:API Output %d = %d
  7cx23885: %s: 417:API result = %d
 7cx23885: %s: 417:%s() cmds = 0x%08x
  7cx23885: %s: 417:%s() PING OK
    3cx23885: %s: Error with mc417_register_write
 3cx23885: ERROR: Hotplug firmware request failed (%s).
    3cx23885: Please fix your hotplug setup, the board will not work without firmware loaded!
 3cx23885: ERROR: Firmware size mismatch (have %zu, expected %d)
   3cx23885: ERROR: Firmware magic mismatch, wrong file?
 7cx23885: %s: 417:Loading firmware ...
    3cx23885: ERROR: Loading firmware failed!
 7cx23885: %s: 417:Verifying firmware ...
  3cx23885: ERROR: Reading firmware failed!
 3cx23885: ERROR: Firmware load failed (checksum mismatch).
    7cx23885: %s: 417:Firmware upload successful.
 3cx23885: %s() f/w load failed
    7cx23885: %s: 417:Mailbox signature found at 0x%x
 3cx23885: Mailbox signature values not found!
 3cx23885: %s() mailbox < 0, error
 3cx23885: ERROR: cx23417 firmware ping failed!
    3cx23885: ERROR: cx23417 firmware get encoder :version failed!
    7cx23885: %s: 417:cx23417 firmware version is 0x%08x
  7cx23885: %s: 417:%s() status = %d, seq = %d
  6cx23885: %s: can't register mpeg device
  6cx23885: %s: registered device %s [mpeg]
 7%s: IRQ Status:  %s %s %s %s %s %s
   7%s: IRQ Enables: %s %s %s %s
 3%s: IR receiver software FIFO overrun
    3%s: IR receiver hardware FIFO overrun
    7%s: rx read: %10u ns  %s  %s
 6%s: 	Enabled:                           %s
   6%s: 	Demodulation from a carrier:       %s
   6%s: 	FIFO data on pulse timer overflow: %s
   6%s: 	FIFO interrupt watermark:          %s
   6%s: 	Loopback mode:                     %s
   6%s: 	Expected carrier (16 clocks):      %u Hz
    6%s: 	Next carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz
  6%s: 	Max measurable pulse width:        %u us, %llu ns
   6%s: 	Pulse width timer timed-out:       %s
   6%s: 	Pulse width timer time-out intr:   %s
   6%s: 	FIFO overrun:                      %s
   6%s: 	FIFO overrun interrupt:            %s
   6%s: 	Busy:                              %s
   6%s: 	FIFO service requested:            %s
   6%s: 	FIFO service request interrupt:    %s
   6%s: 	Modulation onto a carrier:         %s
   6%s: 	FIFO:                              %s
   6%s: 	Output pin level inversion         %s
   6%s: 	Carrier polarity:                  %s
   6%s: 	Carrier (16 clocks):               %u Hz
    6%s: 	Carrier duty cycle:                %2u/16
   6%s: 	Max pulse width:                   %u us, %llu ns
   6%s: 	Low pass filter:                   %s
   6%s: 	Min acceptable pulse width (LPF):  %u us, %u ns
 6%s: 	Pulse timers' start/stop trigger:  %s
   3cx23885: %s: i2c write error!
    3cx23885: %s: i2c read error!
 7cx23885: %s: %s: i2c write error, Reg=[0x%02x], Status=%d
    7cx23885: %s: %s: i2c read error, Reg = 0x%02x, Status = %d
   7cx23885: %s: %s: i2c read Addr=0x%04x, Reg = 0x%02x, data = %02x
 7cx23885: %s: %s: Slot Status Addr=[0x%04x], Reg=[0x%02x], data=%02x, TS config = %02x
    7cx23885: %s: %s: %s: chipaddr=[0x%x] addr=[0x%02x], %s=%x
    7cx23885: %s: %s: chipaddr=[0x%x] setting ci IRQ to [0x%x] 
   7cx23885: %s: %s: Wakeup CI0
  7cx23885: %s: %s: Wakeup CI1
  7cx23885: %s: %s: Unable create CI structure!
 7cx23885: %s: %s: CI initialized!
 7cx23885: %s: %s: Cannot initialize CI: Error %d.
 3cx23885: eeprom i2c read error, status=%d
    3cx23885: eeprom i2c write error, status=%d
   3cx23885: %s: timeout, the slave no response
  7cx23885: %s: alsa: Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer
 7cx23885: %s: alsa: Stopping audio DMA
    3cx23885: BUG: cx23885 can't find device struct. Can't proceed with open
  7cx23885: %s: alsa: Error opening PCM!
    drivers/media/pci/cx23885/cx23885-alsa.c    7cx23885: %s: alsa: Freeing buffer
    7cx23885: %s: alsa: vmalloc_32(%lu pages) failed
  7cx23885: %s: alsa: vmalloc is at addr %p, size=%lu
   4cx23885: %s: cx23885_alsa_map_sg failed
  4cx23885: %s/1: Audio risc op code error
  7cx23885: %s: alsa: Downstream sync error
 4cx23885: %s(): Missing SRAM channel configuration for analog TV Audio
    7cx23885: %s: alsa: registered ALSA audio device
  3cx23885: %s(): Failed to register analog audio adapter
 ir-kbd-i2c dvb-netup-altera-04.fw dvb-netup-altera-01.fw cx25840 cs3308 configure UNKNOWN/GENERIC Hauppauge WinTV-HVR1800lp Hauppauge WinTV-HVR1800 Hauppauge WinTV-HVR1250 DViCO FusionHDTV5 Express Hauppauge WinTV-HVR1500Q Hauppauge WinTV-HVR1500 Hauppauge WinTV-HVR1200 Hauppauge WinTV-HVR1700 Hauppauge WinTV-HVR1400 Leadtek Winfast PxDVR3200 H Compro VideoMate E650F TurboSight TBS 6920 TeVii S470 DVBWorld DVB-S2 2005 NetUP Dual DVB-S2 CI Hauppauge WinTV-HVR1270 Hauppauge WinTV-HVR1275 Hauppauge WinTV-HVR1255 Hauppauge WinTV-HVR1210 Mygica X8506 DMB-TH Magic-Pro ProHDTV Extreme 2 Hauppauge WinTV-HVR1850 Compro VideoMate E800 Hauppauge WinTV-HVR1290 Mygica X8558 PRO DMB-TH LEADTEK WinFast PxTV1200 GoTView X5 3D Hybrid NetUP Dual DVB-T/C-CI RF MPX-885 Mygica X8502/X8507 ISDB-T TerraTec Cinergy T PCIe Dual TeVii S471 Prof Revolution DVB-S2 8000 TurboSight TBS 6981 TurboSight TBS 6980 Leadtek Winfast PxPVR2200 Hauppauge ImpactVCB-e DVBSky T9580 DVBSky T980C DVBSky S950C DVBSky S950 DVBSky S952 DVBSky T982 Hauppauge WinTV-HVR5525 Hauppauge WinTV Starburst ViewCast 260e ViewCast 460e Hauppauge WinTV-QuadHD-DVB Hauppauge WinTV-QuadHD-ATSC Hauppauge WinTV-Starburst2 AVerMedia CE310B Television 7cx23885: %s: video:%s()
 cx23885 PCIe:%s %s (%s) 3cx23885: %s() error
 7cx23885: %s: video:%s(%d)
 cx23885-vbi tuner xc3028-v27.fw xc3028L-v36.fw video vbi Baseband L/R 1 Baseband L/R 2 TV Composite1 Composite2 Composite3 Composite4 S-Video Component Cable TV DVB for debug only 7cx23885: %s: vbi:%s()
 INVALID 70x%08x [ %s c %s c count=%d ]
 4cx23885: %s:   risc%d:  6cx23885: %s:   %04d:  7cx23885: %s: %s: %p
 7cx23885: %s: %s()
 cancel 6cx23885: %s: Unsupported
 insmod option autodetected &dev->lock &dev->gpio_lock cx23885[%d] 7cx23885: %s: %s(portno=%d)
 &port->frontends.lock 0.0.4 cx23885 12 13 14 resync cnt0 cnt1 18 19 20 21 22 23 irq1 irq2 eol sol write skip writec jump sync read readc writerm writecm writecr init risc lo init risc hi cdt base cdt size iq base iq size risc pc lo risc pc hi iq wr ptr iq rd ptr cdt current pci target lo pci target hi line / byte VID A VID A (VBI) TS1 B ch4 ch5 TS2 C TV Audio ch8 ch9 ch2 7cx23885: %s: i2c: R 7cx23885: %s: i2c: <R %02x 7cx23885: %s: i2c: %02x 7cx23885: %s: i2c: >
 3cx23885:  ERR: %d
 7 <W %02x %02x c >
 c %02x ??? 6cx23885: %s: scan bus %d:
 ir_video tda10048 dib7000pc lgdt3303 cx24227 cs3308 tda8295 tda9887 cx25837 flatiron eeprom tuner/mt2131/tda8275 tuner/xc3028L %s: Invalid parameter
 sp2 s5h1409_attach symbol:s5h1409_attach mt2131_attach symbol:mt2131_attach lgdt3305_attach symbol:lgdt3305_attach tda18271_attach symbol:tda18271_attach s5h1411_attach symbol:s5h1411_attach tda829x_attach symbol:tda829x_attach lgdt330x_attach symbol:lgdt330x_attach simple_tuner_attach symbol:simple_tuner_attach xc5000_attach symbol:xc5000_attach xc2028_attach symbol:xc2028_attach tda10048_attach symbol:tda10048_attach dib7000p_attach symbol:dib7000p_attach zl10353_attach symbol:zl10353_attach dib0070_attach symbol:dib0070_attach xc4000_attach symbol:xc4000_attach cx24116_attach symbol:cx24116_attach cx24117_attach symbol:cx24117_attach ds3000_attach symbol:ds3000_attach ts2020_attach symbol:ts2020_attach stv0900_attach symbol:stv0900_attach stv6110_attach symbol:stv6110_attach symbol:lnbh24_attach lnbh24_attach 3cx23885: No LNBH24 found!
 lgs8gxx_attach symbol:lgs8gxx_attach mb86a20s_attach symbol:mb86a20s_attach atbm8830_attach symbol:atbm8830_attach max2165_attach symbol:max2165_attach stv0367ter_attach symbol:stv0367ter_attach stv0367cab_attach symbol:stv0367cab_attach drxk_attach symbol:drxk_attach mt2063_attach symbol:mt2063_attach stv090x_attach symbol:stv090x_attach stb6100_attach symbol:stb6100_attach tda10071_cx24118 tda10071 a8293 si2165 m88ds3103_attach symbol:m88ds3103_attach ts2020 si2168 si2157 m88ds3103 m88rs6000t %s lgdt3306a_attach symbol:lgdt3306a_attach xc3028-v27.fw xc3028L-v36.fw %s/2 Television 7cx23885: %s: 417:%s()
 PCIe:%s UNKNOWN v4l-cx23885-enc.fw NTSC-M mpeg %s (%s) PING_FW START_CAPTURE STOP_CAPTURE SET_AUDIO_ID SET_VIDEO_ID SET_PCR_ID SET_FRAME_RATE SET_FRAME_SIZE SET_BIT_RATE SET_GOP_PROPERTIES SET_ASPECT_RATIO SET_DNR_FILTER_MODE SET_DNR_FILTER_PROPS SET_CORING_LEVELS SET_SPATIAL_FILTER_TYPE SET_VBI_LINE SET_STREAM_TYPE SET_OUTPUT_PORT SET_AUDIO_PROPERTIES HALT_FW GET_VERSION SET_GOP_CLOSURE GET_SEQ_END SET_PGM_INDEX_INFO SET_VBI_CONFIG SET_DMA_BLOCK_SIZE GET_PREV_DMA_INFO_MB_10 GET_PREV_DMA_INFO_MB_9 SCHED_DMA_TO_HOST INITIALIZE_INPUT SET_FRAME_DROP_RATE PAUSE_ENCODER REFRESH_INPUT SET_COPYRIGHT SET_EVENT_NOTIFICATION SET_NUM_VSYNC_LINES SET_PLACEHOLDER MUTE_VIDEO MUTE_AUDIO MISC NTSC-JP PAL-BG PAL-DK PAL-I PAL-M PAL-N PAL-Nc PAL-60 SECAM-L SECAM-DK rc-hauppauge rc-nec-terratec-cinergy-xs rc-tevii-nec rc-total-media-in-hand-02 rc-tbs-nec rc-dvbsky rc-tt-1500 cx23885 IR (%s) pci-%s/ir0 cx23885 rby     tby ror rto rsr tsr roe rte rse tse 7%s: rx read: end of rx
 (timed out) space  mark yes no enabled disabled not loaded overflow marker not empty half full or greater loopback active normal receive half full or less space:burst mark:noburst space:noburst mark:burst 6%s: IR Receiver:
 6%s: IR Transmitter:
 %s/888-ir &state->rx_params_lock &state->tx_params_lock ??? edge falling edge rising edge rising & falling edges ctl read mem write 7cx23885: %s: %s:
 7cx23885: %s: %s
 7cx23885: %s: alsa: %s()
 include/linux/scatterlist.h CX23885 Digital CX23885 Conexant CX23885 %s at %s parm=enable_885_ir:Enable integrated IR controller for supported
		    CX2388[57] boards that are wired for it:
			HVR-1250 (reported safe)
			TerraTec Cinergy T PCIe Dual (not well tested, appears to be safe)
			TeVii S470 (reported unsafe)
		    This can cause an interrupt storm with some cards.
		    Default: 0 [Disabled] parmtype=enable_885_ir:int parm=netup_card_rev:NetUP Dual DVB-T/C CI card revision parmtype=netup_card_rev:int parm=vid_limit:capture memory limit in megabytes parmtype=vid_limit:int parm=irq_debug:enable debug messages [IRQ handler] parmtype=irq_debug:int parm=video_debug:enable debug messages [video] parmtype=video_debug:int parm=vbi_nr:vbi device numbers parm=video_nr:video device numbers parmtype=vbi_nr:array of int parmtype=video_nr:array of int license=GPL author=Steven Toth <stoth@linuxtv.org> description=v4l2 driver module for cx23885 based TV cards parm=vbi_debug:enable debug messages [vbi] parmtype=vbi_debug:int parm=vbibufs:number of vbi buffers, range 2-32 parmtype=vbibufs:int parm=card:card type parmtype=card:array of int parm=debug:enable debug messages parmtype=debug:int parm=dma_reset_workaround:periodic RiSC dma engine reset; 0-force disable, 1-driver detect (default), 2-force enable parmtype=dma_reset_workaround:int version=0.0.4 license=GPL author=Steven Toth <stoth@linuxtv.org> description=Driver for cx23885 based TV cards parm=i2c_scan:scan i2c bus at insmod time parmtype=i2c_scan:int parm=i2c_debug:enable debug messages [i2c] parmtype=i2c_debug:int parm=adapter_nr:DVB adapter numbers parmtype=adapter_nr:array of short parm=alt_tuner:Enable alternate tuner configuration parmtype=alt_tuner:int firmware=v4l-cx23885-enc.fw parm=v4l_debug:enable V4L debug messages parmtype=v4l_debug:int parm=mpeglinesize:number of bytes in each line of an MPEG buffer, range 512-1024 parmtype=mpeglinesize:int parm=mpeglines:number of lines in an MPEG buffer, range 2-32 parmtype=mpeglines:int parm=mpegbufs:number of mpeg buffers, range 2-32 parmtype=mpegbufs:int parm=ir_888_debug:enable debug messages [CX23888 IR controller] parmtype=ir_888_debug:int parm=ci_irq_enable:Enable IRQ from CAM parmtype=ci_irq_enable:int parm=ci_dbg:Enable CI debugging parmtype=ci_dbg:int parm=audio_debug:enable debug messages [analog audio] parmtype=audio_debug:int parm=disable_analog_audio:disable analog audio ALSA driver parmtype=disable_analog_audio:int    <           ¤ÿ    L           ¤ÿ ŒF  0          ¤ÿ äI  <          ¤ÿ t  H          ¤ÿ x  T          $ÿ \  p          $ÿ |            ¤ÿ |            ¤ÿ @J  (	          $ÿ 0	  D	          ¤ÿ „  L	          ¤ÿ `N            $ÿ ˆ  (          ¤ÿ Œ  P          $ÿ \  p          ¤ÿ    ¼          ¤ÿ ¤  È          ¤ÿ tY  Ø          ¤ÿ xY  ä          ¤ÿ |Y  Ğ          ¤ÿ ¨            ¤ÿ ¬             ¤ÿ °  `          ¤ÿ ´  l          ¤ÿ ¸  èC  ƒ`  Ö’  å’  ø’  “  E“  ]“  j“  w“  „“  Í“  ø“                             p  4    p  v   p  x   p x   p 	x   p y   ¬ Õ   p w   p —w   p w   p w   p Ñq   p Óq   p    p €	   ¬Ö
   ¬xÛ   }f   }!o*   }9o   [ è    iˆˆ   €iˆˆ)   iˆˆ(   pÔ"        U,*   p "   p "   p "   p Q"   p Y"$   p ‘"   p •"   p ™"   p "   p ğ"   p ñ"   p ò"   p ó"   p ô"   p õ"   ñQ†   ñW†   p A…   X è   p Q…   ñx…   }"o   TV#   Uäâ   ñ…!   ;~"   qÔ"#    €40%   p Á&   p 8Á&   p *Á5   p øÁ&   a9Ù'   p 3q+   p 7q+   ¬˜Û,   TB€•-   TB˜.   TB•/   Â00   TBP	1   TBR	2   TB‚	3   p 8ğ4   v`6   v`7   p (j8   p (k8   p j9   p k9   p *:   p *ğ;   a 1>   R                                                                                                                                                                                                                                                                            b                                                      ÿ              
           ÿ                        ÿ                        ÿ                                                                                                                                      |                    6       B               €                        €                       €                                                                                                                                                                       ”                                              €   ÿ                  €   ÿ                                                                                                                                                                                             ¬                                                                                                                                                                                                                                                                                  Æ                                                                                                                                                                                                                                                                                  ß                     G       a                €                         €                        €                                                                                                                                                                        ÷                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   '                                                                                                                                                                                                                                                                                 ´                                                                                                                                                                                                                                                                                Ô                                                                                                                                                                                                                                                                                ?                                                                                                                                                                                                                                                                                 [                                                                                                                                                                                                                                                                                 r                                                                                                                                                                                                                                                                                 †                                                                                                                                                                                                                                                                                 ‘                                                                                                                                                                                                                                                                                 ¦                                                                                                                                                                                                                                                                               »                                                                                                                                                                                                                                                                                 Ó                                                                                                                                                                                                                                                                                 ë                           B               €                        €                       €                                                                                                                                                                                                                                                                                                                                                                                                                                                                           L       a                                                                 0                           €                                                                                                                                            /                    L       a                                                                 0                           €                                                                                                                                            K                          B               €                        €                       €                                                                                                                                                                      c                                                                                                                                                                                                                                                                                 y                                                                                                                                                                                                                                                                                 ‘                                                                                                                                                                                                                                                                                ©                     G       a             ‘  €                                                 0                         f €                                                                                                                                            Â                    L       d               €                                                0                                                                                                                                                                          ×                 L       d                                                                                                                                                                                                                                                ø                    W   ÿÿÿÿaÿ             ‘  €                                                 0                         f €                                                                                                                                            ğ                                                                                                                                                                                                                                                                     ø                    L       a                                                               0                          €                                                                                                                                                                                                                                                                                                                                                                                                                           /                                                                                                                                                                                                                                                                                 ë                           B               €                       €                                                                                                                                                                                                  :                                                                                                                                                                                                                                                                                                    S       `                                                                                                                                                                                                                                                  <                     G       a             Ñ  €                      7 €                      ğ  €                                                                                                                                                                       V                                                                                                                                                                                                                                                                                j                                                                                                                                                                                                                                                                                ~                     G       a               €   @@p                      @@p                0     @@p                f €   @@p                                                                                                                                     ˜                                             €                       €                                                                                                                                                                                                   `                                                                                                                                                                                                                                                                                ®                                                                                                                                                                                                                                                                                »                                                                                                                                                                                                                                                                                 È                                                                                                                                                                                                                                                                                 ˆ                                                                                                                                                                                                                                                                                 Õ                                                                                                                                                                                                                                                                                 á                                                                                                                                                                                                                                                                                í                                                                                                                                                                                                                                                                                ù                                          €                        €                       €                                                                                                                                                                                                                                                                                                                                                                                                                                                       +                                              €                       €                      $ €                                                                                                                                                                      9                                              €                       €                       €                        €                                                                                                                                          G                                          €                                                                                                                                                                                                                              b                                          €                                                                                                                                                                                                                               ¬                                           €                       €                                                                                                                                                                                                   ~                                                                                                                                                                                                                                                                                 Ğ                                                                                                                                                                                                                                                                               ğ                                                                                                                                                                                                                                                                               ™                                            °  €                      7 €                                                                                                                                                                                                                                                                                                                                                                                                                                                     À                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cx23885-video                                                           ÷r         à                              YUYV                                     ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ                                             ^   G                                                                                                                                          0     ° ° @    (     @  €  À      6  P  ğ   0       D  „  Ä      B     0 p  P       H  ˆ  È      H                        L  Œ  Ì      L                        P    Ğ      P  @ p Ğ  `       P    Ğ      V   ° 0	  p       T  ”  Ô      _                        X  ˜  Ø      c                        \  œ  Ü                          0     € À @    (     @  €  À      g                        D  „  Ä      B       €  P       H  ˆ  È      H                        L  Œ  Ì      L                        P    Ğ      P  @ @ à  `       P    Ğ      V   €  
  p       T  ”  Ô      _                        X  ˜  Ø      c                        \  œ  Ü                          ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ                               ,  @               Ğ      ,  @               Ğ      :  @               P     ,  @                                             `O  @O  ïüÿÿ                                    `ê  ˜:     Ò    8 ·  j(X
   v   Êÿÿ  ÿÿ   (·ÎÿH˜XZ3                         `   Po  €o      &      à.                                      Y  ²                      `       g                       )                      *                                      a   ”              d   ”                                         Àü›”                Àü›”             `   D    Àv      .ÿ `   D     Àv      .ÿ a   d                  Àv  d                            ²                      Y  ²                      `   Àü›                                      Àü›j                                             @p  p  Ào  o                              a                         a                     Àv                          `   |                h           U   U               c    $ô   `    $ô           h   z   àR               0ö  €õ@ €ó@ sõÀ sóÀ rõà róà tõ  tó  ÿÿÿ         ²                     €S  `S  ÿÿ  ÿÿ                                     `ê  0u      Ò&¿X8€ÃÉ d È   v   3     &›Ê&      €3                          ²                           T                                                         ²S°             W°                                                            ÀT                         U                                                                                äSnØWnÌ[n                                                o             ƒo                   d                 a                    à«   à«à«    à«     à«       @                   a             à«          Èä¬ €>        ÈäØÌ€>         à«    ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ                                                                                                                                                                                                                                                            `                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cx23885                                                                 ÷r         €                                         $#  ü  Ÿ  ÈK  ¨  ß  M  ¨  Õ   N  ¨  ÷   O  ¨  '  ®R  ¨  å  Í`  ¨  …   Ì  <<    ¢Í  ı  p   ˆÎ  <<  v  Î  <<  w                               GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                     
                                                                                                                                                                       ñÿ                   –    0       €     F   ñ        b          
           m           }                        ˜       	    
               ŒF       ¦     Ì     ¾          
 É       G    æ   G                    <       
 7  b  8     U  š       w            L       
 ©           ñÿ¹    1     Å  Ğ  ş     N	  x       Ò  Ú  D     ä  4      
 ğ  Ğ  Š       `  E       °  ˆ     ,  @  €     ;  À       J  P  g     ]  À  W     r      7              ‘  Œ       
 œ  `   \     ±  À   &     É  ÔI       Ñ  ğ        ß  !       î  :  ~             
   0#  ë     $   $  ä    9  ¸  ;     ¾   t       
 S  &  Ğ     c  ó       x  0      
 „  (      
   à&  g     ¬    (     Í  ¬      
 Ù  P'  —     ü  À       
   /  0     b   Ì       
 ,  ğ'       A   (  U     S  _       j  ä       
 u  `(  ‘     “  |       ¶  `       
 Á   )  «     Ù  ˜       ö        
   °)  -       ´       %        
 1  ˜      
 =  °+  6     J  Ñ       \  œ       
 g  í       ‚  €      
   à,  ñ     ª    p     Ë  Ô      
 ×  ¼      
 ã  à-  ï    ü  s  @       ì      
 &  Ğ/  P     ?  ³  %     ]  ¨      
 i   0       {  Ø       ’  ø       
   @  ,    
 ¦  ÷  0     ¾  l      
 Ê  Ğ1  ,     Ü  '       ó  ¬       
 ş  C         `  O     ,         
 8   2       G  ¯  @     b  Œ      
 n  @6  
       ï  }     ˜  x      
 ¤  l       Â  \      
 Î  @  4    ã   F  4    ú     0    
 
   J          ˆ  ^    Â	   J        )  D      
 5  À  $    
 @  à  Ü   
 P  ¶  1     i  ç       †  (        ˜  0  
    
   äI       ®  ş  3     Ç  1       ä  <        ö  <  
    
 ì  t       	  H  /     '	  w       F	  P        Z	  H      
 r	         ˆ	  ¯  #      	  Ò       º	  d        É	  T      
 Ü	  \      
 ï	  ï       
  x        
  p  	    
 1
  |      
 F
         ]
    '     s
  A  :     
           ñÿ¹  `=  1     Å   =  %    ï
  |       Ò  æ  6     ¾   ¸      
 ğ  Ğ>  Š     Ñ  `?       ß  €?  ‚     Á  @  ª     Ù         ¶         
 œ
  8       ‘  È      
 ±
  {  +     Ê
  ¦       ç
  Œ        ù
    
    
   ½  /     &  ì       A           Q        
 I  @J       e           ñÿt  °A  0     ‰  T  „          @    
 ¥  À  @    
 ¬  €  @    
 ³  àA  ¶    Æ   C  `    İ   E  A     õ  PE  ¦        F  ¸     ™   „       +  Ø  F     ]  Ä      
 K  ÀF  Ò     \   G  \     l  	  š     \  x      
 Œ  @  8    
 “  ĞK  ‹     ¢  „      Ë  ¨      
 ¶  š  ‹    ’  d      
   L      
 Í  %  $     ö  8      
 æ  I  N     ä  $      
 ı  —  h     )        
 Â  ø      
 ˜  ä      
 b  Ô      
   ÿ  {    .  ĞV  ‰    :  z  é    ×  ¸      
 K  c  0     a  “  0     y  Ã  2       õ       ¨   ^  â    y  `N       ¸   	  (    
 Æ  €        Ï  €        Ø  €       é   L  Œ    °  @N        ÿ  €           K  Œ        Ã    „  €      
 %  ”      
 3  l      
 ¾  \      
 ‚  @      
 ?      *     L  `J  ”     _      
     l     `    
 |                  ¨  ´        µ  (	      
 Æ  0	      
 ×  0  !     ì  Q         È          D	      
 %  d  u     I  Ù  "     q  Ü          L	      
 ¯  û       F
  	       ]
    '     s
  <  .     Æ           ñÿÔ  g       ê   g  M     ø  ğg  |    Q  Œ         Ğ!      ó  °	      
   pi  E    '  Û"      \   	      
 :  Àk  Ê     C  Ş#  S     Q  À	  	    
 \         
 õ  ˆ       v  Ì	      
 ‚  1$  í     j  Ğ	      
 œ   
      
 ¾   	      
 ‘  ˜	      
 ¥  %  %     ¶  €	      
 ¹  j  *     Ñ  ”       í  ğ        ş    	    
   ª  +     ,  Õ       I         [  (  
    
 q           ñÿ¹  Àn  @        o  J     ‘  Po  &     ¥  €o       ¹  o  %     Ï  C%       j  ˆ      
 ê  Ào  G        Y%       b   p      
   p  %     1  o%       Q  X      
 L  @p  G     b  …%       ó  @      
 }  p  V     Å  ğp       ğ  q       Á   q  "     Ñ  Pq       ß  pq       “  €q  Û     ±  `r  ª     Ê  s       ã  ›%  Å    ÷         ÿ  u  ë       s  }    P2  `(  B    (  V       A  ÀS  $     [  äS       t  pU  
     ‡  T       O          ¡  ÔU       ¸  <U       É  ÈU       ä  ¸U          U         ”U       1  \U       P  ¼U       i  ÔN       p  ôU       ‰  tT       ¨  àU       Á  XT       à  ˆU       õ  |U  
     
  HU       #  S       ;  œN       B  €N       H  R       [  ŒR       n  €R       ‚  hR       –  XR       ®  ÀR        Ã  ¬R       ß   R       û  @R         ,R       2  $R       O  R       j  Q       Œ   Q       ­  ìP       Ë  èP       ç  ÔP         ĞP       !   P  0     6  `P  (     J   P  0     _   P       v   Q  p       €Q       ¨  <T       Ç  ø      
 å  à      
   È      
 ’         
 &  `Q        E  @Q        d  àO        Ø  V          S  ,        ¸N       §   O  ,     À   O  ,     Â  °      
 Ø     0    
 á  `O  0     ô  @O          àR  (       €S  0     /  `S        @   T  <     Z  ÀT  <     }   U  <     ˜  ì  $     ²    #     Ğ         ã  P      
 ú  \      
   3  4     *  g       G  ,       Y  p  
    
 o           ñÿ¹  àv  D     ã  tY       G  xY       £  |Y       ±  0w       Å  Pw       Ñ  pw       ß  w         °w  ™       Px  q     ,  Ğx  a     r  @y  (       py  P       ¤       }  ­k  #       ¨      
 œ  Ày  c     ™  Ğk  #     j  x      
 n  0z  
     ;  @z       ³  ók       Â  ä      
 8  Ğz  
       àz  
     Ê  ğz  ,     Ü  l       ö  À      
 =   {  °     Ç  à  Ü    
 S  Ğ{  ä     c  +l       )  Ü      
 ä  Ô      
 $  À|  P     9  ?l       ’        
 ×  ^l         d      
 ï  À„  è      zl  ñ     ]  $      
   €  t   
 #  °†  »     3  km        ×  8      
 H  p‡  
     Y  €‡  ©    Q  D      
 r  ‹m  _    ¾         
 ó  ,      
 ¶  ô      
 Á  0  Ã       êo  $     Ë  H      
 ğ     >     ¯  p       b        
 Ë  @V  4    á     0    
 î  *p       ˜  ô      
 b   `      
   $      
   ~       (  `  $    
 2  €  Ü   
 A  š  )     Z  Ã       w  @       ‰  ¼  
    
 Ÿ  Ú  Q     »  +       Û  T       ğ  È      
 	  E  =     "  ‚       ?  h       Q  Ø  
    
 g  ™  1       Ê       ›  |       ¬  ä  	    
 Á           ñÿÑ           ñÿŞ           ñÿë           ñÿû  ğ”  _      P—  8    4  ˜  i    J   š  !     a           ñÿn  ĞŸ  5     ‚     Œ           Œ     ¸  «p  E     Ü  0¡  {     ú  °¡  ›       P¢  ü    ’   ¨       /  ğp  ‹    K  P¥  Ï    ^  {r  ±     v   §  Ú    ‘   ¬  º    ¬  À±  ‹    Â     4    
 Í  ,s      è        
 ó         
 ş  `       
    ¨            ¨        (      0    
 <   €      
 N   à  @     j           Š          Ÿ   Ğ      
 ¸            ñÿÅ   Ğµ  ‹     Ñ   8w  ,     ¾   ğ      
 â   dw  X     ¶  à      
 ‘  ü      
 ø            ñÿ!  À·  ı     Ï"  °       !  ¼w  $     ó  `      
 &!  p¹  ª     A!  àw  R     Q  p      
 a!   º  “     v!  2x  *     ¶        
 !  Àº  %    §!  \x  m     b   €      
 Ã!  Éx  "     o"  ¬       İ!  ëx  A     ‘  4      
 ü!  ,y  Y     \  H      
 "  …y  q     ¾   $      
 )"  :  '     F"  a       g"  ¤       }"        
 —"  |        ­"  œ       Ç"  ¸       Ö"         
 é"           ñÿø"  öy       #  z       '#           ñÿ6#  ÀÃ  ’     D#  `Ä  ´     S#   z       ¶  ¨  
    
 i#           ñÿx#  àÇ       Š#  ğÇ       #  È       ¯#  0È  $    *%  ´       È#  ;z  6     \  Ø      
 æ#  `Ê  ò     û#  qz  /     $  `Ë  Â     ,$   z       H$  0Ì  .     \$  `Ì  P    r$  ¹z  d     ‘  À      
 $  °Î       ¡$  {  W     ¢%  ¸       ¸$     @    
 Ì$  t{  I     ó  ğ      
 è$  °  6     %  æ       "%  Ì       6%  `      
 N%  ÿ  ;     r%  :	  "     š%  à       ·%  l      
 Ø%             ç%             ı%             &  V  ¾     &             (&             =&             L&             ^&             k&             |&             &  }  J     £&             µ&             É&  `½       å&             õ&             	'  @  )     '             .'             :'             F'             O'  À@       _'             m'             '             Œ'             ¡'             ¸'             Ä'  ¸	  Ì    Ş'             é'             ù'             (             )(  P,  †     ?(             Q(             _(    y    r(             (             ”(             ¨(             ¹(             Ï(             ì(     `    
 )   *  ¬    ')             /)  P6  æ     A)      
     P)  @  O	    c)             s)             y)  ğ]  ª     )              )  `¶  G    ±)  `S  ÿ     Ã)             Ô)             î)             *             *  0n       *             **  `]       ;*  @       K*   n       b*             y*             ˆ*             Ÿ*  ğ7  a    ¶*             Ï*             å*             ó*             +  0N  Ù     +             &+     Q     5+  4       
 E+      *     Q+             f+   0  -    y+             „+              +  0  c     º+  pÏ  f    Ñ+             ê+  ÀR  ‘     ş+             ,             #,   M  ‰     ;,  àH  E     N,  àĞ       g,             x,             †,  À  d     ˜,             ¨,  °·       ¿,             Ö,             ì,             ô,  €½  ¼     	-             -  Ğ  l     9-             H-             P-             Z-   Å  ¾    k-             {-             †-   v  Ş     -             °-             Á-  ğ¼  $     Ø-             å-             .  ÀÎ  ®     .  €   q     '.             4.             J.  p  ¤    ].             m.             †.             œ.  0š  )    º.   ½       Ğ.             å.             ó.  P“  g     /  ‚  "    #/  `œ      6/  ”  Ñ     N/             d/             {/             †/  à)       ›/             ´/             Ç/             ×/  Ğ’  f     ò/  @½       0             *0             80             H0  à  W     _0  `\  y     p0             Š0  ğ+  _     ¡0             ±0             ¹0             Ñ0  @7  ¢     ê0             1             1             &1     ìC    51             G1             V1             c1             1             ’1      €    ¡1             ±1   ¿  ¡     Æ1  @¾  Ü     à1  À¸  ¬     ô1             2  O  7     2  à\  }     *2             12  `T  ­    H2  xj  5    ]2  pŸ  R     p2  °2      †2             •2             ­2             Ç2  à  0    
 Ø2             å2             ú2             3   H  A     3             +3             33             O3             b3  P³      s3  `L  `     ƒ3  02  g     ü2             •3  l      ª3  `  S     »3             Ã3  PÂ       Õ3  PH  C     è3             ÿ3  à~  "    4             "4             (4             54  pµ  X     G4             _4  À“  B     }4             Œ4  PA  S     œ4             ±4             ¹4             Á4             Ë4         Û4  Â  9     é4             ü4  @“       5             $5             95             J5             _5             x5  8       
 ‡5             5             «5  `Ã  Q     ¿5             Ö5             é5             6  `}  y    6             $6             >6             S6             m6             €6             —6  àÂ  €     ª6  PO  e    ¼6             Ğ6             å6   2  -     ÷6             7             7             "7  ÀL  Ù     67  0I  š    Q7   H  =     e7             v7             …7             ‘7  @  Š    ¦7  Ğ¿  6    ´7             Â7             É7             Ş7             õ7             8             8             &8             78             A8             L8              cx23885-cards.c netup_jtag_io hauppauge_eeprom hauppauge_eeprom.cold cx23885_tuner_callback.cold __func__.6 ir_rx_pin_cfg.5 ir_rxtx_pin_cfg.4 eeprom.3 buf.0 cinfo.2 cx23885_card_setup.cold __func__.1 __UNIQUE_ID_enable_885_ir362 __UNIQUE_ID_enable_885_irtype361 __param_enable_885_ir __param_str_enable_885_ir __UNIQUE_ID_netup_card_rev360 __UNIQUE_ID_netup_card_revtype359 __param_netup_card_rev __param_str_netup_card_rev cx23885-video.c queue_setup buffer_queue buffer_queue.cold __func__.11 cx23885_stop_streaming vidioc_log_status vidioc_g_frequency vidioc_s_tuner vidioc_g_tuner vidioc_g_selection vidioc_g_pixelaspect vidioc_g_std vidioc_g_std.cold __func__.2 vidioc_g_fmt_vid_cap vidioc_enum_fmt_vid_cap formats buffer_finish buffer_prepare buffer_prepare.cold __func__.9 vidioc_try_fmt_vid_cap vidioc_s_fmt_vid_cap vidioc_s_fmt_vid_cap.cold vidioc_querycap vidioc_querycap.cold __func__.23 __func__.22 cx23885_video_wakeup.part.0 cx23885_video_wakeup.part.0.cold __func__.28 cx23885_query_audinput.constprop.0 iname.5 cx23885_query_audinput.constprop.0.cold vidioc_enum_audinput vidioc_g_audinput vidioc_g_audinput.cold __func__.7 cx23885_vdev_init.constprop.0 cx23885_vdev_init.constprop.0.cold __func__.0 cx23885_start_streaming cx23885_start_streaming.cold __func__.10 vidioc_g_input vidioc_g_input.cold __func__.21 __func__.27 vidioc_s_std vidioc_s_std.cold __func__.3 cx23885_flatiron_read.cold __func__.26 cx23885_flatiron_mux.isra.0 cx23885_flatiron_mux.isra.0.cold __func__.18 __func__.17 cx23885_video_mux.isra.0 cx23885_video_mux.isra.0.cold __func__.19 cx23885_audio_mux.isra.0 cx23885_audio_mux.isra.0.cold __func__.16 vidioc_s_audinput vidioc_s_audinput.cold __func__.8 iname.24 cx23885_enum_input.cold __func__.25 vidioc_enum_input vidioc_enum_input.cold __func__.4 cx23885_get_input.cold cx23885_set_input.cold __func__.20 vidioc_s_input cx23885_set_frequency.cold __func__.15 vidioc_s_frequency cx23885_video_irq.cold __func__.14 cx23885_video_unregister.cold __func__.13 cx23885_vbi_template cx23885_video_template cx23885_video_qops cx23885_video_register.cold __func__.12 video_fops video_ioctl_ops __UNIQUE_ID_vid_limit371 __UNIQUE_ID_vid_limittype370 __param_vid_limit __param_str_vid_limit __UNIQUE_ID_irq_debug369 __UNIQUE_ID_irq_debugtype368 __param_irq_debug __param_str_irq_debug __UNIQUE_ID_video_debug367 __UNIQUE_ID_video_debugtype366 __param_video_debug __param_str_video_debug __UNIQUE_ID_vbi_nr365 __UNIQUE_ID_video_nr364 __UNIQUE_ID_vbi_nrtype363 __param_vbi_nr __param_str_vbi_nr __param_arr_vbi_nr __UNIQUE_ID_video_nrtype362 __param_video_nr __param_str_video_nr __param_arr_video_nr __UNIQUE_ID_license361 __UNIQUE_ID_author360 __UNIQUE_ID_description359 cx23885-vbi.c cx23885_vbi_irq.cold __UNIQUE_ID_vbi_debug362 __UNIQUE_ID_vbi_debugtype361 __param_vbi_debug __param_str_vbi_debug __UNIQUE_ID_vbibufs360 __UNIQUE_ID_vbibufstype359 __param_vbibufs __param_str_vbibufs cx23885-core.c cx23885_irq_get_mask cx23885_risc_decode instr.5 bits.6 incr.7 cx23885_risc_field cx23885_dev_unregister cx23885_v4l2_dev_notify cx23885_clear_bridge_error cx23885_wakeup.constprop.0 cx23885_wakeup.constprop.0.cold cx23885_shutdown cx23885_finidev cx23885_sram_channel_setup.cold name.4 cx23885_irq_ts cx23885_irq_ts.cold cx23885_start_dma.cold cx23885_buf_prepare.cold cx23885_buf_queue.cold cx23885_cancel_buffers.cold cx23885_irq_417.cold cx23885_irq cx23885_irq.cold cx23885_gpio_set.cold cx23885_gpio_clear.cold cx23885_gpio_get.cold cx23885_gpio_enable.cold cx23885_initdev broken_dev_id __key.19 __key.20 cx23885_devcount cx23885_sram_channels __key.23 cx23887_sram_channels cx23885_initdev.cold __func__.24 cx23885_init cx23885_pci_driver cx23885_fini cx23885_pci_tbl __UNIQUE_ID_card368 __UNIQUE_ID_cardtype367 __param_card __param_str_card __param_arr_card __UNIQUE_ID_debug366 __UNIQUE_ID_debugtype365 __param_debug __param_str_debug __UNIQUE_ID_dma_reset_workaround364 __UNIQUE_ID_dma_reset_workaroundtype363 __param_dma_reset_workaround __param_str_dma_reset_workaround __UNIQUE_ID_version362 cx23885-i2c.c cx23885_functionality i2c_wait_done i2c_readbytes i2c_readbytes.cold i2c_sendbytes i2c_sendbytes.cold i2c_xfer i2c_xfer.cold __func__.5 cx23885_i2c_algo_template addr_list.6 cx23885_i2c_register.cold i2c_devs cx23885_av_clk.cold __UNIQUE_ID_i2c_scan362 __UNIQUE_ID_i2c_scantype361 __param_i2c_scan __param_str_i2c_scan __UNIQUE_ID_i2c_debug360 __UNIQUE_ID_i2c_debugtype359 __param_i2c_debug __param_str_i2c_debug cx23885-dvb.c p8000_set_voltage dib7070_tuner_reset dib7070_tuner_sleep stb6100_get_bandwidth stb6100_get_bandwidth.cold stb6100_set_bandwidth stb6100_set_bandwidth.cold stb6100_get_frequency stb6100_get_frequency.cold stb6100_set_frequency stb6100_set_frequency.cold cx23885_dvb_gate_ctrl dvbsky_s952_portc_set_voltage dvbsky_t9580_set_voltage cx23885_dvb_set_frontend dvb_register_ci_mac cinfo.0 netup_altera_fpga_rw cx23885_sp2_ci_ctrl hauppauge_generic_config hauppauge_lgdt3305_config hauppauge_hvr127x_config hcw_s5h1411_config hauppauge_tda18271_config hauppauge_ezqam_config tda829x_no_probe hauppauge_hvr1800lp_config hauppauge_generic_tunerconfig fusionhdtv_5_express hauppauge_hvr1500q_config hauppauge_hvr1500q_tunerconfig hauppauge_hvr1500_config ctl.12 hauppauge_hvr1200_config hauppauge_hvr1200_tuner_config hauppauge_hvr1210_config hauppauge_hvr1210_tuner_config dvico_s5h1409_config dvico_s5h1411_config dvico_xc5000_tunerconfig dvico_fusionhdtv_xc3028 ctl.10 ctl.9 tbs_cx24116_config tbs_cx24117_config tevii_ds3000_config tevii_ts2020_config dvbworld_cx24116_config netup_stv0900_config netup_stv6110_tunerconfig_a netup_stv6110_tunerconfig_b mygica_x8506_lgs8gl5_config mygica_x8506_xc5000_config mygica_x8507_mb86a20s_config mygica_x8507_xc5000_config magicpro_prohdtve2_lgs8g75_config magicpro_prohdtve2_xc5000_config mygica_x8558pro_atbm8830_cfg1 mygic_x8558pro_max2165_cfg1 mygica_x8558pro_atbm8830_cfg2 mygic_x8558pro_max2165_cfg2 netup_stv0367_config netup_xc5000_config terratec_drxk_config terratec_mt2063_config prof_8000_stv090x_config prof_8000_stb6100_config hauppauge_hvr4400_tuner_config dvbsky_t9580_m88ds3103_config dvbsky_s950c_m88ds3103_config hauppauge_hvr5525_m88ds3103_config hauppauge_quadHD_ATSC_a_config hauppauge_quadHD_ATSC_b_config hauppauge_hvr1265k4_config hauppauge_hvr1400_dib7000_config ctl.11 dib7070p_dib7000p_config dib7070p_dib0070_config dvb_qops dib7070_agc_config dib7070_bw_config_12_mhz stv0900_ts_regs xc3028_agc_config xc3028_bw_config hauppauge_hvr127x_std_map hauppauge_hvr1200_tda18271_std_map hauppauge_tda18271_std_map __UNIQUE_ID_adapter_nr362 __UNIQUE_ID_adapter_nrtype361 __param_adapter_nr __param_str_adapter_nr __param_arr_adapter_nr __UNIQUE_ID_alt_tuner360 __UNIQUE_ID_alt_tunertype359 __param_alt_tuner __param_str_alt_tuner cx23885-417.c vidioc_try_fmt_vid_cap.cold vidioc_g_fmt_vid_cap.cold vidioc_g_tuner.cold cx23885_tvnorms cx23885_mc417_init.cold cx23885_mbox_func cx23885_mbox_func.cold CSWTCH.127 cx23885_api_cmd cx23885_api_cmd.cold cx23885_api_func cx23885_initialize_codec cx23885_initialize_codec.cold cx23885_417_check_encoder.cold cx23885_417_unregister.cold cx23885_mpeg_template cx23885_qops cx23885_417_register.cold magic.3 __UNIQUE_ID_firmware367 mpeg_fops mpeg_ioctl_ops __UNIQUE_ID_v4l_debug366 __UNIQUE_ID_v4l_debugtype365 __param_v4l_debug __param_str_v4l_debug __UNIQUE_ID_mpeglinesize364 __UNIQUE_ID_mpeglinesizetype363 __param_mpeglinesize __param_str_mpeglinesize __UNIQUE_ID_mpeglines362 __UNIQUE_ID_mpeglinestype361 __param_mpeglines __param_str_mpeglines __UNIQUE_ID_mpegbufs360 __UNIQUE_ID_mpegbufstype359 __param_mpegbufs __param_str_mpegbufs cx23885-ioctl.c cx23885-ir.c cx23885-av.c cx23885-input.c cx23885_input_ir_open cx23885_input_process_measurements cx23885_input_ir_stop cx23885_input_ir_close cx23888-ir.c cx23888_ir_tx_write cx23888_ir_tx_g_parameters cx23888_ir_rx_g_parameters pulse_width_count_to_us.constprop.0 cx23888_ir_tx_shutdown.isra.0 cx23888_ir_rx_shutdown.isra.0 cx23888_ir_irq_handler cx23888_ir_irq_handler.cold cx23888_ir_rx_read cx23888_ir_rx_read.cold cx23888_ir_tx_s_parameters cx23888_ir_rx_s_parameters cx23888_ir_log_status CSWTCH.163 cx23888_ir_log_status.cold CSWTCH.164 CSWTCH.165 cx23888_ir_controller_ops __key.1 __key.0 cx23888_ir_core_ops cx23888_ir_ir_ops __UNIQUE_ID_ir_888_debug360 __UNIQUE_ID_ir_888_debugtype359 __param_ir_888_debug __param_str_ir_888_debug netup-init.c i2c_av_read i2c_av_read.cold netup_initialize.cold cimax2.c netup_write_i2c netup_write_i2c.cold netup_read_i2c.constprop.0 netup_read_i2c.constprop.0.cold netup_read_ci_status netup_read_ci_status.cold netup_ci_op_cam.isra.0 netup_ci_op_cam.isra.0.cold netup_ci_slot_ts_ctl.cold netup_poll_ci_slot_status.cold netup_ci_slot_status.cold netup_ci_init.cold __UNIQUE_ID_ci_irq_enable362 __UNIQUE_ID_ci_irq_enabletype361 __param_ci_irq_enable __param_str_ci_irq_enable __UNIQUE_ID_ci_dbg360 __UNIQUE_ID_ci_dbgtype359 __param_ci_dbg __param_str_ci_dbg netup-eeprom.c netup_eeprom_read.cold netup_eeprom_write.cold cx23885-f300.c f300_get_byte f300_send_byte f300_set_voltage.cold cx23885-alsa.c snd_cx23885_close snd_cx23885_pointer snd_cx23885_page snd_cx23885_card_trigger snd_cx23885_card_trigger.cold snd_cx23885_pcm_open snd_cx23885_pcm_open.cold dsp_buffer_free.isra.0 dsp_buffer_free.isra.0.cold snd_cx23885_hw_free snd_cx23885_hw_params snd_cx23885_hw_params.cold snd_cx23885_prepare cx23885_audio_irq.cold snd_cx23885_pcm_ops cx23885_audio_register.cold __UNIQUE_ID_audio_debug368 __UNIQUE_ID_audio_debugtype367 __param_audio_debug __param_str_audio_debug __UNIQUE_ID_disable_analog_audio366 __UNIQUE_ID_disable_analog_audiotype365 __param_disable_analog_audio __param_str_disable_analog_audio vb2_queue_init m88ds3103_get_agc_pwm free_irq cx23885_irq_417 vb2_dma_sg_memops vb2_dvb_register_bus try_module_get v4l2_norm_to_name video_ioctl2 release_firmware ir_raw_event_handle cx23885_mc417_init pci_enable_device vb2_ioctl_streamoff netup_ci_read_attribute_mem vmalloc_to_page dvb_ca_en50221_init cx23885_tuner_callback __this_module altera_init snd_pcm_new snprintf cx23885_vbi_fmt queue_work_on __stack_chk_guard __symbol_put vb2_dvb_get_frontend vb2_dvb_alloc_frontend pci_dev_put cx23885_sram_channel_dump __kfifo_in tda18271_attach v4l2_ctrl_add_handler i2c_probe_func_quick_read cx23885_flatiron_read snd_card_register snd_card_free mc417_memory_write pci_get_device ir_raw_event_store vb2_ops_wait_finish request_firmware __pci_register_driver v4l2_ctrl_handler_init_class __mod_pci__cx23885_pci_tbl_device_table cx23885_set_tvnorm iounmap cx23885_video_irq cleanup_module cx23885_card_setup param_array_ops kfree cx23885_gpio_enable vb2_ioctl_querybuf netup_initialize cx23885_buf_queue v4l2_subdev_init i2c_transfer_buffer_flags usleep_range_state v4l2_fh_open cx23885_av_clk __kfifo_free cx23885_gpio_get cx23885_ir_fini cx23885_i2c_unregister _raw_spin_lock_irqsave _raw_spin_lock kmem_cache_alloc_trace cx23885_video_register __x86_indirect_thunk_edx pci_unregister_driver fortify_panic __release_region cx23885_risc_vbibuffer __fentry__ mc417_gpio_set cx23885_idcount init_module v4l2_i2c_subdev_addr cx23885_enum_input vmalloc_32 v4l2_device_register_subdev cx23885_417_check_encoder cx23885_audio_register __x86_indirect_thunk_ecx cx23885_buf_prepare _printk v4l2_ctrl_handler_log_status cx23885_risc_databuffer cx23885_irq_remove cx23885_audio_unregister __stack_chk_fail video_devdata mc417_gpio_enable vb2_fop_release netup_ci_slot_shutdown i2c_new_scanned_device cx2341x_handler_setup strnlen netup_ci_slot_ts_ctl v4l_bound_align_image cx23885_ir_pci_int_enable v4l2_ctrl_find vzalloc _dev_info f300_set_voltage i2c_del_adapter module_put cx23885_dvb_unregister __v4l2_ctrl_s_ctrl vb2_plane_cookie netup_ci_write_cam_ctl snd_card_new v4l2_device_unregister_subdev cx23885_audio_irq cx23885_card_list __symbol_get altera_ci_tuner_reset cx23885_gpio_setup vb2_ioctl_dqbuf __x86_indirect_thunk_edi i2c_unregister_device cx23885_input_rx_work_handler netup_ci_read_cam_ctl request_threaded_irq tveeprom_read cx23885_ir_rx_v4l2_dev_notify mc417_memory_read cx23885_input_init cx23885_av_work_handler vb2_ioctl_prepare_buf dvb_ca_en50221_release mutex_lock cx23885_video_wakeup __x86_indirect_thunk_ebp rc_register_device dma_alloc_attrs cx23885_ir_rx_work_handler netup_ci_write_attribute_mem snd_pcm_hw_constraint_pow2 altera_ci_irq vb2_buffer_done cx23885_417_unregister cx23885_gpio_set v4l2_ctrl_subscribe_event cx23885_flatiron_write snd_pcm_set_ops ioremap video_unregister_device cx23885_video_unregister __x86_indirect_thunk_esi __mutex_init i2c_new_client_device cx23885_boards altera_ci_release vb2_ioctl_qbuf vb2_fop_mmap _raw_spin_unlock_irqrestore video_device_alloc cx23885_subids param_ops_short netup_ci_slot_status netup_poll_ci_slot_status netup_ci_slot_reset rc_free_device cx23885_free_buffer cx23885_gpio_clear memset cx23885_cancel_buffers cx23885_dvb_register cx23885_input_fini cx23885_set_frequency pci_set_master dvb_ca_en50221_frda_irq vb2_dvb_dealloc_frontends cx23885_vbi_qops vb2_fop_read v4l2_device_register kasprintf cx23885_irq_add_enable i2c_add_adapter jiffies dvb_ca_en50221_camready_irq rc_allocate_device cx23888_ir_probe cx23885_find_hw cx23885_set_input cx23885_i2c_register mc417_gpio_clear strscpy netup_eeprom_read cx23885_irq_enable vb2_dvb_unregister_bus mc417_register_read dma_free_attrs vfree mutex_unlock cx23888_ir_remove __video_register_device cx23885_ir_tx_v4l2_dev_notify __const_udelay cx23885_vbi_irq vb2_ops_wait_prepare sg_next kmemdup ktime_get cx23885_ir_init netup_ci_exit vb2_ioctl_streamon cx23885_ir_tx_work_handler vb2_fop_poll v4l2_i2c_tuner_addrs v4l2_ctrl_g_ctrl cx2341x_handler_init cx2341x_handler_set_50hz cx23885_bcount v4l2_ctrl_handler_free i2c_transfer netup_get_card_info v4l2_device_unregister pci_disable_device __x86_indirect_thunk_ebx mc417_register_write dma_set_mask tveeprom_hauppauge_analog video_device_release v4l2_subdev_call_wrappers dma_unmap_sg_attrs snd_pcm_period_elapsed netup_eeprom_write cx23885_start_dma v4l2_i2c_new_subdev pci_read_config_byte cx23885_get_input __kfifo_alloc iomem_resource param_ops_int cx23885_risc_buffer cx23885_sram_channel_setup cx23885_irq_disable _raw_spin_unlock altera_ci_init __kfifo_out cx23885_417_register netup_ci_init sg_init_table msleep rc_unregister_device v4l2_event_unsubscribe kmalloc_caches __request_region __request_module dma_map_sg_attrs system_wq flush_work vb2_ioctl_reqbufs      § ˆ   & ¡   § ³   u ¾   > Ø     î             %    6    G    X    i    z    ‹    œ    ­    ¾    Ï    à    ñ            $    :    E    V    g    x    ‰    š    §    ´    Æ    Ñ    Ş    ë    ø                 %  ³ -    2    A  § ]    ¾  & T  Ò q  § È  º Ô   Ş  T ê  ¨ õ  T j  — “  — ¼  — å  —   — 7  — `  — Ã  T ë  ‘ ÷     T   ì   T 2  ‘ >  ì J   T  T a  ì “  T ¯  T O  T k  T   T Ğ  T ì  T /	  T K	  T ˜	  º ¤	   ®	  T º	  ¨ Ä	  T Õ	  ‘ á	  ì ë	  T ÷	   
  T 
  ì 
  T W
  ‘ c
   m
  T z
  ì ©
  ‘ µ
   ¿
  T   T Ğ  T -  ‘ 9   C  T P  ì …  T   ‘ ©   ³  T ø  T !  § 6  u w  u ¦  	  ¯  Y Ç    ×     @     3  ó B    V     @ •    ²  ó Ã   Ù     @     -  Õ C   Y     @ †    £  ã Ò  @ î  ã !  @ =  ã n  @ ‚  Õ ·  Õ É  ; Ø  Õ ê  Õ ÿ  ;   ;   ; 3  ¸ A  § ‡  ¶ ¨  ¶ ¯  # Ñ  §    8   A  § U  u 6  	  @  E ˜  @ °  ± M  u }    ‰  Ú —    !  ¢ C  	  J  E f  u y  “   	    E   	  *  E ¦    ½  Ú ¼    Ä    ë          — )  — R  — p    ¡  — W  	  _    d  “ o    t  8 y        †    ‹  ¸ ‘  § Ñ  § )   x    ‚      ú µ    ¼    Ê  ú Ñ  § ı   ?  ê V  ú a  § h  ¹ ’  ¢ ±  § »  ¹   ± A  § K  ¹ –  ± Á  § Ë  ¹ ÷  	  ÿ   &  ± Q  § Y  ¹ Á  § Î  ¹ !   § *   ¹ 5     =     a   § j   ¹ Á   § Ì     ñ   § ı    !  § (!  Ë ¤!  K ­!    ´!    ı!  K 4"    P"  ö `"    –"  K Æ"  K #    1#  § :#  ¹ @#    ¥#  Á ¯#    !$  § 4$  u ?$  ¹ a$    j$    µ$    ½$    Æ$    î$    r%  ó ã%  u  &  ¸ &  § &  ¹ !&  	  3&   H&  ö O&  ¿ X&    n&    w&   –&  	  œ&   á&  § ş&  + '    '    C'  ê Q'  § ['    c'    –'    ¢'  ¿ ­'    ¾'    Ç'   ñ'  § (  § 	(  ¹ '(  ö A(    I(    a(  § s(    {(    €(  û ¨(  ? ¿(  ö Ê(  	  Ö(  r )  § )    )    ?)  L f)   ±)  § ¹)  ¹ Í)    Õ)    á)  § *  § *  u 4*    ù*  ó R+  ó i+  u …+  d Œ+    ‘+    –+    ›+  ³ ¨+  ¸ ±+  § Á+  ¹ Ì+    Ô+    ñ+  § ,  u 4,  7 ?,  u K,  ¸ Q,  § `,  u ®,  7 ·,    Ç,  u Ò,  ¸ á,  § ï,  u ú,    -     -   _-  7 -  7 ”-    œ-    §-  u À-   Í-  ¸ á-  § è-    ÷-    I.  @ V.  ö q.  ã ¼.  ö Ï.   0/  @ =/  ö Q/  ã ƒ/  ö ­/  Õ Å/  Õ Ñ/  § Ü/    ä/    ø/  ö !0  § *0  ¹ H0  ö \0    d0    ¡0  § ²0    º0    Ù0  ö ù0  ö 1    1  ¿ 1    .1    81   Z1  ö ¶1  ± Ñ1  § Ú1  ¹ å1    í1    2  § 	2  ¹ 2    %2    12  § :2  ¹ @2    N2    g2  ö ¡2  § ±2  § Á2  u Ì2  ¹ !3  Ã 03  2 f3  ± z3  T ˜3  á ¡3  Ê ¬3  " 4  Ã &4  2 74    ?4    O4  w ’4     4    Ø4  Õ ù4  ± 5  T  5  u Œ5  T «5  á ·5  Ê Â5  " ×5  á ã5  Ê î5  " 6  T 6  á $6  Ê /6  " 96  ¸ A6  § Q6  § 6    ¡6    ¨6    ¼6  ' Ù6  Ÿ 7  N "7    )7    A7  § G7    R7    ^7  ¶ x7  ? œ7  ? ¸7  · Á7  ñ Ñ7  ñ ñ7  §  8    8  u 8    8    8    (8  	  /8    ;8   @8    K8    µ8   à8  á ó8  Š 9  " a9    k9  a €9  ] ë9   õ9  a :  ] :  	  :    {:    Œ:  $ ˜:    :    ®:  ò ¼:  u è:  1 í:  	  ;  E E;  « U;  n h;  @ †;  Õ å;  	  <  @ -<  Õ I<  	  Z<  E Â<  	  ì<  @ =  Õ =  ± +=  ± C=  ± M=  ¸ a=  § ¡=  § (>   O>  ú ^>    k>    †>   ¬>  ú ²>    ¹>    Ñ>  § ı>   ??  ê V?  ú a?  § m?   ?  § ”?  Ë õ?  ¦ @  § @    *@    B@  L u@   Á@  § É@  ¹ QA  § _A    gA    tA  Ÿ A  â ”A  N ±A  § ÂA   ×A  ú áA  § cB  ) C  ) UC  ) ¡C  § ßC  I äC  ¥ D  ö D  ö /D  ö HD   SD   ^D   lD  ‹ {D  ë ‹D  ò §D  É ¹D  ö ÉD  ë çD  É ÷D  ö E  § ,E  % <E  Û QE  § ÜE    íE  Å F  § "F    *F    PF  ê nF  +  F    ¨F    ÁF  § 5G   NG  ú ¡G  § ¶G   ½G  œ ÑG  _ ØG  : çG  6 îG  9 øG   H  § H   =H  ú QH  § eH   H  ú ¡H  § µH   ÙH  ú áH  § õH   !I  ú 1I  § DI    TI    ÒI    ÙI    ;K    BK    dK    ÑK  § çK    òK    ùK    L  Ÿ )L  N aL  § sL  Ÿ ¥L  N µL  N ÁL  § M  å ¡M  § ÛM  å 1N  § ‡N  å O  § >O    QO  § dO    nO    ¯O  L µO    ½O    ãO  ö öO    	P  & ŠP  & ¹P  ö êP  ö Q  ö ƒQ    ‹Q    ŞQ   íQ  ú ôQ   R  ú 7R  ö @R    JR   `R  ú •R  ™ ¨R  ™ ±R    ÁR  § âR  Ë èR    òR    %S  µ aS  § ¸S   	T    T    T  ú ET    MT    [T  ú aT  § pT    xT    ÆT  & êT  & U  ö 3U   kU  ê pU    xU    •U  ú ôU  ö V  ™ V  § 8V    GV    UV    \V    iV  Ÿ }V  N ¸V    ½V    ÂV  ³ ÑV  § àV  u ıV   W  ú ëW    öW    X    "X    0X    7X    EX    LX    ZX    aX    oX    vX    X    —X    ©X    °X    ÂX    ÉX    ßX    æX    øX    ÿX    Y  ö 7Y  ö aY  ö ¼Y  @ äY  ã Z  u eZ  ş {Z  ö ›Z  é ·Z  Œ ÓZ   ôZ  ú [  [ 
[  t +[  Ï J[    V[    b[    i[    ‡[    “[    ¥[  ` ·[  ` Ö[    â[    ş[    
\    K\  ã U\  ¸ a\  § ˜\  ö Ÿ\    ¥\  ö ¬\    Ò\    á\  § ]  ö !]    ']  ö .]    V]    a]  § ]    œ]  ö £]    ©]  ö °]    ñ]  § 7^  ö >^    D^  ö K^    ¡^  § ¹^  W ¾^    Í^    ò^  ƒ 	_    _    _  ƒ  _  y R_   m_  ˆ €_  6 ‡_  9 _   `  Ş `  æ )`  / 0`    5`  i L`    Q`  	  j`  ô u`    z`  	  `  ô ‡`     `  	  §`    ¬`   Û`    ç`    û`    a    a  5 $a  © La  ü Za  ü ga  ü €a  Ğ ¤a  ö àa   óa  ú éb  ö c  ö fc  I c  X Šc    Èc  ğ Óc  	  ìc    ñc  	  d  ö d     d  ³ %d    0d    5d    “d  ö «d    ³d    Äd    Éd  	  e  ô ÷e    ÿe    f    f  	  hf  ô Sg  ö yg    ‘g  § ¡g  § ¶g  & ñg  § h    h    (h    0h    Xh    `h    Ëh    Gi    Oi    hi    qi  § ˆi    ¢i    -j    9j    ³j    ¾j    	k    2k    :k    ˜k     k    Ák  § Ôk    Ük    &l    5l    ‘l  § ¢l  u ­l    µl    ×l  o Şl    Im  ¿ ]m    gm   zm   m    ”m    œm    ¡m    ©m    ¹m  	  Ém   Ğm    ×m  ~ Üm  ½ îm  u n    n  ¸ !n  § )n  Ç 1n  § =n    Cn  u Qn    ¡n  u ­n  ¸ Án  § o  § Qo  § oo  ; o  § ‘o  § ¡o  ± «o    Áo  § ëo  ¢ ûo    p  § !p  ± +p    Ap  § kp  ¢ {p    ‘p  § ©p  w Æp  ± àp  w ñp  § ıp  ” q  § q   !q  § ;q  D Qq  § _q   qq  § {q  ² q  § ¦q  ‘ Çq  ± Øq  ì äq  ì ÷q  ± r  ì r   'r  ± 8r   Dr   Wr  ± ar  § †r  ‘ £r  ± ¸r  ì Är  ì Ør  ì är   ør   s   s  § Js  ì ]s  ¢ ‰s   ‘s  § °s  á gt   lt   t  — •t   Ñt  " u  §  u   Šu   ˜u  & v  § v  w &v   ;v  È Bv  Ö Wv  È ^v  Ö sv  È zv  Ö v  È –v  Ö ²v  ÷ Ëv  - áv  § èv    ÷v    w    w    w    1w  § Qw  § bw  ” qw  § }w   ‘w  § ¡w  ² ±w  § ¿w  u Êw  ¹ ßw  	  ëw  r x  ¢ )x  ´ 4x  u Ex  ¸ Qx  § [x  ¹ ¦x  ± Ñx  § Ûx  ¹ y  ± Ay  § Iy  ¹ qy  § yy  ¹ °y    ¸y    Áy  § Éy  ¹ z    z    1z  § 6z   Az  § Kz  ¹ mz  	  uz   z  ± ­z    µz    Ñz  § Öz   áz  § æz  G ñz  § úz  ¹ {    {    {  ¬ !{  § 4{  ¹ V{    ]{    ‡{  Š —{    {    Ñ{  § Ü{  ¹ ô{  ¿ ı{    |    |   7|  ö >|  ¿ I|    Z|    c|   ‚|  	  ˆ|   Á|  § É|  ¹  }    }    }  § }    !}    a}  § –~   ¡~   ¯~  & á~  § ~   ‘   £  &   § B‚   Q‚   _‚  & ‘‚  § (ƒ   9ƒ   Kƒ  & Á„  § Ô„  u à„    ó„    …  Ü …    $…  Ü 0…    J…  ‚ ^…  ‚ s…  ‚ …  ‚ –…    …    Â…  ‚ á…  ‚ ç…   ñ…   ù…    †  & †  Ü ?†  Ü E†    L†    j†  Ü p†    w†    ††  ‚ •†  u ¤†  ¸ ±†  § ¾†  u Ò†    Ù†    H‡  u g‡  ¸ q‡  § ‡  § ‡    š‡  u ¨‡    È‡    Ï‡    ï‡  Ü ˆ  Ü ˆ  < %ˆ  < 8ˆ  < Kˆ  < [ˆ  < cˆ    nˆ  	  }ˆ  † …ˆ    —ˆ    ©ˆ    ®ˆ  ³ ·ˆ  f ½ˆ    Âˆ    Çˆ  ³ ëˆ    ‰  ‚ $‰    :‰    E‰    Y‰  Ü a‰    v‰    ‰  f „‰    Œ‰    ‰  < ¯‰  < Á‰  < Ó‰   è‰  < û‰  < Š  <  Š   .Š   6Š    <Š    cŠ    }Š    ˆŠ  Ü ŸŠ    ªŠ    åŠ    ëŠ    óŠ    ıŠ  T ‹    ‹    „‹  ¾ ·‹  T •Œ  T ±Œ  T ÂŒ  ‚ ÕŒ  u 	  T   ¸      %    1  § `   §  ê »  ú å  D   §   u $   /  u M  ¸ a  § u  u „     u ¯  ¸ Á  § Ø  u ç   ü  u    ¸ 1  § ;  u n    y    „  u   ¸ ¡  § Ä  T Ë  ¯ Ú   á  § é    ñ      ?   6 1  ñ A  § O    W    g  ö ™  	  Ğ  3 ‘    
‘  4 ‘  } ,‘    4‘    9‘  û F‘    R‘  	  h‘  ö s‘  	  ‘  r ”‘  ? ’    ’  a #’  ] ”’  $ ¦’    «’    Ñ’  § $“  × A“  § Q“  § “  [ ¨“  t Á“  § ß“  [ ï“  t ”  § '”  u L”  @ e”  Õ x”   Š”  î –”   ¨”  î ´”   ¿”  u Ö”  ó İ”  ¸ ñ”  § •  u J•  @ d•  ó Ë•  @ å•  ; –  u #—  ± 3—  ± K—  ¸ Q—  § o—  u  —  @ ®—  @ Î—  ó ë—  „ ˜  u ?˜  ó J˜  g o˜  „ }˜  g „˜  ¸ ‘˜  § Ÿ˜  u Ò˜  @ ì˜  ó ÷˜  @ 5™  @ O™  Õ |™  Õ —™  \ ¢™  \ ­™  \ ¸™  u É™  ± Û™  ± ë™  ± õ™  ¸ š  § 1š  § Fš  u wš  u Êš  @ æš  ã M›  @ g›  ; ››  u ä›  @ ø›  ã œ  Õ ;œ  ± Kœ  Õ Uœ  ¸ aœ  § œ  	  ±œ  W Àœ    Üœ  ö áœ  	  ëœ     	     5   Õ    ß    é  	  ô  ä   	  .  	  Y  	    	  ¡  	  á  	  Ÿ   Ÿ   Ÿ   Ÿ   AŸ  	  qŸ  § ‹Ÿ  U ™Ÿ   §Ÿ   ²Ÿ   ÑŸ  §    § )   á ’   " ¡   § ¹   á "¡  " 1¡  § I¡  á ¡¡  " ±¡  § É¡  á A¢  " Q¢  § k¢  u İ¢    è¢    {£   £  { ™£  ú §£    ¶£   Æ£  { Ò£  ú Û£    õ£    2¤   W¤  ú y¤  u ì¤  ã ¥    >¥  ; H¥  ¸ Q¥  § ¨¥   ¾¥  P Ê¥  ú É¦    Ú¦    !§  § ]§  á  «  " ¬  § =¬  á °  " É°   æ°  ú Á±  § J²  	  O²  	  U²  	  Z²  ³ g²  	  l²  	  v²    {²  ³ ’²    —²  ³ ¬²    ±²  ³ ½²    Ã²    È²  ³ Í²  	  Ò²  	  å²    ê²  ³ ï²  	  ô²  	  ³    ³  ³ ³  	  ³  	  .³    3³  ³ B³    G³    Q³  § j³  u u³  W z³    ª³  H ¾³    Å³  • Ú³  	  â³  r ö³  ® ´    ´  	  $´  ô z´  @ –´  Õ ¡´    ¦´  	  ­´  ô í´  @ µ  ó µ  š µ  u 3µ  ± Cµ  ± Xµ   fµ  ¸ qµ  § }µ   ¦µ  Î ±µ  š ¸µ   Ñµ  § æµ  u ¶  7 ¶    2¶  7 ;¶    K¶  u W¶  ¸ a¶  § u¶  u ½¶  7 Æ¶    û¶  7 ·    9·  7 B·    ‚·  7 ‹·    –·  u £·  ¸ ±·  § Á·  § Ö·  u ¸  7 ,¸  u ¢¸    ª¸    ¹¸  ¸ Á¸  § Ñ¸  u ê¸  & ¹  & 1¹  T :¹   G¹  u h¹  ¸ q¹  § ¹  u Î¹  7 Ø¹    à¹    í¹  u º    
º    º  ¸ !º  § /º  u =º  
 Bº   Yº  u ~º   º    —º    ¯º  ¸ Áº  § ãº  u  »  u q»  á >¼   R¼   `¼  & —¼  " œ¼    ¨¼    á¼  ¸ ñ¼  § !½  § A½  § a½  § ½  § “½  u ô½  7 ¾  u ¾    "¾    8¾  ¸ A¾  § P¾  u w¾    ¾    ¯¾  7 Å¾  u Ú¾    é¾    ¿    ¿    ¿  ¸ !¿  § '¿    5¿    h¿  [ u¿  t z¿    ƒ¿    ˜¿  [ ¥¿  t «¿    ³¿    Ñ¿  § ê¿    ğ¿  u ü¿    XÀ    ]À  W lÀ    ¢À    ©À  o °À  k ÁÀ  ç ÑÀ  Ø ÛÀ  Ì âÀ    éÀ  ¼ ğÀ  À ÷À  ÿ PÁ  m jÁ  [ |Á    Á  t ‡Á    Á    šÁ  u ÁÁ    ÉÁ    ĞÁ   ïÁ  ¸ õÁ    Â    Â  § .Â  à 6Â   QÂ  § [Â  u ©Â  7 ²Â    ÂÂ  u ÍÂ  ¸ áÂ  § ìÂ  u Ã  7 (Ã    >Ã  & PÃ  u \Ã  ¸ aÃ  § wÃ   …Ã   Ã   ÁÃ  § çÃ  ‘ óÃ   ıÃ  & Ä  ‘ Ä  ì $Ä  & 2Ä  ‘ >Ä  › aÄ  § yÄ   …Ä  & –Ä  ‘ ¢Ä  ì ¬Ä  & ÅÄ  ‘ ÑÄ   ÛÄ  & ìÄ  ‘ Å  ì !Å  § 3Å  u Å  ‘ ©Å  ì ºÅ  ‘ ÆÅ  ì ĞÅ  & áÅ  ‘ íÅ  ì ÷Å  T Æ  ‘ Æ   Æ  T 4Æ  T pÆ  ‘ |Æ  ì Æ  ‘ ™Æ  ì ©Æ  T ·Æ  ‘ ÃÆ  › ×Æ    èÆ  ‘ ôÆ   şÆ  T Ç  T OÇ  ‘ [Ç  ì lÇ  ‘ xÇ  ì …Ç  u ÚÇ  ¸ áÇ  § ñÇ  § È  § !È  l 1È  § GÈ  Ÿ yÈ    …È    ¨È  L ŞÈ    iÉ    zÉ  N ‘É    ŸÉ    İÉ    öÉ  z Ê  z CÊ    HÊ  ³ aÊ  § sÊ    ƒÊ  è AË    NË    aË  § uË    Ë    “Ë  ! ¢Ë  ! ÉË    ÑË   Ì  A 1Ì  § aÌ  § ÀÌ  W ÏÌ    ÿÌ  ­ Í    Í    &Í   5Í  Ä HÍ  S ƒÍ  l ¾Í  Z ËÍ    áÍ  µ DÎ  ! SÎ  ! iÎ   šÎ    ¬Î    ±Î  § ÁÎ  § ëÎ    	Ï    Ï    cÏ  B qÏ  § ~Ï  u ‰Ï    ¥Ï    ºÏ  o ÄÏ  Í ÏÏ    üÏ  	  Ğ  q Ğ  	  2Ğ   ;Ğ    EĞ  ï RĞ  	  ZĞ   _Ğ  	  lĞ   }Ğ  	  ƒĞ   ŒĞ  € ™Ğ     Ğ    «Ğ  u ÈĞ   ÍĞ    ÒĞ  ¸ áĞ  § ïĞ             %     *   ³ 8   u U     m     |   ¸    § Ÿ     ¤   ³ ²     ·   ³ Â     Ç   ³ Õ   ö İ     â   ³ ø     ı       ³ 
                "  ³ 1    6    =    B  	  G  	  T    Y  ³ p  † ƒ  p ‹  f     –    ¦  ö ²    ·  ³ Ç    Í    Ò  ³ â    ç    ì  ³ ø        	      ³         $    )  	  .  ³ 6    \    a    f  ³ p    u    z        „  ³ ’    —    œ    ¡    ¦  ³ ´    ¹    ¾    Ã  	  È  ³ Ğ    Ø    İ    â    ç  ³ ï    ô    ù  ¤ ş      ¤         #  ³ +    0    5    :  	  ?  ³ G    L    Q  ¤ V    [  ¤ a    f    k    p  ³ x    }    ‚    ‡  	  Œ  ³ ”    ™        £  	  ¨  ³ °    ¶    »    À    Å  ³ Í    Ò    ×    Ü  	  á  ³ é    î    ó  	  ø  ³ ÿ    	              ³ $    )    0    5  ³ =    M    V   `    e    j  ³ ƒ  ö ˜        ¢    §  ³ ¯    ¹    ¾    Ã    È  ³ Ô    Ú    ß    ä  	  é  ³ ó    ø    ı      	    ³           ¤     #  ¤ (    -    2  	  7  ³ ?    E    J    O    T  ³ \    b    g    l  	  q  ³ }    ƒ    Š        ”    ™      ³ «    ¾    Ã    È    Í  ³ Õ    Ú    ß    ä  ³ ë    õ    ú    ÿ  ³                 $  ³ 3    8  ³ E  z L    V    [    `  ³ h    m    r    w  	  |  ³ „    Ÿ    ¤  ³ ©  	  ®    µ        "  $ 1    6  ³ @    E    J    O  	  T  ³ \    e    j  ³ t    ¾    Ã  ³ Ê  ° Ú    ë    ğ    õ  ³ ı              ³         "    '  	  ,  ³ 4    9    >    C    H  ³ P    U  § g    p  	    	  †  ³ š    Ÿ  	  ¤  ³ ¸  	  ½  ³ Ä    å    ê    ï  ³ ÷    	    	    	  ³ 	    !	    &	    +	    0	  ³ 8	    B	    G	    L	    Q	  ³ ]	    p	    u	    z	    	  ³ 	    ¢	    §	    ¬	  ³ ´	    ¹	  § Ô	    Ù	  ³ ö	    ü	    
  ³ (
  	  -
  ³ u
    z
  ³ º
    ¿
  ³ ò
    ÷
  ³     
  ³ "    '  ³ <    A  ³ V    [  ³ s    x  ³     ›         ¥  ³ ¶    Ç    Ì    Ñ  ³ ç    ô    ù    ş  ³             "  ³ 1    6  ³ d  z k    r    w    |  ³ „    ›         ¥    ª  ³ ²    Ã  z ß    ä  ³   	    ³ g    l  ³ ˆ        ’    —  ³ Ÿ    ¦    «    °  ³ ½    Ê    Ñ    Ö    Û    à  ³ é    ğ                  ³     "    )    /    4    9    >  ³ G    N    a    f    k    p  ³ y    €    “    ˜        ¢  ³ «    ²    Å    Ê    Ï    Ô  ³ İ    ä    ÷    ü          ³         )    .    3    8  ³ A    H    [    `    e    j  ³ s    z        ’    —    œ  ³ ¥    ¬    É    Î    Ó    Ø  ³ á    è        
          ³     $    A    F    K    P  ³ \    o    Œ    ‘    –    ›  ³ ¤    «    È    Í    Ò    ×  ³ à    ç        	          ³     #    @    E    J    O  ³ X    _    |        †    ‹  ³ ”    ›    ¸    ½    Â    Ç  ³ Ğ    ×    ô    ù    ş      ³         0    5    :    ?  ³ H    O    l    q    v    {  ³ „    ‹    ¨    ­    ²    ·  ³ À    Ç    Ú    ß    ä    é  ³ ò    ù                  ³ $    +    >    C    H    M  ³ V    ]    p    u    z      ³ ˆ        ¡    ¦    «    °  ³ ¸    Ş    í        	          ³     *    /    4  	  9  ³ E    M    V    [    `  ³ l    t    }    ‚    ‡  ³ “    ˜        ¢  	  §  ³ ²    ¹    ¾    Ã    È  	  Í  ³ Õ    İ  	  é    î    ó  ³ û          ³     &    +    0  ³ @    H    M    R  ³ b    n    s    x  ³     ˜        ¢  ³ ®    ¶    »    À  ³ Ø    à    å    ê  ³ ö    ş          ³ ;  z B  ¯ G    L    Q    V  ³ ]    „    ‰      ³ š    ¥    ª    ¯  ³ »    É    Î    Ó  ³ ß    í    ò    ÷  ³               ³ '    5    :    ?  ³ K    Y    ^    c  ³ o    }    ‚    ‡  ³ “        £    ¨  ³ ´    Â    Ç    Ì  ³ Ø    æ    ë    ğ  ³ ü              ³     (    -    2  ³ >    I    N    S  ³ _    f    k    p  ³ x    ‡    Œ    ‘  ³ ¢    ©    ·    ¼    Á  ³ Î    Õ    ê    ï    ô  ³                 $  ³ 1    8    I    N    S  ³ _    k    p  ³ w    ƒ  	  ˆ  ³     ›       ³ §    ³  	  ¸  ³ ¿    Ë  	  Ğ  ³ Ù    å    ê  ³ ñ    ı      ³ 	              	    ³ '    -    2    7    <  ³ D    I    N    S  	  X  ³ €    ’  ö Ÿ  ö ¯  ö ¿  ö Å  ö Ë    î    ó    ø    ı  ³         !    &    +  ³ @  ö N  h T    \    a    f  	  k  ³ u    ï    ù  T   L $  L ;  L U  L o  L ‰  L £  L ½  L Ú  L á  Ó è    ï    ú            I  ¢ X    ]    b  	  g  ³ o    u  ö ¢    §  ³ ½    Â  ³ È    Ñ    Ø  , ï  ‘   ö   ¡           ³ 2  ö 8  ö Y   f    k    p  ³ „  ö ‰  ö ª   ³    ¸    ½  ³ *    /    4  ³ K    P    U  ³ j  ö ³  F ş       ³    	    = .     3   ³ :     J   Q S     X     ]   ³ z   Q ‡     Œ     ‘   ³ ¯     ´     ¹   ³ !    !!  Ù ;!    @!  ³ G!    S!    o!   }!   „!  Â ‹!  İ !    Ñ!    Ö!    Û!    à!  ³ ê!    ö!    û!  	   "  ³ "    "    *"    /"    4"    9"  ³ I"    X"    ]"  	  b"  ³ "    „"  	  ‰"  ³ ™"    Ÿ"    ¦"    «"    °"  	  µ"  ³ ¿"    Æ"  	  Ë"  ³ ×"    Ü"    á"    æ"    ë"  ³ õ"    ü"  	  #  ³ #    #    #    $#    )#  ³ 9#    B#    G#    L#    Q#  ³ a#    x#  	  }#  ³ #    –#  	  ›#  ³ ¤#    ´#  	  ¹#  ³ Ç#    Ì#  	  Ñ#  ³ Ú#    à#    å#    ê#    ï#  ³ ÷#    $    $    $    $  ³ -$    >$  	  C$  ³ ^$  – |$    $    $    •$    š$  ³ £$    ¯$    »$    À$  ³ Ê$  	  Û$    à$  ³ ê$    ï$  ¤ ÷$    ü$    %    %  ³ %    %    %  ¤ $%    )%    .%    3%  ³ ?%    D%    I%  	  N%  ³ U%    Z%    _%  	  d%  ³ k%    p%    u%  	  z%  ³ %    †%    ‹%  	  %  ³ —%    œ%  § º%  u Ë%  w ı%    &  8 &    &    )&    .&  ³ 5&  R †&  Ú “&    ˜&  ³ Ø&    İ&  O '  Ú !'  ö &'    +'  ³ f'  	  |'    '   ”'  Y ¡'  õ È'  c Ó'  Ö  (  Ú (  ö (    "(  ³ H(  u O(  ¸ a(  § z(  u ª(  w à(    +  	  +  Ñ "+  	  )+  Y .+  	  3+  Ñ I+    P+  ± _+  	  f+  v w+    €+  ³ ’+  	  —+  Ñ ¢+  	  ©+  Y ®+  	  ³+  Ñ É+    Î+  ± Ù+  	  à+  v ñ+    ú+  ³ ,  	  ,  Ñ ,  	  &,  Y +,  	  0,  Ñ N,    S,  ; ],  	  b,  v i,    n,  ³ ¥,    ¯,  	  ´,  Ñ ¿,  	  Æ,  Y Ë,  	  Ğ,  Ñ æ,    ë,  ± ö,  	  ı,  v -    -  ³ ,-  	  1-  Ñ <-  	  C-  Y H-  	  M-  Ñ k-    p-  ; z-  	  -  v †-    ‹-  ³ ”-    ­-  | ¹-    ¿-  	  Ê-  Ñ Õ-  	  Ü-  Y á-  	  æ-  Ñ ü-    .  ± .  	  .  v $.    -.  ³ B.  	  G.  Ñ R.  	  Y.  Y ^.  	  c.  Ñ .    †.  ; .  	  •.  v œ.    ¡.  ³ §.  	  ¬.  Ñ ·.  	  ¾.  Y Ã.  	  È.  Ñ å.    ï.  ; ı.  	  /  v /  	  /  Ñ 8/    =/  ; G/  	  L/  v Q/  	  V/  Ñ e/  	  l/  Y q/  	  v/  Ñ ‡/    /  ³ ›/  Ñ ¦/  	  ­/  Y ²/  	  ·/  Ñ Í/    Ô/  ± ã/  	  ê/  v û/    0  ³ 0  	  0  Ñ &0  	  -0  Y 20  	  70  Ñ M0    T0  ± _0  	  f0  v w0    €0  ³ •0  	  š0  Ñ ¥0  	  ¬0  Y ±0  	  ¶0  Ñ Æ0    Ï0  ; İ0  	  æ0  v ğ0    ù0  ³ 1  	  	1  Ñ 1  	  1  Y  1  	  %1  Ñ @1    G1  ; R1  	  Y1  v j1    s1  ³ ˆ1  	  1  Ñ ˜1  	  Ÿ1  Y ¤1  	  ©1  Ñ Â1  ; Ò1  	  Ù1  v ã1    ì1  ³ ÷1  	  ü1  Ñ 2  	  2  Y 2  	  2  Ñ .2    32  ± >2  	  E2  v V2    _2  ³ t2  	  y2  Ñ „2  	  ‹2  Y 2  	  •2  Ñ ¥2    ³2  ; À2  	  É2  v Ö2  	  Û2  Ñ ê2  	  ñ2  Y ö2  	  û2  Ñ 3    3  ³  3  	  %3  Ñ 03  	  73  Y <3  	  A3  Ñ W3    \3  ± g3  	  n3  v 3    ˆ3  ³ ¿3  	  Ï3  Ñ Ú3  	  á3  Y æ3  	  ë3  Ñ 4  ± 4  	  4  v 4    4  ³ .4    34  ± A4  	  F4  Ñ Q4  	  X4  Y ]4  	  b4  Ñ x4    }4  ± Œ4  	  “4  v ¤4    ­4  ³ ¿4  	  Æ4  Y Ë4  	  Ğ4  Ñ á4    æ4  ³ ı4    5  	  5  Ñ 5  	  5  Y #5  	  (5  Ñ >5    C5  ± N5  	  U5  v f5    o5  ³ „5  	  ‰5  Ñ ”5  	  ›5  Y  5  	  ¥5  Ñ Â5    Ì5  	  Ñ5  Ñ Ü5  	  ã5  Y è5  	  í5  Ñ 6  ¢ 6  	  6  v 06  	  56  Ñ D6  	  K6  Y P6  	  U6  Ñ f6    k6  ³ ‰6  	  6  Ñ ™6  	   6  Y ¥6  	  ª6  Ñ Ç6    Ì6  ± Ù6  	  Ş6  v å6    ê6  ³ ÷6  	  ü6  Ñ 7  	  7  Y 7  	  7  Ñ 57    :7  ± G7  	  L7  v ]7    b7  ³ }7    Œ7  ; ¬7  	  ±7  Ñ ¼7  	  Ã7  Y È7  	  Í7  Ñ ê7    ñ7  ± ü7  	  8  v 8    8  ³ 28  	  \8  Ñ g8  	  n8  Y s8  	  x8  Ñ 8  ± ˜8  	  8  v ¤8    ©8  ³ »8    À8  ± Õ8  ¢ ç8  	  ì8  Ñ ÷8  	  ş8  Y 9  	  9  Ñ 9    #9  ± .9  	  59  v F9    O9  ³ †9  	  –9  Ñ ¡9  	  ¨9  Y ­9  	  ²9  Ñ É9  ± Ò9  	  ×9  v Ş9    ã9  ³ õ9    ú9  ± :  	  :  Ñ :  	  :  Y $:  	  ):  Ñ ?:    D:  ± O:  	  V:  v g:    p:  ³ …:  	   :  Ñ «:  	  ²:  Y ·:  	  ¼:  Ñ Ü:  ; é:  	  î:  v õ:    ú:  ³ ;    	;  ³ ;  	  ;  Ñ %;  	  ,;  Y 1;  	  6;  Ñ L;    Q;  ± \;  	  c;  v t;    };  ³ ›;  Æ ¿;  	  Ä;  Ñ Ï;  	  Ö;  Y Û;  	  à;  Ñ ö;    û;  ± <  	  <  v <    <  ³ +<  	  0<  Ñ ;<  	  B<  Y G<  	  L<  Ñ b<    g<  ± r<  	  w<  v ~<    …<  ³ —<  	  œ<  Ñ §<  	  ®<  Y ³<  	  ¸<  Ñ Î<    Õ<  ± à<  	  ç<  v ø<    =  ³ =  	  =  Ñ &=  	  -=  Y 2=  	  7=  Ñ I=    N=  ; W=  	  \=  v c=    h=  ³ z=  Æ „=  	  ‰=  Ñ ”=  	  ›=  Y  =  	  ¥=  Ñ »=    À=  ± Ë=  	  Ğ=  v ×=    Ş=  ³ 
>  	  >  Ñ >  	  !>  Y &>  	  +>  Ñ C>    J>  ; U>  	  \>  v m>    v>  ³ ‹>  	  >  Ñ ›>  	  ¢>  Y §>  	  ¬>  Ñ ¾>    Ã>  ; Ğ>  	  Ù>  v æ>  	  ë>  Ñ ú>  	  ?  Y ?  	  ?  Ñ ?    %?  ³ 0?  	  5?  Ñ @?  	  G?  Y L?  	  Q?  Ñ l?    s?  ; ‚?  	  ‰?  v š?    £?  ³ º?    ¿?  ; Ñ?  	  Ö?  Ñ á?  	  è?  Y í?  	  ò?  Ñ @    @  ± @  	  @  v 0@    9@  ³ N@  	  S@  Ñ ^@  	  e@  Y j@  	  o@  Ñ @    @  ; –@  	  ›@  v ¢@    §@  ³ É@    Ó@  	  Ø@  Ñ ã@  	  ê@  Y ï@  	  ô@  Ñ 
A    A  ± A  	  !A  v 2A    ;A  ³ PA  	  UA  Ñ `A  	  gA  Y lA  	  qA  Ñ A    A  ; ˜A  	  A  v ¤A    ©A  ³ ËA    ÕA  	  ÚA  Ñ åA  	  ìA  Y ñA  	  öA  Ñ B    B  ± B  	  #B  v 4B    =B  ³ RB  	  WB  Ñ bB  	  iB  Y nB  	  sB  Ñ ƒB    ‘B  ; šB  	  ŸB  v ¦B    «B  ³ ÍB    ×B  	  ÜB  Ñ çB  	  îB  Y óB  	  øB  Ñ C    C  ±  C  	  'C  v 8C    AC  ³ VC  	  [C  Ñ fC  	  mC  Y rC  	  wC  Ñ ‰C    “C  ; C  	  ¢C  v ©C    ®C  ³ ·C    ĞC  | ÛC  	  àC  Ñ ëC  	  òC  Y ÷C  	  üC  Ñ D    D  ± $D  	  +D  v <D    ED  ³ ZD  	  _D  Ñ jD  	  qD  Y vD  	  {D  Ñ ‘D    µD  	  ºD  Ñ ÅD  	  ÌD  Y ÑD  	  ÖD  Ñ ìD    óD  ± şD  	  E  v E    E  ³ 4E  	  9E  Ñ DE  	  KE  Y PE  	  UE  Ñ eE    lE  ; yE  	  ‚E  v E  	  ”E  Ñ £E  	  ªE  Y ¯E  	  ´E  Ñ ÅE    ÎE  ³ ÙE  	  ŞE  Ñ éE  	  ğE  Y õE  	  úE  Ñ F    F  ± &F  	  -F  v >F    GF  ³ \F    cF  ; ƒF  	  ˆF  Ñ “F  	  šF  Y ŸF  	  ¤F  Ñ ËF    ĞF  ± ÛF  	  åF  v öF    G  ³ G  	  G  Ñ 'G  	  .G  Y 3G  	  8G  Ñ \G    aG  ; nG  	  sG  v ŠG  	  G  Ñ G  	  ¥G  Y ªG  	  ¯G  Ñ ÀG    ÅG  ³ êG  	  ïG  Ñ úG  	  H  Y H  	  H  Ñ !H    &H  ± 1H  	  8H  v IH    RH  ³ gH  	  lH  Ñ wH  	  ~H  Y ƒH  	  ˆH  Ñ ˜H    ¦H  ; ³H  	  ¸H  v ÅH  	  ÊH  Ñ ÙH  	  àH  Y åH  	  êH  Ñ ûH     I  ³ I  	  I  Ñ I  	  "I  Y 'I  	  ,I  Ñ BI    GI  ± VI  	  ]I  v nI    wI  ³ ŒI    šI  ; ¬I  	  ±I  Ñ ¼I  	  ÃI  Y ÈI  	  ÍI  Ñ ãI    êI  ± õI  	  üI  v J    J  ³ +J  	  0J  Ñ ;J  	  BJ  Y GJ  	  LJ  Ñ ^J    cJ  ; rJ  	  yJ  v ƒJ    ŒJ  ³ —J  	  œJ  Ñ §J  	  ®J  Y ³J  	  ¸J  Ñ ÓJ    ÚJ  ; åJ  	  ìJ  v ıJ    K  ³ K  	   K  Ñ +K  	  2K  Y 7K  	  <K  Ñ NK    SK  ; `K  	  eK  v oK    tK  ³ ÛK  	  ëK   L  	  L  Y "L  õ ML  c XL  Ö iL  ¢ sL  	  ™L   ³L  	  ºL  Y ÃL  õ îL  c ùL  Ö <M  	  UM   nM  Y ~M  õ ©M  c ´M  Ö ÙM  	  ŞM  Ñ éM  	  ğM  Y õM  	  úM  Ñ N    N  ; 'N  	  ,N  v 6N    ;N  ³ ]N  	  ¡N   ¾N  	  ÅN  Y ØN  õ O  c O  Ö O  ¢ (O  	  UO   oO  	  vO  Y O  õ ªO  c µO  Ö ğO  	  õO  Ñ  P  	  P  Y P  	  P  Ñ +P    0P  ; ;P  	  BP  v SP    \P  ³ ”P  ^ ¦P  	  «P   ÄP  Y ÏP  õ úP  c Q  Ö ;Q    jQ  	  –Q   ¯Q  Y ¿Q  õ éQ  c ôQ  Ö R  	  AR   ZR  Y eR  õ R  c ›R  Ö ÕR  	  S   S  Y *S  õ TS  c _S  Ö xS  	  ­S   ÆS  Y ÑS  õ üS  c T  Ö $T  	  )T  Ñ 4T  	  ;T  Y @T  	  ET  Ñ _T    dT  ; oT  	  vT  v ‡T    T  ³ ÈT  ^ ÚT  	  ßT   øT  Y U  õ .U  c 9U  Ö ÃU    ØU    ñU  	  V   V  Y (V  õ RV  c ]V  Ö zV  ¢ ‰V  ¢ “V  	  ²V  ^ ÆV   ßV  Y êV  õ W  c  W  Ö ®W  	  ÒW   ïW  Y ùW  õ $X  c /X  Ö HX  	  yX   ’X  Y X  õ ÇX  c ÒX  Ö Y  	  Y  Ñ Y  	  $Y  Y )Y  	  .Y  Ñ HY    OY  ; ZY  	  aY  v rY    {Y  ³ œY  	  ²Y   ÌY  	  ÓY  Y ÜY  õ Z  c Z  Ö 'Z  	  DZ   _Z  	  fZ  Y qZ  õ œZ  c §Z  Ö ğZ  	  [   3[  	  :[  Y J[  õ u[  c €[  Ö ˜[  	  É[   ä[  	  ë[  Y û[  õ \  È $\  Ö ?\  c J\  Ö U\  È \\  Ö ¯\    ´\    ¹\  ³ ü\  	  (]   ?]  	  F]  Y V]  õ ]  c Œ]  Ö ¥]  	  Ö]   ñ]  	  ø]  Y ^  õ *^  È 1^  Ö L^  c W^  Ö b^  È i^  Ö ›^     ^    ¥^  ³ ó^  	  _   6_  	  =_  Y M_  õ x_  c ƒ_  Ö ›_  	  Ì_   ç_  	  î_  Y ş_  õ  `  È '`  Ö B`  c M`  Ö X`  È _`  Ö ’`    —`    œ`  ³ ¾`  	  Ã`  Ñ Î`  	  Õ`  Y Ú`  	  ß`  Ñ õ`    ú`  ± a  	  a  v a    &a  ³ `a  	  ua   Œa  	  “a  Y £a  õ Ça  È Îa  Ö éa  c ôa  Ö b  È b  Ö :b    ?b    Db  ³ ub  	  zb  Ñ …b  	  Œb  Y ‘b  	  –b  Ñ ¬b    ±b  ± ¼b  	  Ãb  v Ôb    İb  ³ c  	  ,c   Cc  	  Jc  Y Zc  õ ~c  È …c  Ö  c  c «c  Ö ¸c  È ¿c  Ö øc  	  ıc  Ñ d  	  d  Y d  	  d  Ñ /d    4d  ± ?d  	  Fd  v Wd    `d  ³ šd  	  ¯d   Æd  	  Íd  Y İd  õ e  c e  Ö Se    Xe  ³ ve    {e  ³ Œe  n e  n Îe  ¢ èe  ¢ îe  o óe    f  b Cf  È Jf  Ö qf  È xf  Ö Ÿf  È ¦f  Ö Ëf   İf    óf  ; $g  	  )g  Ñ 4g  	  ;g  Y @g  	  Eg  Ñ Yg  ± bg  	  gg  v ng    sg  ³ …g    Šg  ± ˜g  	  g  Ñ ¨g  	  ¯g  Y ´g  	  ¹g  Ñ Ôg  ; ãg  	  èg  v ïg  	  ôg  Ñ ÿg  	  h  Y h  	  h  Ñ h    "h  ³ (h  	  1h  ³ Lh  ¢ Zh  w ih  	  nh  Ñ yh  	  €h  Y …h  	  Šh  Ñ ¨h    ­h  ± ºh  	  Äh  v Õh    ßh  ³ i    i  ó 9i    Qi    ki  ó ui    zi  ³ Œi    i  ; Ài  * ìi  ; ói  	  øi  Ñ j  	  
j  Y j  	  j  Ñ $j    +j  ; 8j  	  =j  v Ij    Nj  ³ bj  u ij  ¸ yj  § —j    œj    ¡j  ³ Ãj  x Ìj    Ñj    Öj  ³ ëj  w ÿj    k  ³ ;k    Bk  a bk  ] ‡k    Œk    ‘k  ³ ºk    ¿k    Äk  ³ Ìk    İk    âk    çk  ³ ïk    ÷k    ük    l  ³ l    l    l    l  	  l  ³ 'l    ,l    1l  ¤ 6l    ;l  ¤ Hl    Ml    Rl  ³ Zl    _l    dl    il  	  nl  ³ vl    l    „l    ‰l  ³ ‘l    ™l    l    £l  ³ «l    °l    µl  ³ ¾l    Çl    Ìl    Ñl  ³ Ùl    æl  	  íl    òl  ³ ıl    
m  	  m    m  ³ !m    .m  	  5m    :m    ?m    Dm  ³ Lm    Sm    \m    em    pm    um    zm    m  ³ ‡m    m    •m    šm  ³ ¢m    §m    ¬m  ³ ºm    Àm    Åm    Êm  ³ Òm    ×m    Üm  ³ åm  f ëm    ğm    õm    úm  	  ÿm  ³ n    n    n  ³  n  f (n    -n    2n    7n  ³ Kn    Qn    Xn    ]n    bn    gn  ³ rn    wn    |n    n  	  †n  ³ n    “n  	  ˜n    n  ³ ¢n    §n  ³ ¯n    ´n    ¹n    ¾n  ³ Ån    Ên    Ïn    Ôn    Ùn  ³ òn    ùn    şn    o    o  	  o  ³ o    o    o    $o  ³ +o    0o    5o  ³ >o  f Do    Io    No  ³ Wo  f ]o    bo    go    lo  ³ so    xo    }o    ‚o  	  ‡o  ³ o    ”o    ™o    o  ³ ¥o    ªo    ¯o  ³ ´o    ¹o    ¾o  ³ Îo    Óo    Øo  ³ æo    óo    øo    ıo    p  ³ 
p    p    p    p  	  p  ³ &p    =p    Bp  ³ Kp    Sp    [p    `p  ³ gp    tp    yp    ~p  	  ƒp  ³ ‹p    p    •p    šp  	  Ÿp  ³ §p    ¬p  § ñp  	  ùp  	  q  	  q  	  +q  	  <q  	  Jq  	  sq    xq  ³ }q  	  Šq    ‘q    ›q  	  ¯q  	  Àq  	  Îq  	  äq    éq  ³ õq    r    r  ³ r    /r    4r  ³ Gr    Qr    fr    kr  ³ wr    r  	  “r  	  r    ¢r  ³ «r    ²r    ¸r  	  ½r  ³ Är    Ğr  	  Õr  	  ãr    èr  ³ ğr    ùr  	  şr  	  s  	  s  	  s     s  ³ (s    ˆs    s  ³ s  	  £s    ¨s  ³ És    Îs  ³ Ös  	  às  	  ìs    ñs  ³ ös  	  ûs  	  t    t  ³ &t    +t  ³ 0t  	  5t  	  Gt    Lt  ³ `t    et  ³ yt    ~t  ³ ƒt  	  ˆt  	  št    Ÿt  ³ ¥t  	  ªt  ³ ¿t    Ät  ³ Ët  	  Ğt  	  ãt    èt  ³ ít  	  òt  	  u    
u  ³ u  	  u  	  'u    ,u  ³ ?u    Du  ³ Iu  	  Nu  	  iu    nu  ³ u    “u  ³ ¤u    ©u  ³ óu  	  ùu  	   v    v  ³ v    #v  ³ 3v    8v  ³ =v  	  Bv  	  Tv    Yv  ³ }v    ‚v  ³ êv    ïv  ³ şv    w    w  	   w    %w  ³ 9w    >w    Cw  ³ Jw    Ow    Tw    Yw  ³ `w    ew    jw    ow  ³ vw    {w    €w    …w  ³ Œw    ‘w    –w    ›w  ³ ¢w    §w    ¬w    ±w  ³ ¸w    Âw    Çw    Ìw    Ñw  ³ Üw    ïw    ôw    ùw    şw  ³ x    x    x    x    #x  ³ .x    Ax    Fx    Kx    Px  ³ Xx    gx  	  qx  	  ~x  	  x    ”x    ™x    x  ³ ªx    ¯x  	  ¹x  	  Ãx  	  Ğx    Õx    Úx    ßx  ³ çx    òx    ÷x    üx    y  ³ 	y    y    y    y     y  ³ (y    -y    2y    7y    <y  ³ Iy    Ny    Sy    Xy    ]y  ³ ey    jy    oy    ty  	  yy  ³ y    ‡y    Œy    ‘y    –y  ³ y    £y    ¨y    ­y    ²y  ³ ºy    ¿y    Äy    Éy  	  Îy  ³ Öy    Ûy    ày    åy    êy  ³ òy    øy    ıy  ³ z    z    z  ³ z    !z    &z    +z  ³ 7z    Bz    Hz  	  Mz  ³ Uz    az    fz  ³ mz    rz    |z  ³ ‚z    z    •z  ³ œz    ©z    ®z  ³ µz    Èz    Íz  ³ Üz    áz    æz    ëz  ³ ÷z    {    {  ³ {    %{    *{  ³ O{  z V{    d{    i{  ³ p{    u{    z{    {  ³ ˆ{    •{    š{  ³ ¡{    ¦{    ­{    ²{  ³ ¹{       §    	          ³    	       !   o &   ‡         £ À   	  Ä   	  È   	  D  	  H  	  L  	  P  	  T  	  X  	  \  	  `  	  d  	  h  	  à    ä           s 8    H  s p    €  s ¨  ] ¬  ’ °  ø ¸  Ô À  ß Ğ  . Ô  j Ø    Ü    ä    è    ì             $    <    @    l    p    t    x    „    °  í ´  V À  o Ä   Ì  0 Ğ  e Ø  ù Ü  ˜ à  »        (   …              $    (    h  J l    ˆ  J Œ    à    ä  ( è  … ô    ø                 À  	  Ä  	  È  	  Ì  	  Ğ  	  Ô  	  Ø  	  Ü  	  à  	  ä  	  è  	  ì  	  ğ  	  ô  	  ø  	  ü  	    	    	    	    	     	  $  	  (  	  ,  	  0  	  4  	  @  	  D  	  H  	  L  	  P  	  T  	  X  	  \  	  `  	  d  	  h  	  l  	  p  	  t  	  <	  J @	     
  	  $
  	  8
  	  d
  	     	    	    	    	    	  0  	  @  	  €  	  „      	                $  ( (  … 4    8    @    D    H    h  ı l    €  	  „  	  ˆ  	  Œ  	    	  ”  	  ˜  	  œ  	     	  ¤  	  ¨  	  ¬  	  °  	  ´  	  ¸  	  ¼  	  À  	  Ä  	  È  	  Ì  	  Ğ  	  Ô  	  Ø  	  Ü  	  à  	  ä  	  è  	  ì  	  ğ  	  ô  	  ø  	  ü  	     	    	    	    	    	    	    	    	     	  $  	  (  	  ,  	  0  	  4  	  8  	  <  	  @  	  D  	  H  	  L  	  P  	  T  	  X  	  \  	  `  	  d  	  h  	  l  	  p  	  t  	  x  	  |  	  €  	  „  	  ˆ  	  Œ  	    	  ”  	  ˜  	  œ  	     	  ¤  	  ¨  	  ¬  	  °  	  ´  	  ¸  	  ¼  	  À  	  Ä  	  È  	  Ì  	  Ğ  	  Ô  	  Ø  	  Ü  	  à  	  ä  	  è  	  ì  	  ğ  	  €    „         Ø        H  ] L  ’ P  ø X  Ô `  ß p  . t  j x    |    „    ˆ    Œ                    $    `  o d   l  0 p  e x  ù |  ˜ €  »      ¤  ( ¨  … ´    ¸    À    Ä    È    à  	  ô  	    	    	  0  	  D  	  X  	  l  	  €  	  ”  	  ¨  	     	  $  	  (  	  ,  	  0  	  4  	  8  	  <  	  @  	  D  	  H  	  L  	  P  	  `    t    €    „    ˆ    Œ        ”         Ä         $    ,    0    4    8    @    T                                                   $     (     ,     0     4     8     <     @     D     H     L     P     T     X     \     `     d     h     l     p     t     x     |     €     „     ˆ     Œ          ”     ˜     œ           ¤     ¨     ¬     °     ´     ¸     ¼     À     Ä     È     Ì     Ğ     Ô     Ø     Ü     à     ä     è     ì     ğ     ô     ø     ü                                           $    (    ,    0    4    8    <    @    D    H    L    P    T    X    \    `    d    h    l    p    t    x    |    €    „    ˆ    Œ        ”    ˜    œ         ¤    ¨    ¬    °    ´    ¸    ¼    À    Ä    È    Ì    Ğ    Ô    Ø    Ü    à    ä    è    ì    ğ    ô    ø    ü                                          $    (    ,    0    4    8    <    @    D    H    L    P    T    X    \    `    d    h    l    p    t    x    |    €    „    ˆ    Œ        ”    ˜    œ         ¤    ¨    ¬    °    ´    ¸    ¼    À    Ä    È    Ì    Ğ    Ô    Ø    Ü    à    ä    è    ì    ğ    ô    ø    ü                                          $    (    ,    0    4    8    <    @    D    H    L    P    T    X    \    `    d    h    l             o    J              o    J $     (     ,   o 0   J 8     <     @   o D   J L     P     T   o X   J `     d     h   o l    t     x     |   o €    ˆ     Œ        o ”   J œ           ¤   o ¨   J °     ´     ¸   o ¼    Ä     È     Ì   o Ğ   J Ø     Ü     à   o ä   J ì     ğ     ô   o ø   J            o   J           o     (    ,    0  o 4  J <    @    D  o H  J P    T    X  o \  J d    h    l  o p  J x    |    €  o „  J Œ        ”  o ˜  J      ¤    ¨  o ¬  J ´    ¸    ¼  o À  J È    Ì    Ğ  o Ô  J Ü    à    ä  o è  J ğ                                                   $     (     ,     0        	  ´  	  È  	  Ü  	  ğ  	    	  	  	  ,
  	  @  	  T  	  h    |      	  ¤  	  ¸  	  Ì  	  à  	  ô  	    	    	  0  	  D  	  X  	  l  	  €  	  ”  	  ¨  	  ¼  	  Ğ   	  ä!  	  ø"  	  $     %  	  4&  	  H'  	  \(  	  p)  	  „*  	  ˜+    ¬,    À-  	  Ô.  	  è/  	  ü0  	  2    $3  	  84  	  L5  	  `6    t7  	  ˆ8  	  œ9  	  °:  	  Ä;  	  Ø<  	  ì=  	   ?  	  @  	  (A    <B  	  PC    dD    xE  	  €G    ´I    hJ  	  lJ    pJ    tJ     K  	  ,K  	  XK  	  „K  	  °K  	  ÜK  	  L  	  4L  	  `L  	   L  	  ÌL  	  øL  	  $M  	  PM  	  |M  	  ¨M  	  ÔM  	   N  	  €N  	  œN  	  ¸N  	  ÔN  	  O    O    ¤O    ¨O    äQ    èQ    ìQ    ğQ    ÌR    ,S    0S    äS    tT    T     W    TY                                   $     (     0     4     <     @     H     L     T     X     `     d   	  l     p     x     |      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.text.unlikely .rel.init.text .rel.exit.text .rel.rodata .rel__mcount_loc .rodata.str1.4 .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .rel.data .rel__bug_table .bss .comment .note.GNU-stack                                                      4   (                  2             `   óĞ                 .   	   @        (6              <             SÑ  ½{                 8   	   @       ,Æ  S              O             M *                  K   	   @       , @               ^             :M 
                  Z   	   @       l                m             `M                   i   	   @       | À	     
         y             ác p                 u   	   @       <# à              †      2       Tg :>                •      2       ¥ K                ¤             Ùº \	                 ±             8Ä ô                 ­   	   @       *                ½             ,Æ 4                  ¹   	   @       <- h               Ì             `Æ €Y                  È   	   @       ¤- X              Ö             à „                  Ò   	   @       ü0 °               â             €  ¼                  ç      0       €  C                ğ              Ã!                                  Ä! à5     ]        	              ¤W ^8                               ¬1                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ELF                      ¬2     4     (            GNU  À       À          èüÿÿÿWV‰Æ‰Ğ‰ÊS‹Ü   ‹|$‹™  ƒãıƒø‹Ü   Àƒàş  	Ø‰  ‹†Ü      ‹ 1Û€ä_…Ò•ÃÁã	Ã‹†Ü   ‰˜   ‰Ø‰ÚÁøƒà…ÿEø‹†Ü   €Î ‰   ¸Ç  èüÿÿÿ‹†Ü   ‰˜   ‰ø[^_ÃfèüÿÿÿS‰ÃÃ¼  ƒìHd¡    ‰D$D‰àèüÿÿÿ‹D$,=9‰ ‡L  =7‰ ‡   =Õˆ ‡6  =Óˆ ‡   =Q… ‡   =O… ‡   =í„ ‡
  =ë„ ‡   =Wu „   ‡î  =9K „   ‡İ  =ÅØ „   ‡Ì  =»Ø „   ‡»  =ÙN „   ‡ª  =L „   ‡™  =o> „   ‡ˆ  =e> „   ‡w  =“8 „   ‡f  =77 „   ‡U  =Ó6 „   ‡D  =É6 „   ‡3  =6 „   ‡"  =ı4 „   ‡  =™4 „   ‡   ='3 „   =Ã2 ‡ê   =¤2 ‡?   =»0 „   ‡Î   =û, „   ‡½   =ñ, „   ‡¬   =Ó, „   ‡›   =É, „   ‡Š   =9+ „   w}=? „   wp=+ „   = w^= ‡W   = „   wF= „   w9=¿ „   w,=¼ „   w=a „   w=V  ‡üÿÿÿ=ğU  wPSh    èüÿÿÿƒÄé   éo   ´&    v èüÿÿÿQÿUWVSƒú†Ó   ‹8‰Î…É…í   ‹¸  ƒùwƒùws[‰ğ^_]Ãƒù„Í   ƒù,„®   wåƒù*u¸ûÿÿÿ½   ‹—Ü   Â  ‹
!È‰»È   ¶    ¸X‰A èüÿÿÿƒëuñ‹‡Ü     ‹€  	Å‰*[‰ğ^_]Ãº   Óâ÷Â`2 u€æu@ƒù…pÿÿÿ¸ıÿÿÿ½   ëv ƒéƒá÷„uÿÿÿéPÿÿÿ´&    1ö[‰ğ^_]Ã´&    ‹hƒıtƒı…(ÿÿÿéCÿÿÿ‹P‰øèüÿÿÿ‰ğ[^_]Ã¸şÿÿÿé2ÿÿÿ´&    èüÿÿÿS‰Ã‹€¸  ƒø„	  wwƒø	„#  †   ƒø„'  †Q  ƒøt†  ƒø„½  ¹   ‰Øº `  èüÿÿÿº `  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº @  èüÿÿÿ¸d   [éüÿÿÿ´&    ƒø)‡  ƒø'‡  ƒø!„Ã  †G  ƒø&„r  ƒø'…é  ‹ƒÜ   (  ‹ƒâş‰‹ƒÜ     ‹Ê   ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âşÿşÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âüÿüÿ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹Ê  ‰¹   ºø*  ¸'  èüÿÿÿ‹ƒÜ     ‹âıÿıÿ‰‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿûÿ‰‹ƒÜ     ‹€    ‰¸<   [éüÿÿÿ´&    fƒøvPƒø„î   ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   ‰Øèüÿÿÿ¸d   [éüÿÿÿƒø$…[  ¹   ‰Øº`  èüÿÿÿ‰Øº`  èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   [éüÿÿÿv ƒø„/  v]ƒøuT‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâû‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãfƒøt£ƒøuò‹ƒÜ     ‹€    ‰[Ãv ƒø„  †  ƒø†hÿÿÿƒøu·‹ƒÜ   º6   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ      ‹ƒÊ‰¸È   èüÿÿÿ‹ƒÜ      ‹€æ÷‰¸È   èüÿÿÿ‹ƒÜ      ‹€   €Ì‰¸È   [éüÿÿÿ´&    ƒø„·  †É   ƒøt	ƒø…ÿÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ãƒø
…¸şÿÿ‹ƒÜ     ‹Ê   ‰¸   èüÿÿÿ‹ƒÜ     ‹ƒâğ‰¸   èüÿÿÿ‹ƒÜ     ‹€    ‰[Ã¶    ƒø„aşÿÿƒø„nÿÿÿƒø…Aşÿÿ¹   ‰Øº À  èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿº À  ‰Øèüÿÿÿ¸d   èüÿÿÿ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ‰Øº   èüÿÿÿ¸   [éüÿÿÿt& ƒø1„÷  †   ƒø5w\ƒø3wc‹ƒÜ   º7   ‰(  ¹   ‰Øº  èüÿÿÿº  ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº  [éüÿÿÿfƒø%„Œıÿÿé>ıÿÿfƒè8ƒø‡0ıÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   é=üÿÿƒø-„cÿÿÿ†ª   ƒø1„çüÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âûÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  ‰‹ƒÜ     ‹âıÿşÿ‰º7   ‹ƒÜ   ‰(  º   ‹ƒÜ   ‰$  ‹ƒÜ   º Ã  ‰   1Ò‹ƒÜ   ‰  [Ãƒø*„éûÿÿƒø,„|ıÿÿ[Ã¶    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âùÿşÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹ƒÊ‰º7   ‹ƒÜ   ‰(  º P  ‹ƒÜ   ‰$  º   ‹ƒÜ   éXÿÿÿ´&    ¹   ‰Øº   èüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿ‰Øº   [éüÿÿÿt& ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹ƒâú‰¸   èüÿÿÿébüÿÿf¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸d   èüÿÿÿº   éIúÿÿ´&    ‹ƒÜ     ‹Ê   ‰‹ƒÜ     ‹âûÿüÿ‰¸d   èüÿÿÿ‹ƒÜ     ‹Ê  é-şÿÿ´&    t& èüÿÿÿUWVS‰Ãƒì0‹‹¸  d¡    ‰D$,1Àƒù„w  vEƒéƒùw¸   Óà© ãud¨…×  ƒùt71ö‹D$,d+    …±  ƒÄ0‰ğ[^_]Ãt& ƒù„§   1öƒùvĞƒùuh    1öjèüÿÿÿXZë¹´&    fƒùu«‹5   …ötŸº   ‰Øèüÿÿÿ‰ƒ¨  …À„?  ‹ƒ”  …À„wÿÿÿ‹Pd‹…Ò„jÿÿÿ‹Z…Û„_ÿÿÿ‹    ¹   …Ò„æ  ‹r…ö„Û  º   èüÿÿÿ1öé2ÿÿÿf‹   …É„ ÿÿÿº   ‰Øèüÿÿÿ‰ƒ¨  …À„À  ‹ƒ”  …À„øşÿÿ‹Pd‹…Ò„ëşÿÿ‹Z…Û„àşÿÿ‹    ¹    …Ò„x  ‹r…ö„m  º   èüÿÿÿ1öé³şÿÿv ‰Øèüÿÿÿ‰Æ…À…Ÿşÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‹ƒ”  …À„}şÿÿ‹Pd‹…Ò„pşÿÿ‹Z…Û„eşÿÿ‹    ¹   …Ò„İ  ‹z…ÿ„Ò  º   èüÿÿÿé:şÿÿ´&    v ‰Øèüÿÿÿ‰Æ…À…şÿÿº   ‰Øèüÿÿÿ‰ƒ¨  ‰Á‹ƒ”  …ÀtA‹Pd‹…Òt8‹z…ÿt1‹    ¹    …Ò„@  ‹j…í„5  º   èüÿÿÿ‹‹¨  v …É„¶ıÿÿ‹Ad‹@…À„Ú   ‹x…ÿ„Ï   ¡   ‰â…À„Ô   ‹h…í„É   ‰Èèüÿÿÿ‹‹¨  ÆD$ ÆD$
 ÆD$…É„_ıÿÿ‹Ad‹@…Àtw‹x…ÿtp¡   ‰â…À„¸   ‹h…í„­   ‰Èèüÿÿÿ‹‹¨  ÆD$
…É„ıÿÿ‹Ad‹@…À„ıÿÿ‹X…Û„ıÿÿ¡   ‰â…ÀtN‹x…ÿtG‰Èèüÿÿÿéåüÿÿt& ÆD$
ë½´&    fÆD$ ÆD$
 ÆD$é[ÿÿÿ‰Èèüÿÿÿ‹‹¨  é2ÿÿÿ‰Èèüÿÿÿéüÿÿº   èüÿÿÿ‹‹¨  éÉşÿÿ‰Èèüÿÿÿ‹‹¨  éNÿÿÿº   èüÿÿÿéhüÿÿº   1öèüÿÿÿéWüÿÿº   èüÿÿÿéFüÿÿ¾íÿÿÿé>üÿÿèüÿÿÿ´&    fèüÿÿÿ‹ˆ¸  ƒù3wpS‰Ãƒùvƒé¸   Óà© ãu¨u3[Ãƒùt
ƒùt&ƒùuî‰Øº   èüÿÿÿÇƒ¨      [Ã´&    f‰Øº   èüÿÿÿ‰ØèüÿÿÿÇƒ¨      [ÃÃ´&    ´&    èüÿÿÿ‹ˆ¸  ƒù3wƒùvƒéº   Óâ÷Â ãuƒâu,Ãv ƒùt
ƒùtƒùuí‹¨  …Òtãº   éüÿÿÿt& ‹ˆ¨  …ÉtÊº   éüÿÿÿt& èüÿÿÿWVS‰Ãƒì‹»`  d¡    ‰D$1À…ÿ„9  ‹ƒ¸  ‰Âƒø	t‡æ  ƒø„¬  Hûƒù‡1  ‰Âƒø&†  ƒø2„µ  †ç  ƒø5„f  †Ø  ƒø;„ˆ  †¶  HÄƒù‡\  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     ƒø"‡½  Jú¸   Óà©ÅXß„³  j “
  ¹9   CjDèüÿÿÿ‰ƒ”  ZY…À„Ô  “ğ   ‰˜   ‹ƒ”  Ç€      ‹ƒ”  …À„ª  ‹Pd‹…Ò„  ‹R…Ò„’  ‹    …É„|  ‹I…É„q  èüÿÿÿ‹“¸  éo  ƒøt3†Û   Höƒù‡X  Çƒ     Çƒ      Çƒ$     v ÇƒØ     ÇƒÜ     Çƒà     ƒú"‡¶  ƒú‡ğşÿÿBÿƒø†ùşÿÿƒú„"  ƒú„  ‹D$d+    …3  ƒÄ[^_ÃfHÊƒù‡%şÿÿ¾P   ƒô  ¹   º    f‰³ö  èüÿÿÿ‹»`  …ÿ„
  ‹ƒ¸  ééıÿÿƒø…€  ÇƒØ    ‹ƒÜ   º   ÇƒÜ     Çƒà     Çƒä      Çƒè  ÌG ‰„ Çƒ     ‹“¸  Çƒ      Çƒ$     éûşÿÿt& èüÿÿÿv ‹“¸  ƒú6…Á   j “ü   C¹A   jAèüÿÿÿ‹“¸  _Xƒú…Şşÿÿ‹D$d+    …  ƒÄ‰Ø[^_éüÿÿÿHÈƒù†Jıÿÿé¡  fƒø3…§   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     BÜƒø‡Mÿÿÿ¹ùş}£Á‚>ıÿÿƒú6„?ÿÿÿƒú7…Uÿÿÿj »ü   s¹A   j@‰ú‰ğèüÿÿÿ‰ú¹A   ‰ğj jAèüÿÿÿ‹“¸  ƒÄéÿÿÿt& ƒø4…æ  ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     éKÿÿÿ¶    ƒø)wKƒø'…  ÇƒØ     ÇƒÜ     Çƒà     Çƒ    Çƒ      Çƒ$     éNüÿÿƒø-„°şÿÿv,ƒø2„;  ÇƒØ     ÇƒÜ     Çƒà     éøûÿÿƒø,„·üÿÿé
  v ƒø‡×   ƒø„ö  †€üÿÿƒø„fıÿÿ†	  ƒø…×   ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é­üÿÿ´&    ¾P   º    ƒl  ¹   f‰³n  èüÿÿÿ‹ƒ¸  ‰Âƒø†®   HÜƒù‡³úÿÿ¾   Óæ‰ñ÷Æ… ó…H  á   …müÿÿƒø&‡’úÿÿHâ¾   Óæ‰ñöÁ¨…×şÿÿƒá…­úÿÿƒø&„¡  Çƒ     ‰ÂÇƒ      Çƒ$     éÂûÿÿt& ƒø„rúÿÿvHêƒù†…şÿÿë¾´&    …vşÿÿë¯…Àt«‰Á¾   Óæ‰ñ÷Æ†<……   á`  u]ƒø……şÿÿ‹‹`  …É…uÿÿÿ€=    „tKºà   ‰ØèÑçÿÿ‹ƒ¸  ‰Âéµùÿÿt& ƒú…müÿÿ‹5   …ö„Oûÿÿé1úÿÿt& ‹“`  …Ò…ùÿÿº    ‰Øè†çÿÿé|ûÿÿ‹‹`  …Ét™éşÿÿÇƒØ     ÇƒÜ     Çƒà     éóúÿÿt& ÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é”ùÿÿ´&    ÇƒØ     ‹ƒÜ   ÇƒÜ     Çƒà       Çƒ     Çƒ      Çƒ$     ‹Ê  ‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹ƒâı‰¹   º'  ¸è  1öèüÿÿÿv ‹ƒÜ     ‹ƒâø‰¹   º'  ¸è  èüÿÿÿ‹ƒÜ     ‹:‰ğ‰ñÁø÷Ñ¶€    ƒáÓø‰Áƒá‰ÈƒÈ÷Ğ!ø	ÈƒÈ‰¹   º'  ¸è  ƒÆèüÿÿÿƒşHu†‹ƒÜ     ‹ƒÊ‰‹“¸  éOùÿÿ´&    fÇƒØ     ÇƒÜ     Çƒà     Çƒ     Çƒ      Çƒ$     é5ùÿÿ´&    ÇƒØ     ÇƒÜ     Çƒà     éùÿÿƒ»`   „{ıÿÿéD÷ÿÿ‰Ø‰\$ÇD$H   ÇD$    èüÿÿÿƒü   º    èüÿÿÿ¡ŒF  …À„6  é1  èüÿÿÿèüÿÿÿ‹P(Ç   ‹‚ì  ¯‚ğ  ‹’è  ¯B‹T$Áè‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ãƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®ø  ø  ‰Âƒx  9étS‹«ˆ  M    ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ‹E‹‹  ƒ=x  ‰H‡ø  ƒÄ‰ø[^_]éüÿÿÿt& ‹®ü  ‰†ü  ‰‹x  ‰«|  ‰E ƒ=x  ‡Ö  ƒÄ‰ø[^_]éüÿÿÿfèüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâî‰½¸  ø  ‰øèüÿÿÿ‰Æ‹…ø  9Ãt@v ‹…ø  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…ø  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿVSèüÿÿÿ‹°(  ‹FƒÆ9Æt$X´‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Æuà1À[^Ã´&    t& èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwPÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u§¸êÿÿÿëæ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwH‹…ÀuK‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  u¯¸êÿÿÿëæèüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ‹‡¸  ƒø4tƒè8ƒøwX‹…Àu[F¹   ºª  ƒÇèüÿÿÿ‹X´9Çt(t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¶    ƒ¿€  uŸ¸êÿÿÿëæèüÿÿÿS‰Ëèüÿÿÿƒ;‹(  uH‹Cƒèƒøw=ÇC    ÇC    ÇCĞ  ‹‚x  % ù  ƒøÀƒà`à  ‰C1À[Ã´&    f¸êÿÿÿ[Ã´&    fèüÿÿÿV‰ÎS‹\$èüÿÿÿ‹€(  ‹€x  %ÿÿ ƒşu(ƒøÒƒâÕƒÂ6ƒøÀ‰ƒàÏƒÀ;‰C1À[^Ã´&    ¸êÿÿÿëî´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…  ‹†x  ‹–|  ‰1À‰S[^Ã´&    fèüÿÿÿVS‰Ëèüÿÿÿ‹(  ‹‚ì  ‰C‹Šğ  ‰K‹²ô  ‰s‹²è  ‹6‰s‹’è  ¯BÇC   Áè¯È‰C1À‰K[^Ãt& èüÿÿÿ‹…Àu¡ÔI  ‰A,1ÀÃ´&    v ¸êÿÿÿÃ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿU1ÒWVS‰Ãƒì‹ ‹p(‰Øèüÿÿÿ‹k‰Ç‹†è  ‹@¯†ì  Áè‰ƒ€  ‰Â…í…¼   ¯–ğ  …Ò…  ‹–ô  ƒú„À   wTƒú„+  ƒú…  ‹®Ì   ÿ¶ğ  “„  j P‰èj jÿ‹èüÿÿÿƒÄƒ=x  ‡6  1ÀƒÄ[^_]Ã¶    ƒú„  ƒú…J  ‹ğ  ‹®Ì   “„  ÑéQ¯Èj P‰èj Q‹èüÿÿÿƒÄë¥´&    v ¯–ğ  9ST‚  ‰SP‹–ô  ƒú…@ÿÿÿ‹x  ÷†x   ù  ti¸    ‹‰  …É„´   …Ò…p  Ç$    ‰Á‹–Ì   «„  ‰T$‹–ğ  ÑêR‰êQQPÿt$‹‹D$èüÿÿÿƒÄé	ÿÿÿ¶    ‹®Ì   ÿ¶ğ  “„  j Pjÿj ‹‰èèüÿÿÿƒÄéÙşÿÿ¶    ‹ğ  ‹®Ì   “„  ÑéQ¯Èj PQëÉ´&    v ƒÄ¸êÿÿÿ[^_]Ãv …Ò…’  ‰$‰Á1ÀéIÿÿÿ¸êÿÿÿéŠşÿÿèüÿÿÿVS‰Ëèüÿÿÿ‹5ÔI  ‹€(  9s…­   ‹€x  ‹S% ù  ƒøÀƒà`à  …Òt^ƒúwyƒú…   º   j s¹Ğ  j PC‰Sº0   j P‰ğjèüÿÿÿ‹C¯ØI  ÇC   ƒÄÁè‰C¯C‰C1À[^Ã‰Áº   Ñé9Kw§º   ‰Èë´&    fJüƒù¹   CÑë…¸êÿÿÿ[^Ã´&    Ñè‰Á‰Èéjÿÿÿt& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìpd¡    ‰D$l‰Øèüÿÿÿ¹   ‹€(  |$ÇD$   ‰$1Àƒ=x  ó«‡´  ‰é‰ò‰Øè·şÿÿ‰D$‰Á…À…U  ‹<$‹Ÿ   …Û…]  ‹—`  …Ò…O  ‹‡Ä  …À…A  ¡ÔI  9E¸ÔI  EÁƒ=x  ‰‡è  ‹E‰‡ì  ‹U‰—ğ  ‹M‰ô  ‡Ğ  ‹E‹]0ÇD$$   ‹<$‹u‹M‹U‰D$(‰D$‹E(ƒÇ‰t$ f‰D$0f‰D$‹E,‰L$,f‰D$2f‰D$·Ãf‰\$4‹‰T$‰\$ƒëL;|$tYt& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ‹D$(‹T$‹t$ ‹L$,‰D$·D$0f‰D$·D$2f‰D$·D$4‰U‹T$‰E0‹$‰U·T$‰u‰U(·T$‰M‰U,‹€ô  ‰E‹D$ld+    u‹D$ƒÄp[^_]ÃÇD$ğÿÿÿëİèüÿÿÿ´&    t& èüÿÿÿUWVS‰ËèüÿÿÿºĞ  ¹   ‹°(  ‰Øèüÿÿÿº    i†¸    ‹¸    ‰øèüÿÿÿƒøÿ„ù  k‰Áƒø tƒÁƒù ‡ï  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hØ  PèüÿÿÿÇCT …‹†¸  ƒÄƒø4tƒè8ƒøwÇCT …1À[^_]Ãt& ƒ¾€  uä[1À^_]ÃèüÿÿÿW‰×V‰ÆS‹‹@P‰V‰ƒôşÿÿèüÿÿÿ‰ƒœıÿÿ‰“ ıÿÿƒ=x  ‡  ³ˆıÿÿ‹‹C‰B‰‰ğº   Ç   ÇC"  [^_éüÿÿÿ´&    fèüÿÿÿWVS‰Ã¡x  …À…+  ‹ƒúwrs{1ÀÇC    ƒçü‰ñÇF,    )ùƒÁ0Áéó«‹<•À   º    ‰øèüÿÿÿ‰Áƒøÿ„Q  ƒø tƒÁƒù ‡G  ‰ú‰ğèüÿÿÿÇC$   1À[^_Ã´&    ¸êÿÿÿëí´&    fèüÿÿÿ‰ÈéTÿÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹(  kŠl  i‚¸    ‹„,   ¹   ƒèƒøv‹Šp  ‰¡x  …À…[  ‰Ø[éûşÿÿ´&    t& èüÿÿÿU‰ÍWV‰ÖS‰Ãƒì¡x  …À…x  èüÿÿÿ‰Â…Àt]‰Ç¹Í   Có¥‰‚¬  ƒ  Ç‚      ‰‚0  iƒ¸    Uÿ°    ‚À  hà  j ‰T$Pèüÿÿÿ‹T$ƒÄ‰š(  ƒÄ‰Ğ[^_]Ã´&    ´&    èüÿÿÿV‹x  S‹X(‹³ø  …Ò…”  ‹ƒÜ   @  ‹ƒâî‰‹“d  ‹Nÿv‰Øèüÿÿÿ‹ƒÜ   º   ‰0  Çƒ       ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ‰X1À[^Ãt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…°  1À[Ãv èüÿÿÿ‰Ğ‰Ê‹9Èt	éìüÿÿt& Ã´&    èüÿÿÿUWV‰ÆS‰Ë¹   ƒìd‰$|$d¡    ‰D$`1ÀÇD$   ó«‹=x  ÇD$   …ÿ…6  ‹†|  ‹$3–x  1Ø	Ğ„ÿ   ‹   …É…  ‹–`  …Ò…ú   ‹†Ä  …À…ì   ‹$~‰|  ‰İÇ†ì  Ğ  ‰†x  % ù  Ç†ô     ƒøÀƒà`à  ‰†ğ  ‹FX´9Ç„‡   ‰t$v ‹Cd‹@…Àt‹p…öt‹$‰é‰Øèüÿÿÿ‹CLX´9ÇuÙ‹t$‹ğ  ‹–ô  ‹F‹¶ì  ‰L$‰t$X´‰T$9Çt-t& ‹Cd‹@…Àt‹p…ötL$1Ò‰Øèüÿÿÿ‹CLX´9ÇuØ1À‹T$`d+    u8ƒÄd[^_]Ã¸ğÿÿÿëä‰×‰Ğ‰ÚèüÿÿÿPWh˜  h˜  h0  èüÿÿÿƒÄé¡şÿÿèüÿÿÿt& èüÿÿÿWVS‹t$‹|$èüÿÿÿ‹˜(  ¡x  …À…Í  ‰ò‰ù‰Ø[^_éşÿÿ´&    v èüÿÿÿS
  ƒìd‹    ‰\$1ÛˆT$º   f‰T$T$ˆL$¹   ‰T$‰âÇ$L   èüÿÿÿ‹T$d+    uƒÄ[Ãèüÿÿÿèüÿÿÿƒì 
  d‹    ‰L$1ÉˆT$T$¹   ‰T$º   f‰T$T$f‰L$¹   ‰T$‰âÆD$ Ç$L   ÇD$L  èüÿÿÿƒø…é  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    v èüÿÿÿWVS‰Ãƒìd¡    ‰D$1À¡x  …À…ÿ  ƒú„©   ƒú…ˆ   º   ‰ØèüÿÿÿƒÈƒÈ |$T$¹   ³
  ˆD$f‰L$‰ğ¹   ÆD$ÇD$L   ‰|$èüÿÿÿ¸   ¹   f‰D$¸   T$f‰D$‰ğÇD$L   ‰|$èüÿÿÿ¡x  …À…$  ‹D$d+    uƒÄ[^_Ãt& º   ‰Øèüÿÿÿƒà÷é[ÿÿÿèüÿÿÿ´&    ´&    èüÿÿÿU‹-x  WV‰ÖS‰Ã…í…o  ‹“¸  ‰³l  Bê‰Ñƒø†›   ƒú!„’   ‹ƒ”  …À„Ä   ‹Hd‹y…ÿ„&  ‹?…ÿ„  kî‹   iÒ  ‹”0   …É„^  ‹)…í„T  j 1Éèüÿÿÿ_‹“¸  ƒú„‹   ƒú „‚   ƒúvlBßƒøw
¹Iè#£Árk[^_]Ã¶    kşiÂ  ƒ¼,   …Wÿÿÿº   ‰Øèüÿÿÿ‹“¸  ‹ƒ”  ‰Ñ…À…Cÿÿÿ´&    ƒú t}ƒútxƒúw”ƒúv¡¸ £Ğs—f‹ƒ”  …ÀtV‹Hd‹I…ÉtL‹y…ÿtEkî‹   iÒ  ‹”4   …Étc‹i…ít\j 1Éèüÿÿÿ‹‹¸  Zëfƒú t¸ƒú… ÿÿÿë­‰Ñköº   iÉ  ‹„4   ƒøtƒø…ÿÿÿº   ‰Ø[^_]é;ıÿÿv j 1Éèüÿÿÿ‹‹¸  Xë¸¶    j 1ÉèüÿÿÿYé§şÿÿèüÿÿÿS‰Ãƒì¡x  …À…¯  kÒiƒ¸    ƒ¼4   tƒÄ‰Øº   [éÑüÿÿƒÄ‰Øº   [éÀüÿÿèüÿÿÿV‰ÎSèüÿÿÿ‹˜(  k“l  iƒ¸    ‹„,   ƒèƒøv,‹ƒúw=¡x  …À…Ô  ‰“p  ‰ØƒÂèhüÿÿ1À[^Ãv ƒ>¸êÿÿÿº    [DÂ^Ãt& ¸êÿÿÿëÛèüÿÿÿUWV‰ÖS‰Ãƒì‹x  …É…ó  ‹>ƒÿ‡ı   kÇi“¸    ‹”,   …Ò„á   ÇF$   i“¸    ‹„,   º    ‹,…@  F‰$‰èèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡  ‹$‰êèüÿÿÿÇF0÷r k×ÇF4    iƒ¸    ‹„,   º   ƒèƒøwÇF$   º   ‰V(9»l  t1ÀƒÄ[^_]Ãt& ‹CƒÃx´9ÃtåƒÆ8‹Gd‹@…Àt‹H$…Ét	‰ò‰øèüÿÿÿ‹GLx´9ÃuÜë¼¸êÿÿÿë·v èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡x  …À…#  ‰Ú‰ğ[^é¤şÿÿt& èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹€l  ‰‹x  …Ò…?  1À[Ãv èüÿÿÿVS‰Ëèüÿÿÿ‹x  ‹°(  …Ò…\  ƒûw9kÓi†¸    ‹„,   …Àt!‰Ú‰ğèhûÿÿ‰ğ‰ÚèOıÿÿ1À[^Ã´&    v ¸êÿÿÿëì´&    fèüÿÿÿë‰´&    fèüÿÿÿUWVS‰Ëƒì$d‹    ‰T$ 1Òèüÿÿÿ‹°(  ‹†¸  ƒø:wƒø#‡Î   Pìƒâû„â   ƒè8ƒø‡6  ‹…Ò…<  ‹Cº		˜ n‰†Ü  F\èüÿÿÿ‰Ç…À„A  èüÿÿÿ‰D$…À„  ‹Fp´9Å„8  f‹Fd‹@…Àt‹H…Ét	‰Ú‰ğèüÿÿÿ‹FLp´9ÅuÜ¸d   èüÿÿÿ‹D$Ç$    …À…ˆ  ‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿé]  v PÜ¹ q £Ñrƒø4„4ÿÿÿé#ÿÿÿt& ‹K‹†x  ÇD$   ‹–|  ÇD$   ‰Ü  ‰D$F\‰T$º		˜ ‰L$èüÿÿÿ‰D$Ç$   …Àtèüÿÿÿ‰$…À„k  ‹=x  …ÿ…«  †,  º   èüÿÿÿ…À„í   ‹@‰D$‹†¸  Pìƒâû„ä   ƒè$ƒøwº q £Â‚Î   ‹D$…À„Õ  ‹ˆÔ  …É„Õ  ‹FnX´9Åt?´&    ‹Cd‹@…Àt‹x…ÿt‹–x  ‹|  ‰Øèüÿÿÿ‹CLX´9ÅuÒ‹D$‹ˆÔ  ‹D$T$èüÿÿÿ¸d   èüÿÿÿ‹$…É„ş   Ç$    ‹D$ d+    …  ‹$ƒÄ$[^_]Ãv ƒ¾€  …½ıÿÿv Ç$êÿÿÿëÈ´&    †ø  ‰D$é#ÿÿÿ´&    f‹FÇD$   p´9Å…Êıÿÿ¸d   èüÿÿÿÇ$    ë€´&    ‹|$‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿéjşÿÿt& ‹G‹@èüÿÿÿº   ‰øèüÿÿÿ‹G‹@èüÿÿÿ‹Fp´9Å…Pıÿÿ¸d   èüÿÿÿé‚ıÿÿ‹|$‹G‹@èüÿÿÿ1Ò‰øèüÿÿÿ‹G‹@èüÿÿÿéáşÿÿèüÿÿÿv èüÿÿÿéfüÿÿ¶    èüÿÿÿWV‰ÆS‹€Ü      ‹ …Â„¡   ‹†Ü   ‰Ó‰$  ÷Â  t+÷Â   …  öÇ…†   öÇtƒ=x  ‡ë  t& 1ÿöÃu‰Ú‰ğèüÿÿÿ[^ø_Ã´&    v ¾¸  ‰øèüÿÿÿ‹†Ü   ‹ˆ   ‹–ø  †ø  9Ğt‰Êèàïÿÿ‰ø¿   èüÿÿÿë©f[1À^_Ã´&    v ƒ=x  ‡L  1ÿöÃtƒëš´&    v èüÿÿÿ‹x  S‰Ã…Ò…h  ‰Øº   èüÿÿÿ‹ƒä  …Àt‹ğ  ƒâuYèüÿÿÿÇƒä      ‹ƒà  …Àt‹ğ  ƒâu%èüÿÿÿÇƒà      ‹ƒ0  …Àt,‰Ø[éüÿÿÿt& èüÿÿÿëÙ´&    fèüÿÿÿë¥´&    f[Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì@‹x  d¡    ‰D$<1À…É…@  ¿@  ¾ F  ¹Í   º  ó¥¸   ‰Í¹   èüÿÿÿ¸ÔI  º   =ÔI  YUYVÇƒx     EÅÇƒ|      ‰ƒè  ƒø  ‰ƒø  ‰ƒü  ƒX  ‰ƒX  ‰ƒ\  ‰ØÇƒô     Çƒì  Ğ  Çƒğ  à  èüÿÿÿƒ»€  t‹ƒˆ  ƒø†ÿ  ³  »  ‰ğèüÿÿÿ‹‹|  ‹“x  ‰Øèüÿÿÿ1Ò‰Øèàôÿÿ1Ò‰ØèÇöÿÿ‰ğèüÿÿÿ‹ƒÌ   Çƒ     Çƒ     ˆ   ‰›,  ‰ƒ  ‰øÇƒ<     Çƒ@     Çƒ4     Çƒ      Çƒ$      Çƒ8      ‰³  èüÿÿÿ‰Â…Àˆ  ‹ƒÌ   ‰›Œ  «d  Çƒd     ˆ   ‰³x  ‰ƒl  ‰èÇƒh     Çƒœ     Çƒ      Çƒ”     Çƒ€      Çƒ„      Çƒ˜      èüÿÿÿ‰Â…Àˆ–   ¹K  º F  ‰Øè=îÿÿ‰ƒà  ‰¸¸  ‹ƒà  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡  ‹ƒà  ˆä      ‹“Œ  ‹ƒà  ³¼  ‹• J  ‹à   ÿ21Òjèüÿÿÿ‰ÂXY…Ò‰„  é\  ´&    ‰Ø‰$èüÿÿÿ‹$‹D$<d+    …†  ƒÄ@‰Ğ[^_]Ã¶“„  s„Ò…^  ¸   èüÿÿÿ¹(  i“ˆ  ˆ  P‰ğj ”ü   èüÿÿÿ_]‰Æ…ö„¹ıÿÿ‹ƒ€  ÇD$    ÇD$    ‰D$‰ğÇD$    ÇD$   èüÿÿÿf‰D$‹FdÇD$    ‹@…Àt*‹H$…Ét#¡   T$…À„   ‹x$…ÿ„•  ‰ğèüÿÿÿ‹ƒ¸  ƒø„ï   ƒø*„æ   ƒø'…&ıÿÿ‹ƒ€  ÇD$(    ÇD$,    ‰D$D$ ÇD$0    ÇD$4    ÇD$8    ÇD$ <  ÇD$$@   ‰D$‹Fd‹@…À„Îüÿÿ‹H(…É„Ãüÿÿ¡   T$…À„!  ‹x(…ÿ„  ‰ğèüÿÿÿé›üÿÿ´&    v iÀˆ  j ¹(  R„ü   ‰Â‰ğèüÿÿÿ‰ÆXZé©şÿÿ´&    fƒ»€  …Ùıÿÿéäıÿÿ¶    ‹“€  ÇD$(    ÇD$,    ‰T$T$ ÇD$0    ÇD$4    ÇD$8    ÇD$ .  ÇD$$@   ‰T$‹Vd‹R…Ò„Âşÿÿ‹J(…É„·şÿÿ¡   T$…Àt0‹x(…ÿt)‰ğèüÿÿÿ‹ƒ¸  é‘şÿÿ´&    ‰ğèüÿÿÿéfşÿÿt& ‰ğèüÿÿÿ‹ƒ¸  éhşÿÿ¶    ‰ğèüÿÿÿé…ûÿÿèüÿÿÿfffffffèüÿÿÿ‹@(‹T$‹€x  Ç   % ù  ƒøÀ%€C   ‡  ‰1ÀÃ´&    ´&    èüÿÿÿUWVS‰Ã«x  ƒì‹ ‹“ˆ  ‹p(‹ƒ  ƒÀX  ¾¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‹†X  9Átt‹ƒˆ  ‰L$   ‹†\  ‰$‰øèüÿÿÿ‹L$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿ‹$‹“  ƒ=|  ‹@‰P‡ı  ƒÄ[^_]Ã´&    f‰ø‰$èüÿÿÿ‹$‰Â‹†\  ‰®\  ‰‹x  ‰ƒ|  ‰(‰øèüÿÿÿƒ=|  ‡â  ƒÄ[^_]Ã´&    t& èüÿÿÿUWVS‹h(‹…Ü   @  ‹ƒâİ‰½¸  X  ‰øèüÿÿÿ‰Æ‹…X  9Ãt@v ‹…X  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…X  9ØuÃ[‰ò‰ø^_]éüÿÿÿ¶    èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿW1ÒVS‰Ã‹ ‹p(‰Øèüÿÿÿ‹S‰Á‹†x  % ù  ƒøÀƒàƒÀ…Òu	[¸êÿÿÿ^_Ãiø@  ‹ST9úrêG×‰SP“„  ‹Ì   PiÀ   j h   P‰Øj ‹	èüÿÿÿ1ÀƒÄ[^_Ã´&    ´&    èüÿÿÿV‹|  S‹X(‹³X  …Ò…  ‹ƒd  ÿv¹   P,‰Øèüÿÿÿ‹ƒÜ   º   ‰ˆ  ‹ƒÜ   ‰4  Çƒ`      ‰Øº   èüÿÿÿ‹ƒÜ      ‹ƒÊ"‰‹ƒÜ      ‹ƒÊ ‰‹ƒÜ   @  ‹€@  ƒÈ"‰X1À[^Ã¶    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCÀü›ÇC   ÇCGREYÇC    ÇC$    ‹€x  öÄùt'ÇC
   ÇC  ÇC   ÇC    1À[Ã´&    ©ÿÿ tîÇC   1ÀÇC?  ÇC   ÇC    [ÃèüÿÿÿƒâtFVS‰Ã¡|  …À…4  ³¸  ‰ğèüÿÿÿ‹ƒÜ   ‹ˆ$  “X  ‰Øèüÿÿÿ‰ğèüÿÿÿ¸   [^Ã1ÀÃffffffèüÿÿÿV°è   S‰Ã‰ğèüÿÿÿ‰Â‹ƒÜ   ‹˜  ‰ğèüÿÿÿ‰Ø[^ÃèüÿÿÿUWV‰ÎS‰Óƒì€|$4 ‹T$ tÇ    pƒÀÇ@ø    Ç@ü    ‰ÇƒúÿtÊ € €ƒÇ‰‹T$,…Ò„­   ‹D$0Ç$    …À•D$¶    ‹K‹l$$…öt-‰Øë´&    f)Îèüÿÿÿ…ö„  ‹H9ñvê‹l$$‰Ã)ñõ‹$…Àti€|$ tb1Ò÷t$0ƒúWÀ%     ;L$$rRD$$   ‰s‰wÇG    ‰×‹t$(ƒ$‹$î9D$,…kÿÿÿƒÄ‰ø[^_]Ã´&    f¸   W;L$$s®	È‰T$   ‰s‰Ø‰wÇG    +kèüÿÿÿ‹T$‰Ã‹@9Åv5‰Á‰Ø‰Óv É   ƒÃ‰Kô‹P‰SøÇCü    +hèüÿÿÿ‹H9érØ‰Ú‰Ã‰èz   ‰‹CÇB    ‰BéDÿÿÿ¶    ‹H‹l$$‰Ãéêşÿÿ´&    v èüÿÿÿWVS‹ˆÌ   ‰Ã‹‘\  ‹X  ‹±P  ‹¹T  ‰Ñ	Át
ƒÀƒÒ )ğúR‰ù‰òP¸    èüÿÿÿğÿ”ÀZY„Àt|‹ƒ¸  iĞ  ƒº   tiĞ  ‹’   ƒú„‚   ƒú„Ÿ   iÀ  ‹€   ƒø„¤   ƒøt7ƒ
  èüÿÿÿƒ|  èüÿÿÿƒô   èüÿÿÿ‹ƒÜ   [^_éüÿÿÿ[^_Ãt& ‰ØèüÿÿÿëÀ´&    ‰Øèüÿÿÿ‹ƒ¸  éoÿÿÿ¶    ƒÜ  èüÿÿÿ‹ƒ¸  iĞ  ‹’   ƒú…aÿÿÿ‰Øèüÿÿÿ‹ƒ¸  éOÿÿÿ´&    ƒ   èüÿÿÿiƒ¸    ‹€   é<ÿÿÿèüÿÿÿ…Àt7S‹X`ú v@túv@t[Ã9ƒ¤  uõ‹[éüÿÿÿ9ƒ¤  uå‹[éüÿÿÿÃ´&    ´&    èüÿÿÿö€4  t*S‹˜Ü   ‹‹  ‹˜Ü   ‹“”  …Ét…Òu
[Ã´&    Ã‹˜Ü   ‰‹  ‹˜Ü   ‰“”  ‹˜Ü   Ã  ‹‹˜Ü   Ã ‹‹˜Ü   Ã  ‹‹˜Ü   Ã ‹RQhè
  ‹€Ì   ˆ   PèüÿÿÿƒÄ[Ã´&    v èüÿÿÿUW¿   V‰ÆSƒì‰$ëM´&    ‹„  …Ò…Ô  ‹‹Cƒï‰B‰º   ‰èÇ   ÇC"  èüÿÿÿ·V‹$)Ğ…À~O…ÿ~K‹9ğtE‹èüÿÿÿ‰“ ıÿÿ«ˆıÿÿ‰ƒœıÿÿ‹FP‰V‰ƒôşÿÿ‹F·Ğ;$u‚ƒ=„  v‡é÷  t& ƒÄ[^_]Ã´&    èüÿÿÿWV1öS‰Ã‹€Ü   ‰°   ‹ƒÜ   ‰°   ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°@ ‹ƒÜ   ‰°   »è   ‰øèüÿÿÿ‰Â‹ƒÜ   ˆ  ‹‰1‰øèüÿÿÿ‹ƒÜ   ‰°   ‹ƒÜ   ‰°0  ‹ƒÜ   ‰°@  ‹ƒÜ   ‰°P  ‹ƒÜ   ‰°`  [^_Ã´&    ´&    èüÿÿÿWV‰ÆS‹¸È   _ü‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèøşÿÿ‹†L  ‰Úèüÿÿÿ‰ğèüÿÿÿ‰Øè½ûÿÿGXèüÿÿÿ‰øèüÿÿÿ‰Ø[^_éüÿÿÿt& èüÿÿÿW¸è   V‰ÖS‰Ã‰øèüÿÿÿ	³ä   ‰Â‹ƒÜ   ˆ  ‹€  	ğ‰[‰ø^_éüÿÿÿ´&    ´&    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â#ä   t‹†Ü   ˆ  ‹€  	Ø‰[‰ø^_éüÿÿÿ´&    ¶    èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‰Â‹†Ü   ˆ  ‹€  ÷Ó!Ã‰[‰ø^_éüÿÿÿv èüÿÿÿW¸è   V‰Æ‰øS‰Óèüÿÿÿ‹Ü   ‰Â‰Ø÷Ğ™  !†ä   ‹‰  !È‰[‰ø^_éüÿÿÿ´&    t& èüÿÿÿUW‰×VS‰Ãƒì‹r¡„  …ö„  …À…8	  ‹GƒÁ1Òƒáø‰D$‹G‰L$‰$÷ñº   9ĞFĞ‰T$ƒø†>  ‹ƒÜ   º  p‰Pº   ‹ƒÜ   ‰P1Ò‹ƒÜ   ‰P1íÇD$    ‹t$‹L$9$ro‰|$1Éfƒ=„  ‡]	  ‰ğƒàüƒÜ   ‰$‹D$‹x‹$ï‰8FƒàüƒÜ   ‰FƒàüƒÜ   ‰FƒàüƒÜ   ‰ƒÁƒÆl$9L$w‹|$‹G‹W(ƒàüƒÜ   …Ò…  ‹T$(‰‹G1ÒƒÀƒàüƒÜ   ‰‹G‹L$ƒÀƒàüƒÜ   ‰‹G‹t$ƒÀÁæƒàüÁîƒÜ   ‰0‹G‹WƒÀƒàüƒÜ   ‰‹G(…À‹G„÷   ƒÀº  €ƒàüƒÜ   ‰º   1Ét& ‹GĞƒàüƒÜ   ‰ƒÂƒúPuè‹G‹WƒàüƒÜ   ‰‹G‹L$ƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰0‹T$‹G ÁêƒàüƒÜ   ƒê‰ƒ=„  ‡	  ƒÄ1À[^_]Ãº   ‰éøşÿÿt& …À…	  ‹G1ÒƒàüƒÜ   ‰‹GƒàüƒÜ   ‰‹G$ƒàüƒÜ   ‰‹G ƒàüƒÜ   ‰ƒÄ1À[^_]Ãt& ƒÀº   ƒàüƒÜ   ‰éÿÿÿ¶    èüÿÿÿUWV‰ÆS‰Óâ  ‹8…€  öÃt`ƒ=„  ‡k  ®´   ‰èèüÿÿÿ‹†¸   ƒàü‡Ü   ‹†˜   èÚùÿÿ‰èèüÿÿÿ‹†è   ƒàü‡Ü   ‰¸   [^_]Ã´&    …ÛuÙ[1À^_]Ãt& èüÿÿÿUhW‰×V‰Æ‰èSèüÿÿÿ‹FV9Ât/X´ë´&    v ‹CLX´9Ât9»   uî‰èèüÿÿÿ‰Ø[^_]Ã‰è1Ûèüÿÿÿ‰Ø[^_]ÃèüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j èŸôÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èqôÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿUˆ   WV‰ÎS‰Ó‹|$‹T$‹l$j KhÀ  ¯×ÁêTRÁâ‰èüÿÿÿZY‰C…Àt:1Ò…í”Â1ÉR‰òUWj ÿt$$jÿèİóÿÿ1Ò‰C+CƒÄƒÀ;w[‰Ğ^_]Ãt& ºôÿÿÿëì´&    èüÿÿÿUWV‰Î1ÉS‹l$‰Ó‹|$$ƒıÿ•Á1Òƒ|$ÿ•Âˆ   Ñ‹T$T$ j ¯×hÀ  ÁêT¯ÑKTRÁâ‰èüÿÿÿZY‰C…Àtnƒıÿtj‰é‰òj Wÿt$,ÿt$,j è/óÿÿƒÄƒ|$ÿt'1Òƒıÿ”ÂR‰òj Wÿt$,ÿt$,h   ‹L$0èóÿÿƒÄ‰C+C1ÒƒÀ;w[‰Ğ^_]Ã´&    ºôÿÿÿëè´&    èüÿÿÿS‹š„  ‹Šˆ  ‹€Ì   j ÿ²”  ÿ²  ˆ   ‰ÚèüÿÿÿƒÄ[Ã´&    fèüÿÿÿU‰ÕW‰ÏV‰ÆSƒì‹„  ‹…É…q  ‰Øè×õÿÿ‹†À   ƒàüƒÜ   ‹‹–ô   ÷Ò!Ê‰kV,‹¤   ‰Ø“d  ÿ·  èüÿÿÿƒ=„  Z‡²  ‹†Ä   ‹–¤   ƒàüƒÜ   ‰iƒ¸        ‹PöÂu‹@¨„Ÿ  ƒú„  ¸¼ èüÿÿÿ‹†ì   …Àtƒàü‹–  ƒÜ   ‰‹†È   ‹–  ƒàüƒÜ   ‰‹†à   ‹–   ƒàüƒÜ   ‰‹†Ü   ‹–  ƒàüƒÜ   ‰‹†Ì   ‹–ü   ƒàüƒÜ   ‰¸¼ èüÿÿÿ‹†¼   º   ƒàüƒÜ   ‰ÇE    ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàü‹»Ü   ‰‡L  ‹ƒ¸  iĞ  ƒº   u"‹ƒÜ   L  ‹ ƒàû‹»Ü   ‰‡L  ‹ƒ¸  iÀ  ƒ¸   uE‹ƒÜ   L  ‹ ƒàş‹»Ü   ƒÈ
‰‡L  ‹ƒÜ   H  ‹Ê  €‰ºE ‹ƒÜ   ‰,  ‹ƒh  =u  t-w  ƒø‡,  ¡„  …À…–  ‰Øèºóÿÿ‹†ä   ƒàüƒÜ   ‹–ø   ‰‹†À   ƒàüƒÜ   ‹–ô   ‰»è   ‰Øè{óÿÿ‰ø‹®ğ   èüÿÿÿ	«ä   ‰Â‰øèüÿÿÿ‰øèüÿÿÿ‰Â‹ƒä   …Àut‰øèüÿÿÿ‰Øè>óÿÿ‹ƒÜ      ‹ƒÊ ‰‰Øè%óÿÿiƒ¸    ƒ¸   tbƒ=„  wi‰øèüÿÿÿ‰Â‹ƒÜ     ‹ ‰øèüÿÿÿ‰Øèåòÿÿ1ÀƒÄ[^_]Ãv ‹‹Ü   Á  ‹)	è‰éuÿÿÿ1Ò‰Øèüÿÿÿéeıÿÿfº   ‰ØèüÿÿÿëéÊ  ´&    t& èüÿÿÿUWV‰ÖS‰Ãƒì‹º¤   ‹*¯º¨   1Òèüÿÿÿ‹„  ‰Â…É…!  ‹C…ÀuC¸êÿÿÿ…ÿu(‹…Ì   j Ã„  ÿ¶¨   ÿ¶¤   ‹
‰Úèüÿÿÿ1ÀƒÄƒÄ[^_]Ã´&    v ;{Tw‰{Pë¼¸êÿÿÿëİ´&    ´&    èüÿÿÿUWV‰ÆS‰Óƒì‹8‹ƒ  ‹’ˆ  ƒÀÇ¸  ‰B‹ƒŒ  Ç   p‹ƒ  ‹“Œ  ƒÀ‰B‹ƒŒ  Ç@    ‰øèüÿÿÿ‹®˜   ˜   ‰Âƒx  9étT‹«ˆ  M    ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹E‹‹  ‰H¡„  …À…l  ƒÄ‰ø[^_]éüÿÿÿt& ‹®œ   ‰†œ   ‰‹x  ‰«|  ‰E ‹„  …É…E  ƒÄ‰ø[^_]éüÿÿÿèüÿÿÿUW‰ÇVSƒì‹„  …Ò…“  ‹0‹‡ä   ƒàü†Ü   ‹‹—ø   ÷Ò!Ê‰‹‡À   ƒàü†Ü   ‹‹—ô   ÷Ò!Ê‰»   v ¸X‰A èüÿÿÿƒëuñ»d   ë´&    v …Àt,¸X‰A èüÿÿÿƒët‹†Ü   ‹  ‹†Ü   ”  ‹ …ÒuĞi†¸    ƒ¸   t}‡´   Ÿ˜   ‰$èüÿÿÿ‰D$ë?v ‹·˜   ‹‹F®ˆıÿÿ‰B‰‰èº   Ç   ÇF"  èüÿÿÿ¡„  …À…Õ  ‹‡˜   9Øuº‹T$‹$ƒÄ[^_]éüÿÿÿ´&    ‹†Ü   L  ‹ ƒàõ‹Ü   ƒÈ‰L  ‹‡ì   1Òƒàü†Ü   ‰‹‡Ì   º   ƒàü†Ü   ‰i†¸    ƒ¸   …$ÿÿÿ1Ò‰ğèüÿÿÿéÿÿÿv èüÿÿÿ…Ò„ƒ   UWV‰ÖS‰Ã‹€”  ƒàüƒÜ   ‹8ƒ=„  wd÷Æ 3 …û  ÷Æ   t.ƒ=„  ‡G  «  ‰èèüÿÿÿƒt  ‰úè†ïÿÿ‰èèüÿÿÿ‹ƒÄ  ƒàüƒÜ   ‰0¸   [^_]Ãt& 1ÀÃ‹ƒÀ  ƒàüƒÜ   ‹ WPRhÔ  hD  èüÿÿÿƒÄéqÿÿÿfèüÿÿÿUWVS‰Óƒì<d¡    ‰D$8‹‚Ü   ‹¨  ºè   ‰øèüÿÿÿ‰Â‹ƒÜ   ‹°  ‰øèüÿÿÿ‰è!ğ‰$„%  ‹ƒÜ   $  ‹ ‰D$‹ƒÜ      ‹ ‰D$‹ƒÜ   T  ‹ ‰D$‹ƒÜ   P  ‹ ‰D$‹ƒÜ   ‹ˆ4  ‹ƒÜ   0  ‹ ‰D$‹ƒÜ   D  ‹ ‰D$‹ƒÜ   @  ‹ ‰D$‹ƒÜ   ‹   ‹ƒÜ      ‹ ‰D$ ‹ƒ”  ƒàüƒÜ   ‹ ‰D$$‹ƒØ  ƒàüƒÜ   ‹ ƒ=„  ‰D$(‡x  ‰è%   ‰D$÷Å€„ò   ÷Å   tƒ=„  ‡K  ÷Å   tƒ=„  ‡o  ÷Å   tƒ=„  ‡  ÷Å   tƒ=„  ‡'  ÷Å   tƒ=„  ‡ß  ÷Å   …Ú  ÷Å   tƒ=„  ‡>  ÷Å   „Ò  ƒ=„  ‡  ÷Å   „  ƒ=„  ‡ü  ÷Å  € u÷Å   tƒ=„  ‡Ø  ÷Å   „ú  ƒ=„  ‡´  iƒ¸    1ö‹€  ƒø„2  ƒø„a  …Ét"iƒ¸    ‹€   ƒø„Ì  ƒø„S  ‹D$…Àt"iƒ¸    ‹€   ƒø„º  ƒø„9  ‹L$…É…-  ‹T$…Ò…‘  ‹D$…Àt[‹ƒ¨  ÆD$7 …ÀtL‹Pd‹…ÒtC‹J$‰L$…Ét8‹    …Ò„x  ‹J$‰L$…É„i  T$7‰Ñ‰ê‹l$èüÿÿÿ€|$7 tƒÆ÷$   …Ñ   …öu-Ç$    ‹D$8d+    …9  ‹$ƒÄ<[^_]Ã´&    v ‹ƒÜ   ‹<$‰¸  Ç$   ëÂ´&    ÷Å  €„Ëşÿÿ‰ê‰Ø‰L$èüÿÿÿ‹L$‰Æiƒ¸    ‹€  éšşÿÿt& ÷Å  € „“şÿÿ‰Ø‰L$èüÿÿÿ‹L$Æé}şÿÿ¶    ‹T$‰Øèüÿÿÿ‹T$Æ…Ò„ÆşÿÿëUt& ‰øèüÿÿÿ‰Â‹ƒÜ     ‹áÿÿÿ÷‰‰øƒÆèüÿÿÿ‹˜  ¸    ‹    èüÿÿÿ…ö„íşÿÿéÿÿÿt& ‹L$‹T$‰ØèüÿÿÿÆ‹D$…À„µşÿÿéUşÿÿ¶    ƒ=„  †µşÿÿé_  ¶    ƒ=„  ‡“  ÷Å   ….ıÿÿ÷Å   „/ıÿÿƒ=„  †"ıÿÿéš  ´&    f‰Ê‰ØèüÿÿÿÆéıÿÿ‹T$‰ØèüÿÿÿÆéµıÿÿ¶    ÷Å  € „ıÿÿƒ=„  †	ıÿÿé»  ´&    v ‹D$…À„ıÿÿƒ=„  †úüÿÿév  fƒÜ  ‰Êè³ïÿÿÆé)ıÿÿt& ‹T$ƒ   è™ïÿÿÆé9ıÿÿf‰êL$7‹l$èüÿÿÿé”ıÿÿèüÿÿÿ´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
	È‰÷Ãøÿ t>i†¸    ƒ¸   „_  ƒ¸   „_  ‹†Ü      ‹
‰ØÁè·À	È‰ã  ø …w  [^Ã´&    èüÿÿÿV‰Æ‰ĞS‰Óƒàt‹Ü   ‘  ‹
÷Ğ!È‰÷Ãøÿ t@i†¸    ƒ¸   „  ƒ¸   „  ‹†Ü      ‹
‰ØÁè·À÷Ğ!È‰ã  ø …§  [^Ãv èüÿÿÿV‰ÆS‰Óƒâuh÷Ãøÿ u‰Ø%  ø …¿  [^Ã´&    i€¸    ƒ¸   „Ù  ƒ¸   „Ù  ‹†Ü   ‹   ‰Ø[^Áè!ĞÁà%øÿ Ã¶    ‹€Ü   ‹€  Áè!Ø[^ƒàÃèüÿÿÿW‰ÏV‰ÆS‰Óƒât ‹€Ü     ‰ØÁà%   …Éts‹
	È‰÷Ãøÿ tFi†¸    ƒ¸   „ñ  ƒ¸   „ñ  ‹–Ü   Áë·Û‚$  …ÿt‹’$  ÷Ó!Ó‰[^_Ãv ‹’$  	Ó‰[^_Ã´&    v ‹
÷Ğ!È‰ë‰¶    èüÿÿÿU¹8  ºÀ  W‰ÇVSƒì¡4   èüÿÿÿ…À„š  ‰Ã¡`N  …À„Ç   º   ƒøtM1É¸"  ºQ  1öèüÿÿÿ…Àu%ƒÆƒş„š   ‹õ	  ‹õ 	  1Éèüÿÿÿ…ÀtÛèüÿÿÿº   ´&    ¶ƒ4  kƒàş	Ğ‰êˆƒ4  ‡ˆ   ‰D$èüÿÿÿ‰Æ…Àx/j C\1Éº   ‰D$èüÿÿÿ‹³˜   X…öt,‹$èüÿÿÿ‰èèüÿÿÿ‰ØèüÿÿÿƒÄ‰ğ[^_]Ãt& 1ÒëŒt& ‹$‰»Ì   ¾ûÿÿÿÇƒ˜  àÿÿÿ‰C@ƒœ  ‰ƒœ  ‰ƒ   ƒ°  ‰ƒ°  ‰ƒ´  ƒÄ  ‰ƒÄ  ‰ƒÈ  ‰øÇƒ¤      Çƒ¬  àÿÿÿÇƒ¸      ÇƒÀ  àÿÿÿÇƒÌ      ÇC< E  èüÿÿÿ‰D$…À…7ÿÿÿƒ  ¹€  º°  Çƒè       Çƒ¸      èüÿÿÿƒ¤  ¹€  º»  èüÿÿÿğÿ¡€  ‹¼  ‰L$‰ƒŒ  PPhË  Q‰€  èüÿÿÿ‹ƒÌ   ƒÄ·@"f=€ˆ„§  f=Rˆt¸u  ¹ ?«º L  ‰‹ğ   ‹„  ‰ƒh  ‰“d  …É…'  ‹ƒŒ  ‹…@N  ;    ‚(  Çƒ¸  ÿÿÿÿ‹    ‹t$ëf‹“¸  ƒÆƒúÿuR9Ît1‹ƒÌ   ·õ    f9P$uÚ·õ   f9P&uÌ‹õ   ‰“¸  ëÃÇƒ¸      ‰Øèüÿÿÿ‹ƒÌ   ‹“¸  fx"Rˆ„Ù  iÊ  ‹‰(   …Ét‰‹ğ   ƒú+„Ÿ  ‹P³è   ¶Rt‰“Ô   ‹@Áèƒà‰ƒØ   ‰ğèüÿÿÿ‹ä      ‰Â‰ğèüÿÿÿ‹ƒ¸  Çƒø       ‰›ô   iĞ  ‰›|  Çƒl    Çƒh    Çƒp     Çƒt    Çƒx    Çƒd     Çƒ€     Çƒô	    Çƒğ	    Çƒø	     Çƒü	    Çƒ 
    Çƒì	     Çƒ
     ‰›
  Çƒ|    Çƒx    Çƒ€     Çƒ„    Çƒˆ    Çƒt     ‹’   ƒâıƒú„ü  iÀ  ‹€   ƒàıƒø„™  ‹‹Ì   ‹‘\  ‹X  ‰T$‹‘T  ‰D$‹P  ‰T$‹T$‰D$‹D$‰Ñ	ÁtƒÀƒÒ +D$T$‰D$‰T$j ¸    ÿt$ÿt$ÿt$‹T$$‹L$(èüÿÿÿƒÄ…À„  ‹³Ì   ‹T  ‹†P  ‹–\  ‰L$‹X  ƒÁ)Á–X  ¾    ‹T$DÎèüÿÿÿ‹“Œ  ¹•  ‰ƒÜ   ‰ƒà   ‹ƒ¸  9•@N  º£  EÊ‹“Ì   QPiÀ  ÿ°    ·B&P·B$Pÿt$$hP  èüÿÿÿ¡„  ƒÄ…À„[  éD  ´&    ‰“¸  ƒúÿ„Óüÿÿ‹ƒÌ   é6ıÿÿfx&7q…UıÿÿÇƒğ   @x}éFıÿÿƒú9„É  ƒú8…ıÿÿÇƒ¸  <   ‹Ø@  …Ò„ıÿÿ‰“ğ   éıÿÿ¡„  …À…	  ƒ¸  ‰›   ¹€  ºõ  ‰ƒ¸  ‰ƒ¼  ƒ4  Çƒ     Çƒ    Çƒ(      Çƒ,  ÀG ÇƒÔ      Çƒ$     èüÿÿÿƒ,  Çƒ°      ‰ƒ,  ‰ƒ0  ‹ƒ0  …Àu
Çƒ0     ÇƒØ    ÇƒÜ  0 Çƒà  @ Çƒä  P Çƒè  T Çƒì  X Çƒğ  \ Çƒô  ` Çƒø  d Çƒü  h Çƒ   l Çƒ      Çƒ  @  Çƒ  D  Çƒ(     Çƒ     éıÿÿ‹„  …Ò…Q  ƒt  ‰›Ü  ¹€  ºõ  ‰ƒt  ‰ƒx  ƒğ  ÇƒĞ     ÇƒÔ    Çƒä      Çƒè  ÀG Çƒ      Çƒà     èüÿÿÿƒè  Çƒl      ‰ƒè  ‰ƒì  ‹ƒì  …Àu
Çƒì     Çƒ”    ‹ƒ¸  Çƒ˜  4 Çƒœ  @ Çƒ   P Çƒ¤  T Çƒ¨  X Çƒ¬  \ Çƒ°  ` Çƒ´  d Çƒ¸  h Çƒ¼  l ÇƒÈ  D ÇƒÀ  0  ÇƒÄ  4  Çƒä     ÇƒÌ     é²ûÿÿÇƒ¸  =   ‹ìA  …Ò„\úÿÿé;ıÿÿ¾ôÿÿÿé$øÿÿ¸x  ¹€ğúº K  é]ùÿÿfffffffèüÿÿÿ¸	 ÿÃt& èüÿÿÿV‰ÆS»@   ë¸à èüÿÿÿƒët)‹F‹‹€x  ƒàü‚Ü   ‹ ¨u×¸   [^Ã´&    1À[^Ãv èüÿÿÿU‰ÕWVSƒì‹x‰D$¡Œ  ‹…Àt	öÁ„"  ·Ef…À„Â   ‹Œ  …Û…ê!  ‰ë1ö‰Õ‰Úë1f‹‡€  ƒàü…Ü   ‹‹Bˆ0¡Œ  …À…o"  ·BƒÆ9ğvm‹Ÿp  ƒè‰$‰ÙË  É  9ğ‹‡|  FÙ·
ƒàü…Ü   Áá‰‹‡t  ƒàü…Ü   ‰‹D$èáşÿÿ‹$…À…vÿÿÿ¡Œ  …À…   ¸ûÿÿÿƒÄ[^_]Ãt& ‹‡|  ·M ƒàüÁá‚Ü   ‰‹‡t  ƒàü‚Ü   ‹—p  ƒÊ‰‹D$è}şÿÿ…Àt°‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t‹5Œ  …ö…Ì!  ƒÄ1À[^_]Ã¸úÿÿÿéuÿÿÿé¿"  t& èüÿÿÿU‰ÕW‰ÏVSƒì‹p‰D$¡Œ  ‰L$‹·M…ÿ„b  …À…#  ‹|  ·E ‹ºÜ   ƒãüÁàßf…É„‰  ‹]¶	Ø‰D$‹†p  fƒù„-    ‰$‹D$‰‹†„  ƒàü‚Ü   ‰‹†t  ‹$‰T$ƒàü‚Ü   ‰‹D$è}ıÿÿ…À„  ‹Œ  …Ò‹T$…a#  ·Eƒøù   ‰é¿   ‰Õ‰Êëv´&    fË  ‹†|  ‰$‹T$ƒàü…Ü   ‰‹†„  ƒàü…Ü   ‰‹†t  ƒàü…Ü   ‰‹D$è÷üÿÿ…À„   ‹Œ  ‹$…É…¤#  ·BƒÇ9ø~v‹Jƒè‹p  ¶99ø{ÿÿÿ‰ØË    ƒ|$ DØégÿÿÿ…À„şÿÿé9#  v ‰Á  É  ƒ|$ EÈ‰$é¾şÿÿv ‹Œ  …É…õ"  ¸ûÿÿÿƒÄ[^_]Ãt& ‰‹†t  ƒàü‚Ü   ‹–p  ƒÊ‰‹D$è+üÿÿ…ÀtÅ‹D$‹@‹‹€x  ƒàü‚Ü   ‹ ¨t¡Œ  …À…×"  ƒÄ1À[^_]Ã¸úÿÿÿë´&    t& èüÿÿÿU‰ÕW‰ÇV‰ÎSƒì‹Œ  …Ò…Ú#  1Û…ö<é•   t& C‰$9ğ}D‰Áö@t· f9tJ1É‰øèYıÿÿ…Àx1ƒÃ9Ş~^[¡Œ  ÁáT …À…÷#  öBt±1É‰øè¨ûÿÿ…ÀyÏƒÄ[^_]Ãt& ‰Ë·I‰øèıÿÿ…Àxã‰Ú¹   ‰øèyûÿÿ‹$ë›t& ƒÄ‰ğ[^_]Ã¶    èüÿÿÿUWVS‰ÃƒìH‹3d¡    ‰D$D1À¡Œ  …À…ï$  ‰òk1À¹œ   ‰ïó«»x  ¹}   ÇC    ÇC   Çƒ  cx23Çƒ  885 ‰|$ó«º¼  º    Çƒ|  cx23Çƒ€  885 Çƒ„  inteÇƒˆ  rnal‹†Ì   ˆ   ‰Chƒ  ‰$‰øèüÿÿÿ‰Áƒø†¨   ƒø0…å$  ‹$‰úèüÿÿÿF‰[‰ƒ„   ‰èèüÿÿÿ‹ƒl  ‰«  …À…Ì$  ‹=Œ  …ÿ…$  ¡ˆ  …À…-$  t$1À¹   º!	  ‰÷ó«¹	   ‰ğèüÿÿÿ‰ò¹Ì	  ‰èh    èüÿÿÿ‹ƒl  Z‹T$Dd+    u&ƒÄH[^_]Ãt& ƒø0„XÿÿÿƒÁƒù0†Lÿÿÿé%  èüÿÿÿèüÿÿÿƒÀèüÿÿÿ1ÀÃèüÿÿÿS‰Ãƒì‹Œ  d¡    ‰D$1À…É…%  ƒú¸D  º   ¹   f‰D$”À€f‰T$T$ˆD$D$‰D$ƒ
  ÇD$D  è&ıÿÿ‹D$d+    uƒÄ[Ãèüÿÿÿfffffffèüÿÿÿ‹@(Ç€¤   ğ  Ç€¨       Ç   ‹ˆ¤   ¯ˆ¨   ‰È‹L$‰1ÀÇ    Ãèüÿÿÿ‹€$  ‹@ ‹ ‹€Ü      ƒút …Òtº   ‰1ÀÃfº   ‰1ÀÃ¶    º   ‰1ÀÃ¶    èüÿÿÿS1É‹˜4  …Ò”Áº   Q1É‹[èüÿÿÿZ[Ã´&    v èüÿÿÿ1ÀÃ´&    èüÿÿÿS‹ˆà  …Étèüÿÿÿ‰Ã…Àˆ?%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸<  ‰X  ‹Ğ  Ç€<      …Òt‰Ãèüÿÿÿ‰»<  ‰Æ…ÀˆU%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‹ˆÜ  …Étèüÿÿÿ‰Ã…Àˆk%  1Û‰Ø[Ã´&    t& èüÿÿÿWVS‹¸X  ‰<  ‹Ğ  Ç€X      …Òt‰Ãèüÿÿÿ‰»X  ‰Æ…Àˆ%  1ö‰ğ[^_Ã´&    fèüÿÿÿS‰Ó‹”   Hƒú~2‰Èèüÿÿÿ…Àt‹@…Àt‹ˆ  …Ét‰Ú[éüÿÿÿ¶    [Ã¶    º   ‰ÈèüÿÿÿëÇ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹@(éüÿÿÿv èüÿÿÿ‹@(‹˜   Šˆıÿÿ˜   èüÿÿÿ1ÀÃ´&    ´&    èüÿÿÿ‰Â‹ ‹@(‹ éüÿÿÿ´&    ¶    èüÿÿÿ‹‹R(éüÿÿÿèüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº 0  ‹h ‹} ‰øèüÿÿÿƒût!ƒût|…ÛtH‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& ‰øº    èüÿÿÿ‰øº   èüÿÿÿ‹@  ‰Ú‰ğ[^_]éüÿÿÿt& èüÿÿÿU¹   WV‰ÆS‹€$  ‰Óº   ‹h ‹} ‰øèüÿÿÿƒût!ƒût\…Ût8‹@  ‰Ú‰ğèüÿÿÿ1À[^_]Ãf‰øº   èüÿÿÿº   ‰øèüÿÿÿëÎ¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë®¶    ‰øº   èüÿÿÿº   ‰øèüÿÿÿë¶    èüÿÿÿVS‰Ã‹€$  ‹p ‹‹¸  ƒøw9ƒøw9ƒøuƒ»@  º    ‰Èt?èüÿÿÿ‹–<  …Òt(‰Ø[^éüÿÿÿ´&    ƒø!uáº   ‰ÈëÓ´&    v 1À[^Ãv èüÿÿÿë¿èüÿÿÿUW‰ÏVSƒì‹0‰$‹\$®¤  ‰èèüÿÿÿ‹†Ü   º   ‰$  ‹†Ü      ‰ø¶À€ÌÇ‰‹†Ü      ‹€æû‰‰ù‹†Ü   ¶ıÏ Ë  ‰¸   ‹†Ü      ‹€æ÷‰€<$ ‹†Ü   „Ò   ºÿ  ‰$  ‹†Ü      ‹€æş‰‹†Ü      ‹
€<$À% Àÿÿ-@  !È‰‹=    ¡    ƒÇ9Çy&é   t& ¸2   ¹   ºd   èüÿÿÿ¡    9Çx‹†Ü   ‹˜   öÇuÒ¶Û‹†Ü      ‹€   €ÌÃ‰‹D$‰‰èèüÿÿÿ1À€<$ u‹D$‹ Áøƒà‡ƒÄ[^_]Ã´&    ¶Û€ÏÃ‰˜   é(ÿÿÿ1Ûë¤v èüÿÿÿUWVS‰Ãƒì‹5    ‹€Ü   ‰$‹|$ƒÆ   ‹ ‹‹Ü   …ÿ„—   Á$  ‹)Íÿ   ‰)‰Á€äı€Í…ÒEÁ‹‹Ü   €äò‰Â€Ì€Î…ÿEÂ‰   ët& ‹    9Öx¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €Î‰¶À…ÿEøƒÄ[^‰ø_]Ã´&    f©$  ‰é‹m å ÿÿÿ‰)Š$é^ÿÿÿt& èüÿÿÿVpº   S‰Ã‰ğèüÿÿÿ…Àt‹@…Àt‰ğèüÿÿÿ‹³8  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³4  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³0  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹³,  …öt‹FT‹@èüÿÿÿ‰ğèüÿÿÿ‹‹¸  ƒøt!ƒøu
‹S‰ÈèüÿÿÿÇƒ      1À[^Ãv ‰Øèüÿÿÿ1ÀÇƒ      [^ÃfèüÿÿÿS‹tY  ‹@(‰˜€  ‹xY  ‰˜„  Ç   ¡tY  ¯xY  ‹L$‰¡|Y  [‰1ÀÃ´&    t& èüÿÿÿ‹…ÀuÇA,MPEG1ÀÃv ¸êÿÿÿÃfèüÿÿÿ‰Â‹ ‹@(Ü  éüÿÿÿ´&    v èüÿÿÿ‰Â‹ ‹@(éüÿÿÿ´&    ´&    èüÿÿÿ‹‹R(ÂÜ  éüÿÿÿ´&    t& èüÿÿÿUWVSƒì(d‹    ‰T$$1Òèüÿÿÿ‹°(  †¼  nPhI  j"|$Wèüÿÿÿ‹FƒÄ9Åt'X´t& ‹Cd‹ …Àt‹…Òt‰Øèüÿÿÿ‹CLX´9Åuà†Ô  ‰úèüÿÿÿ‹D$$d+    u
ƒÄ(1À[^_]Ãèüÿÿÿ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  tLÇF   ‹‡Ü  ƒÇ‰F‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  t<‹GƒÇX´9Çt)t& ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿS‰Ëèüÿÿÿ‹€(  ‹|  ‹€x  ‰S‰1À[Ã´&    èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…©k  1À[ÃèüÿÿÿS‰Ëèüÿÿÿ‹€(  ÇCMPEGÇC    ‹€  ¯„  ÇC    ‰S‹ˆ  ‰S‹Œ  ÇC   ‰S‹¤  …Ò…Ìk  1À[Ã´&    ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿWV‰ÎSèüÿÿÿ‹¸(  ƒ¿€  th‹…Òu[F¹   ºN  ƒÇèüÿÿÿ‹X´9Çt*¶    ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÇuÜ¡¤  …À…ïk  1À[^_Ã¸êÿÿÿëõ¸êÿÿÿëîv èüÿÿÿéüÿÿÿ¶    èüÿÿÿéüÿÿÿ¶    èüÿÿÿVS‰Ëèüÿÿÿ‹°(  ¡¤  …À…l  ‰Ú‰ğ[^éüÿÿÿt& èüÿÿÿUWVS‹l$1Û‹|$èüÿÿÿº   ‹°(  1Àë´&    f›‹…ä  ‹…è  !ê!ø	ĞuƒÃƒûuß¸êÿÿÿ[^_]Ãt& ‰ê‰ù‰ğèüÿÿÿ…Àuç›Áá‘à  ‹‰à  ‰  ‹J‰   ‹J‰$  ‹J‹R‰(  ‰–,  [^_]ÃèüÿÿÿUWVS‰Ëèüÿÿÿº   ‹°(  ¾¼  ‰øèüÿÿÿƒø‡1l  ‰ÁƒøtƒÁƒù‡'l  ‰ú‰Økèüÿÿÿ‹†Ü  º    i€¸    ‹¸    ‰øèüÿÿÿ‰Áƒøÿ„1l  ƒø tƒÁƒù ‡'l  ‰ú‰èèüÿÿÿ‹–Ì   ‹‚´   …Àu‹‚ˆ   PC0hr  Pèüÿÿÿ¸ …º …ÇCT …ƒÄƒ¾€  DÂ‰CT[1À^_]Ã´&    t& èüÿÿÿS‰Ëèüÿÿÿ‹(  ÇCMPEGÇC    ‹‚€  ¯‚„  ÇC    ÇC   ‰C¡¤  …À…;l  1À[Ãèüÿÿÿƒ=¤  S‰Ã‡Zl  ‹ƒÜ   º7   ‰(  º   ‹ƒÜ   ‰$  º à  ‹ƒÜ   ‰   [Ã¶    èüÿÿÿWV¾   S‰Ã‰Ğ‰Ê‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ X  ‰¾   ‹»Ü   €Íø‰   ¶Î‹³Ü   ‰ÏÏ Y  ‰¾   ‹»Ü   €Íù‰   ‰Ñ‹³Ü   Áé¶É‰ÏÏ Z  ‰¾   ‹»Ü   €Íú‰   Áê‹»Ü   ‰ÖÎ [  ‰·   ‹»Ü   €Îû‰—   ¶Ğ‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹“Ü   ·À‰‚   º^  ‹ƒÜ   ‰   ºş  ‹ƒÜ   ‰   ‹5    ƒÆë¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  ¶Ò‹»Ü   ‰Ñ€Í\·É‰   fÊ ü‹»Ü   ·Ò‰—   fÁè‹»Ü   ‰Â€Î]·Ò‰—   f ı‹»Ü   ·À‰‡   º ^  ‹ƒÜ   ‰   º ş  ‹ƒÜ   ‰   ‹5    ƒÆë ´&    f¡    9Æˆ[  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º X  ‹ƒÜ   ‰   º ˜  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ Y  ‹ƒÜ   ‰°   ¾ ™  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Z  ‹«Ü   ‰µ   ¾ š  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‹«Ü   ‰   ‹«Ü   µ   ½ [  ‰.‹«Ü   µ   ½ ›  ‰.‹«Ü   ‹µ   Áæ¶Ò	ò‹t$Áæ·ö	Ö‹“Ü   ‰Š   Áà%  ÿ 	ğ‰Â‹$‰ƒÄ‰ø[^_]Ã´&    ¿ÿÿÿÿé¹şÿÿ´&    ´&    èüÿÿÿWV¾   S‰Ã‰È‹»Ü   ‰·$  ¶É‹³Ü   ‰ÏÏ P  ‰¾   ‹»Ü   €Íğ‰   ¶Ì‹³Ü   ‰ÏÏ Q  ‰¾   ‹»Ü   €Íñ‰   ‰Á‹³Ü   Áé¶É‰ÏÏ R  ‰¾   ‹»Ü   €Íò‰   Áè‹»Ü   ‰ÆÎ S  ‰·   ‹»Ü   €Ìó‰‡   ‰Ğ‹»Ü   Áèƒà?‰ÆÎ@T  ‰·   ‹»Ü   @ô  ‰‡   ¶Æ‹»Ü   ‰ÆÎ U  ‰·   ‹»Ü   €Ìõ‰‡   ¶Ò‹ƒÜ   ‰Ñ€ÍV‰ˆ   ‹ƒÜ   €Îö‰   ‹5    ƒÆët& ¡    9Æx'¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄtÛ[1À^_Ãt& [¸ÿÿÿÿ^_Ã´&    èüÿÿÿUWVS‰Ã‰Ğƒì‹»Ü   ‰$¹   ‰$  Áê‹»Ü   ƒâ?‰ÖÎ T  ‰·   ‹»Ü   €Îô‰—   ¶Ô‹»Ü   ‰ÖÎ U  ‰·   ‹»Ü   €Îõ‰—   ¶À‹»Ü   ‰Á€ÍV‰   ‹»Ü   €Ìö‰‡   ‹5    ƒÆë´&    ¡    9Æˆc  ¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄt×1ÿ‹ƒÜ   ºÿ  ‰$  º S  ‹ƒÜ   ‰   º “  ‹ƒÜ   ‰   ‹ƒÜ   ‹   ¹ ğ  ‹ƒÜ   ‰ˆ   ¾ R  ‹ƒÜ   ‰°   ¾ ’  ‹ƒÜ   ‰°   ‹ƒÜ      ‹ ‰D$‹«Ü   ‰   ¾ Q  ‹«Ü   ‰µ   ¾ ‘  ‹«Ü   ‰µ   ‹«Ü   ‹…   ‰D$‹«Ü   ‰   ‹«Ü   µ   ½ P  ‰.‹«Ü   µ   ½   ‰.‹«Ü   ‹µ   ‰ğÁâ¶ğ	ò‹t$Áææ  ÿ 	Ö‹“Ü   ‰Š   ‹D$Áà·Ğ‹$	ò‰ƒÄ‰ø[^_]Ã¶    ¿ÿÿÿÿé±şÿÿ´&    ´&    èüÿÿÿU‰ÍWV‰ÖS‰Ãƒìd¡    ‰D$1Àƒ=¤  ÇD$    ‹|$(‡!m  ‹ƒ¼  ‰áPü‰Øèüÿÿÿ‹$=xV4…ıl  ‹“¼  L$‰Øèüÿÿÿ‹D$…À…Ùl  ‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹ƒ¼  ‰ñ1öP‰Øèüÿÿÿ‹ƒ¼  ¹ô  P‰Øèüÿÿÿ…í~5t& ‹ƒ¼  ‹·T‰Øèüÿÿÿƒ=¤  ‡¾l  ƒÆ9õuØƒşv ‹ƒ¼  1ÉT‰ØƒÆèüÿÿÿƒşuå‹“¼  ¹   ‰ØÇD$   èüÿÿÿ‹5    ƒÆ
ë¡    9Æˆ«l  ¸Æ§  èüÿÿÿ‹“¼  L$‰ØèüÿÿÿöD$tÑ‹T$$‹l$$1ö…Ò~-v ‹ƒ¼  ‰ùT‰Øèüÿÿÿƒ=¤  ‡‘l  ƒÆƒÇ9õuÖ‹ƒ¼  L$P‰Øèüÿÿÿƒ=¤  ‡vl  ‹“¼  1É‰Øèüÿÿÿ‹D$‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    èüÿÿÿUWVSƒìHd¡    ‰D$D1À‹t$\‹|$dƒ=¤  ‡gm  \$l…ÿt{‰4$D$pl$‰ê¸v ‹pü‰ÃƒÀƒÂ‰rü9Èuî‹4$U‰ù‰ğÿt$l‹T$hè¡ıÿÿZY‹t$h…öt‹‹L$ƒ|$h‰
u	‹S‹L$‰
‹T$Dd+    uƒÄH[^_]Ã´&    v l$ë§èüÿÿÿt& èüÿÿÿéFıÿÿ¶    èüÿÿÿUWVS‰Ãƒì,‹¤  ‰T$d¡    ‰D$(1À…É…ëm  j j h€   SèõşÿÿƒÄ…À‰;  ƒ=¤  ‡Ån  ÇD$    ÇD$    L$º   ‰Øèüÿÿÿº  ‰Ø|$‰ùèüÿÿÿ¹íÿÿÿºX  ‰Øèüÿÿÿ¹ÿÿÿÿºT  ‰Æ‰Øèüÿÿÿ¹  €ºø  	Æ‰Øèüÿÿÿ¹   ºü  	Æ‰Øèüÿÿÿ1Éºd   	Æ‰Øèüÿÿÿ	ğ…¯n  ‹ƒÌ   º‚  ˆˆ   D$èüÿÿÿ…À…n  ‹D$‹úÀ …n  ‹@8§  ‰Át8h/  èüÿÿÿ‹D$èüÿÿÿXhD  hh0  èüÿÿÿXZÇD$ÿÿÿÿéô  ¶    xf»Uªu¿ƒ=¤  ‡.  ‰$1í1ö‰Ëë	´&    ‰Í‹«‹$‰ê)Î‰L$ƒîèüÿÿÿ…À…Òm  ‹D$M‹ Áè9ÈwĞ‹¤  ‹$…É…]o  …íx%v L$‰ê‰Øèüÿÿÿ…À…Do  ‹D$tƒísŞ…ö…+o  ‹D$èüÿÿÿ¡¤  …À…o  ¹ÿÿÿÿºT  ‰Øèüÿÿÿ‹L$º   ‰Æ‰Øèüÿÿÿ‹L$º  	Æ‰ØèüÿÿÿºX  	ÆL$‰Øèüÿÿÿ‹L$ºX  	Æ‰Øƒáèèüÿÿÿ¹ @  º   	Æ‰Øèüÿÿÿ¹ @  º  	Æ‰Øèüÿÿÿº   	ÆL$‰Øèüÿÿÿ‰ùº  ‰Øèüÿÿÿ…öˆo  ƒ=¤  ÇD$xV4ÇD$xV4ÇD$ 4xVÇD$$V4x‡so  1ö1íëv 1öƒÅıÀ „¥o  ‰ù‰ê‰Øèüÿÿÿ‹D$9D´uÚƒÆƒşuÔ¡¤  ƒÅ…À…ºm  ‰«¼  j j h€   SèíûÿÿƒÄ…ÀˆQ  Wjj hÄ   SèÒûÿÿƒÄ…Àˆ¢m  ‹=¤  …ÿ…‡m  ¸È   èüÿÿÿ‹5¤  …ö…rn  ‹ƒ   ¾   ‰ß% ù  ƒøÀƒà`à  ‰ƒŒ  ÿ³ˆ  Pj jh‘   Sèfûÿÿ‹ƒˆ  ƒÄf‰ƒÈ  ‹ƒŒ  f‰ƒÊ  1À÷ƒ   ÿÿ •Àf‰ƒÌ  ƒÀ  èüÿÿÿjjj jhÜ   Sèûÿÿjjj jhÜ   Sèûÿÿ¸<   ƒÄ0èüÿÿÿhğ   hğ   j jhÖ   SèÜúÿÿj j j j j j j j j j j j j jh×   SèµúÿÿƒÄXj@jhÀ€` hU‘U‘jjh ½ j jhÈ   SèŒúÿÿƒÄ,´&    fFí1Ûj ƒøj –Ãj SVj jh·   Wè^úÿÿ‰ğƒÆƒÄ$   €j j j SPj jh·   Wè:úÿÿƒÄ$ƒşu²j j jhÚ   Wè!úÿÿ¸<   èüÿÿÿj j hÍ   Wèúÿÿ¸<   ƒÄ$èüÿÿÿ¹€   ºH  ‰øèüÿÿÿ‹L$…Éu"‹D$(d+    u?‹D$ƒÄ,[^_]Ã´&    fj j j jh   Wè­ùÿÿ¸
   èüÿÿÿƒÄÇD$    ë´èüÿÿÿéÎo  é(n  ´&    èüÿÿÿUº   WVSƒì‹h(‰èt  è/úÿÿ‰Æ…Àty½¸  ‰øèüÿÿÿ‰$‹…t  9ØtD´&    ‹…t  ‹P‹-x  ‰Q‰
º   Ç€x     Ç€|  "  èüÿÿÿ‹…t  9ÃuÃ‹$‰øèüÿÿÿ‰ğƒÄ[^_]Ã´&    ‹…t  ‰Úˆˆıÿÿ…Ü  èüÿÿÿ‰ğƒÄ[^_]Ã´&    ¶    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë$ƒÄ‰ğ[º  ^éïÿÿèüÿÿÿ´&    ´&    èüÿÿÿV‰ÆS‰Óº  ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ‹D$d+    u·Ë‰ğº  ÷Ñ#$ƒÄ[^é²îÿÿèüÿÿÿ´&    ¶    èüÿÿÿW‰ÏV‰ÆS‰Óº   ƒìd¡    ‰D$1À‰á‰ğèüÿÿÿ·Ë…ÿt&$‹D$d+    uƒÄ‰ğº   [^_éLîÿÿt& ÷Ñ#$ëÖèüÿÿÿ´&    t& èüÿÿÿƒìd‹    ‰T$1ÒT$ÇD$    Ç$    RT$Rjj hÆ   PèC÷ÿÿ¡¤  ƒÄ…À…æo  ‹D$d+    uƒÄÃèüÿÿÿ´&    ¶    èüÿÿÿS‹X(j j jj jh‚   Sèòöÿÿ¸ô  èüÿÿÿ‰ØèüÿÿÿƒÜ  ƒÄ[éüÿÿÿfèüÿÿÿS‰Ã¡¤  …À…
p  ‹ƒÄ  …Àt%‹ğ  ƒâu&èüÿÿÿƒÔ  èüÿÿÿÇƒÄ      [Ã´&    v èüÿÿÿëØ´&    fèüÿÿÿUWVS‰Ãƒì¡¤  …À…op  iƒ¸    ƒ¸   …N  Çƒ(      ³À  º2   Çƒ,      ‰ğÇƒ  •  Çƒ      Çƒ$      ÇƒŒ  à  Çƒˆ  Ğ  ÇƒÄ     èüÿÿÿ‰Å…ÀtƒÄ‰è[^_]Ãt& ‰›D  1Ò‰ğ»Œ  @  ÇƒH  p‡  ”Âèüÿÿÿ“Ô  C\1Éj èüÿÿÿ‹ƒÜ  ‰D$X¡¤  …À…‹p  èüÿÿÿ‰Â…Àt^‰Ç¾@V  ¹Í   ó¥hœ  ‹ƒÜ  i€¸    ÿ°    ‚À  h¡  j ‰T$Pèüÿÿÿ‹D$‹T$ƒÄÇ‚      ƒÀ‰‚¬  ‹ƒÌ   »  ‰“Ä  ³È  ÇƒÈ     ˆ   ‰›ğ  ‰ƒĞ  ‰ğÇƒÌ     Çƒ      Çƒ     Çƒø     Çƒä     Çƒè      Çƒü      ‰»Ü  èüÿÿÿ…Àˆ   ‹ƒÄ  ‰˜(  ‹ƒÄ  ‰¸0  ‹ƒÄ  ‰°¸  ‹ƒÄ  Ç€ä     ƒ»€  t‹ƒÄ  ˆä      ‹ƒÄ  ¹ÿÿÿÿ‹à   ÿ21ÒjèüÿÿÿY_‰Æƒ¼  …ö‰&p  éSp  ƒÄ‰Å[‰è^_]Ãt& ½íÿÿÿéşÿÿfffèüÿÿÿSğºp’Â¶ÒÁâğºp’Ã‰ÑƒÉ„ÛEÑğºp’Ã‰ÑƒÉ„ÛEÑğºp sƒÊ‹H(…Ét-¬  [éüÿÿÿ´&    …Òuâ[Ã´&    v èüÿÿÿğºp Ãt& èüÿÿÿ‹H`öÂtğ€‰¸  öÂtğ€‰¸  öÂtğ€‰¸  ƒâtğ€‰¸  ‘¨  9  t‰Ñ‹    ¸    éüÿÿÿt& ‰Ğéÿÿÿ´&    fèüÿÿÿƒâ‹H`tğ€‰Ì  9  t‹    Á¼  ¸    éüÿÿÿt& ğº±Ì   ÃfffffffèüÿÿÿWVS˜hæÿÿƒì‹@üd‹    ‰T$1ÒÆD$ …Àt6‹Pd‹…Òt-‹r$…öt&‹    L$…Òtx‹z$…ÿtqº   èüÿÿÿ€|$ u<º   ‰Øèüÿÿÿº   ƒÈ€¶È‰Øèüÿÿÿº#   ‰Øèüÿÿÿº#   ƒÈ€¶È‰Øèüÿÿÿ‰Øº   èüÿÿÿ‹D$d+    uƒÄ[^_Ãt& º   èüÿÿÿëèüÿÿÿfffffffèüÿÿÿVSƒì0‹€h  d‹    ‰T$,1Ò‹…Û„)  ‹ƒ¨  …À„  ÇƒØ      ‹Pd‹R…Ò„³   ‹J…É„¨   ‹   …Ò„Ê  ‹r…ö„¿  ‰âèüÿÿÿ‹‹¸  ‹ƒ¨  ƒù†{  ƒé!ƒùw#º   Óâ÷Âğ…0  â‚  …€   t& …À„È   ‹Pd‹R…Ò„º   ‹J…É„¯   ‹   …Ò„Y  ‹Z…Û„N  ‰âèüÿÿÿ1ÀéŠ   ‹‹¸  ƒù†™   ƒé!ƒùw¯»   Óã‰Ù÷Ãğ…¬   á‚  t’ÇD$ è€  ÇD$$ø§  ÇD$ ] ÇD$    ÇD$  ÇD$¦ß¼ ÇD$”  ÇD$!   ÆD$…À…?ÿÿÿ´&    1À‹T$,d+    …¿   ƒÄ0[^Ã´&    ƒù†ÿÿÿ»   Óãã uƒù„kÿÿÿ…Ò…ışÿÿëµt& ÇD$    ÇD$  ÇD$ÕÜ2 ÇD$ ÆD$é¶şÿÿ¶    ƒù†§şÿÿº   Óââ u¸ƒù…şÿÿéÿÿÿf‰âèüÿÿÿé<şÿÿt& ‰âèüÿÿÿé<ÿÿÿt& ¸íÿÿÿé0ÿÿÿèüÿÿÿèüÿÿÿU‰ÅW1ÿVSì  ‹Ô  ‰T$d¡    ‰„$  1À‹…¨  ÇD$    …Àtz‹Pd‹R…Òtp‹2…ötj‹   …É„„   ‹   ‹	‰$…ÉtuL$Q¹   ‹t$T$èüÿÿÿZ‹D$‰ÆÁît1ÿ‹CTüƒÇèüÿÿÿ9şuí‹D$¿   …À…xÿÿÿt& €|$ uA‰ø„Àu1‹„$  d+    u^Ä  [^_]ÃL$Q¹   T$èüÿÿÿXë‹CèüÿÿÿëÅ‹[T$ÇD$    ÇD$    ‰ØÆD$èüÿÿÿÆƒ0  ‰Øèüÿÿÿë’èüÿÿÿ´&    èüÿÿÿWVS‰Ãƒì0d¡    ‰D$,‹ƒ¨  …À„ü   ÇƒØ     ‹Pd‹R…Òt'‹J…Ét ‹   …Ò„
  ‹r…ö„ÿ   ‰âèüÿÿÿ€|$
 ‹5   …   ´&    1ÀÆD$
f‰D$‹ƒ¨  …Àtr‹Pd‹R…Ò„¡   ‹J…Ét4‹   …Ò„—   ‹z…ÿ„Œ   ‰âèüÿÿÿ‹ƒ¨  …Àtp‹Pd‹R…Òtf‹J…Ét_‰â…ötT‹~…ÿtMèüÿÿÿ¶D$
„À„{ÿÿÿv ƒ˜  èüÿÿÿƒ¬  èüÿÿÿƒÀ  èüÿÿÿ‹D$,d+    u6ƒÄ0[^_Ãv èüÿÿÿ¶D$
ë±t& ‰âèüÿÿÿéoÿÿÿt& ‰âèüÿÿÿéüşÿÿèüÿÿÿ´&    èüÿÿÿ‹€h  ‹ …Àtézşÿÿ´&    v Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì0‹‹¨  d¡    ‰D$,1À…Ét…Òt‹ƒ¸  ƒøw-ƒø‡  t& ‹D$,d+    …Ó  ƒÄ0[^_]Ã´&    ƒè!ƒøwØ¾ƒñ£ÆsÎ‰Öƒæƒâ„Ü   ‹Ad‹@…À„ú   ‹x…ÿ„ï   ¡   ‰â…À„0  ‹h…í„%  ‰Èèüÿÿÿ‹‹¨  ‹ƒØ  ÆD$ …À•D$
…É…À   …ö…  ‹ƒØ  ÆD$…À‹ƒ¨  •D$
…À„>ÿÿÿ‹Pd‹R…Ò„0ÿÿÿ‹J…É„%ÿÿÿ‹   …Ò„ß   ‹Z…Û„Ô   ‰âèüÿÿÿé ÿÿÿ¾€£Æƒòşÿÿ‰Öƒæƒâ…$ÿÿÿ…ö„Üşÿÿ‹D$,d+    …¯   ƒÄ0‰Ø1Ò[^_]é›ûÿÿv ‹ƒØ  ÆD$ …À•D$
‹Ad‹@…À„2ÿÿÿ‹x…ÿ„'ÿÿÿ¡   ‰â…ÀtZ‹h…ítS‰Èèüÿÿÿé	ÿÿÿ´&    ‰Èèüÿÿÿ‹‹¨  éÖşÿÿ¶    º   ‰Øè$ûÿÿéáşÿÿ´&    ‰âèüÿÿÿé,şÿÿt& ‰Èèüÿÿÿé¶şÿÿèüÿÿÿ´&    èüÿÿÿUWVS‹ˆ¨  …É„Ä  ‹ˆ¸  ‰Æƒù"„¸  †Ú  ƒù0„I  †‹  ¿Á  ƒù3‡e  t& ¡   ¹   ºÀ  èüÿÿÿ‰Å…À„|  ‰0i†¸    ÿ°    hÖ  hÀ  èüÿÿÿ‰EƒÄ…À„^  ‹–Ì   ‹‚´   …À„§  Phæ  hÀ  èüÿÿÿ‰EƒÄ…À„%  ¸   èüÿÿÿ‰Ã…À„í  ‰E‹Eº   ‰ƒØ  ‹Ef‰“æ  ‰ƒÜ  ¸   f‰ƒà  ‹†Ì   ·P$f…Ò…E  ·@ f‰ƒâ  ‹†Ì   ·@"f‰ƒä  ‹†Ì   ‰»ì  ˆ   ‰«h  ‰C$‰ØÇƒ4  üÿ¿Çƒ8      Çƒà  ğ”  Çƒä   š  Çƒè  ñ  ‰®Ô  èüÿÿÿ‰Ç…À…î   [‰ø^_]Ã´&    ¿íÿÿÿƒù:uè¿g  éŒşÿÿt& ƒù)w#¿¶  ƒù'‡uşÿÿ¿íÿÿÿ[‰ø^_]Ã´&    fƒé-ƒùwã¿Á  éNşÿÿ¶    ƒùw3¿íÿÿÿƒùv‹¸   Óà© u•ƒù…tÿÿÿ¿  éşÿÿ´&    ¿œ  ƒù!„şÿÿ¿íÿÿÿë‹v ‹‚ˆ   éNşÿÿt& f‰“â  ‹†Ì   ·@&éºşÿÿf¿Ë  éÆıÿÿ¶    ‰ğè™ùÿÿ‰ØÇ†Ô      èüÿÿÿ‹Eèüÿÿÿ‹Eèüÿÿÿ‰èèüÿÿÿ‰ø[^_]Ã´&    v ¿ôÿÿÿëÑ´&    f¿t  éfıÿÿ¿ôÿÿÿé®şÿÿ¿ôÿÿÿëµ¿ôÿÿÿë¶´&    ´&    èüÿÿÿS‰Ãèùÿÿ‹ƒÔ  …Àt9‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  ‹@èüÿÿÿ‹ƒÔ  èüÿÿÿÇƒÔ      [Ãfffffffèüÿÿÿ‰Â‰È‹’”   ‹’À   ‹’Ü   Š  ‹’  ƒÊ ‰‹T$‰1ÀÃ´&    t& èüÿÿÿWVS‹°”   ‰Ó¾P  ‰øèüÿÿÿ‹–$  ‰‹–(  ‰S‹–,  ‰S‹–0  ‰S‹–4  ‰S‹–8  ‰S‹–<  ‰S‹–@  ‰S‹–D  ‰S ‹–H  ‰S$‹†L  ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿWVS‹°”   ‰Ó¾ğ   ‰øèüÿÿÿ‹–Ä   ‰‹–È   ‰S‹–Ì   ‰S‹–Ğ   ‰S‹–Ô   ‰S‹–Ø   ‰S‹–Ü   ‰S‹–à   ‰S‹–ä   ‰S ‹–è   ‰S$‹†ì   ‰C(‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸P  VS‹°À   ‰Ã‰øèüÿÿÿ‹†Ü     ‹ƒâß‰‹†Ü      ‹âıÿÿ‰‹†Ü      ‹ƒâß‰ºÿÿ  ‹†Ü   ‰  Æƒ.  ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿW¸ğ   V‰ÆS‹˜À   ‰øèüÿÿÿ‹ƒÜ     ‹ƒâì‰‹ƒÜ      ‹â¿şÿÿ‰‹ƒÜ      ‹ƒâï‰‹ƒÜ      ‹ƒâó‰1Ò‹ƒÜ   ‰  ºÿÿ  ‹ƒÜ   ‰  Æ†Î   ‰øèüÿÿÿ1À[^_Ãt& èüÿÿÿUW‰ÇVSì„   ‰D$‰L$d¡    ‰„$€   ‹‡”   ‹°À   ‰D$‹†Ü      ‹ ‰D$ ‹†Ü   ‹¸  ‹†Ü   ‹˜  ‰Ø‰Ú‰ıÆ ƒàƒâ ƒå ‰D$‰Øƒà‰$‰øƒà‰D$‰øƒàƒ=¨  ‰D$‡ìp  …ít…Ò…¶  !û‹|$‰Ø‹$ƒàˆD$…Û‹D$•Â…ÿ¨   •À!Â‹D$ˆT$…À„p  ‹L$1Û…É„b  ‹D$  ‰$¶    ‹†Ü   @  ‹ ‰Ââÿÿıÿ‰TÜ@ƒÃ©   t=ƒûuØ‰èèüÿÿÿ¹@   ‰Ç‹$T$@èüÿÿÿ‰ú‰Ã‰èèüÿÿÿƒû@„ê   éõq  t& ‰èÁãèüÿÿÿ‰Ù‰Ç‹$T$@èüÿÿÿ‰ú‰$‰èèüÿÿÿ;$…õq  ‹D$ÇD$<    Æ €|$ …r  €|$ ¸ÿşÿÿt*ƒL$<‹|$ ‹Ü   !ø‰   ‹†Ü   ‰¸   ‹D$Æ ‰èèüÿÿÿ‹L$‰Â‹  +  =ÿ  vƒL$<‰èèüÿÿÿ‹\$<…Ût‹L$…É…©   ‹„$€   d+    …Ä   Ä„   1À[^_]Ã1Ûé¹şÿÿ´&    f€|$ tU1Ûéşÿÿ‹†Ü     ‹ƒâß‰‹D$ÇD$<   …Àt ‹@`…Àt‹h8…ít‹D$L$<ºv@èüÿÿÿ‹D$Æ éüıÿÿ€|$ ÇD$<    „ ÿÿÿéQr  t& ‹|$‹G`…À„Hÿÿÿ‹X8…Û„=ÿÿÿL$<º v@‰øèüÿÿÿé(ÿÿÿèüÿÿÿt& èüÿÿÿU‰ÕWVSƒì$‹”   ‰D$ ‹‚  ‹š  ‰D$ƒáøu‹D$8Ç     ƒÄ$1À[^_]Ã´&    º   ‰L$‰ø‰$èüÿÿÿ‹$‹L$‰Æ‚  ‰êèüÿÿÿ‰ò‰$‰øèüÿÿÿ‹$‹|$8‰Èƒàø‰‰ÈÁèt¤·ÛDÅ ƒÃ‰D$‰$Áû‰\$éë   ‰ğ‹\$Áèƒà‰D$…Ûtƒt$‹D$ƒàˆD$1ÛÇD$    ·ö1ÿ‹L$‹$¤÷Áæ‰ğ‰ş‰ÏƒÈ¯ò¯ø‰D$÷$$ş¿è  òiòè  ÷çò1ö‰Ñ‰Ç…Òt‰Öº÷ÚKÑî‰ğ÷â‰ĞÁêÁè‰ÖkÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‰òƒùvƒÀº ¡ =çhÍw
ºÓMb÷âÁêÁã‰U 
\$ƒ=¨  ÇE    ˆ]‡wr  ƒÅ;l$„œşÿÿ‹u ‰ğ%ÿÿ =ÿÿ … ÿÿÿÆD$ »   ÇD$   ÇD$    éÿÿÿèüÿÿÿUWVSƒì$‹°”   €z
 ‹®À   …Î  ‹B‰Ó‰D$…À…  †P  ‰$èüÿÿÿ¶C
ˆ†.  ÇC    Ç†(      Ç   Ç†$     ‹…Ü     ‹ƒâß‰‹…Ü      ‹âıÿÿ‰¶K‹…Ü   Áá   ‹1Èƒàß1È‰¶Cˆ†/  „À„å   ‹K1ÒÁá‰ÈÑè€ù7÷ñ‰Á=   ‡M  ƒø†¬  @ÿ‰Ï‰D$Áç‰D$¸üŸ÷é‰D$‰øÑè‰T$ 1Ò€ù7÷÷iÑ   ¹÷ÚKƒÂÑê‰Ç‰Ğ÷áÁê‰T$‹…Ü   ‹L$‰ˆ  ‰{ºY·Ñ‰¾4  kKdÁ8  ‰È÷â¸   Áê	ùq  ƒÒÿ9ÂGĞ‹…Ü   ‰  ƒÂ‹D$kÒdƒÂÁê‰S‰–8  ‹T$ é  v ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ñ‰Ç…Òt‰ÈºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Á‰ø‰Ê¿è  ÷÷‰Ñ‹T$ùó  vƒÀƒÒ 1É‰D$‰×…Òt"¹@  ‰Ğ÷á‰ø)ĞÑè‰ÊÁê‰Ğ‰ÑÁà)Ğ)Ç‰ú‹D$¿ÿÿ ÷÷‰Ê¿   1É9Ç‰Ï×‚¨  ¿   9ÇÑƒ  ƒÀÿ‰ÁƒÒÿ‰D$‰ÇƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹…Ü   ‰¸  ‹D$‹T$ÇD$    ‰Ñ‰Ç…Òt‰Ğº÷ÚKÑè÷â‰ĞÁêÁè‰T$kÀ6)Á‰ø‰Ê¿6   ÷÷‰Ñ‹T$ƒùvƒÀ‰C‰†0  ‹D$‰†d  ‹D$‰C(‰†L  ‹…Ü      ‹€æ÷‰¶K‹…Ü   Áá   ‹1È€äï1È‰¶Cˆ†=  ¶K‹…Ü   Áá   ‹1È€ä1È‰¶Cˆ†<  ¶C	ˆ†-  ¶Cˆ†,  „ÀtC€{	 t‹…Ü     ‹ƒÊ ‰‹…Ü   €{ ¿€  D|$   ‹€   1ø%ıÿÿ1ø‰‹$èüÿÿÿ1ÀƒÄ$[^_]ÃfƒÄ$‰ğ[^_]éöÿÿfÇD$÷J ¿ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  é|şÿÿ¶    ÇD$÷J ¿3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  éÓüÿÿ¶    ÇD$”   ¿   ÇD$   ÇD$0ø?ÇD$    éşÿÿ¶    ÇD$”   ¿Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   éküÿÿ¶    ¸Úÿÿÿéÿÿÿ¶    èüÿÿÿUWVSƒì$‹¨”   €z
 ‹½À   …  ‹B‰Ó‰D$…À…}  …ğ   ‰$èüÿÿÿ¶C
ˆ…Î   ÇC    Ç…È       Ç   Ç…Ä      ‹‡Ü     ‹ƒâì‰‹‡Ü      ‹â¿şÿÿ‰¶K‹‡Ü   Áá   ‹1Èƒàï1È‰¶Cˆ…Ï   „À„5  ‹s1ÒÁæ‰ğÑè€ù7÷ö‰Á=   ‡  ƒø†Œ  @ÿ‰Î‰D$Áæ‰D$¸üŸ÷é‰D$‰ğÑè‰T$ 1Ò€ù7÷öiÑ   ¹÷ÚKƒÂÑê‰Æ‰Ğ÷áÁê‰T$‹‡Ü   ‹L$‰ˆ  ‰sºËk(¯‰µÔ   ÇC2   Ç…Ø   2   ‹KÁáq	‰ğ÷â)ÖÑîò1öÁê9S sQ
¾ÍÌÌÌ‰Ğ÷æ¾   ÁêƒÁ‰S ºOìÄN‰È÷âÁê9S$vº«ªªª‰ÈƒÎ÷âÁê‰S$‹‡Ü      ‹1ğƒàü1ğ‰‹C ‹T$ ‰…ä   ‹C$‰…è   ‹D$é  t& ‹Cº ¡ ÇD$    9ĞGÂº6   ÷â‰Ö‰Á…Òt‰ğºÓMb÷â‰ĞÁêÁè‰T$iÀè  )Æ‰È‰ò¹è  ÷ñ‰Ö‹T$şó  vƒÀƒÒ 1ö‰D$‰Ñ…Òt"‰Èº@  ÷â‰È)ĞÑè4‰òÁê‰Ğ‰ÖÁà)Ğ)Á‰Ê‹D$¹ÿÿ ÷ñ‰ò1É¾   9Æ‰ÎÖ‚˜  ¾   9ÆÑƒ‘  ƒÀÿ‰ÁƒÒÿ‰D$‰ÆƒÁ¸üŸ‰T$÷é‰T$iÑ   ¹÷ÚK‰D$ƒÂÑê‰Ğ÷áÁê‰T$‹‡Ü   ‰°  ‹D$‹T$1ö‰D$‰Ñ…Òt‰Öº÷ÚKÑî‰ğ÷â‰Ğ‰ÖÁèÁîkÀ6)Á‰Ê‹D$¹6   ÷ñ‰Ñ‰òƒùvƒÀ‰C‰…Ğ   ‹D$‰…  kK6‹·Ü   Áô  ùÿÿç‡.  ùŸ  †²  ‰ÈºÓMb÷â¸÷ÚK‰ÑÁéiÑè  ƒÂÑê÷âÁê‰  ‹D$‰S‰•à   ‰C(‰…ì   ‹‡Ü      ‹€æû‰‹‡Ü      ‹ƒÊ‰¶Cˆ…Ü   ‰…  ¶C	ˆ…Í   ¶Cˆ…Ì   „À…«   ‹$èüÿÿÿ1ÀƒÄ$[^_]Ã´&    fƒÄ$‰è[^_]érñÿÿfÇD$÷J ¾ÿÿ  ÇD$ÿÿ  ÇD$  üÇD$Ÿ  éŒşÿÿ¶    ÇD$÷J ¾3   ÇD$  üÇD$ Ÿ  ÇD$ÿÿ  ÇD$ÿÿ  é“üÿÿ¶    º«„ ¹ÿÿ  éòşÿÿ´&    fµ   ‰ğèüÿÿÿÇ…      Ç…      ‰Â‰ğèüÿÿÿ€{	 t‹‡Ü     ‹ƒÊ‰€{ ¸@  DD$‰Ã‹‡Ü      ‹€   1Ø%¿şÿÿ1Ø‰éáşÿÿt& 1Ò1Ééhşÿÿ´&    ÇD$”   ¾   ÇD$   ÇD$0ø?ÇD$    é„ıÿÿ¶    ÇD$”   ¾Ì¿ ÇD$0ø?ÇD$     ÇD$   ÇD$   é‹ûÿÿ¶    ¸Úÿÿÿéeşÿÿ¶    èüÿÿÿUWVS‰Ãƒì$‹€”   ‹€À   ‹¸Ü   ‹·   ‹¸Ü   ‹  ‰L$‹¸Ü   ‹  ‰L$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹¸Ü   ‹¿  ‰|$‹€Ü   ‹€  ƒÃp‰D$½f  ¿^  Sh  èüÿÿÿ÷Æ   XZ¸W  º[  DÂPSh3  èüÿÿÿ‰ğ‰éƒÄƒàEÏ‰$QSh<3  èüÿÿÿ‰êƒÄ÷Æ@   E×RSh46  èüÿÿÿ‰ñƒáÿ4   ShĞ7  èüÿÿÿºz  ¸o  ƒÄ÷Æ @  DÂPShl3  èüÿÿÿº”  ¸Š  ƒÄ÷Æ   DÂPShœ3  èüÿÿÿº¹  ¸©  ·|$ƒÄ÷Æ    DÂPShÌ3  èüÿÿÿ‹L$ƒÄ…É„(s  écv  t& èüÿÿÿU¹h  ºÀ  WV‰ÆSƒì0d¡    ‰D$,1À¡$   èüÿÿÿ…À„Ø  ¸  º   ‰Ã¹   Ç€       ‰øhÀ  èüÿÿÿZ…À…™  ‰³À   º`  ‰Øèüÿÿÿ†¼  ‰›”   PCph7  j PèüÿÿÿF‰ÚÇƒ      èüÿÿÿ‰ÅƒÄ…À…	  ‹†Ü   ‰¨  ¹¨  ƒğ   ºA  èüÿÿÿ‰è¹
   t$Ç$   ‰÷ó«‹CdÆD$
ÇD$ ÇD$ ¸ˆ  ÇD$$ˆ  ‹@ÆD$ÇD$ Œ  …Àt(‹H…Ét!¡   ‰â…À„¨   ‹x…ÿ„   ‰ØèüÿÿÿƒP  ¹¨  ºX  ‰÷èüÿÿÿ1À¹
   ó«‹CdÇD$   Ç$   ‹@ÆD$
ÆD$ÇD$ Œ  …Àt2‹H…Ét+¡   ‰â…ÀtI‹p…ötB‰Øèüÿÿÿë´&    f‰øèüÿÿÿ‹D$,d+    uAƒÄ0‰è[^_]Ãf‰Øèüÿÿÿé^ÿÿÿt& ‰ØèüÿÿÿëÎ´&    ‰Ø½ôÿÿÿèüÿÿÿë¹½ôÿÿÿë²èüÿÿÿ¶    èüÿÿÿVº   Sèüÿÿÿ…Àt<‰Ã‹€”   èìÿÿ‹ƒ”   è“ëÿÿ‹³”   ‰Øèüÿÿÿ†  èüÿÿÿ‰ğèüÿÿÿ1À[^Ã¸íÿÿÿëöffffèüÿÿÿS‰ÃfÁÂ¹   ƒìd¡    ‰D$1À¸   f‰T$‰âf‰D$D$‰D$‰ØÇ$D   èüÿÿÿƒø…Jw  ¹   ‰â‰ØÇD$  èüÿÿÿƒø…4w  ¶D$‹T$d+    uƒÄ[Ãèüÿÿÿt& èüÿÿÿVS˜
  ‰Øƒìd‹    ‰T$1Òº  èGÿÿÿ¹   º  ƒàïf‰L$t$¹   f‰T$‰âˆD$‰ØÇ$D   ‰t$èüÿÿÿƒø…¢w  ¹   ¸ê   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$³èüÿÿÿƒø…Œw  ¹   ¸	   ‰â‰t$f‰L$¹   f‰D$‰ØÇ$D   ÇD$èüÿÿÿƒø…vw  º  ‰Øè~şÿÿ¹   ƒÈf‰L$‰â¹   ‰t$¾  ˆD$‰ØÇ$D   f‰t$èüÿÿÿƒø…`w  ‹D$d+    uƒÄ[^Ãèüÿÿÿffffèüÿÿÿ1ÀÃ´&    èüÿÿÿU¶ÒWV‰ÆS‰ËƒìPd‹    ‰L$L1É‹D$hf‰$1Éf‰L$‹|$dPˆ\$f‰T$T$‰T$ƒøs4…Àup¹   ‰â‰ğèüÿÿÿƒøu}1À‹T$Ld+    …‚   ƒÄP[^_]Ãf‹l$L$)Í‰T$‹Tü)ï‰T	è‰úƒàüƒørªƒàü1ÿ‹,:‰,9ƒÇ9Çróë–¶    ¶ˆL$¨t…·Tşf‰Tévÿÿÿ¶    ‹°  …Ò…¸w  ¸ÿÿÿÿémÿÿÿèüÿÿÿv èüÿÿÿWVSƒì‹p,d‹    ‰L$1ÉÆD$€…Òu|¸¬Ä  èüÿÿÿ¶VL‹FH1Éj|$Wè½şÿÿY‰ÃX…Ûu5¸¬Ä  èüÿÿÿ¶VL‹FH1ÉÆD$ jWè•şÿÿ¸è  èüÿÿÿ1Ò‰ğèüÿÿÿXZ‹D$d+    uƒÄ‰Ø[^_Ã´&    v »êÿÿÿëÙèüÿÿÿt& èüÿÿÿV‰ÎS‰Óƒì d‹    ‰T$1Ò¶ÓL$‰t$f‰T$f‰T$‹T$,‰L$¹   f‰L$¹   f‰T$T$ÆD$ ÇD$   èüÿÿÿƒøu)¡°  …À…Üw  1À‹T$d+    u"ƒÄ [^Ã´&    ‹°  …Ò…x  ¸ÿÿÿÿëÑèüÿÿÿ¶    èüÿÿÿS‰Ã1Òƒì(d¡    ‰D$$1ÀC¬èüÿÿÿ¡    9Cx‹Cü…Àt‹D$$d+    uOƒÄ([Ãt& ¶Sø‹Côj!L$èöşÿÿY‰Â¡    è  ‰C…ÒuÄ¡°  …À….x  ¶D$ÁàÁøƒà‰Cüë¤èüÿÿÿ´&    ¶    èüÿÿÿUWV‰ÆSƒì‹D$,‰L$‰$‹D$4‰D$d¡    ‰D$‹Fd‹…Ò…Ş  ¶ù9~lte¶VL‹FHjL$è^şÿÿ‰ÂX…Òt‹D$d+    …¶  ƒÄ‰Ğ[^_]Ãt& ¶D$¶VLƒàó
D$ˆD$‹FHjL$Q1ÉècüÿÿY]‰Â…Àu´‰~l«¤  ‰èèüÿÿÿ‹ƒÜ   º   ‰$  ‹ƒÜ      ¶D$0€ÌÇ‰‹ƒÜ      ‹€æû‰‹ƒÜ      ‹D$0¶Ä€ÌË‰‹ƒÜ      ‹€æ÷‰€<$ …Ş   ‹ƒÜ      ¶D$€ÌÃ‰‹ƒÜ      ‹¿ÿıÿÿ€~L@¹ÿşÿÿEÏ!Ê‰‹ƒÜ      ‹
€<$À% Àÿÿ-@  !È‰‹=    ƒÇë´&    f‹    9×x¸Ç  èüÿÿÿ‹ƒÜ      ‹ öÄuÚ‹“Ü   Š   ‹’   €ÎÃ‰¶Ğ‰è‰T$èüÿÿÿ¡°  ‹T$…À…Xx  1À€<$ DĞé_şÿÿ¶    ‹ƒÜ   ºÿ  ‰$  é"ÿÿÿºêÿÿÿé9şÿÿèüÿÿÿ´&    t& èüÿÿÿS¶\$¶É‹@,SQ¹   j è±ıÿÿƒÄ[Ã´&    t& èüÿÿÿ¶É‹@,j Q¹   jè†ıÿÿƒÄÃfèüÿÿÿS¶\$‹@,SQ1Éj ègıÿÿƒÄ[Ãfèüÿÿÿ‹@,j Q1ÉjèLıÿÿƒÄÃ´&    èüÿÿÿWVS‰ÓƒìT‹p,d‹    ‰T$P1Ò…Û…‹   ¶VL‹FHjL$è¹ûÿÿ¶VL¶D$¹   ‹~HÇD$
   ƒÈ`f‰T$T$ˆD$‰T$T$ˆD$‰øÆD$ èüÿÿÿYƒøu‰Ø‹T$Pd+    u*ƒÄT[^_Ãt& ‹°  …Ò…Åx  »ÿÿÿÿëÑv ¸êÿÿÿëÉèüÿÿÿt& èüÿÿÿVSƒìP‹p,d‹    ‰\$L1Û…Ò…®   …Éur»   ¸   8^htF‹°  …É…çx  ¶VL‹FH¹   ˆ\$ÇD$   f‰$T$‰T$‰âÆD$èüÿÿÿƒøuHˆ^h‹FP‹T$Ld+    uLƒÄP[^Ã´&    ƒ=¬  ÛƒãüƒÃƒ=¬  ÀƒàüƒÀéuÿÿÿt& ‹°  …Òt±é	y  ¸êÿÿÿë§èüÿÿÿt& èüÿÿÿ‹°  V‰ÆS‰Ó…É…ey  1À÷Ã  €t÷Ã  € uEã   u¸   [^Ãt& ‹8  ‹    ¸    ƒÁTèüÿÿÿ¡°  …ÀtÑé(y  ´&    f‹†ô  ‹    HT¸    èüÿÿÿ‹°  …Ò…Iy  ã   t”ëŸ´&    ´&    èüÿÿÿUW¿31  V‰ÆS»   ƒì(‹-°  d¡    ‰D$$1Àƒ=¬  ÇD$    ÀÇD$ D  ƒàüÇD$
    ƒÀÇD$  D ÇD$    ÇD$   f‰\$ÆD$ ˆD$ÇD$  f‰|$"…í…ºy  ¡   ¹t   ºÀ  èüÿÿÿ‰Ã…À„y  ‰†  ‹Fƒø„T  ƒø…  ÆCLAºA   ‹ƒ=¬  Ç    ÇC    ¸ü   À1ÉÇC    ƒàü‰{HƒÀÇC    ˆChÇC    ÇC    ÇC     ÇC$    ÇC(    ‰[,‰sdj"D$P‰øè±öÿÿ¶SL¹   ‰Ç‹CHjl$&Uè—öÿÿ¶SL¹   	Ç‹CHjUèöÿÿƒÄ	Çuzj1ÉF(‰ÚèüÿÿÿY‰Ç…ÀueCXÇCTàÿÿÿKT‹    ‰CX‰C\¸    ÇC` º  èüÿÿÿ‹°  …Ò…Öy  ‹D$$d+    uNƒÄ(‰ø[^_]Ã¶    ¶SLéåşÿÿ´&    ¡°  …À…y  ‰Øèüÿÿÿë½´&    v ÆCL@º@   é°şÿÿèüÿÿÿƒ=°   ¿ôÿÿÿtÌéy  ´&    v èüÿÿÿ…Àt/S‹˜  …Ût‹C,…Àt‰Øèüÿÿÿ‰Ø[éüÿÿÿ¶    [Ã¶    Ãfffèüÿÿÿƒì d‹    ‰L$1É¹   ˆT$‰âf‰L$L$‰L$¹   f‰L$L$‰L$¹   Ç$P   ÇD$P  ÆD$ èüÿÿÿƒø…òy  ¶D$‹T$d+    uƒÄ Ãèüÿÿÿ´&    ´&    èüÿÿÿSƒìd‹    ‰\$1Û»   ˆT$‰âf‰\$\$ˆL$¹   Ç$P   ‰\$èüÿÿÿƒø…z  »
   ´&    ¸X‰A èüÿÿÿƒëuñ1À‹T$d+    uƒÄ[ÃèüÿÿÿèüÿÿÿW‰ÇV‰Öº?   S»@   èüÿÿÿˆFf‰Ú‰øèüÿÿÿˆDÀƒÃƒûFuëv ‰Ú‰øèüÿÿÿˆDÀƒÃƒûLuë[^_ÃfffffffèüÿÿÿW¿   V1öS‰Ã´&    ¹   º   ‰Øöèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ	Æ‰ø,‰ÇuŒ‰ğ[^_Ã´&    ´&    èüÿÿÿW¿   V‰ÖS‰ÃëEt& èüÿÿÿ¸R÷ öèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ‰ø,‰ÇtX¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿº   ‰Ø¹   èüÿÿÿ‰ğº   „À‰Ø‰wÿÿÿèüÿÿÿérÿÿÿt& [^_Ã´&    t& èüÿÿÿW¿8  VSƒìd‹    ‰L$1Éf‰<$ÆD$ƒú„I  ƒú„p  …Ò„P  ‹€$  L$º   ‹@ ‹‰à´&    v PƒÀ9Áuö÷Ú¹   ‰ØˆT$º   èüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸R÷ èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºà   ‰Øè2şÿÿ¸   èüÿÿÿº   |$t$ë´&    f¶ƒÇ‰Øèşÿÿ9şuï¹   º   ‰Ø1öèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿv ¸   ƒÆèüÿÿÿ1Éº   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ„Àt‰ğ<vÏ‰ğ<„z  ¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¸   èüÿÿÿºá   ‰ØèRıÿÿ¸   èüÿÿÿ‰Øè¡üÿÿº   8Ğ‰×Fø1öv ‰Øè‰üÿÿ‰ğ‰ùƒÆ8Èuî¹   º   ‰Øèüÿÿÿº   ‰Øèüÿÿÿ¹   º   ‰Øèüÿÿÿ‰Øº   èüÿÿÿ1À‹T$d+    uNƒÄ[^_Ã¶    »  ÆD$f‰\$é´ıÿÿt& ¾  ÆD$ f‰t$éœıÿÿt& 1ÉÆD$ f‰L$é‡ıÿÿèüÿÿÿfèüÿÿÿ1ÀÃ´&    èüÿÿÿ‹P‹H|‹R‹AP·Òƒè!Ğ¯ALÃfèüÿÿÿ‹@|4  ‰Ğéüÿÿÿ´&    t& èüÿÿÿUWV‰ÖSƒì‹X{‰øèüÿÿÿ…ö„=  ½êÿÿÿƒş…  ‹3‹k ‹†d  ‰D$  ‰$¡´  ƒÀƒø‡7z  ‹†Ü   @  ‹ƒâî‰ÿu‹M ‹T$‹èüÿÿÿ‹†Ü   ‹U ‰P  º   ‹†Ü   ‰X  º   ‹†Ü   ‰0  ¡´  ÇC    ZƒÀƒø‡  ‹†Ü   º ‰P  ºÿÿÿÿ‹†Ü   ‰T  ‹†Ü     ‹*‹‹ˆä   ‰ÈƒÈ÷Ğ!è	ÈƒÈ‰‹†Ü      ‹ƒÊ ‰‹†Ü   @  ‹€@  ƒÈ‰‹5´  …ö…‹   1í‰øèüÿÿÿ‰èƒÄ[^_]Ã´&    ¡´  ‹3ƒÀƒø‡Uz  ‹†Ü   @  ‹ƒâî‰‹†Ü     ‹ƒâ÷‰‹†Ü   P  ‹âşïşÿ‰¡´  …Àt‹–d  ‹1íÂ  èüÿÿÿéxÿÿÿ‹$‹1íèüÿÿÿéfÿÿÿ‹D$‹S‹M ‹€  ¯ÊƒÀƒàü†Ü   ‹ ÑèQRPÿu ‹¼  Ph<;  èüÿÿÿƒÄé¢şÿÿ´&    t& èüÿÿÿUWVS‹h‹X|…í„mz  ‰Ç¹   1Ò‰Øèüÿÿÿ‰Æ…Àˆ¯   ‰}$1À»ø   ¹   ó«Æƒ)  1öÇƒø    Çƒü      Çƒ       Çƒ  €   Çƒ  €»  Çƒ  €»  Çƒ     Çƒ     Çƒ    Çƒ!    ‹E ‹€d  ‹€  =   tÁèƒàø‰ƒ  ‰ƒ   [‰ğ^_]Ã¡´  ƒÀƒøvëé‚z  ´&    ´&    èüÿÿÿ‹P…Ò„°   VS‰Ã¡´  ƒÀƒø‡œz  ‹s ‹N …Éua‹FèüÿÿÿÇF    ‹FèüÿÿÿÇF    ‹S ‹C‹J‹rj ÿrˆ   ÿr‰òèüÿÿÿ‹C èüÿÿÿÇC     ƒÄÇC    [^Ã´&    ‹V‹N$‹Cj jˆ   èüÿÿÿÇF     X‹s Zéwÿÿÿ´&    ´&    ´&    èüÿÿÿS‰Ã‹@|‹€4  …Àt‹Cèÿÿÿ‹C|Ç€4      1À[ÃfèüÿÿÿUW‰ÇVS‰Óƒì‹p‹@|‹€4  …Àt‰ğèÙşÿÿ‹G|Ç€4      ‹ƒL  ‰F‹“X  ¯Â‰V‰F…À„Ô  Bÿ…Ğ…Ñ  ¡   ¹(   ºÀ  èüÿÿÿ‰Ã…À„µ  ‹F‰‹F‰^ ˆÿ  ‰Íá ğÿÿ‰È‰$Áíèüÿÿÿ‹$…À‰C„ˆ  ‹´  ƒÂƒú‡µz  1Òèüÿÿÿ‰k$Dm Áàèüÿÿÿ‰C…À„  ‹S$èüÿÿÿ‹C$…ÀtU1íë#´&    f	ÂÇA    ƒÅ‰ÇA   9k$v.‰èÁàCèüÿÿÿ…À„±   ‹KTm Ñ‹ƒâ¨t¾t& ‹n ‹F‹M$‹Uj ˆ   jèüÿÿÿ‰E Y]…À„Üz  ‹K‹FjSÿvÿvèüÿÿÿ‰ÂƒÄ…Àxu‹CÇ   q‹C‹S‰P‹CÇ@    ‹V ‹G|‹R‰4  ‹G|‹V‰@  ‹G|1ÒÇ€8      Ç€<      ë7‹CèüÿÿÿÇC    ‹CèüÿÿÿÇC    ºôÿÿÿ‰Ø‰$èüÿÿÿÇF     ‹$ƒÄ‰Ğ[^_]Ã´&    ¶    ºôÿÿÿëŞ¡´  ºôÿÿÿƒÀƒøv¸é÷z  èüÿÿÿ1ÀÃ´&    èüÿÿÿUWVS…Êts‹¨0  ‰Æ‰Ó‹€Ü   ‰T  ÷Â   …{  öÇuƒã¿   uR[‰ø^_]Ãv ¡´  ƒÀƒø‡V{  ‹†Ü   º   ‰0  ¿   [^‰ø_]Ã´&    v 1ÿ[^‰ø_]Ã´&    ‹†Ü   ‹€   ‰E‹E$èüÿÿÿ‰ø[^_]ÃfèüÿÿÿVS‰Ãƒìd¡    ‰D$1À¡¸  …À…+  ‹ƒd  ‹€  …À„p{  ‹ƒÌ   T1Éºÿÿÿÿj(h    ˆ   èüÿÿÿƒÄ…Àˆ¡{  ‹$T$1É‹°ˆ  ‰‹ƒÌ   ÇF    ‰F‹$‰FRº  jj èüÿÿÿƒÄ…Àˆ°   ‹D$¹   º  Ã¼  ‰°¸  ƒÀXèüÿÿÿ‹D$¹   º   èüÿÿÿ‹$¹   º)  ƒÀèüÿÿÿh1  ‹D$ƒÀ$Pèüÿÿÿ‹D$SP$ƒÀDRhB  Pèüÿÿÿ‹D$èüÿÿÿƒÄ…Àx-ƒ=´  ÿ…ˆ{  ‹D$d+    u ƒÄ‰ğ[^Ã´&    1öëà‹$èüÿÿÿé¡{  èüÿÿÿ´&    v èüÿÿÿ‹€0  ‹@éüÿÿÿˆ«©ÿÿƒù‡  ºÓê€â„  ÿt$,Sh8   èüÿÿÿƒÄ‹D$Dd+    u=ƒÄH[Ãˆ[Íşÿº @Óê€â„  ëÄˆıæşÿº Óê€â„  ë¬ˆªÿÿë”èüÿÿÿèüÿÿÿVS˜¼  ‹€Ì   ƒx$ uSSSSShd   èüÿÿÿƒÄëSSSShD  èüÿÿÿƒÄS1öh  èüÿÿÿXZiÆ  ÿ°    VFSh\  èüÿÿÿƒÄƒş?uß[^ÃQ¾êÿÿÿh   h€  èüÿÿÿƒÄém  €=    Yt}h   h¨  èüÿÿÿ‹ƒ¸  _Y‰Âé  ¢   ¶   ¸"   ¾   €úEğVRh  èüÿÿÿ‹‹Ì   ‰òD$Áˆ   èüÿÿÿƒÄ…ÀuP‹$D$èüÿÿÿ‹$èüÿÿÿéB  ÿ5#   iƒ¸    ÿ°    ƒ¼  Phè  èüÿÿÿ‹ƒ¸  ƒÄ‰Âé  Vh<  èüÿÿÿ[^ë­‰$ÿsSh4  h  èüÿÿÿ‹T$ƒÄé‚  ‰$ÿsSh4  hP  èüÿÿÿ‹T$ƒÄé‚  hŒ   hŒ   hµ  èüÿÿÿƒÄé=   ‹†è  ÿ³  ÿ0ÿpÿ¶ğ  ÿ¶ì  ÿsSh  h(  èüÿÿÿ1ÀƒÄ$é¶!  h  h  hÄ  èüÿÿÿ‹ƒ€  ƒÄé`"  h  h  h  èüÿÿÿ‹ƒ€  ƒÄé#  ht   ht   hµ  èüÿÿÿƒÄéj$  QRPht   ht   hx  èüÿÿÿƒÄéî$  ¸0  èüÿÿÿ¸(  èüÿÿÿÿv³ˆıÿÿWÿ³ŒıÿÿVh¬  h°  èüÿÿÿƒÄé'  hÌ   hÌ   hµ  èüÿÿÿƒÄéc'  ¸0  èüÿÿÿ¸(  èüÿÿÿQhä   hä   hä  èüÿÿÿƒÄéI(  h`   h`   hµ  èüÿÿÿƒÄé{(  h  h  hµ  èüÿÿÿƒÄé)  Ph  h  h  èüÿÿÿƒÄéÕ)  hœ   hœ   hµ  èüÿÿÿƒÄéÔ+  h€  hè  èüÿÿÿXZé·,  R‰T$hÔ  hÔ  hd  èüÿÿÿ‹T$ƒÄé-  h¼  1öhŒ  èüÿÿÿXZƒ=x   uƒÆƒş$uïéœ-  ‰ò‰Øèüÿÿÿ¶ÀPVh¼  h°  èüÿÿÿƒÄëÑi€¸    kÒ„    ÿpDÿp@ÿp<ÿp8ÿp0Vhì  hì  hØ  èüÿÿÿƒÄ$é÷-  R‰T$h¨  h¨  hä  èüÿÿÿ‹T$ƒÄéä/  Rhø   hø   hÿ  èüÿÿÿ‹ƒÄéd0  hl  hl  hµ  èüÿÿÿƒÄéº0  ¸0  èüÿÿÿ¸(  èüÿÿÿh¬   h¬   hµ  èüÿÿÿƒÄéí1  Ph  h  h  èüÿÿÿƒÄé%2  Sh   h   hÿ  èüÿÿÿƒÄƒû†S2  ƒ=x   „Œ2  h   h   h(  èüÿÿÿ¸êÿÿÿƒÄé2  ÿt$ÿt$ÿ3ÿt$hŒ  hŒ  hL  èüÿÿÿƒÄé?4  hŒ  hˆ  èüÿÿÿ[^éù4  h   hx  h\	  èüÿÿÿƒÄé¬6  ƒ=x  vh   hx  h´  èüÿÿÿƒÄ†¼  Phè  èüÿÿÿ‹–d  ‰ğèüÿÿÿXZé6  h   hx  h	  èüÿÿÿƒÄé–6  h\  h\  hµ  èüÿÿÿƒÄéR7  ‹“à  ‹‚  …À„Ü   PVhÄ	  èüÿÿÿ¹Q  º@  ‰Øè\(  ‰ƒä  ƒÄ‰¨¸  ‹ƒä  Ç€ä    ‹ƒ¸  ƒø4tƒè8ƒø‡”   ‹ƒä  ˆä      ‹ƒä  ‹“Œ  ‹• J  ‹à   ÿ2º   jèüÿÿÿY_…Ày‰$Vhğ	  èüÿÿÿXZ‹$é¤:  hD  hD  hµ  èüÿÿÿƒÄé8  ‰$Vh˜	  èüÿÿÿ]X‹$é¤:  ‹‚è   éÿÿÿƒ»€  ‹ƒä  „oÿÿÿˆä      ‹ƒä  éZÿÿÿ‹“ä  ‹‚  …Àt#PVh
  èüÿÿÿ‰Øèüÿÿÿ1Ò‰ƒ0  ƒÄé±:  ‹‚è   ëÕÿsSh¸  hD
  èüÿÿÿƒÄék>  ÿsSh¸  h|
  èüÿÿÿƒÄék>  h   h   hĞ  èüÿÿÿƒÄé*@  hÈ  hÈ  h¸
  èüÿÿÿƒÄégA  èüÿÿÿUW‰ÇÁïVS‰Ã‹½   …Àu¸é  P¾   ½   Shñ  èüÿÿÿƒÄ‰è‰ñÓà…Øtÿ4µ  hÿ  èüÿÿÿXZNƒşuİãÿ  Sh  èüÿÿÿ‹½€  ZY…Àu¸   [^_]ÃPÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  Pÿt$ÿ³ŒıÿÿUhÄ  h  èüÿÿÿƒÄé*F  ÿ2hx  hx  hL  èüÿÿÿƒÄédK  ‰$ÿ2hx  hx  hx  èüÿÿÿ‹L$ƒÄéTI  ‰$‹L$‹AèPVhx  hx  hÔ  èüÿÿÿ1Ò‹L$ƒÄéÙI  ÿt$ÿt$ÿ7ÿ³h  hx  hü  èüÿÿÿƒÄéBK  èüÿÿÿU1íW‰ÇV°¼  S‰Óƒìÿ2Vh8  èüÿÿÿƒÄ‹Cèƒàü‡Ü   ‹ PƒÅÿµ<  Vhh  èüÿÿÿƒÄƒı8uÓ1í‹CD¨8ƒàü‡Ü   ‹‰$UEVh  èüÿÿÿ‹D$èşÿÿƒÄƒıuÍ‰4$‰ù1í‰ß‹G­    ‰L$ØƒàüÜ   ‹0U_S»   ÿt$h  èüÿÿÿ‰ğuèÌıÿÿ‰Â‰l$‹l$ƒÄ‡×9ûs1‹B‰T$°ƒàü…Ü   ‹ SCPVFÿt$h´  èüÿÿÿ‹T$ƒÄëË‰é‹l$‡úÕƒı†lÿÿÿ‰û‹4$‰Ï‹C‹SÂRPVhà  èüÿÿÿ‹CP`RPVh  èüÿÿÿ‹Cƒàü‡Ü   ‹ ƒÄ PVh0  èüÿÿÿ‹Cƒàü‡Ü   ‹ PVhT  èüÿÿÿ‹C ƒàü‡Ü   ‹ PVhx  èüÿÿÿ‹C$ƒàü‡Ü   ‹ ƒÄ$PVhœ  èüÿÿÿƒÄ[^_]Ã÷Ã   t ƒ=„  vh   h¨  hÀ  èüÿÿÿƒÄ÷Ã   t$ƒ=„  †Ç   h   h¨  hğ  èüÿÿÿƒÄ‰İå   öÇt ƒ=„  v;h   h¨  h   èüÿÿÿƒÄ…ít ƒ=„  vh   h¨  hP  èüÿÿÿƒÄ‡¼  Ph€  èüÿÿÿ‹†À   ƒàü‡Ü   ‹‹–ô   ÷Ò!Ê‰kV,‰ø—d  èüÿÿÿXZé)L  jh¨  h¨  èüÿÿÿƒÄéùK  ‰İå   öÇu”ékÿÿÿhd  hd  h”  èüÿÿÿƒÄé‹Q  kV,‰Ø“d  èüÿÿÿ‹¼  ‰$ÿ·  ÿ·ˆ  Ph   èüÿÿÿ1À‰\$ƒÄ‰t$‰Æ‹‡„  Áè9Æƒ¹   V»   ÿt$h.  èüÿÿÿµ    ‰D$‹‡ˆ  ‹°èûÿÿVƒÄ‰t$‰Ö‰l$‰ı‰Ç‹…ˆ  9ß†e  SƒÃÿ4°VƒÆÿt$h0  èüÿÿÿƒÄëÔÿ³ô  ÿ³ğ  ÿ³ì  hd  hd  hÔ  èüÿÿÿƒÄénO  PRhd  h\  èüÿÿÿ¸êÿÿÿƒÄéiR  ‹\$‹t$é½O  ‹.hL  hL  h˜  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü      ‹ PhL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‰èè¬A  PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   P  ‹ PhL  hL  h4  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @  ‹ PhL  hL  hl  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   `  ‹ PhL  hL  h¤  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   @ ‹ PhL  hL  hÜ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   L  ‹ PhL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ,  ‹ PhL  hL  hL  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   d  ‹ PhL  hL  h„  èüÿÿÿƒÄƒ=„   „CR  ‹†¸   ƒàü…Ü   ‹ Pÿ¶¸   hL  hL  h¼  èüÿÿÿƒÄƒ=„   „CR  ‹†¼   ƒàü…Ü   ‹ Pÿ¶¼   hL  hL  hğ  èüÿÿÿƒÄƒ=„   „CR  ‹†À   ƒàü…Ü   ‹ Pÿ¶À   hL  hL  h$  èüÿÿÿ‹†ì   ‹„  ƒÄ…À…~  …Ò„CR  ‹†Ä   ƒàü…Ü   ‹ Pÿ¶Ä   hL  hL  hŒ  èüÿÿÿƒÄƒ=„   „CR  ‹†È   ƒàü…Ü   ‹ Pÿ¶È   hL  hL  hÀ  èüÿÿÿƒÄƒ=„   „CR  ‹†Ì   ƒàü…Ü   ‹ Pÿ¶Ì   hL  hL  hô  èüÿÿÿƒÄƒ=„   „CR  ‹†Ğ   ƒàü…Ü   ‹ Pÿ¶Ğ   hL  hL  h(  èüÿÿÿƒÄƒ=„   „CR  ‹†Ô   ƒàü…Ü   ‹ Pÿ¶Ô   hL  hL  h\  èüÿÿÿƒÄƒ=„   „CR  ‹†Ø   ƒàü…Ü   ‹ Pÿ¶Ø   hL  hL  h  èüÿÿÿƒÄƒ=„   „CR  ‹†Ü   ƒàü…Ü   ‹ Pÿ¶Ü   hL  hL  hÄ  èüÿÿÿƒÄƒ=„   „CR  ‹†à   ƒàü…Ü   ‹ Pÿ¶à   hL  hL  hø  èüÿÿÿƒÄƒ=„   „CR  ‹†ä   ƒàü…Ü   ‹ Pÿ¶ä   hL  hL  h,  èüÿÿÿƒÄƒ=„   „CR  ‹†è   ƒàü…Ü   ‹ Pÿ¶è   hL  hL  h`  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü     ‹ PhL  hL  h”  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   8  ‹ PhL  hL  hÈ  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   <  ‹ PhL  hL  hü  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   H  ‹ PhL  hL  h0  èüÿÿÿƒÄƒ=„   „CR  ‹…Ü   ‹€L  PhL  hL  hd  èüÿÿÿƒÄéCR  ‹\$‰ù‹t$‰ï‹l$<   pu‹\$‹t$é½O  Îéùÿÿ…Ò„CR  ƒàü…Ü   ‹ Pÿ¶ì   hL  hL  hX  èüÿÿÿ‹„  ƒÄéFüÿÿ‰$Sh8  h8  hF  èüÿÿÿ‹T$ƒÄéòR  ‰$h$  ÿsSh$  h¼  èüÿÿÿ‹T$ƒÄéT  ‰$h$  ÿsSh$  hè  èüÿÿÿ‹T$ƒÄéT  h  h  h]  èüÿÿÿ‹7ƒÄƒ=„   „zT  hø  hø  h]  èüÿÿÿƒÄézT  ÿvhr  ÿ¶ŒıÿÿUhä  h  èüÿÿÿƒÄéxU  ƒ¼  VPh€  èüÿÿÿƒÄ÷Æ   tƒ=„   thÔ  h¼  èüÿÿÿ]X÷Æ   tƒ=„   thÔ  hè  èüÿÿÿY_÷Æ   tƒ=„   „ô   hÔ  h  èüÿÿÿXZ‰÷ç    ÷Æ   tƒ=„   t/hÔ  hD  èüÿÿÿY]…ÿtƒ=„   thÔ  hl  èüÿÿÿXZ‰÷ç   ÷Æ   tƒ=„   t/hÔ  h˜  èüÿÿÿ]X…ÿtƒ=„   thÔ  hÀ  èüÿÿÿY_‹ƒœ  ƒàüƒÜ   ‹‹“Ğ  ÷Ò!Ê‰k“ä  ,‰Ø“d  èüÿÿÿ‰Øèüÿÿÿé}V  hÔ  hì  èüÿÿÿXZé\V  ‰÷ç    ÷Æ   …Qÿÿÿé.ÿÿÿ‰L$h   h¸  h   èüÿÿÿ‹L$$ƒÄéÿX  ‰L$jh¸  hÄ  èüÿÿÿ‹L$$ƒÄé°X  ‰L$h  € h¸  h  èüÿÿÿ‹L$$ƒÄéÑX  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄévX  ‰L$h   h¸  h¼  èüÿÿÿ‹L$$ƒÄéLX  ‰L$h   h¸  hè  èüÿÿÿ‹L$$ƒÄéaX  ‰L$h   h¸  hd  èüÿÿÿ‹L$$ƒÄé"X  ‰L$h   h¸  h  èüÿÿÿ‹L$$ƒÄé7X  ‰L$jh¸  h@  èüÿÿÿ‹L$$ƒÄé‚X  ‰L$h   h¸  ht  èüÿÿÿ‹L$$ƒÄéì[  ‰L$h   h¸  hH  èüÿÿÿ‹L$$ƒÄéæX  ‰L$jh¸  hğ  èüÿÿÿ‹L$$ƒÄéÄ[  ‰L$jh¸  h˜  èüÿÿÿ‹L$$ƒÄéu[  ‰L$jh¸  hl  èüÿÿÿ‹L$$ƒÄé—X  VUh¸  h  èüÿÿÿƒÄéZ  ‰T$0‰L$,VUh¸  h  èüÿÿÿ‹L$<‹T$@ƒÄƒ=„  †öW  Rÿt$ÿt$h¸  hP  èüÿÿÿ‹L$@ƒÄƒ=„  †öW  ‰L$ÿt$ ÿt$ÿt$h¸  h”  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$$ÿt$Q‰L$h¸  hÜ  èüÿÿÿ‹L$ ƒÄƒ=„  †öW  ÿt$(ÿt$ ÿt$h¸  h   èüÿÿÿ‹L$ ƒÄéöW  †¼  PhÌ  èüÿÿÿYXé¬\  Æ¼  Vhy  èüÿÿÿXZéÒ\  †¼  Phü  èüÿÿÿYXé.]  Æ¼  Vhy  èüÿÿÿXZéV]  †¼  Phy  èüÿÿÿX1ÀZé]  †¼  Ph4  èüÿÿÿYXé°]  †¼  Phl  èüÿÿÿXZéK^  jh€  h€  h×  èüÿÿÿƒÄé³d  Ph”  h”  hœ  èüÿÿÿƒÄéû`  hl  hl  h]  èüÿÿÿƒÄ»h  u  u‹ƒÜ    ‹ƒâï‰‰ØèLE  ‹ƒ¸  iĞ  ‹²   ‰³€  ¶Š    ˆ‹„  ˆL$‹Š$   ‰L$‰‹ˆ  ‹Š   Â    ƒ=„   ‰‹Œ  ¶R!ˆ“  tVÿt$¶D$PVh”  h”  h”  èüÿÿÿƒÄƒ=„   t%¶ƒ  Pÿ³Œ  h”  h”  hÜ  èüÿÿÿƒÄ‹ƒ¸  iÀ  ƒ¸   …(  ‰Øèüÿÿÿƒ=„   th\  h\  h]  èüÿÿÿƒÄ‰Øè¼F  ‹ƒÜ     ƒÈÿ‰‹‹Ü   ‰$  ‹‹Ü   ‰4  ‹‹Ü   ‰D  ‹‹Ü   ‰T  ‹‹Ü   ‰d  ‹ƒÜ   H  ‹â   €‰º P ‹ƒÜ   ‰L  ‰ØèLE  ¸d   èüÿÿÿ‹“d  ¹@  ‰Øj èüÿÿÿ‹ƒd  ¹€   j P,‰Øèüÿÿÿ‹ƒd  ¹ğ  j PX‰Øèüÿÿÿ‹ƒd  ¹€   j „   ‰Øèüÿÿÿ‹ƒd  ¹€   j °   ‰Øèüÿÿÿ‹ƒd  ¹ğ  j Ü   ‰Øèüÿÿÿ‹ƒd  ¹€   j   ‰Øèüÿÿÿ‹ƒd  ¹€   j 4  ‰Øèüÿÿÿ‹ƒd  ¹€   ƒÄ j `  ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øè¬A  ‰ØèLE  ƒô   èüÿÿÿƒ|  èüÿÿÿƒ
  èüÿÿÿ‰Øèüÿÿÿ‹Cp´XC‰D$FL;D$„Ÿ   ‹Fd‹@…Àt‹…Òt‰ğèüÿÿÿ‹vLƒîLëÔjh€  h€  h×  èüÿÿÿƒÄéÿe  ƒ¸   „ËıÿÿéÍıÿÿ‹ƒÌ   ¾êÿÿÿÿ°T  ÿ°P  ‹|$WhÜ  èüÿÿÿ‹ƒÌ   ·P&R·@$PWh  èüÿÿÿƒ-€  ƒÄ éx_  ‰Øèüÿÿÿƒ»¸  7u1Éº   ‰Øèüÿÿÿ‹ƒ¸  iĞ  ƒº   u"‰Øèüÿÿÿ…Àyh”  h  èüÿÿÿY^‹ƒ¸  iĞ  ‹Š   Â    ƒù…ù  ‹B…Àt‰ƒì  ƒÜ  èüÿÿÿ…À‰ş  h”  h\  èüÿÿÿX‹ƒ¸  ZiÀ  ‹       ƒú…Ø  ‹@…Àt‰ƒ0  ƒ   èüÿÿÿ…Àyh”  hÌ  èüÿÿÿXZ‹ƒÜ   ‹  ¶Â€ú„Á  €âğuCƒø„Ü  ƒøt5ƒø„ì  w*ƒø„’  ƒøtƒø„¢  w…À„¢  ƒè„£  ÿ³ì   h@  h4  èüÿÿÿ‹ƒì   ƒÄ…À„c  Ph@  hh  èüÿÿÿƒÄ‹ƒ¸  iĞ  ƒº   ~‹ƒÜ    ‹€æş‰‹ƒ¸  ƒø„:  ƒø#„1  ¶G,‹Ñ   º   ˆƒĞ   ‰øèüÿÿÿ¶ƒÑ   ‹—P  ‹T  ¶³Ğ   ‰D$‹‡L  ‰D$‹‡´   …À„  QRÿt$ÿt$ VPÿt$(hÄ  èüÿÿÿ‰øƒÄ èüÿÿÿ‹D$ƒÊÿ1Éèüÿÿÿ‰Æ…À„à   ÿt$h   èüÿÿÿXZéx_  ƒù…5şÿÿ‰Øèüÿÿÿ…Àyh”  h˜  èüÿÿÿY^‹ƒ¸  éşÿÿƒú…Lşÿÿ‰Øèüÿÿÿ…À‰=şÿÿh”  h   èüÿÿÿY^é'şÿÿ¸±   ‰ƒì   éşÿÿj h@  h”  èüÿÿÿƒÄé—şÿÿ¸À   ëÕ‹ƒÜ    ‹€æş‰é¸şÿÿ‹ƒÌ   ºĞ   fx"€ˆ¸¥   DÂë£‹‡ˆ   éäşÿÿ‹‡L  S1ÉºĞV  ÿt$h€   èüÿÿÿƒÄ…Ày+‰D$ÿ·L  ÿt$h@   èüÿÿÿ‰ØèœC  ‹t$ƒÄéx_  ‹ƒ¸  ƒøtƒøuº  € ‰Øèüÿÿÿëº  €‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿé_  ‹ƒÌ   ºÀ   fx"€ˆ¸¤   DÂéïşÿÿ¸°   éåşÿÿ¸    éÛşÿÿ¸¡   éÑşÿÿh°	  h°	  h¸   èüÿÿÿ1ÀƒÄéØh  ‰$…ÉtXh°	  hk  èüÿÿÿ·EZY‹$f…À…0h  1ÀéØh  ·E‰L$‰$Ph°	  h°	  h”   èüÿÿÿ‹T$‹L$ƒÄéh  ·E ÀƒÀPh°	  h  èüÿÿÿ·E‹T$ƒÄëš‹B‰$¶0Ph°	  h  èüÿÿÿ‹T$ƒÄ€ã…`h  ƒ=Œ   „`h  h°	  h¶  èüÿÿÿY[‹$é`h  jûhÍ  èüÿÿÿX¸ûÿÿÿZéØh  h 	  h 	  h¸   èüÿÿÿ1ÀƒÄé?k  jûhÍ  èüÿÿÿX¸ûÿÿÿZé?k  ‰$ÿt$Qh 	  h 	  hÜ   èüÿÿÿ·M‹T$ƒÄé¢i  ‰$Qh 	  h 	  h”   èüÿÿÿ·M‹T$ƒÄé¢i  ‹E‰T$¶ P·E ÀPhâ  èüÿÿÿ‹T$ƒÄö$…9j  ‰$hò  èüÿÿÿX‹$é9j  ‹B‰$¶8Phø  èüÿÿÿ€ãXZ‹$…¾j  hò  èüÿÿÿ[‹$é¾j  QhÀ	  hÀ	  h!  èüÿÿÿƒÄéÜk  ·B‰L$P·‰T$PVhÀ	  hÀ	  h8!  èüÿÿÿ‹T$‹L$ƒÄé5l  ¾¼  ÿs1öWh	  èüÿÿÿƒÄj‹D$1Éf‰³z  T$èüÿÿÿY…ÀyEƒÆş€   uØ‹ƒl  …À…ãm  é©m  ¾¼  ÿsWhĞ	  h˜!  èüÿÿÿƒÄƒ=ˆ   tÈë‹‹µ 
  …ÀtPVWhÈ!  èüÿÿÿƒÄë¸ 	  ëçÆ¼  ÿsVh "  èüÿÿÿƒÄë‰¸	  èüÿÿÿÿshĞ	  hĞ	  ht!  èüÿÿÿ‹ƒÄé·l  ¸˜	  èüÿÿÿR‰T$h€	  h€	  h,"  èüÿÿÿ‹T$ƒÄéQn  hˆ  h 	  èüÿÿÿXZé­o  hp  h 	  èüÿÿÿXZéıo  hX  h 	  èüÿÿÿXZé-p  h@  h 	  èüÿÿÿXZé}p  èüÿÿÿUº   W¿êÿÿÿVS‰ÃìL  ‹3d¡    ‰„$H  1ÀCèüÿÿÿ…À„g  ‹–¸  ƒú„Ô   wVƒútzƒú…½   †ü   º  èüÿÿÿ‹SK@BÿkÀ‹°  ‰s@f‹€”  f‰CDQRhx"  èüÿÿÿ‰ØèüÿÿÿƒÄëuƒú0wƒú-‡  „”   ë_ƒê11ÿƒú†„   é×  ƒ{uF¹   T$H†l  fÇ†n  P èüÿÿÿ„$è   Ph°"  èüÿÿÿ‹„$ğ   ‰C@‹„$ô   f‰CDY[1ÿé„  S(€  ‰4$1ÿ‰T$‹S‰D$‰àÇD$u  èüÿÿÿéX  ƒ{wÇ¹   T$H†l  fÇ†n  P èüÿÿÿ‹C”Ä   RPi†¸    ÿ°    hĞ"  èüÿÿÿ‹C„Ä  ƒÄ‹f‹@‰S@f‰CDéeÿÿÿl$C(¹   ‰\$‰$‰ï1Àº·	  ó«¹   ‰è¿íÿÿÿÇD$s  èüÿÿÿfÇD$&@ ‰d$,Ujèüÿÿÿ‰ê†ü   èüÿÿÿ‰ÅXZ…í„   ı ğÿÿ‡   ‹ET…Àtz‹@èüÿÿÿ„Àu	‰èèüÿÿÿëe1ÿƒ{‰«8  uW¹   T$H†l  fÇ†n  P èüÿÿÿ„$  Pi†¸    ÿ°    hø"  èüÿÿÿ‹„$  ‰C@‹„$  ƒÄf‰CD‹„$H  d+    tèüÿÿÿÄL  ‰ø[^_]ÃèüÿÿÿUº   WVSì  ‰D$d¡    ‰„$   ‹D$ÇD$    ÇD$    ‹ ‰$‹D$ƒÀ‰D$èüÿÿÿ‰Å¸êÿÿÿ…í„›A  ‹$¼  ‰E‰D$‹D$Ç€”       Ç€  p  ‹$‹€¸  ƒø„ğ  ‡â   ƒøwvƒø‡Ü  ƒø„
  w9ƒø„ó  wƒø„ì  ƒø„…  é<  ƒø„Ã  ƒø„­  éş;  ƒø
„#  ƒø„=  ƒø†×
  ƒø	„Y  éÕ;  ƒøw2ƒø‡  ƒø„õ  ƒø„X  ƒø„t  ƒø„í  é;  ƒø„  wƒø„ì  ƒø„;  é|;  ƒø„û  éø  ƒø)w}ƒø'‡¸  ƒø!„İ  w0ƒø„t  wƒø„Ï  ƒø„†  é3;  ƒø„å  é%;  ƒø$„~  wƒø"„”  ƒø#„g  é;  ƒø%„D   ƒø&„#!  éì:  ƒø2„û*  wBƒø/„¯)  w ƒø-„R%  ƒø.„/(  ƒø,„  é¶:  ƒø0„(  ƒø1„)%  éŸ:  ƒø5„˜#  wƒø3„ª,  ƒø4„$.  é}:  ƒø:„9  wƒø8„Ã1  ƒø9…`:  é˜5  ƒø;„ô-  ƒø<„£1  ƒø=…@:  éx5  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét7‹$°ü   ¸V  ‰òèüÿÿÿ‰Ã…À…3  ¸»	  1öèüÿÿÿÇE    éê9  h#  1ö1ÛèüÿÿÿÇE    XéÏ9  ¸
  èüÿÿÿ‰Á…Àuh
  jèüÿÿÿ¸
  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ÀS  èüÿÿÿ‰Ã…Àu3¸
  1öèüÿÿÿÇE    ép9  hx#  1ö1ÛèüÿÿÿÇE    XéU9  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…Àt,‹$º`   ˆ„  ‹EhäS  èüÿÿÿ[…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿY‹<$‹Eƒ¿¸  t	1ö1ÛéÕ8  ‹Ô   ‹|$1ö1Û‰—<  Ç€Ô   s  é²8  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸pU  èüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    éS8  hØ#  1ö1ÛèüÿÿÿÇE    Xé88  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ_‰ÃX…Ût,‹$º`   ˆ„  ‹EhT  èüÿÿÿ^…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿ[‹$hT  º`   1ö1Ûˆ„  ø  èüÿÿÿYé©7  ƒ=   ¸»	  …Ñ  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ÔU  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    é=7  h#  1ö1ÛèüÿÿÿÇE    _é"7  ‰E¸v
  èüÿÿÿ‰Ã…Àuh…
  jèüÿÿÿ¸v
  èüÿÿÿY^‰Ã…Àt,‹$¹B   „  ‹Eh<U  èüÿÿÿZ…Àu¸v
  èüÿÿÿëh$  èüÿÿÿX¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…À„®   ‹$ˆ„  ‹EhT  º`   èüÿÿÿ[…À…†ıÿÿ¸*
  1ö1ÛèüÿÿÿéQ6  ‰E¸v
  èüÿÿÿ‰Ã…À„˜  ‹$¹B   „  ‹Eh<U  èüÿÿÿZ…Àu
¸v
  èüÿÿÿ¸*
  èüÿÿÿ‰Ã…À…Œ  h:
  jèüÿÿÿ¸*
  èüÿÿÿY^‰Ã…À…j  h¨#  1ö1ÛèüÿÿÿYéÆ5  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét7‹$°ü   ¸V  ‰òèüÿÿÿ‰Ã…À…¯   ¸»	  1öèüÿÿÿÇE    éf5  h#  1ö1ÛèüÿÿÿÇE    _éK5  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ[^‰Á…Àt3‹$°ü   ¸ÈU  ‰òèüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    éê4  h#  1ö1ÛèüÿÿÿÇE    YéÏ4  ‰]¸à	  èüÿÿÿ‰Ã…Àuhî	  jèüÿÿÿ¸à	  èüÿÿÿ_‰ÃX…Ût-‹E¹¸U  j ‰òèüÿÿÿ[…À…¦ûÿÿ¸à	  1ö1Ûèüÿÿÿéq4  hH#  1ö1ÛèüÿÿÿYé]4  ¸›
  èüÿÿÿ‰Ã…Àuh«
  jèüÿÿÿ¸›
  èüÿÿÿ‰ÃXZ…Ût8‹$º   °ü   ¸ U  ‰ñèüÿÿÿ‰Ã…Àu3¸›
  1öèüÿÿÿÇE    é÷3  h8$  1ö1ÛèüÿÿÿÇE    XéÜ3  ‰E¸Â
  èüÿÿÿ‰Ã…ÀuhÖ
  jèüÿÿÿ¸Â
  èüÿÿÿ_‰ÃX…Ût-‹E‰òj@¹a   èüÿÿÿ^‰Ã…À…±úÿÿ¸Â
  1öèüÿÿÿé~3  hh$  1ö1ÛèüÿÿÿYéj3  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸”U  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    é3  h#  1ö1ÛèüÿÿÿÇE    Xéğ2  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àtj‹$¹\U  „  ‹Eèüÿÿÿ…À…Ãùÿÿ¸ñ
  1ö1Ûèüÿÿÿé2  ‰E¸ñ
  èüÿÿÿ‰Ã…À…Š  hÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ‰ÃXZ…Û…h  hœ$  1ö1ÛèüÿÿÿYéA2  ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸¼U  èüÿÿÿ‰Ã…Àu3¸»	  1öèüÿÿÿÇE    éâ1  h#  1ö1ÛèüÿÿÿÇE    _éÇ1  ‰E‹$Ç„$Œ       „  Æ„$Œ   a‰„$ˆ   ¸  Ç„$       èüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿYë‹ˆØ  …Ét
ºÔN  èüÿÿÿ1ö1Ûé 1  ¸7  èüÿÿÿ‰Á…ÀuhG  jèüÿÿÿ¸7  èüÿÿÿ‰ÁXZ…Ét5‹$ü   ¸ôU  èüÿÿÿ‰Ã…À…„úÿÿ¸7  1öèüÿÿÿÇE    é½0  hü$  1ö1ÛèüÿÿÿÇE    _é¢0  h…
  jèüÿÿÿ¸v
  èüÿÿÿY^‰Ã…À…Fúÿÿh$  èüÿÿÿ_é`úÿÿ‹$ˆ„  ‹EhtT  éãùÿÿ¸7  èüÿÿÿ‰Á…ÀuhG  jèüÿÿÿ¸7  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸àU  èüÿÿÿ‰Ã…Àu3¸7  1öèüÿÿÿÇE    éû/  hü$  1ö1ÛèüÿÿÿÇE    Xéà/  ‰E¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿ^_‰Ã…À„Ñùÿÿ‹$ˆ„  ‹EhXT  éùÿÿ¸^  èüÿÿÿ‰Â…Àuhn  jèüÿÿÿ¸^  èüÿÿÿ‰ÂXY…Òtl„$ˆ   èüÿÿÿ…À…Ì0  ¸^  èüÿÿÿ¸íÿÿÿé74  ‹D$‹@‰D$Xÿ¸^  èüÿÿÿ‰Â…À…Š  hn  jèüÿÿÿ¸^  èüÿÿÿ^_‰Â…À…h  h,%  èüÿÿÿ¸íÿÿÿ_éŞ3  ‹D$‹@‰D$pÿ¸»	  èüÿÿÿ‰Á…ÀuhÊ	  jèüÿÿÿ¸»	  èüÿÿÿ‰ÁXZ…Ét.iÆˆ  ‹<$”ü   ¸ˆU  èüÿÿÿ…À…úûÿÿ¸»	  èüÿÿÿëh#  èüÿÿÿXÇE    ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ[_‰Á…Àt8iÆˆ  ‹<$”ü   ¸|U  èüÿÿÿ…À…Œûÿÿ¸Q
  èüÿÿÿÇE    é&õÿÿhØ#  èüÿÿÿÇE    Yéõÿÿiöˆ  ‹$¹HU  ”0ü   ‹Eèüÿÿÿ…À…êôÿÿé"ûÿÿ‹D$‹@‰D$Xÿ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ^_‰Á…Àt:iÛˆ  ‹$´ü   ¸S  ‰òèüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    éM-  h\%  1ö1ÛèüÿÿÿÇE    Yé2-  ‰E¸  Ç„$Œ       Ç„$       ‰´$ˆ   Æ„$Œ   aèüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿXë‹ˆØ  …Ét
ºœN  èüÿÿÿ1ö1Ûé“,  „$ˆ   èüÿÿÿ…À…a0  é'ıÿÿ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ[^‰Á…Àt1‹$ü   ¸S  èüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    é,  h\%  1ö1ÛèüÿÿÿÇE    Yé ,  ‰E‹$Ç„$Œ       „  Æ„$Œ   a‰„$ˆ   ¸  Ç„$       èüÿÿÿ‰Á…Àuh"  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét‹E”$ˆ   èüÿÿÿ…Àu¸  èüÿÿÿë!hÌ$  èüÿÿÿ_ë‹ˆØ  …Ét
º€N  èüÿÿÿ1ö1ÛéY+  ¸…  èüÿÿÿ‰Á…Àuh”  jèüÿÿÿ¸…  èüÿÿÿ[^‰Á…Àt1‹$ü   ¸S  èüÿÿÿ‰Ã…Àu3¸…  1öèüÿÿÿÇE    éú*  h\%  1ö1ÛèüÿÿÿÇE    Yéß*  ‰E¸Ï  Ç„$ˆ   a †Ç„$Œ   Ğ  èüÿÿÿ‰Ã…Àuhİ  jèüÿÿÿ¸Ï  èüÿÿÿ‰ÃXZ…Ût,‹$Œ$ˆ   „  ‹Eèüÿÿÿ…À…;.  ¸Ï  èüÿÿÿëhè%  èüÿÿÿXÿt$h&  èüÿÿÿ_]é+  ¸ò  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ò  èüÿÿÿ[^‰Á…Àt1‹$„  ¸R  èüÿÿÿ‰Ã…Àu3¸ò  1öèüÿÿÿÇE    éí)  h@&  1ö1ÛèüÿÿÿÇE    YéÒ)  ‰E1ö1ÛÇ€      é¼)  ‹D$‹@ƒøtƒøtu1ö1Ûé¢)  ¸  èüÿÿÿ‰Á…Àuh&  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét%‹$„  ¸ŒR  èüÿÿÿ‰Ã…Àu¸  èüÿÿÿëhp&  1Ûèüÿÿÿ_‰]1ö1Ûé6)  ¸  èüÿÿÿ‰Á…Àuh&  jèüÿÿÿ¸  èüÿÿÿ[^‰Á…Àt%‹$„  ¸ŒR  èüÿÿÿ‰Ã…Àu¸  èüÿÿÿëhp&  1ÛèüÿÿÿY‰]1ö1ÛéÊ(  ¸<  èüÿÿÿ‰Á…ÀuhJ  jèüÿÿÿ¸<  èüÿÿÿ‰ÁXZ…Ét3‹$°„  ¸€R  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éi(  h &  1ö1ÛèüÿÿÿÇE    XéN(  ‰E¸_  èüÿÿÿ‰Ã…Àuhm  jèüÿÿÿ¸_  èüÿÿÿ_‰ÃX…Ût‹E‰ñºhR  èüÿÿÿ…Àu¸_  èüÿÿÿëhĞ&  èüÿÿÿY‹E1ö1ÛÇ€      éİ'  ¸ò  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ò  èüÿÿÿ‰ÁXZ…Ét%‹$„  ¸XR  èüÿÿÿ‰Ã…Àu¸ò  èüÿÿÿëh@&  1ÛèüÿÿÿX‰]1ö1Ûéq'  ‹D$‹@ƒøtƒø„+  é¬ıÿÿ¸‚  èüÿÿÿ‰Ã…Àuh‘  jèüÿÿÿ¸‚  èüÿÿÿ^_‰Ã…Àt5‹$1É°ü   ¸ÀR  ‰òèüÿÿÿ‰Ã…Àu3¸‚  1öèüÿÿÿÇE    éô&  h '  1ö1ÛèüÿÿÿÇE    YéÙ&  ‰E¸§  èüÿÿÿ‰Ã…Àuh¶  jèüÿÿÿ¸§  èüÿÿÿ‰ÃXZ…Ûtc‹E‰ñº¬R  èüÿÿÿ…À…È(  ¸§  1ö1Ûèüÿÿÿé~&  ‰E¸§  èüÿÿÿ‰Ã…À…»   h¶  jèüÿÿÿ¸§  èüÿÿÿ‰ÃXZ…Û…™   h0'  1ö1ÛèüÿÿÿXé1&  ¸‚  èüÿÿÿ‰Ã…Àuh‘  jèüÿÿÿ¸‚  èüÿÿÿ^_‰Ã…Àt<‹$¹   °ü   ¸ÀR  ‰òèüÿÿÿ‰Ã…À…aÿÿÿ¸‚  1öèüÿÿÿÇE    éÇ%  h '  1ö1ÛèüÿÿÿÇE    Yé¬%  ‹E‰ñº R  èüÿÿÿ…À…#(  éÿşÿÿ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸@R  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é1%  h'  1ö1ÛèüÿÿÿÇE    Xé%  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹,R  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  é$  ¸1  èüÿÿÿ‰Á…ÀuhA  jèüÿÿÿ¸1  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸$R  èüÿÿÿ‰Ã…Àu3¸1  1öèüÿÿÿÇE    é/$  hÀ'  1ö1ÛèüÿÿÿÇE    Xé$  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹R  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  éŒ#  ¸  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸Q  èüÿÿÿ‰Ã…Àu3¸  1öèüÿÿÿÇE    é-#  h'  1ö1ÛèüÿÿÿÇE    Xé#  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^_‰Ã…Àt&‹$¹ Q  „  ‹Eèüÿÿÿ…Àu¸ñ
  èüÿÿÿëhœ$  èüÿÿÿY‹E‹|$1ö1Û‹Ô   ‰—<  Ç€Ô   s  éŠ"  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸pU  ‰òèüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    é)"  hØ#  1ö1ÛèüÿÿÿÇE    Xé"  ‰E¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿ_‰ÃX…Ût%‹E‰ñhT  º`   èüÿÿÿ^…Àu¸*
  èüÿÿÿëh¨#  èüÿÿÿ[‹$hT  º`   1ö1Ûˆ„  ø  èüÿÿÿYé†!  ¸Q
  èüÿÿÿ‰Á…Àuh`
  jèüÿÿÿ¸Q
  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸pU  ‰òèüÿÿÿ‰Ã…Àu3¸Q
  1öèüÿÿÿÇE    é%!  hØ#  1ö1ÛèüÿÿÿÇE    Xé
!  ‰E¸*
  èüÿÿÿ‰Ã…Àu"h:
  jèüÿÿÿ¸*
  èüÿÿÿY_‰Ã…À„ûêÿÿ‹E‰ñhT  éOêÿÿ‹D$‹@ƒøtƒø„)  é÷ÿÿ¸X  èüÿÿÿ‰Á…Àuhh  jèüÿÿÿ¸X  èüÿÿÿ‰ÁXZ…Ét3‹$°ü   ¸ìP  ‰òèüÿÿÿ‰Ã…Àu3¸X  1öèüÿÿÿÇE    éK   hğ'  1ö1ÛèüÿÿÿÇE    Xé0   ‰E¸  èüÿÿÿ‰Ã…Àuh  jèüÿÿÿ¸  èüÿÿÿ_‰ÃX…Ûtc‹E¹èP  ‰òèüÿÿÿ…À…
çÿÿ¸  1ö1ÛèüÿÿÿéÕ  ‰E¸  èüÿÿÿ‰Ã…À…¶   h  jèüÿÿÿ¸  èüÿÿÿY_‰Ã…À…”   h (  1ö1ÛèüÿÿÿYéˆ  ¸X  èüÿÿÿ‰Á…Àuhh  jèüÿÿÿ¸X  èüÿÿÿ‰ÁXZ…Ét7‹$°„  ¸ÔP  ‰òèüÿÿÿ‰Ã…À…fÿÿÿ¸X  1öèüÿÿÿÇE    é#  hğ'  1ö1ÛèüÿÿÿÇE    Xé  ‹E¹ĞP  ‰òèüÿÿÿ…À…æÿÿéÿÿÿ‹D$ƒx‡  ¸¤  èüÿÿÿ‰Á…Àuh¶  jèüÿÿÿ¸¤  èüÿÿÿ‰ÁXZ…ÉtE‹$¸ü   ‹D$‰ú‹@‰D$HkÀ P  èüÿÿÿ‰Ã…Àu9¸¤  ¾   èüÿÿÿÇE    ék  hP(  ¾   1ÛèüÿÿÿÇE    XéM  ‰E¸ñ
  èüÿÿÿ‰Ã…Àuhÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿ^‰ÃX…Ût{‹D$‰ú‹@Hÿ‰D$‹EkÉÁ`P  èüÿÿÿ…À…Õ   ¸ñ
  èüÿÿÿé²  ‰CÇ€     ¸ñ
  èüÿÿÿ‰Æ…À…S!  hÿ
  jèüÿÿÿ¸ñ
  èüÿÿÿY‰ÆX…ö…1!  hœ$  èüÿÿÿYé_  ‹D$‹@ƒøtƒø„&  éÌóÿÿ¸ú  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ú  èüÿÿÿ‰ÁXZ…Ét1‹$ü   ¸ P  èüÿÿÿ‰Ã…Àu3¸ú  1öèüÿÿÿÇE    é  h¸(  1ö1ÛèüÿÿÿÇE    Xéı  ‰E¸  èüÿÿÿ‰Ã…Àuh'  jèüÿÿÿ¸  èüÿÿÿ^_‰Ã…Àtf‹$º P  ˆ„  ‹Eèüÿÿÿ…À…Ğãÿÿ¸  èüÿÿÿém  ‰E¸  èüÿÿÿ‰Ã…À…°   h'  jèüÿÿÿ¸  èüÿÿÿ_‰ÃX…Û…   hä(  èüÿÿÿYé$  ¸ú  èüÿÿÿ‰Á…Àuh  jèüÿÿÿ¸ú  èüÿÿÿ‰ÁXZ…Ét5‹$ü   ¸8P  èüÿÿÿ‰Ã…À…lÿÿÿ¸ú  1öèüÿÿÿÇE    éó  h¸(  1ö1ÛèüÿÿÿÇE    XéØ  ‹$ºP  ˆ„  ‹Eèüÿÿÿ…À…Üâÿÿéÿÿÿ¸<  èüÿÿÿ‰Á…ÀuhJ  jèüÿÿÿ¸<  èüÿÿÿ[^‰Á…Àt3‹$°„  ¸€R  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éT  h &  1ö1ÛèüÿÿÿÇE    Yé9  ‰E¸_  èüÿÿÿ‰Ã…Àuhm  jèüÿÿÿ¸_  èüÿÿÿ‰ÃXZ…Ût*‹E‰ñºhR  èüÿÿÿ‰Ã…À…âÿÿ¸_  1öèüÿÿÿéŞ  hĞ&  1ö1ÛèüÿÿÿXéÊ  ¸<  èüÿÿÿ‰Ã…ÀuhK  jèüÿÿÿ¸<  èüÿÿÿ^_‰Ã…Àt8‹$¹   °ü   ¸ Q  ‰òèüÿÿÿ‰Ã…Àu3¸<  1öèüÿÿÿÇE    éd  h)  1ö1ÛèüÿÿÿÇE    YéI  ‰E¸a  èüÿÿÿ‰Ã…Àuhp  jèüÿÿÿ¸a  èüÿÿÿ‰ÃXZ…Ût&‹E‰ñº€Q  èüÿÿÿ…À…Í  ¸a  èüÿÿÿéÀ  hD)  èüÿÿÿXé°  ‹D$Ç„$ˆ   ` iÇ„$Œ   @   ‹@ÇD$d    Ç„$   T  Ç„$”       ƒøtƒø„J  éy  œ$È   1À¹   º†  ‰ßó«¹   ‰Øèüÿÿÿ„$ˆ   fÇ„$Ş    ‰„$ä   h—  jèüÿÿÿ‹D$‰Ú°ü   ‰ğèüÿÿÿY‰ÇX…ÿ„ü  ÿ ğÿÿ‡ğ  ‹GT…À„å  ‹@èüÿÿÿ„À‰øu
èüÿÿÿéÍ  ‹”$”   èüÿÿÿ¹   º   ‰E‹D$‰¸,  ‹E‰ß‰D$d1Àó«¹   ‰ØèüÿÿÿfÇ„$Ş    D$d‰„$ä   h   jèüÿÿÿ‰Ú‰ğèüÿÿÿ‰ÃXZ…Û„[  û ğÿÿ‡O  ‹CT…À„D  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿé,  ‹D$‰˜4  é4  T$@1Àt$<¹   œ$È   ‰×Uó«‰ß¹   ‰T$<ó«º¦  ‰Ø¹   ÆD$@!ÇD$D $ô èüÿÿÿfÇ„$Ş   d ‰´$ä   Sjèüÿÿÿ‹D$‰Úü   èüÿÿÿ_‰ÃX…Û„   û ğÿÿ‡”  ‹CT…À„‰  ‹@èüÿÿÿ„Àu‰Øèüÿÿÿéq  ‹D$‰˜,  ‹E…ÀtwÇ€      ¸*
  èüÿÿÿ‰Ã…Àuh:
  jèüÿÿÿ¸*
  èüÿÿÿY^‰Ã…Àt/‹$º`   ˆ„  ‹Eh<T  èüÿÿÿZ…Àu¸*
  èüÿÿÿéù  h¨#  èüÿÿÿ]éé  1ö1Ûé  œ$È   1À¹   º†  ‰ßÇ„$ˆ   ` ió«¹   ‰ØÇ„$Œ   @   Ç„$   T  Ç„$”       ÇD$d    èüÿÿÿ„$ˆ   f.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/kobject_ns.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/linux/fwnode.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/property.h \
  include/linux/resource_ext.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/sound/core.h \
    $(wildcard include/config/SND_DYNAMIC_MINORS) \
    $(wildcard include/config/SND_MAX_CARDS) \
    $(wildcard include/config/SND_MAJOR) \
    $(wildcard include/config/SND_DEBUG) \
    $(wildcard include/config/SND_MIXER_OSS) \
    $(wildcard include/config/SND_OSSEMUL) \
    $(wildcard include/config/ISA_DMA_API) \
    $(wildcard include/config/SND_VERBOSE_PRINTK) \
    $(wildcard include/config/SND_DEBUG_VERBOSE) \
    $(wildcard include/config/GAMEPORT) \
  include/sound/pcm.h \
    $(wildcard include/config/SND_PCM_OSS) \
    $(wildcard include/config/SND_VERBOSE_PROCFS) \
    $(wildcard include/config/SND_PCM_XRUN_DEBUG) \
  include/sound/asound.h \
  include/uapi/sound/asound.h \
  include/sound/memalloc.h \
    $(wildcard include/config/GENERIC_ALLOCATOR) \
    $(wildcard include/config/SND_DMA_SGBUF) \
    $(wildcard include/config/HAS_DMA) \
  include/linux/dma-direction.h \
  include/sound/minors.h \
  include/linux/pm_qos.h \
    $(wildcard include/config/CPU_IDLE) \
  include/sound/pcm_oss.h \
    $(wildcard include/config/SND_PCM_OSS_PLUGINS) \
  drivers/media/pci/cx18/cx18-driver.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/pci.h \
    $(wildcard include/config/PCI_IOV) \
    $(wildcard include/config/PCIEAER) \
    $(wildcard include/config/PCIEPORTBUS) \
    $(wildcard include/config/PCIEASPM) \
    $(wildcard include/config/HOTPLUG_PCI_PCIE) \
    $(wildcard include/config/PCIE_PTM) \
    $(wildcard include/config/PCIE_DPC) \
    $(wildcard include/config/PCI_ATS) \
    $(wildcard include/config/PCI_PRI) \
    $(wildcard include/config/PCI_PASID) \
    $(wildcard include/config/PCI_P2PDMA) \
    $(wildcard include/config/PCI_DOMAINS_GENERIC) \
    $(wildcard include/config/PCI_DOMAINS) \
    $(wildcard include/config/PCI_QUIRKS) \
    $(wildcard include/config/ACPI_MCFG) \
    $(wildcard include/config/HOTPLUG_PCI) \
    $(wildcard include/config/EEH) \
  include/uapi/linux/pci.h \
  include/uapi/linux/pci_regs.h \
  include/linux/pci_ids.h \
  include/linux/dmapool.h \
  arch/x86/include/asm/pci.h \
    $(wildcard include/config/PCI_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/VMD) \
  arch/x86/include/asm/memtype.h \
  include/asm-generic/pci.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/i2c-algo-bit.h \
  include/media/v4l2-ioctl.h \
  include/media/tuner.h \
  include/media/v4l2-mc.h \
  include/media/i2c/ir-kbd-i2c.h \
  include/media/rc-core.h \
    $(wildcard include/config/LIRC) \
  include/linux/kfifo.h \
  include/media/rc-map.h \
    $(wildcard include/config/IR_RC5_DECODER) \
    $(wildcard include/config/IR_JVC_DECODER) \
    $(wildcard include/config/IR_SONY_DECODER) \
    $(wildcard include/config/IR_NEC_DECODER) \
    $(wildcard include/config/IR_SANYO_DECODER) \
    $(wildcard include/config/IR_MCE_KBD_DECODER) \
    $(wildcard include/config/IR_RC6_DECODER) \
    $(wildcard include/config/IR_SHARP_DECODER) \
    $(wildcard include/config/IR_XMP_DECODER) \
    $(wildcard include/config/IR_IMON_DECODER) \
    $(wildcard include/config/IR_RCMM_DECODER) \
    $(wildcard include/config/MEDIA_CEC_RC) \
  include/linux/input.h \
  include/uapi/linux/input.h \
  include/uapi/linux/input-event-codes.h \
  include/uapi/linux/lirc.h \
  drivers/media/pci/cx18/cx18-mailbox.h \
  drivers/media/pci/cx18/cx18-av-core.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  drivers/media/pci/cx18/cx23418.h \
  include/media/drv-intf/cx2341x.h \
  include/media/demux.h \
  include/uapi/linux/dvb/dmx.h \
  include/media/dmxdev.h \
  include/media/dvbdev.h \
    $(wildcard include/config/DVB_MAX_ADAPTERS) \
    $(wildcard include/config/MEDIA_CONTROLLER_DVB) \
    $(wildcard include/config/MEDIA_ATTACH) \
  include/media/dvb_ringbuffer.h \
  include/media/dvb_vb2.h \
    $(wildcard include/config/DVB_MMAP) \
  include/media/videobuf2-core.h \
  include/linux/dma-buf.h \
    $(wildcard include/config/DMABUF_SYSFS_STATS) \
  include/linux/iosys-map.h \
  include/linux/file.h \
  include/linux/dma-fence.h \
  include/media/frame_vector.h \
  include/media/videobuf2-dma-contig.h \
  include/media/videobuf2-v4l2.h \
  include/media/videobuf2-vmalloc.h \
  include/media/dvb_demux.h \
  include/media/dvb_frontend.h \
  include/uapi/linux/dvb/frontend.h \
  include/media/dvb_net.h \
    $(wildcard include/config/DVB_NET) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/XDP_SOCKETS) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/IPV6) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/XFRM) \
    $(wildcard include/config/IP_VS) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/flow.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/uapi/linux/socket.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/net/flow_dissector.h \
  include/linux/siphash.h \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
  include/uapi/linux/if_ether.h \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
    $(wildcard include/config/MPTCP) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/skbuff.h \
    $(wildcard include/config/BRIDGE_NETFILTER) \
    $(wildcard include/config/NET_TC_SKB_EXT) \
    $(wildcard include/config/NET_SOCK_MSG) \
    $(wildcard include/config/SKB_EXTENSIONS) \
    $(wildcard include/config/IPV6_NDISC_NODETYPE) \
    $(wildcard include/config/NET_SWITCHDEV) \
    $(wildcard include/config/NET_REDIRECT) \
    $(wildcard include/config/NETFILTER_SKIP_EGRESS) \
    $(wildcard include/config/NET_RX_BUSY_POLL) \
    $(wildcard include/config/NETWORK_SECMARK) \
    $(wildcard include/config/PAGE_POOL) \
    $(wildcard include/config/NETWORK_PHY_TIMESTAMPING) \
    $(wildcard include/config/MCTP_FLOWS) \
    $(wildcard include/config/NETFILTER_XT_TARGET_TRACE) \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  arch/x86/include/asm/archrandom.h \
  include/linux/sockptr.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/inetdevice.h \
    $(wildcard include/config/INET) \
  include/linux/ip.h \
  include/uapi/linux/ip.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/media/videobuf-core.h \
  include/media/videobuf-vmalloc.h \
  drivers/media/pci/cx18/cx18-queue.h \
  drivers/media/pci/cx18/cx18-streams.h \
  drivers/media/pci/cx18/cx18-fileops.h \
  drivers/media/pci/cx18/cx18-alsa.h \
  drivers/media/pci/cx18/cx18-alsa-pcm.h \

drivers/media/pci/cx18/cx18-alsa-pcm.o: $(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o)

$(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   cmd_drivers/media/pci/cx18/cx18-alsa-pcm.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa-pcm.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -I ./drivers/media/dvb-frontends -I ./drivers/media/tuners  -DMODULE  -DKBUILD_BASENAME='"cx18_alsa_pcm"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa-pcm.o drivers/media/pci/cx18/cx18-alsa-pcm.c 

source_drivers/media/pci/cx18/cx18-alsa-pcm.o := drivers/media/pci/cx18/cx18-alsa-pcm.c

deps_drivers/media/pci/cx18/cx18-alsa-pcm.o := \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/init.h \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
    $(wildcard include/config/CFI_CLANG) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/linux/kstrtox.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/ratelimit_types.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  include/linux/instruction_pointer.h \
  include/media/v4l2-device.h \
    $(wildcard include/config/VIDEO_V4L2_SUBDEV_API) \
  include/media/media-device.h \
    $(wildcard include/config/MEDIA_CONTROLLER) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/mutex.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
    $(wildcard include/config/PREEMPT_COUNT) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  include/linux/preempt.h \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  include/linux/time64.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/vdso/math64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/COMPAT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/TRACEPOINTS) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  include/linux/osq_lock.h \
  include/linux/debug_locks.h \
  include/media/media-devnode.h \
  include/linux/poll.h \
  include/linux/ktime.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/tsc.h \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/wait.h \
  include/linux/spinlock.h \
  include/linux/bottom_half.h \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/uapi/linux/wait.h \
  include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
    $(wildcard include/config/MIGRATION) \
  include/linux/wait_bit.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  include/linux/rbtree_types.h \
  include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  include/generated/bounds.h \
  include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  include/linux/path.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/list_lru.h \
    $(wildcard include/config/MEMCG_KMEM) \
  include/linux/nodemask.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/NUMA) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/linux/shrinker.h \
    $(wildcard include/config/MEMCG) \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/gfp.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags-layout.h \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/kconfig.h \
  include/linux/radix-tree.h \
  include/linux/pid.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/uapi/linux/openat2.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcuwait.h \
  include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/STACK_GROWSUP) \
  include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
  include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  include/uapi/linux/signal.h \
  arch/x86/include/asm/signal.h \
  arch/x86/include/uapi/asm/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/x86/include/uapi/asm/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/ARCH_TASK_STRUCT_ON_STACK) \
    $(wildcard include/config/DEBUG_RSEQ) \
  include/uapi/linux/sched.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/rhashtable-types.h \
  include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/x86/include/uapi/asm/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  include/uapi/asm-generic/hugetlb_encode.h \
  arch/x86/include/uapi/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/x86/include/asm/shmparam.h \
  include/linux/plist.h \
    $(wildcard include/config/DEBUG_PLIST) \
  include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  include/linux/hrtimer_defs.h \
  include/linux/timerqueue.h \
  include/linux/seccomp.h \
    $(wildcard include/config/SECCOMP) \
    $(wildcard include/config/HAVE_ARCH_SECCOMP_FILTER) \
    $(wildcard include/config/SECCOMP_FILTER) \
    $(wildcard include/config/CHECKPOINT_RESTORE) \
    $(wildcard include/config/SECCOMP_CACHE_DEBUG) \
  include/uapi/linux/seccomp.h \
  arch/x86/include/asm/seccomp.h \
  arch/x86/include/asm/unistd.h \
  arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_32.h \
  include/asm-generic/seccomp.h \
  include/uapi/linux/unistd.h \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/latencytop.h \
  include/linux/sched/prio.h \
  include/linux/sched/types.h \
  include/linux/syscall_user_dispatch.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  include/linux/posix-timers.h \
  include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  include/uapi/linux/rseq.h \
  include/linux/kcsan.h \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/linux/sched/jobctl.h \
  include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  include/linux/uaccess.h \
  include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO_OUTPUT) \
    $(wildcard include/config/CC_HAS_ASM_GOTO_TIED_OUTPUT) \
    $(wildcard include/config/ARCH_HAS_COPY_MC) \
    $(wildcard include/config/X86_INTEL_USERCOPY) \
  arch/x86/include/asm/smap.h \
  arch/x86/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  arch/x86/include/asm/uaccess_32.h \
  include/linux/cred.h \
    $(wildcard include/config/DEBUG_CREDENTIALS) \
  include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
    $(wildcard include/config/SYSCTL) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  include/linux/sched/user.h \
    $(wildcard include/config/WATCH_QUEUE) \
  include/linux/percpu_counter.h \
  include/linux/ratelimit.h \
  include/linux/rcu_sync.h \
  include/linux/delayed_call.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/errseq.h \
  include/linux/ioprio.h \
  include/linux/sched/rt.h \
  include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  include/uapi/linux/ioprio.h \
  include/linux/fs_types.h \
  include/linux/mount.h \
  include/linux/mnt_idmapping.h \
  include/linux/slab.h \
    $(wildcard include/config/DEBUG_SLAB) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/SLAB) \
    $(wildcard include/config/SLUB) \
    $(wildcard include/config/SLOB) \
  include/linux/overflow.h \
  include/linux/percpu-refcount.h \
  include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
    $(wildcard include/config/KASAN_INLINE) \
  include/linux/kasan-enabled.h \
  include/uapi/linux/fs.h \
  include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/uapi/linux/poll.h \
  arch/x86/include/generated/uapi/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/uapi/linux/eventpoll.h \
  include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_DEVICE) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU) \
    $(wildcard include/config/ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) \
    $(wildcard include/config/DMA_OPS_BYPASS) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
    $(wildcard include/config/SYSFS_DEPRECATED) \
  include/linux/dev_printk.h \
  include/linux/energy_model.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/kobject_ns.h \
  include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_DEBUG) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  include/linux/sched/idle.h \
  include/linux/sched/sd_flags.h \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  include/linux/device/class.h \
  include/linux/device/driver.h \
  include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/cdev.h \
  include/media/media-entity.h \
  include/linux/fwnode.h \
  include/uapi/linux/media.h \
  include/media/v4l2-subdev.h \
    $(wildcard include/config/VIDEO_ADV_DEBUG) \
  include/uapi/linux/v4l2-subdev.h \
  include/uapi/linux/v4l2-common.h \
  include/uapi/linux/v4l2-mediabus.h \
  include/uapi/linux/media-bus-format.h \
  include/linux/videodev2.h \
  include/uapi/linux/videodev2.h \
  include/uapi/linux/v4l2-controls.h \
  include/media/v4l2-async.h \
  include/media/v4l2-common.h \
    $(wildcard include/config/VIDEO_V4L2_I2C) \
    $(wildcard include/config/SPI) \
  include/media/v4l2-dev.h \
  include/linux/i2c.h \
    $(wildcard include/config/I2C) \
    $(wildcard include/config/I2C_SLAVE) \
    $(wildcard include/config/I2C_BOARDINFO) \
    $(wildcard include/config/I2C_MUX) \
  include/linux/acpi.h \
    $(wildcard include/config/ACPI_DEBUGGER) \
    $(wildcard include/config/ACPI_TABLE_LIB) \
    $(wildcard include/config/LOONGARCH) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/ACPI_PROCESSOR_CSTATE) \
    $(wildcard include/config/ACPI_HOTPLUG_CPU) \
    $(wildcard include/config/ACPI_HOTPLUG_IOAPIC) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/ACPI_WMI) \
    $(wildcard include/config/ACPI_NUMA) \
    $(wildcard include/config/HIBERNATION) \
    $(wildcard include/config/ACPI_HOTPLUG_MEMORY) \
    $(wildcard include/config/ACPI_CONTAINER) \
    $(wildcard include/config/ACPI_GTDT) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ACPI_TABLE_UPGRADE) \
    $(wildcard include/config/ACPI_WATCHDOG) \
    $(wildcard include/config/ACPI_SPCR_TABLE) \
    $(wildcard include/config/ACPI_GENERIC_GSI) \
    $(wildcard include/config/ACPI_LPIT) \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ACPI_PCC) \
  include/linux/irqdomain.h \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
    $(wildcard include/config/IRQ_DOMAIN_NOMAP) \
  include/linux/irqhandler.h \
  include/linux/of.h \
    $(wildcard include/config/OF_DYNAMIC) \
    $(wildcard include/config/SPARC) \
    $(wildcard include/config/OF_PROMTREE) \
    $(wildcard include/config/OF_KOBJ) \
    $(wildcard include/config/OF_NUMA) \
    $(wildcard include/config/OF_OVERLAY) \
  include/linux/mod_devicetable.h \
  include/linux/property.h \
  include/linux/resource_ext.h \
  include/acpi/acpi.h \
  include/acpi/platform/acenv.h \
  include/acpi/platform/acgcc.h \
  include/acpi/platform/aclinux.h \
    $(wildcard include/config/ACPI_REDUCED_HARDWARE_ONLY) \
    $(wildcard include/config/ACPI_DEBUG) \
  include/linux/ctype.h \
  arch/x86/include/asm/acenv.h \
  include/acpi/acnames.h \
  include/acpi/actypes.h \
  include/acpi/acexcep.h \
  include/acpi/actbl.h \
  include/acpi/actbl1.h \
  include/acpi/actbl2.h \
  include/acpi/actbl3.h \
  include/acpi/acrestyp.h \
  include/acpi/platform/acenvex.h \
  include/acpi/platform/aclinuxex.h \
  include/acpi/platform/acgccex.h \
  include/acpi/acoutput.h \
  include/acpi/acpiosxf.h \
  include/acpi/acpixf.h \
  include/acpi/acconfig.h \
  include/acpi/acbuffer.h \
  include/linux/dynamic_debug.h \
  include/acpi/acpi_bus.h \
    $(wildcard include/config/X86_ANDROID_TABLETS) \
    $(wildcard include/config/ACPI_SYSTEM_POWER_STATES_SUPPORT) \
    $(wildcard include/config/ACPI_SLEEP) \
  include/acpi/acpi_drivers.h \
    $(wildcard include/config/ACPI_DOCK) \
  include/acpi/acpi_numa.h \
    $(wildcard include/config/ACPI_HMAT) \
  include/acpi/acpi_io.h \
  include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
  arch/x86/include/asm/io.h \
    $(wildcard include/config/MTRR) \
    $(wildcard include/config/X86_PAT) \
  arch/x86/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/GENERIC_EARLY_IOREMAP) \
  include/asm-generic/iomap.h \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/GENERIC_IOREMAP) \
    $(wildcard include/config/VIRT_TO_BUS) \
    $(wildcard include/config/GENERIC_DEVMEM_IS_ALLOWED) \
  include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  include/linux/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMALLOC) \
  arch/x86/include/asm/vmalloc.h \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
  arch/x86/include/asm/acpi.h \
    $(wildcard include/config/ACPI_APEI) \
  include/acpi/pdc_intel.h \
  arch/x86/include/asm/numa.h \
    $(wildcard include/config/NUMA_EMU) \
  arch/x86/include/asm/numa_32.h \
  include/linux/regulator/consumer.h \
    $(wildcard include/config/REGULATOR) \
  include/linux/suspend.h \
    $(wildcard include/config/VT) \
    $(wildcard include/config/SUSPEND) \
    $(wildcard include/config/HIBERNATION_SNAPSHOT_DEV) \
    $(wildcard include/config/PM_SLEEP_DEBUG) \
    $(wildcard include/config/PM_AUTOSLEEP) \
  include/linux/swap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/FRONTSWAP) \
    $(wildcard include/config/THP_SWAP) \
    $(wildcard include/config/MEMCG_SWAP) \
  include/linux/memcontrol.h \
  include/linux/cgroup.h \
    $(wildcard include/config/CGROUP_CPUACCT) \
    $(wildcard include/config/SOCK_CGROUP_DATA) \
    $(wildcard include/config/CGROUP_DATA) \
    $(wildcard include/config/CGROUP_BPF) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/seq_file.h \
  include/linux/string_helpers.h \
  include/linux/ns_common.h \
  include/linux/nsproxy.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  include/linux/kernel_stat.h \
  include/linux/interrupt.h \
    $(wildcard include/config/IRQ_FORCED_THREADING) \
    $(wildcard include/config/GENERIC_IRQ_PROBE) \
    $(wildcard include/config/IRQ_TIMINGS) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/KVM_INTEL) \
    $(wildcard include/config/X86_THERMAL_VECTOR) \
    $(wildcard include/config/X86_MCE_THRESHOLD) \
    $(wildcard include/config/X86_MCE_AMD) \
    $(wildcard include/config/X86_HV_CALLBACK_VECTOR) \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/sections.h \
  include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/CGROUP_NET_CLASSID) \
    $(wildcard include/config/CGROUP_NET_PRIO) \
  include/linux/u64_stats_sync.h \
  include/linux/bpf-cgroup-defs.h \
  include/linux/psi_types.h \
  include/linux/kthread.h \
  include/linux/cgroup_subsys.h \
    $(wildcard include/config/CGROUP_DEVICE) \
    $(wildcard include/config/CGROUP_FREEZER) \
    $(wildcard include/config/CGROUP_PERF) \
    $(wildcard include/config/CGROUP_HUGETLB) \
    $(wildcard include/config/CGROUP_PIDS) \
    $(wildcard include/config/CGROUP_RDMA) \
    $(wildcard include/config/CGROUP_MISC) \
    $(wildcard include/config/CGROUP_DEBUG) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/MEMORY_BALLOON) \
    $(wildcard include/config/BALLOON_COMPACTION) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/DEBUG_VM_VMACACHE) \
  include/linux/page_counter.h \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
  include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/ARCH_USES_HIGH_VMA_FLAGS) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/PPC) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_HAS_PTE_DEVMAP) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/ANON_VMA_NAME) \
  include/linux/mmap_lock.h \
  include/linux/page_ext.h \
  include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  include/linux/stackdepot.h \
    $(wildcard include/config/STACKDEPOT_ALWAYS_INIT) \
  include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  include/linux/sizes.h \
  include/linux/pgtable.h \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/GUP_GET_PTE_LOW_HIGH) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_ENABLE_THP_MIGRATION) \
    $(wildcard include/config/X86_ESPFIX64) \
  arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/DEBUG_WX) \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  arch/x86/include/asm/pkru.h \
  arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/X86_DEBUG_FPU) \
  arch/x86/include/asm/coco.h \
  include/asm-generic/pgtable_uffd.h \
  include/linux/page_table_check.h \
  arch/x86/include/asm/pgtable_32.h \
  arch/x86/include/asm/pgtable-3level.h \
  arch/x86/include/asm/pgtable-invert.h \
  include/linux/huge_mm.h \
  include/linux/sched/coredump.h \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/DEBUG_FS) \
  include/linux/blk_types.h \
    $(wildcard include/config/FAIL_MAKE_REQUEST) \
    $(wildcard include/config/BLK_CGROUP_IOCOST) \
    $(wildcard include/config/BLK_INLINE_ENCRYPTION) \
    $(wildcard include/config/BLK_DEV_INTEGRITY) \
  include/linux/bvec.h \
  include/linux/highmem.h \
  include/linux/cacheflush.h \
  arch/x86/include/asm/cacheflush.h \
  include/asm-generic/cacheflush.h \
  include/linux/highmem-internal.h \
  arch/x86/include/asm/highmem.h \
  arch/x86/include/asm/tlbflush.h \
  arch/x86/include/asm/invpcid.h \
  arch/x86/include/asm/pti.h \
  include/linux/bio.h \
  include/linux/mempool.h \
  include/linux/uio.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  include/uapi/linux/uio.h \
  include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  include/linux/pagemap.h \
  include/linux/hugetlb_inline.h \
  include/uapi/linux/mempolicy.h \
  include/linux/freezer.h \
  include/uapi/linux/i2c.h \
  include/linux/spi/spi.h \
    $(wildcard include/config/SPI_SLAVE) \
  include/linux/scatterlist.h \
    $(wildcard include/config/NEED_SG_DMA_LENGTH) \
    $(wildcard include/config/DEBUG_SG) \
    $(wildcard include/config/SGL_ALLOC) \
    $(wildcard include/config/ARCH_NO_SG_CHAIN) \
    $(wildcard include/config/SG_POOL) \
  include/linux/gpio/consumer.h \
    $(wildcard include/config/OF_GPIO) \
    $(wildcard include/config/GPIO_SYSFS) \
  include/uapi/linux/spi/spi.h \
  include/media/v4l2-fh.h \
  include/media/v4l2-mediabus.h \
  include/sound/core.h \
    $(wildcard include/config/SND_DYNAMIC_MINORS) \
    $(wildcard include/config/SND_MAX_CARDS) \
    $(wildcard include/config/SND_MAJOR) \
    $(wildcard include/config/SND_DEBUG) \
    $(wildcard include/config/SND_MIXER_OSS) \
    $(wildcard include/config/SND_OSSEMUL) \
    $(wildcard include/config/ISA_DMA_API) \
    $(wildcard include/config/SND_VERBOSE_PRINTK) \
    $(wildcard include/config/SND_DEBUG_VERBOSE) \
    $(wildcard include/config/GAMEPORT) \
  include/sound/pcm.h \
    $(wildcard include/config/SND_PCM_OSS) \
    $(wildcard include/config/SND_VERBOSE_PROCFS) \
    $(wildcard include/config/SND_PCM_XRUN_DEBUG) \
  include/sound/asound.h \
  include/uapi/sound/asound.h \
  include/sound/memalloc.h \
    $(wildcard include/config/GENERIC_ALLOCATOR) \
    $(wildcard include/config/SND_DMA_SGBUF) \
    $(wildcard include/config/HAS_DMA) \
  include/linux/dma-direction.h \
  include/sound/minors.h \
  include/linux/pm_qos.h \
    $(wildcard include/config/CPU_IDLE) \
  include/sound/pcm_oss.h \
    $(wildcard include/config/SND_PCM_OSS_PLUGINS) \
  drivers/media/pci/cx18/cx18-driver.h \
  include/linux/delay.h \
  arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/pci.h \
    $(wildcard include/config/PCI_IOV) \
    $(wildcard include/config/PCIEAER) \
    $(wildcard include/config/PCIEPORTBUS) \
    $(wildcard include/config/PCIEASPM) \
    $(wildcard include/config/HOTPLUG_PCI_PCIE) \
    $(wildcard include/config/PCIE_PTM) \
    $(wildcard include/config/PCIE_DPC) \
    $(wildcard include/config/PCI_ATS) \
    $(wildcard include/config/PCI_PRI) \
    $(wildcard include/config/PCI_PASID) \
    $(wildcard include/config/PCI_P2PDMA) \
    $(wildcard include/config/PCI_DOMAINS_GENERIC) \
    $(wildcard include/config/PCI_DOMAINS) \
    $(wildcard include/config/PCI_QUIRKS) \
    $(wildcard include/config/ACPI_MCFG) \
    $(wildcard include/config/HOTPLUG_PCI) \
    $(wildcard include/config/EEH) \
  include/uapi/linux/pci.h \
  include/uapi/linux/pci_regs.h \
  include/linux/pci_ids.h \
  include/linux/dmapool.h \
  arch/x86/include/asm/pci.h \
    $(wildcard include/config/PCI_MSI_IRQ_DOMAIN) \
    $(wildcard include/config/VMD) \
  arch/x86/include/asm/memtype.h \
  include/asm-generic/pci.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/DMA_API_DEBUG) \
    $(wildcard include/config/NEED_DMA_MAP_STATE) \
  include/linux/i2c-algo-bit.h \
  include/media/v4l2-ioctl.h \
  include/media/tuner.h \
  include/media/v4l2-mc.h \
  include/media/i2c/ir-kbd-i2c.h \
  include/media/rc-core.h \
    $(wildcard include/config/LIRC) \
  include/linux/kfifo.h \
  include/media/rc-map.h \
    $(wildcard include/config/IR_RC5_DECODER) \
    $(wildcard include/config/IR_JVC_DECODER) \
    $(wildcard include/config/IR_SONY_DECODER) \
    $(wildcard include/config/IR_NEC_DECODER) \
    $(wildcard include/config/IR_SANYO_DECODER) \
    $(wildcard include/config/IR_MCE_KBD_DECODER) \
    $(wildcard include/config/IR_RC6_DECODER) \
    $(wildcard include/config/IR_SHARP_DECODER) \
    $(wildcard include/config/IR_XMP_DECODER) \
    $(wildcard include/config/IR_IMON_DECODER) \
    $(wildcard include/config/IR_RCMM_DECODER) \
    $(wildcard include/config/MEDIA_CEC_RC) \
  include/linux/input.h \
  include/uapi/linux/input.h \
  include/uapi/linux/input-event-codes.h \
  include/uapi/linux/lirc.h \
  drivers/media/pci/cx18/cx18-mailbox.h \
  drivers/media/pci/cx18/cx18-av-core.h \
  include/media/v4l2-ctrls.h \
  include/media/media-request.h \
  include/media/hevc-ctrls.h \
  drivers/media/pci/cx18/cx23418.h \
  include/media/drv-intf/cx2341x.h \
  include/media/demux.h \
  include/uapi/linux/dvb/dmx.h \
  include/media/dmxdev.h \
  include/media/dvbdev.h \
    $(wildcard include/config/DVB_MAX_ADAPTERS) \
    $(wildcard include/config/MEDIA_CONTROLLER_DVB) \
    $(wildcard include/config/MEDIA_ATTACH) \
  include/media/dvb_ringbuffer.h \
  include/media/dvb_vb2.h \
    $(wildcard include/config/DVB_MMAP) \
  include/media/videobuf2-core.h \
  include/linux/dma-buf.h \
    $(wildcard include/config/DMABUF_SYSFS_STATS) \
  include/linux/iosys-map.h \
  include/linux/file.h \
  include/linux/dma-fence.h \
  include/media/frame_vector.h \
  include/media/videobuf2-dma-contig.h \
  include/media/videobuf2-v4l2.h \
  include/media/videobuf2-vmalloc.h \
  include/media/dvb_demux.h \
  include/media/dvb_frontend.h \
  include/uapi/linux/dvb/frontend.h \
  include/media/dvb_net.h \
    $(wildcard include/config/DVB_NET) \
  include/linux/netdevice.h \
    $(wildcard include/config/DCB) \
    $(wildcard include/config/HYPERV_NET) \
    $(wildcard include/config/WLAN) \
    $(wildcard include/config/AX25) \
    $(wildcard include/config/MAC80211_MESH) \
    $(wildcard include/config/NET_IPIP) \
    $(wildcard include/config/NET_IPGRE) \
    $(wildcard include/config/IPV6_SIT) \
    $(wildcard include/config/IPV6_TUNNEL) \
    $(wildcard include/config/RPS) \
    $(wildcard include/config/NETPOLL) \
    $(wildcard include/config/XPS) \
    $(wildcard include/config/XDP_SOCKETS) \
    $(wildcard include/config/BQL) \
    $(wildcard include/config/RFS_ACCEL) \
    $(wildcard include/config/FCOE) \
    $(wildcard include/config/XFRM_OFFLOAD) \
    $(wildcard include/config/NET_POLL_CONTROLLER) \
    $(wildcard include/config/LIBFCOE) \
    $(wildcard include/config/WIRELESS_EXT) \
    $(wildcard include/config/NET_L3_MASTER_DEV) \
    $(wildcard include/config/IPV6) \
    $(wildcard include/config/TLS_DEVICE) \
    $(wildcard include/config/VLAN_8021Q) \
    $(wildcard include/config/NET_DSA) \
    $(wildcard include/config/TIPC) \
    $(wildcard include/config/ATALK) \
    $(wildcard include/config/DECNET) \
    $(wildcard include/config/MPLS_ROUTING) \
    $(wildcard include/config/MCTP) \
    $(wildcard include/config/NET_CLS_ACT) \
    $(wildcard include/config/NETFILTER_INGRESS) \
    $(wildcard include/config/NETFILTER_EGRESS) \
    $(wildcard include/config/NET_SCHED) \
    $(wildcard include/config/PCPU_DEV_REFCNT) \
    $(wildcard include/config/GARP) \
    $(wildcard include/config/MRP) \
    $(wildcard include/config/NET_DROP_MONITOR) \
    $(wildcard include/config/MACSEC) \
    $(wildcard include/config/NET_FLOW_LIMIT) \
    $(wildcard include/config/NET_DEV_REFCNT_TRACKER) \
    $(wildcard include/config/ETHTOOL_NETLINK) \
  include/linux/prefetch.h \
  arch/x86/include/asm/local.h \
  include/linux/dynamic_queue_limits.h \
  include/net/net_namespace.h \
    $(wildcard include/config/NF_CONNTRACK) \
    $(wildcard include/config/IEEE802154_6LOWPAN) \
    $(wildcard include/config/IP_SCTP) \
    $(wildcard include/config/NETFILTER) \
    $(wildcard include/config/NF_TABLES) \
    $(wildcard include/config/WEXT_CORE) \
    $(wildcard include/config/XFRM) \
    $(wildcard include/config/IP_VS) \
    $(wildcard include/config/MPLS) \
    $(wildcard include/config/CAN) \
    $(wildcard include/config/CRYPTO_USER) \
    $(wildcard include/config/SMC) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/NET_NS_REFCNT_TRACKER) \
  include/net/flow.h \
  include/linux/socket.h \
  arch/x86/include/generated/uapi/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/x86/include/generated/uapi/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/uapi/linux/socket.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/libc-compat.h \
  include/net/flow_dissector.h \
  include/linux/siphash.h \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
  include/uapi/linux/if_ether.h \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/XFRM_STATISTICS) \
    $(wildcard include/config/TLS) \
    $(wildcard include/config/MPTCP) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/IP_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_CLASSID) \
    $(wildcard include/config/IP_MROUTE) \
    $(wildcard include/config/IP_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/IP_ROUTE_MULTIPATH) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/IPV6_MULTIPLE_TABLES) \
    $(wildcard include/config/IPV6_SUBTREES) \
    $(wildcard include/config/IPV6_MROUTE) \
    $(wildcard include/config/IPV6_MROUTE_MULTIPLE_TABLES) \
    $(wildcard include/config/NF_DEFRAG_IPV6) \
  include/net/dst_ops.h \
  include/uapi/linux/icmpv6.h \
  include/net/netns/nexthop.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/netfilter.h \
    $(wildcard include/config/NETFILTER_FAMILY_ARP) \
    $(wildcard include/config/NETFILTER_FAMILY_BRIDGE) \
    $(wildcard include/config/NF_DEFRAG_IPV4) \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/conntrack.h \
    $(wildcard include/config/NF_CT_PROTO_DCCP) \
    $(wildcard include/config/NF_CT_PROTO_SCTP) \
    $(wildcard include/config/NF_FLOW_TABLE) \
    $(wildcard include/config/NF_CT_PROTO_GRE) \
    $(wildcard include/config/NF_CONNTRACK_EVENTS) \
    $(wildcard include/config/NF_CONNTRACK_LABELS) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/linux/netfilter/nf_conntrack_dccp.h \
  include/uapi/linux/netfilter/nf_conntrack_tuple_common.h \
  include/linux/netfilter/nf_conntrack_common.h \
  include/uapi/linux/netfilter/nf_conntrack_common.h \
  include/linux/netfilter/nf_conntrack_sctp.h \
  include/uapi/linux/netfilter/nf_conntrack_sctp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/netns/mpls.h \
  include/net/netns/can.h \
  include/net/netns/xdp.h \
  include/net/netns/smc.h \
  include/net/netns/bpf.h \
  include/net/netns/mctp.h \
  include/net/net_trackers.h \
  include/linux/ref_tracker.h \
    $(wildcard include/config/REF_TRACKER) \
  include/linux/skbuff.h \
    $(wildcard include/config/BRIDGE_NETFILTER) \
    $(wildcard include/config/NET_TC_SKB_EXT) \
    $(wildcard include/config/NET_SOCK_MSG) \
    $(wildcard include/config/SKB_EXTENSIONS) \
    $(wildcard include/config/IPV6_NDISC_NODETYPE) \
    $(wildcard include/config/NET_SWITCHDEV) \
    $(wildcard include/config/NET_REDIRECT) \
    $(wildcard include/config/NETFILTER_SKIP_EGRESS) \
    $(wildcard include/config/NET_RX_BUSY_POLL) \
    $(wildcard include/config/NETWORK_SECMARK) \
    $(wildcard include/config/PAGE_POOL) \
    $(wildcard include/config/NETWORK_PHY_TIMESTAMPING) \
    $(wildcard include/config/MCTP_FLOWS) \
    $(wildcard include/config/NETFILTER_XT_TARGET_TRACE) \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/VMGENID) \
    $(wildcard include/config/ARCH_RANDOM) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/prandom.h \
  arch/x86/include/asm/archrandom.h \
  include/linux/sockptr.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/x86/include/asm/checksum.h \
  arch/x86/include/asm/checksum_32.h \
  include/linux/netdev_features.h \
  include/linux/sched/clock.h \
    $(wildcard include/config/HAVE_UNSTABLE_SCHED_CLOCK) \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/page_pool.h \
    $(wildcard include/config/PAGE_POOL_STATS) \
  include/linux/ptr_ring.h \
  include/linux/seq_file_net.h \
  include/net/netprio_cgroup.h \
  include/net/xdp.h \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/SECURITY_NETWORK) \
  include/linux/security.h \
    $(wildcard include/config/SECURITY_INFINIBAND) \
    $(wildcard include/config/SECURITY_NETWORK_XFRM) \
    $(wildcard include/config/SECURITY_PATH) \
    $(wildcard include/config/SECURITYFS) \
  include/linux/kernel_read_file.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/uapi/linux/if.h \
  include/uapi/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/inetdevice.h \
    $(wildcard include/config/INET) \
  include/linux/ip.h \
  include/uapi/linux/ip.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/NET_INGRESS) \
    $(wildcard include/config/NET_EGRESS) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/linux/etherdevice.h \
  include/linux/crc32.h \
  include/linux/bitrev.h \
    $(wildcard include/config/HAVE_ARCH_BITREVERSE) \
  arch/x86/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/packed_struct.h \
  include/media/videobuf-core.h \
  include/media/videobuf-vmalloc.h \
  drivers/media/pci/cx18/cx18-queue.h \
  drivers/media/pci/cx18/cx18-streams.h \
  drivers/media/pci/cx18/cx18-fileops.h \
  drivers/media/pci/cx18/cx18-alsa.h \
  drivers/media/pci/cx18/cx18-alsa-pcm.h \

drivers/media/pci/cx18/cx18-alsa-pcm.o: $(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o)

$(deps_drivers/media/pci/cx18/cx18-alsa-pcm.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   cmd_drivers/media/pci/cx18/cx18.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18.o drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   cmd_drivers/media/pci/cx18/cx18.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18.o drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ELF                      ¤"      4     (            GNU  À       À          èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     À           ¤ÿ    ×    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       
 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       
 Ä  °  R     ß    ©    ù  €   @    
   Ä  0     +  H       
 6  @       
 A  P       
 L  »   -     e  è        ‚          ”  À   
    
 ª             ¶             Ä             Ğ             Ù             ë             ı               U   ?                  !             '             >             U             c             n           z             ˜                           ¶             Ç             Ï             Ü             ç             ÷             	                          9             X             u             “             ›             ¯             ½             Ê             á           ñ     £                 $             2  À  Ğ     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     À   ]  Ò   @  Ü     è   	  û       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W  Š      G  ”    ¡  B  Á  B  Ñ  B  à  ?  ì  N    B          ™  =     S  Ü  F  â    ê      =  @  =  R    Z    x  F  †  Y  ’    Ÿ    ±  B  Ê  J  Ó  P  â  M  ø  U    B  #  9  C  J  m  L  ©  U  ¸  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Á  B  É  	  Ù  9  ì  7  ù          K  &  O  I  H  V    g    p  R  {  9  Œ  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡     Œ   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ     ˆ   E  •     ›         E  ¨     »     Á     Æ   E  Ó     Ø     İ   E  ä     é     î   	  ó     ø   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡    Œ    ‘    –  E      £    ª    ¯    ´    ¹  E  À    Å    Ê  A  Ï    Ô  A  İ    ã    è  E  ğ       B          E     T       €     „     ”     ˜                                                          $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h  ˆ              <             ğ  ”                  8   	   @       ğ  ˆ               K             „  ô                 G   	   @       x  H              ^             x	                    Z   	   @       À   (               m              	  Ê                   i   	   @       è   (      
         u      2       l
  	                ˆ             u  (                  „   	   @       !  P               •      2         +                 ¤             È  ÿ                  ±             È  (                  ­   	   @       `!  @               ½             ğ                    ¹   	   @        !                 È             ü                     Î             ü                    Ó      0       ü  &                 Ü              "                                   $  à     5         	                b                               ¸!  ì                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ELF                      ¤"      4     (            GNU  À       À          èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ¤ÿ     À           ¤ÿ    ×    s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ñÿ       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       
 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       
 Ä  °  R     ß    ©    ù  €   @    
   Ä  0     +  H       
 6  @       
 A  P       
 L  »   -     e  è        ‚          ”  À   
    
 ª             ¶             Ä             Ğ             Ù             ë             ı               U   ?                  !             '             >             U             c             n           z             ˜                           ¶             Ç             Ï             Ü             ç             ÷             	                          9             X             u             “             ›             ¯             ½             Ê             á           ñ     £                 $             2  À  Ğ     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q          «   6  °   I  »     À   ]  Ò   @  Ü     è   	  û       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ‚  W  Š      G  ”    ¡  B  Á  B  Ñ  B  à  ?  ì  N    B          ™  =     S  Ü  F  â    ê      =  @  =  R    Z    x  F  †  Y  ’    Ÿ    ±  B  Ê  J  Ó  P  â  M  ø  U    B  #  9  C  J  m  L  ©  U  ¸  9  U  Q  o  X  €  D    U  ¦  U  µ  G  Á  B  É  	  Ù  9  ì  7  ù          K  &  O  I  H  V    g    p  R  {  9  Œ  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ‡     Œ   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ƒ     ˆ   E  •     ›         E  ¨     »     Á     Æ   E  Ó     Ø     İ   E  ä     é     î   	  ó     ø   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    €  E  ‡    Œ    ‘    –  E      £    ª    ¯    ´    ¹  E  À    Å    Ê  A  Ï    Ô  A  İ    ã    è  E  ğ       B          E     T       €     „     ”     ˜                                                          $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `                    .   	   @       h  ˆ              <             ğ  ”                  8   	   @       ğ  ˆ               K             „  ô                 G   	   @       x  H              ^             x	                    Z   	   @       À   (               m              	  Ê                   i   	   @       è   (      
         u      2       l
  	                ˆ             u  (                  „   	   @       !  P               •      2         +                 ¤             È  ÿ                  ±             È  (                  ­   	   @       `!  @               ½             ğ                    ¹   	   @        !                 È             ü                     Î             ü                    Ó      0       ü  &                 Ü              "                                   $  à     5         	                b                               ¸!  ì                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/pci/cx18/cx18-alsa.o := ld -m elf_i386   -r -o drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            cmd_drivers/media/pci/cx18/cx18.mod := { echo  drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o;  echo; } > drivers/media/pci/cx18/cx18.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  cmd_drivers/media/pci/cx18/cx18.mod := { echo  drivers/media/pci/cx18/cx18-driver.o drivers/media/pci/cx18/cx18-cards.o drivers/media/pci/cx18/cx18-i2c.o drivers/media/pci/cx18/cx18-firmware.o drivers/media/pci/cx18/cx18-gpio.o drivers/media/pci/cx18/cx18-queue.o drivers/media/pci/cx18/cx18-streams.o drivers/media/pci/cx18/cx18-fileops.o drivers/media/pci/cx18/cx18-ioctl.o drivers/media/pci/cx18/cx18-controls.o drivers/media/pci/cx18/cx18-mailbox.o drivers/media/pci/cx18/cx18-vbi.o drivers/media/pci/cx18/cx18-audio.o drivers/media/pci/cx18/cx18-video.o drivers/media/pci/cx18/cx18-irq.o drivers/media/pci/cx18/cx18-av-core.o drivers/media/pci/cx18/cx18-av-audio.o drivers/media/pci/cx18/cx18-av-firmware.o drivers/media/pci/cx18/cx18-av-vbi.o drivers/media/pci/cx18/cx18-scb.o drivers/media/pci/cx18/cx18-dvb.o drivers/media/pci/cx18/cx18-io.o;  echo; } > drivers/media/pci/cx18/cx18.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                cmd_drivers/media/pci/cx18/cx18-alsa.mod := { echo  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o;  echo; } > drivers/media/pci/cx18/cx18-alsa.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            cmd_drivers/media/pci/cx18/cx18-alsa.mod := { echo  drivers/media/pci/cx18/cx18-alsa-main.o drivers/media/pci/cx18/cx18-alsa-pcm.o;  echo; } > drivers/media/pci/cx18/cx18-alsa.mod
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            drivers/media/pci/cx18/cx18.ko
drivers/media/pci/cx18/cx18-alsa.ko
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             drivers/media/pci/cx18/cx18.ko
drivers/media/pci/cx18/cx18-alsa.ko
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cmd_drivers/media/pci/cx18/modules.order := {   echo drivers/media/pci/cx18/cx18.ko;   echo drivers/media/pci/cx18/cx18-alsa.ko; :; } | awk '!x[$$0]++' - > drivers/media/pci/cx18/modules.order
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               cmd_drivers/media/pci/cx18/modules.order := {   echo drivers/media/pci/cx18/cx18.ko;   echo drivers/media/pci/cx18/cx18-alsa.ko; :; } | awk '!x[$$0]++' - > drivers/media/pci/cx18/modules.order
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               #include <linux/module.h>
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

MODULE_INFO(depends, "snd-pcm,cx18,snd");


MODULE_INFO(srcversion, "0A31E57A452F6B591496F85");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           #include <linux/module.h>
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

MODULE_INFO(depends, "snd-pcm,cx18,snd");


MODULE_INFO(srcversion, "0A31E57A452F6B591496F85");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           #include <linux/module.h>
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

MODULE_INFO(depends, "videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit");

MODULE_ALIAS("pci:v000014F1d00005B7Asv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "1CE209474DA443168BA26F2");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              #include <linux/module.h>
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

MODULE_INFO(depends, "videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit");

MODULE_ALIAS("pci:v000014F1d00005B7Asv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "1CE209474DA443168BA26F2");
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ELF                      ¨      4     (   srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                                                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Linux                Linux        GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ       #     *   #        @   <        X   H   	     m   Q        €   `   7     —            Ÿ           §            µ              Á               cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ì     ô     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4   —                  9                           @       5   	   @                        S                0                  _      0       0                   h              C                     x             D  (                                l  Ğ      
         	              <  Ğ                                  ‹                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                      ¨      4     (   srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                                                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Linux                Linux        GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ       #     *   #        @   <        X   H   	     m   Q        €   `   7     —            Ÿ           §            µ              Á               cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ì     ô     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4   —                  9                           @       5   	   @                        S                0                  _      0       0                   h              C                     x             D  (                                l  Ğ      
         	              <  Ğ                                  ‹                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          cmd_drivers/media/pci/cx18/cx18-alsa.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18_alsa.mod"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa.mod.o drivers/media/pci/cx18/cx18-alsa.mod.c

source_drivers/media/pci/cx18/cx18-alsa.mod.o := drivers/media/pci/cx18/cx18-alsa.mod.c

deps_drivers/media/pci/cx18/cx18-alsa.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18-alsa.mod.o: $(deps_drivers/media/pci/cx18/cx18-alsa.mod.o)

$(deps_drivers/media/pci/cx18/cx18-alsa.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               cmd_drivers/media/pci/cx18/cx18-alsa.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18_alsa.mod"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa.mod.o drivers/media/pci/cx18/cx18-alsa.mod.c

source_drivers/media/pci/cx18/cx18-alsa.mod.o := drivers/media/pci/cx18/cx18-alsa.mod.c

deps_drivers/media/pci/cx18/cx18-alsa.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18-alsa.mod.o: $(deps_drivers/media/pci/cx18/cx18-alsa.mod.o)

$(deps_drivers/media/pci/cx18/cx18-alsa.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ELF                            4     (   srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                      cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Linux                Linux        GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ       #     &   #   +     :   N   `     P   ®        h   º   	     }   Ã   
        Í   7     §            ¯           ·            Å              Ñ               cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ì     ô     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4                    9             @             @       5   	   @       l                 S             @  0                  _      0       p                   h              ƒ                     x             „  (                                ¬  à               	              Œ  à                                |  ‹                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ELF                            4     (   srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                      cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Linux                Linux        GCC: (GNU) 11.2.0           GNU  À        À                                  ñÿ       #     &   #   +     :   N   `     P   ®        h   º   	     }   Ã   
        Í   7     §            ¯           ·            Å              Ñ               cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 __this_module init_module cleanup_module Ì     ô     .symtab .strtab .shstrtab .text .data .bss .modinfo .rel.gnu.linkonce.this_module .note.Linux .comment .note.GNU-stack .note.gnu.property                                                       4                      !             4                      '             4                      ,             4                    9             @             @       5   	   @       l                 S             @  0                  _      0       p                   h              ƒ                     x             „  (                                ¬  à               	              Œ  à                                |  ‹                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          cmd_drivers/media/pci/cx18/cx18.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18.mod"' -DKBUILD_MODNAME='"cx18"' -D__KBUILD_MODNAME=kmod_cx18 -c -o drivers/media/pci/cx18/cx18.mod.o drivers/media/pci/cx18/cx18.mod.c

source_drivers/media/pci/cx18/cx18.mod.o := drivers/media/pci/cx18/cx18.mod.c

deps_drivers/media/pci/cx18/cx18.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18.mod.o: $(deps_drivers/media/pci/cx18/cx18.mod.o)

$(deps_drivers/media/pci/cx18/cx18.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                cmd_drivers/media/pci/cx18/cx18.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18.mod"' -DKBUILD_MODNAME='"cx18"' -D__KBUILD_MODNAME=kmod_cx18 -c -o drivers/media/pci/cx18/cx18.mod.o drivers/media/pci/cx18/cx18.mod.c

source_drivers/media/pci/cx18/cx18.mod.o := drivers/media/pci/cx18/cx18.mod.c

deps_drivers/media/pci/cx18/cx18.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/X86) \
  include/uapi/video/edid.h \
  arch/x86/include/asm/math_emu.h \
  arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/PARAVIRT) \
    $(wildcard include/config/IA32_EMULATION) \
  arch/x86/include/asm/segment.h \
    $(wildcard include/config/XEN_PV) \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/PHYSICAL_START) \
    $(wildcard include/config/PHYSICAL_ALIGN) \
    $(wildcard include/config/DYNAMIC_PHYSICAL_MASK) \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/HIGHMEM4G) \
    $(wildcard include/config/HIGHMEM64G) \
    $(wildcard include/config/PAGE_OFFSET) \
  arch/x86/include/uapi/asm/ptrace.h \
  arch/x86/include/uapi/asm/ptrace-abi.h \
  arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/PARAVIRT_DEBUG) \
  arch/x86/include/asm/desc_defs.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PROC_FS) \
  arch/x86/include/asm/pgtable_32_types.h \
  arch/x86/include/asm/pgtable-3level_types.h \
  include/asm-generic/pgtable-nop4d.h \
  include/asm-generic/pgtable-nopud.h \
  arch/x86/include/asm/nospec-branch.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/GEN-for-each-reg.h \
  arch/x86/include/asm/spinlock_types.h \
  include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  include/asm-generic/qrwlock_types.h \
  arch/x86/include/asm/proto.h \
  arch/x86/include/uapi/asm/ldt.h \
  arch/x86/include/uapi/asm/sigcontext.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  include/linux/stdarg.h \
  include/linux/align.h \
  include/linux/limits.h \
  include/uapi/linux/limits.h \
  include/vdso/limits.h \
  include/linux/kstrtox.h \
  include/linux/minmax.h \
  include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  include/linux/kern_levels.h \
  include/linux/ratelimit_types.h \
  include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  include/linux/once_lite.h \
  include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  include/linux/instruction_pointer.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_32.h \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/FLATMEM) \
  include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  include/uapi/linux/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_32.h \
  include/linux/fortify-string.h \
  include/linux/range.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  include/linux/pfn.h \
  include/asm-generic/getorder.h \
  arch/x86/include/asm/msr.h \
  arch/x86/include/asm/msr-index.h \
  arch/x86/include/asm/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
  include/linux/bitmap.h \
  include/linux/find.h \
  include/linux/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/cmpxchg_32.h \
  arch/x86/include/asm/atomic64_32.h \
  include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  include/linux/atomic/atomic-long.h \
  include/linux/atomic/atomic-instrumented.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  arch/x86/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  include/linux/instrumentation.h \
    $(wildcard include/config/DEBUG_ENTRY) \
  include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  arch/x86/include/uapi/asm/msr.h \
  include/linux/tracepoint-defs.h \
  arch/x86/include/asm/special_insns.h \
  include/linux/irqflags.h \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  arch/x86/include/asm/irqflags.h \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/vmxfeatures.h \
  arch/x86/include/asm/vdso/processor.h \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/X86_FEATURE_NAMES) \
  include/vdso/time32.h \
  include/vdso/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  include/linux/highuid.h \
  include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/IOMMU_SVA) \
  include/linux/mm_types_task.h \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  arch/x86/include/asm/tlbbatch.h \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/x86/include/uapi/asm/auxvec.h \
  include/linux/kref.h \
  include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  arch/x86/include/asm/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  include/linux/restart_block.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/COMPAT) \
  include/linux/bottom_half.h \
  include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
  include/linux/smp_types.h \
  include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/X86_LOCAL_APIC) \
    $(wildcard include/config/DEBUG_NMI_SELFTEST) \
  arch/x86/include/generated/asm/mmiowb.h \
  include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  include/linux/spinlock_types.h \
  include/linux/rwlock_types.h \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/PARAVIRT_SPINLOCKS) \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/qspinlock.h \
  include/asm-generic/qspinlock.h \
  arch/x86/include/asm/qrwlock.h \
  include/asm-generic/qrwlock.h \
  include/linux/rwlock.h \
    $(wildcard include/config/PREEMPT) \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  include/linux/refcount.h \
  include/linux/rbtree.h \
  include/linux/rbtree_types.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  include/linux/rcutree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  include/linux/osq_lock.h \
  include/linux/completion.h \
  include/linux/swait.h \
  include/linux/wait.h \
  include/uapi/linux/wait.h \
  include/linux/uprobes.h \
    $(wildcard include/config/UPROBES) \
  arch/x86/include/asm/uprobes.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  include/linux/debug_locks.h \
  include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/SRCU) \
  include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  include/linux/ktime.h \
  include/linux/jiffies.h \
  include/vdso/jiffies.h \
  include/generated/timeconst.h \
  include/vdso/ktime.h \
  include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  include/linux/clocksource_ids.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  include/linux/rcu_segcblist.h \
  include/linux/srcutree.h \
  include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  arch/x86/include/asm/sparsemem.h \
  include/generated/bounds.h \
  include/linux/seqlock.h \
  include/linux/ww_mutex.h \
    $(wildcard include/config/DEBUG_RT_MUTEXES) \
    $(wildcard include/config/DEBUG_WW_MUTEX_SLOWPATH) \
  include/linux/rtmutex.h \
  arch/x86/include/asm/mmu.h \
    $(wildcard include/config/MODIFY_LDT_SYSCALL) \
  include/linux/kmod.h \
  include/linux/umh.h \
  include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/CONTIG_ALLOC) \
    $(wildcard include/config/CMA) \
  include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  include/linux/mmzone.h \
    $(wildcard include/config/FORCE_MAX_ZONEORDER) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP) \
    $(wildcard include/config/HUGETLB_PAGE_FREE_VMEMMAP_DEFAULT_ON) \
    $(wildcard include/config/KSM) \
  include/linux/local_lock.h \
  include/linux/local_lock_internal.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  arch/x86/include/asm/mmzone.h \
  arch/x86/include/asm/mmzone_32.h \
  include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/SCHED_CLUSTER) \
  include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  include/linux/percpu.h \
    $(wildcard include/config/NEED_PER_CPU_EMBED_FIRST_CHUNK) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  arch/x86/include/asm/topology.h \
    $(wildcard include/config/SCHED_MC_PRIO) \
  arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/EISA) \
    $(wildcard include/config/X86_MPPARSE) \
  arch/x86/include/asm/mpspec_def.h \
  arch/x86/include/asm/x86_init.h \
  arch/x86/include/asm/apicdef.h \
  include/asm-generic/topology.h \
  include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  arch/x86/include/asm/elf.h \
    $(wildcard include/config/X86_X32_ABI) \
  arch/x86/include/asm/user.h \
  arch/x86/include/asm/user_32.h \
  arch/x86/include/asm/fsgsbase.h \
  arch/x86/include/asm/vdso.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
    $(wildcard include/config/X86_VSYSCALL_EMULATION) \
    $(wildcard include/config/PROVIDE_OHCI1394_DMA_INIT) \
    $(wildcard include/config/X86_IO_APIC) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/PCI_MMCONFIG) \
    $(wildcard include/config/ACPI_APEI_GHES) \
    $(wildcard include/config/INTEL_TXT) \
  arch/x86/include/generated/asm/kmap_size.h \
  include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  include/asm-generic/fixmap.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/HAVE_KVM) \
    $(wildcard include/config/HYPERV) \
    $(wildcard include/config/PCI_MSI) \
  arch/x86/include/asm/cpu_entry_area.h \
  arch/x86/include/asm/intel_ds.h \
  arch/x86/include/asm/pgtable_areas.h \
  arch/x86/include/asm/pgtable_32_areas.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  include/linux/idr.h \
  include/linux/radix-tree.h \
  include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  include/linux/kconfig.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/IA64) \
    $(wildcard include/config/PPC64) \
  include/linux/rbtree_latch.h \
  include/linux/error-injection.h \
  include/asm-generic/error-injection.h \
  include/linux/cfi.h \
    $(wildcard include/config/CFI_CLANG_SHADOW) \
  arch/x86/include/asm/module.h \
    $(wildcard include/config/UNWINDER_ORC) \
  include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  arch/x86/include/asm/orc_types.h \
  include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  include/linux/elfnote.h \
  include/linux/elfnote-lto.h \
    $(wildcard include/config/LTO) \
  include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  arch/x86/include/asm/vermagic.h \
    $(wildcard include/config/M486SX) \
    $(wildcard include/config/M486) \
    $(wildcard include/config/M586) \
    $(wildcard include/config/M586TSC) \
    $(wildcard include/config/M586MMX) \
    $(wildcard include/config/MCORE2) \
    $(wildcard include/config/M686) \
    $(wildcard include/config/MPENTIUMII) \
    $(wildcard include/config/MPENTIUMIII) \
    $(wildcard include/config/MPENTIUMM) \
    $(wildcard include/config/MPENTIUM4) \
    $(wildcard include/config/MK6) \
    $(wildcard include/config/MK7) \
    $(wildcard include/config/MK8) \
    $(wildcard include/config/MELAN) \
    $(wildcard include/config/MCRUSOE) \
    $(wildcard include/config/MEFFICEON) \
    $(wildcard include/config/MWINCHIPC6) \
    $(wildcard include/config/MWINCHIP3D) \
    $(wildcard include/config/MCYRIXIII) \
    $(wildcard include/config/MVIAC3_2) \
    $(wildcard include/config/MVIAC7) \
    $(wildcard include/config/MGEODEGX1) \
    $(wildcard include/config/MGEODE_LX) \

drivers/media/pci/cx18/cx18.mod.o: $(deps_drivers/media/pci/cx18/cx18.mod.o)

$(deps_drivers/media/pci/cx18/cx18.mod.o):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ELF                      p'      4     (                èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ¤ÿ     À           ¤ÿ    ×    s           GNU  À       À               GNU p¢7ğòœ°J©ö=O"í&ù•        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ñÿ       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       	 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       	 Ä  °  R     ß    ©    ù  €   @    	   Ä  0     +  H       	 6  @       	 A  P       	 L  »   -     e  è        ‚          ”  À   
    	 ª           ñÿº  ÿ   #     Ó  "       é  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ‘             £             ±  U   ?     À             Ç             Í             ä             û             	                                      >             F             \             m             u             ‚                                       ¯             Ã             ß             ş                          9             A             U             c             p             ‡           —     £    ³             Ê             Ø  À  Ğ     ì             ù              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q          «   B  °   U  »     À   i  Ò   L  Ü     è     û       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  ‚  c  Š      S  ”    ¡  N  Á  N  Ñ  N  à  K  ì  Z    N          ™  I     _  Ü  R  â    ê      I  @  I  R    Z    x  R  †  e  ’    Ÿ    ±  N  Ê  V  Ó  \  â  Y  ø  a    N  #  E  C  V  m  X  ©  a  ¸  E  U  ]  o  d  €  P    a  ¦  a  µ  S  Á  N  É    Ù  E  ì  D  ù          W  &  [  I  T  V    g    p  ^  {  E  Œ  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  ‡     Œ   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     ƒ     ˆ   Q  •     ›         Q  ¨     »     Á     Æ   Q  Ó     Ø     İ   Q  ä     é     î     ó     ø   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    €  Q  ‡    Œ    ‘    –  Q      £    ª    ¯    ´    ¹  Q  À    Å    Ê  M  Ï    Ô  M  İ    ã    è  Q  ğ       N          Q     `       €     „     ”     ˜                                                          $              B     f     c          B     f  $                     Ì   O  ô  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @                       	   @       ä  ˆ              )             Ğ  ”                  %   	   @       l"  ˆ               8             d  ô                 4   	   @       ô"  H              K             X	                    G   	   @       <%  (               Z             €	  Ê                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       Œ%  P               ‚      2       }  +                 ‘             ¨  –                              @  (                  š   	   @       Ü%  @               ª             h                    ¦   	   @       &                 µ             t                     »             t  (                  Î             œ  $                  á             À  0                  ñ                           @       í   	   @       4&                                                       0          9                              9                                   <        A         	              Ü                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ELF                      p'      4     (                èüÿÿÿ…Àt?S‰Ã‹€ˆ  …Àt‹…Òt
Ç‚$&      èüÿÿÿÇƒˆ      ÇƒŒ      [Ã´&    Ã´&    èüÿÿÿUWV‰ÆS‰÷ƒìd¡    ‰D$1ÀƒÇ„Î   ‹–  …Ò„ı   ‹†,&  …À…   ‹FÇ$    1ÉºÿÿÿÿTj ˆ   h    èüÿÿÿƒÄ…À…t   ¡   ¹   ºÀ  ‹,$èüÿÿÿ‰Ã…À„Z   ‰8¹   º
   ‰h‰…ˆ  Ç…Œ      ‹x‹(GèüÿÿÿG$ÿuøƒÇDh   j Pèüÿÿÿÿudÿuøh@  jPWèüÿÿÿ‰ØƒÄ$èüÿÿÿ…À…   ‰,&  ‹$èüÿÿÿ…À…¨   ö    u-‹D$d+    uƒÄ1À[^_]Ãv ö    tİéüÿÿÿèüÿÿÿéä   ffffèüÿÿÿ‹@Ç@    Ç@    1ÀÃ´&    èüÿÿÿ1ÀÃ´&    èüÿÿÿVS‹p^‰Øèüÿÿÿ‹v‰Â‰Øèüÿÿÿ‰ğ[^Ã´&    t& èüÿÿÿUWVS‰Ãƒì‹5   ‰$‰Ê…ö…2  ‹h…í„Â   ‹}|…ÿ„·   ‹w`Áî„   9ò‚  ‹4  …É„7  ‰Ğ1Ò÷ö‹S‰D$‹GT‰D$‰Ğ¯ÆÈ‰Ñ‰D$‹D$Ğ‰Â‹D$9Âsv¯t$‹$‹D$‰ñèüÿÿÿ‰èèüÿÿÿ‹C‹t$ğ‰C‹OT‰Â)Ê9ÈCÂs‰s‰ñ‰C‹GL9Æƒ   ƒÄ‰è[^_]éüÿÿÿ‹-   …í…  ƒÄ[^_]Ã´&    v ‰Â‰L$‹D$)Ê‰Ñ‹$¯Î‰L$èüÿÿÿ‹D$‹L$+L$‹$È‹L$¯ÆÊ‰Á‹‡4  èüÿÿÿéTÿÿÿ´&    ‹   …Û…   ƒÄ[^_]Ã´&    v )Á‰è‰Kèüÿÿÿ‰èƒÄ[^_]éüÿÿÿ¶    ‹   …É„Pÿÿÿé‡  ´&    ¶    èüÿÿÿWVS‹p‹ƒ   »T  èüÿÿÿ1Ò‰øèüÿÿÿğ€£¨  ï‰øèüÿÿÿÇƒ(&      ‹   èüÿÿÿ1À[^_Ã´&    ´&    èüÿÿÿU‰ÅWVSƒìl‹u|d¡    ‰D$h‹E‹‰$Cø‰D$‰Çƒ   èüÿÿÿ‹ƒ<&  ‹“”  ‰|$dH‰D$\D$‰‹<&  ‰T$`èüÿÿÿ…À…"  ‰Â‹ƒ¨  „ÀxƒT  ‰D$ğº«¨  s6‹$‰T$‹    èüÿÿÿ‹T$‹D$hd+    …ò   ƒÄl‰Ğ[^_]Ãt& ‰Ğ¾ø   ¹   º   ó«‰ğÇ†ø    Ç†ü      Ç†       Ç†  €   Ç†  €»  Ç†  €»  Ç†     Ç†     Ç†  ¨ @Ç†!  1  Ç†(  b   èüÿÿÿ‹<$‹L$‰o‰ğ   Çƒ(&      ğ€‹¨  ‹D$èüÿÿÿ‰$‹   èüÿÿÿ‹$éÿÿÿ‹$‹    èüÿÿÿºğÿÿÿéışÿÿèüÿÿÿ´&    èüÿÿÿU1Éº   W‰ÇVSƒì‹/d¡    ‰D$1À‹GTjj èüÿÿÿ‰ÆƒÄ…À…Ô  ÇG    ‹$¹€   º   èüÿÿÿ1Éº   j j ‹D$èüÿÿÿ‹\$ºP   ÇC    ƒÃX‰»`  ‹}d‰øèüÿÿÿ‰ÁXZƒùÿ„Ê  ƒùPtƒÁƒùP‡À  ‰ú‰Øèüÿÿÿ‹D$d+    u
ƒÄ‰ğ[^_]ÃèüÿÿÿS‹@@…Àuh    h    èüÿÿÿXZë6‹$&  …ÒuƒÀh    Ph0   èüÿÿÿƒÄë‹‹BèüÿÿÿÇƒ$&      1À[Ãhh  èüÿÿÿº    ¸   èüÿÿÿ1É1Òh    èüÿÿÿÇ        hŒ  èüÿÿÿƒÄÃh(   h    h`   èüÿÿÿƒÄéb  ƒÆPh   Vhl  èüÿÿÿƒÄ‹$…Àtèüÿÿÿ‰Øèüÿÿÿh(   Vh  èüÿÿÿƒÄéb  ƒÆjôh   Vh  èüÿÿÿƒÄë»ƒÆPh   VhĞ   èüÿÿÿƒÄëµƒÆh(   Vh˜   èüÿÿÿƒÄéb  Ç†,&      ƒÆPh   Vh¨  èüÿÿÿƒÄéaÿÿÿh(   h    èüÿÿÿY[éb  h(   h    hà  èüÿÿÿƒÄéb  hd   hd   hp  èüÿÿÿƒÄéê  hd   hL  èüÿÿÿ^_éê  Q‰L$ÿt$Phd   h´  èüÿÿÿ‹k‹T$ƒÄ…ítD‹}|…ÿ…3  ƒ=    „ê  hd   h$  èüÿÿÿXZéê  hd   hœ  èüÿÿÿXZéê  ƒ=    „ê  hd   hü  èüÿÿÿY[éê  ¸H   èüÿÿÿ¸@   èüÿÿÿƒÅPhP   UhĞ  èüÿÿÿƒÄép  èüÿÿÿhH  èüÿÿÿX1ÀÇ    P   Ã           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   °                 À      Ğ                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P          À  Ğ     °    À  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int srcversion=0A31E57A452F6B591496F85 depends=snd-pcm,cx18,snd retpoline=Y intree=Y name=cx18_alsa vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII    8           ¤ÿ     À           ¤ÿ    ×    s           GNU  À       À               GNU p¢7ğòœ°J©ö=O"í&ù•        Linux                Linux                                   cx18_alsa                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                 ñÿ       U     *           	 5       I     P   P   H    _           s   (       	 ~          	 ‰            ˜   U   ?     §            ¾           Õ      #     ğ   =          O   Y       ¨        4           B  8       	 T           ñÿd          y  À         Ğ  %     Š          £    À     s   d       	 Ä  °  R     ß    ©    ù  €   @    	   Ä  0     +  H       	 6  @       	 A  P       	 L  »   -     e  è        ‚          ”  À   
    	 ª           ñÿº  ÿ   #     Ó  "       é  ;         G  	       P       )  _  7     @           H          P             \           j             s                          ‘             £             ±  U   ?     À             Ç             Í             ä             û             	                                      >             F             \             m             u             ‚                                       ¯             Ã             ß             ş                          9             A             U             c             p             ‡           —     £    ³             Ê             Ø  À  Ğ     ì             ù              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug cx18-alsa.mod.c __UNIQUE_ID_srcversion99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 driver_find __this_module snprintf snd_pcm_new __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches    N  '   J  Q   N  b   E  q          «   B  °   U  »     À   i  Ò   L  Ü     è     û       ^        C  )    1  C  ;  g  C    Q  F  Y    _  c  m  E  ‚  c  Š      S  ”    ¡  N  Á  N  Ñ  N  à  K  ì  Z    N          ™  I     _  Ü  R  â    ê      I  @  I  R    Z    x  R  †  e  ’    Ÿ    ±  N  Ê  V  Ó  \  â  Y  ø  a    N  #  E  C  V  m  X  ©  a  ¸  E  U  ]  o  d  €  P    a  ¦  a  µ  S  Á  N  É    Ù  E  ì  D  ù          W  &  [  I  T  V    g    p  ^  {  E  Œ  S  	             Q  )     /     4   Q  C   G  V     [   Q  `   h  e     j   A  s     x   b  ~   `  ‡     Œ   Q                    Q       !     '     ,   Q  ;   G  B   J  G     M     R   Q  Z     d     j     o   Q  }     ƒ     ˆ   Q  •     ›         Q  ¨     »     Á     Æ   Q  Ó     Ø     İ   Q  ä     é     î     ó     ø   Q           
          Q      !    &    +  Q  2    A    F    K  Q  d    j    q    v    {    €  Q  ‡    Œ    ‘    –  Q      £    ª    ¯    ´    ¹  Q  À    Å    Ê  M  Ï    Ô  M  İ    ã    è  Q  ğ       N          Q     `       €     „     ”     ˜                                                          $              B     f     c          B     f  $                     Ì   O  ô  H   .symtab .strtab .shstrtab .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .note.gnu.property .note.gnu.build-id .note.Linux .rel.gnu.linkonce.this_module .bss .comment .note.GNU-stack                                                         @                       	   @       ä  ˆ              )             Ğ  ”                  %   	   @       l"  ˆ               8             d  ô                 4   	   @       ô"  H              K             X	                    G   	   @       <%  (               Z             €	  Ê                   V   	   @       d%  (      	         b      2       L
  	                u             U  (                  q   	   @       Œ%  P               ‚      2       }  +                 ‘             ¨  –                              @  (                  š   	   @       Ü%  @               ª             h                    ¦   	   @       &                 µ             t                     »             t  (                  Î             œ  $                  á             À  0                  ñ                           @       í   	   @       4&                                                       0          9                              9                                   <        A         	              Ü                                 D&  )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    cmd_drivers/media/pci/cx18/cx18-alsa.ko := ld -r -m elf_i386 --build-id=sha1  -T scripts/module.lds -o drivers/media/pci/cx18/cx18-alsa.ko drivers/media/pci/cx18/cx18-alsa.o drivers/media/pci/cx18/cx18-alsa.mod.o;  true
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ELF                      (W     4     ( ' &             èüÿÿÿS‰Ãh    jèüÿÿÿ‹    XY…ÒtƒP¦ÿÿ[éüÿÿÿt& [Ã´&    ´&    èüÿÿÿUWVSƒì‹°È   F~ø‰$ö    …s   †¨Y  èüÿÿÿö    …_   ‹†4&  …À~‰øèüÿÿÿº   ‰ø®Ü1  èüÿÿÿ”Q  ´&    ‰èƒÅtèüÿÿÿ9İuò®d  ô)  ¶    ‹…Üşÿÿ…Àt‰èèüÿÿÿÅğ  9İuåºˆ   ‰øèüÿÿÿ‰øèüÿÿÿ‹†Ì1  èüÿÿÿº   ‰øèüÿÿÿ‰øèüÿÿÿ‹Fü‰ú‹€L  èüÿÿÿ…ÿt&‹†L&  …Àtö    …‡   èüÿÿÿÇ†L&      ‹†@&  ‹–D&  j h   ‰Ñ‰Â¸    èüÿÿÿ‹Füèüÿÿÿ‹†80  ZY…Àt$®<0  ¸0  ë´&    ‹E ƒÅèüÿÿÿ9İuñ†h  èüÿÿÿÿvdÿt$hY   èüÿÿÿ‰ğèüÿÿÿ‰øƒÄ[^_]éüÿÿÿèüÿÿÿWVS‰Ó‰Şèüÿÿÿ÷ŞöƒÆd‹=    ë´&    …Àt4„Ét0‰ò‡Wèüÿÿÿ1Ò¹   …Ûtã‹â   u!‹Áêƒâ‰Ñƒñ…ÀuÌ[‰Ğ^_Ã´&    v º   [^‰Ğ_Ãt& èüÿÿÿU‰Å‰ÑWzVƒçü‰ÖS)ùƒÁDÁéì  d¡    ‰„$   1ÀÇ    ÇB@    ºÀ  ó«¹ô  ¡$   èüÿÿÿ…ÀtR‰Ã¹   @ºk   èüÿÿÿ…œR  ¹   ‰C¸P   ‰âf‰C‰Øèüÿÿÿ…Àu‹Eh‹ ƒø	u5‰â‰ğèüÿÿÿ‰Øèüÿÿÿ‹„$   d+    u-Ä  [^_]Ã´&    v «   ƒø¡   …Ày¸é«   èüÿÿÿv èüÿÿÿUWV¾   Sƒì|‰$d¡    ‰D$x1ÀğÁ5    ƒş3  ¹   ºÀ  ¸ÀY  èüÿÿÿ‰Ã…À„–
  ‹$‰3S‰T$‰Cˆ   ‰D$èüÿÿÿ‰D$…À…‚  C³<  ‰D$‰Çÿ3h}   j$Pèüÿÿÿÿ3W1ÿh…   èüÿÿÿ¡ü   ‹-Ì   ÇƒT      Çƒ|      ƒÄ‰ƒ@  ¡ø   ‰«d  ‰ƒ<  ¡ô   ‰ƒP  ¡ğ   ‰ƒD  ¡ì   ‰ƒH  ¡è   ‰ƒL  ¡Ğ   ‰ƒh  ¡È   ‰ƒx  ¡Ä   ‰ƒl  ¡À   ‰ƒp  ¡¼   ‰ƒt  ¡ä   ‰ƒ„  ¡à   ‰ƒ€  ¡Ü   ‰ƒ”  ¡Ø   ÇƒŒ  Ë  ‰ƒˆ  ¡Ô   Çƒ˜      ‰ƒ  ‰\$ëo¶    ‹…Û~g‹ND…É~`ƒÿ„²  ƒÿ„©  ¸,   £ø‚Ï  …íˆk  ¯é…í…ÿ  IÅÁø
‰Áá
‰NDö    …„  ƒÇƒÆƒÿt&‹n(…íu“ƒÇÇ    ƒÆÇF$    ÇF@    ƒÿuÚ‹\$¶  ‹‹4…    ‹…   ‹…   ‰³X  ‰“\  ‰ƒ`  €ùM„ÿ  9  €ùB„ë  B  ¸   1Ò€ùHtô  ¸   1Ò€ùI…Ï  ´&    ‰ƒ Y  ‰“¤Y  ƒşÿ„o  Fÿ·Àèüÿÿÿ‰Ch…À…š  ‹«X  …í…å  ‹Cfƒx$p„Ï  ‹t$·Æèüÿÿÿ‰Ch‰Á…À„Œ  ‹à   …ÀtN·f…ÒtF‹kƒÀ·}"ëf·ƒÀf…Òt-f9×uğ·Püf9U$uæ·Pşf9U&„v  ·ƒÀf…ÒuØt& ƒÆëv Áá
ºIÖ¹ò‰È÷ê
Áùº ‡  Áø)È‹L$iÀ ‡  9ĞLÂ‰ˆ  ‹ND…íˆ´  ¯Í…Éÿÿ IÁÁø‰é2şÿÿt& €ù-u{¶  €ùL„-    €ùB„ö  >  €ù-…·  ¶   €ùM„q  w  ¸    1Ò€ùJ„şÿÿ¸ €  1Ò€ùK„şÿÿ€ù-…‚  1À1Òémşÿÿt& €ù6…Ê  ¸   1ÒéSşÿÿ€ùNtn€ùb…V  ¸   1Òé9şÿÿ€ùK…–  ¸à   1Òé$şÿÿ¸à   1Ò€ùD„şÿÿ€ùGtÊéş   ´&    v €ùi„5  ~E¸   1Ò€ùm„åıÿÿ€ùnud¶  ¸   1Òƒáß€ùC…Æıÿÿ¸   1Òéºıÿÿ´&    v €ùd„wÿÿÿNÿÿÿ¸   1Ò€ùg„‘ıÿÿ€ùh…È  ¸   1Òé|ıÿÿ€ùk„Cÿÿÿéş   ¶    ‰ØÁà
™÷ù‰F(é˜üÿÿ´&    ‰ØÁà™÷ù‰F(é†üÿÿÁá
¸«ªªª÷á‹L$Áê
Rº   Áà	9ĞBÂ‰”  éşÿÿ1Àèüÿÿÿ‰Ch‹C·P"R·@ P‹|$Whø  èüÿÿÿ‹C·P&R·@$PWh(  èüÿÿÿ‹ChƒÄ ÿpWhî   èüÿÿÿWhd  èüÿÿÿWh¼  èüÿÿÿWh  èüÿÿÿ‹ChƒÄ$‹P‰Sx‹P‰Sl‹€Ä   ‰Cpƒ»X  ÿ„P  ‹C¹   ³Ø1  ‹T  ‹€P  ‰“L&  º  ‰ƒH&  ƒ(  èüÿÿÿƒX  ¹   º  èüÿÿÿƒˆ   ¹   º.  èüÿÿÿ¹   ºC  ƒœ   èüÿÿÿÿt$hX  jVèüÿÿÿVjh 
 h^  èüÿÿÿ‰ƒÔ1  ƒÄ …À„i  ‹œQ  ƒè1  “ Q  ¶    ‰X‰HlÇ@üàÿÿÿ‰ ‰@ƒÀtÇ@”    9Âuàƒœ  «  ÇƒD&     º2   ‰ƒ”  ‰èÇƒ      Çƒ     Çƒœ      Çƒ˜      èüÿÿÿ‰D$…À…X  ƒ$  ‹“ğ  ¹   ‰CD‹ƒ   ‹€€   ‰ƒ  ‹ƒü  ‹€€   ‰ƒ  ‹ƒì  ‹’€   ‹€€   ÁâÀ	Ğ‹“è  ‚€   ºa  ‰ƒ  ƒ°1  èüÿÿÿƒ˜1  ¹   ºl  èüÿÿÿƒ¤1  ¹   º~  èüÿÿÿƒ¼1  º  ¹   èüÿÿÿƒ`&  “D1  Çƒ\&     ‰ƒ('  ƒl1  ‰ƒl1  ‰“H1  ‰“D1  “X1  ‰“X1  ‰ƒ\1  ‰“l1  ‰“p1  ‰ƒX1  ö    …†  ‹$èüÿÿÿ…À…Å  ‹D$1Éºÿÿÿÿèüÿÿÿ…À…ÿ  j ¸    hŸ  j h   ‹“H&  ‹‹L&  èüÿÿÿƒÄ…À„£  ‹<$L$ º   ‰øèüÿÿÿ·L$ º   ‰øƒÉf‰L$ ·Éèüÿÿÿ¶G,º   L$ˆƒP&  ‰øèüÿÿÿ¶D$<?w‹¸   …Ò…Ã  ‹ƒH&  ‹“L&  ö    …  ¹   èüÿÿÿ‰ƒT&  …À„6     ‰“X&  ‹( Ç‰Ğ%   ÿ=   „  =   ÿ„œ  Rÿt$h˜  èüÿÿÿƒÄé®  ´&    ¸  2 1Ò€ùd„àøÿÿ€   ƒég€ù‡(  ¸   1ÒéÂøÿÿ¸   1Òé¶øÿÿ€ùktK~¿€ùl…(  ¶  ¸  @ 1Òƒáß€ùC…Œøÿÿ¸  € 1Òé€øÿÿ€ùH+¸   1Ò€ùFkøÿÿ€ùDum¸  2 1ÒéZøÿÿ€ùbt‡é(  €ùKtåé(  ¶    ¸   1Òé4øÿÿ¸   1Òé(øÿÿ¸ €  1Ò€ùk„øÿÿ€ùmtß€ùj…  ¸    1Òéş÷ÿÿé(  é(  é  éş   1Àèüÿÿÿ‰Chÿt$hÔ  èüÿÿÿ‹ChZY…À„øÿÿé%ûÿÿéş   ÇD$ôÿÿÿ‹D$xd+    u‹D$ƒÄ|[^_]Ãéş   éş   èüÿÿÿ´&    t& èüÿÿÿS‰ÃìŒ   d¡    ‰„$ˆ   1À‰\$X‹ƒ4&  ©  @ …¸  ğº«4&  ¸    ‚k  ‰Øèüÿÿÿ…À…Æ  ğ€‹4&  j h¹   jh  Sèüÿÿÿj h  SèüÿÿÿƒÄ j jh  Sèüÿÿÿ‰ØèüÿÿÿƒÄ…À…™  j h¹   jh  Sèüÿÿÿj h  SèüÿÿÿƒÄ j jh  Sèüÿÿÿ‹C`ƒÄ…Àt1‹Pd‹…Òt(‹R…Òt!‹    …É„ã   ‹I…É„Ø   èüÿÿÿv ‹“ Y  ÇD$\    ÇD$`   ‰ĞÇD$d   €ô ƒ¤Y  „‘   €ætÇD$d4  ‹‹œY  ‰âA‰ƒœY  1Àèüÿÿÿ‹ƒ¨Y  ‹“¬Y  ƒƒ Y  ‰Áƒ“¤Y   ñÿÿÿ 	Ñu¸   1ÒRP1ÀT$èüÿÿÿ1ÀL$dT$èüÿÿÿ[X1À‹”$ˆ   d+    u0ÄŒ   [Ã´&    ÇD$d´  éoÿÿÿv èüÿÿÿé&ÿÿÿ¸úÿÿÿëÀèüÿÿÿfffffèüÿÿÿfƒø	w·À‹…`  Ã´&    v 1ÀÃ´&    ¶    èüÿÿÿUWV‰Æ‰Ğ·ÒS‰Ëƒì‹Nh|Ñ¶Nvf9Á†§   ‰¶º    ‹,…8  C‰$‰èèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡ü  ‹$‰êèüÿÿÿ€?t6ÇC$   ¶Nw¸   1ÒÓàƒè‰C(¸ÿÿÿ ‰S4‰C01ÀƒÄ[^_]Ã´&    ÇC$   ¶Nw¸   Óàƒè‰C(‹†¨Y  ‹–¬Y  ëÃf¸êÿÿÿëÂ´&    fèüÿÿÿU·êWV‰ÆS‰ËLm ƒì‹@hÇ    ÇC0    DˆL‰Ù‰$C‰Ç‰D$1Àƒçü)ùƒÁ4Áéó«¶Fwf9ĞvW‹$º    ¶ ‹4…  ‰ğèüÿÿÿ‰Áƒøÿ„  ƒø tƒÁƒù ‡  ‹D$‰òèüÿÿÿ‰+1ÀÇC$   ƒÄ[^_]Ãt& ¸êÿÿÿëíèüÿÿÿSƒx‰Ñ‹À% ÿşÿ‹“X&   Qò Â‹…Ét(ƒÊƒX&  ¹
   ´&    v ‰‹9Útƒéuó[ÃƒâşƒX&  ¹
   f‰‹9Útåƒéuó[ÃèüÿÿÿSƒx‰Ñ‹À% ÿşÿ‹“X&   Qò Â‹…Ét(ƒÊƒX&  ¹
   ´&    v ‰‹9Útƒéuó[ÃƒâıƒX&  ¹
   f‰‹9Útåƒéuó[Ãèüÿÿÿ‹ƒxÀ‹’X&  % ÿşÿ„Qò ‹ ƒàÃ´&    t& èüÿÿÿ‹ƒxÀ‹’X&  % ÿşÿ„Qò ‹ ƒàÃ´&    t& èüÿÿÿU‰ÑW¿   VÓçS‰ÃƒìD‹4•à  d¡    ‰D$@1À¶‚   iÀp  ¬œR  ƒÿt]¶‚  ÿ€   „Â   „Àt>j ƒÃ‰ñ‰êP‰ØèüÿÿÿZY…Àt'‰¸   1À‹T$@d+    …B  ƒÄD[^_]Ã¶    ¸ÿÿÿÿëÚÿsp{‰ñ‰êj ‰øèüÿÿÿYZ…Àt
Ç€      ‹Cp‰ñ‰êƒÀP‰øj èüÿÿÿYZ…Àt
Ç€      ‹Cp‰ê‰ñƒÀ
P‰øj èüÿÿÿ]Z…Àt—Ç€      1Àégÿÿÿ¶    ¿şÿÿÿf‰D$1À¹   f‰|$|$“ÜW  ó«‰$‰ğº   èüÿÿÿ‰Áƒøÿ„.  ƒøtƒÁƒù‡$  ‰òD$èüÿÿÿ‹ClÇƒÜW  l  ÇƒôW     ‰ƒàW  ‹$ÇƒäW    ÇƒèW      ‰D$$‰èj L$T$èüÿÿÿ[= ğÿÿ—À¶À÷Øé­şÿÿèüÿÿÿ´&    ´&    èüÿÿÿUhW‰×V‰Æ‰èSèüÿÿÿ‹FV9Ât/X´ë´&    v ‹CLX´9Ât9»   uî‰èèüÿÿÿ‰Ø[^_]Ã‰è1Ûèüÿÿÿ‰Ø[^_]ÃèüÿÿÿUWV‰ÆSƒìö    @…B  †ÌW  Ç$    ®|W  ‰D$F T  ‰D$‹$¹
   ’¼Æ|W  1Àó«iÂp  ‹|$ÇE   ÇE0  ¹œ   ÇE@  ÇE  ÇE   ÇE Ğ  ‰Wº0   ‰7‰} ¼œR  1Àó«Çcx18‰ØÇƒüıÿÿ    ÇC i2cÇC driÇCver ÇC    ÇC    ÇC    ÇC    ÇC     ÇC$    ÇC(    ÇC,    ‰«şÿÿ‹>èüÿÿÿƒø0‡8  „W  Øÿ4$ƒÅ(Ãp  Wh  Pèüÿÿÿ‹|$ƒD$ƒÄ‰»üÿÿ‹~‡ˆ   ‰ƒìûÿÿƒ<$…  ‹†X&   Qò ‹ =/À …  ‹†X&  »  À ˆ Ç ¸
   t& ‰‹ƒúÿtâÀ À tƒèuê»
   t& ¸X‰A èüÿÿÿƒëuñ‹†X&  º
   »À À ˆ Ç v ‰‹ƒøÿt%À À =À   tƒêuæ»
   ¸X‰A èüÿÿÿƒëuñ‹†X&  »  À ˆ Ç ¸
   v ‰‹ƒúÿtâÀ À tƒèuê»
   t& ¸X‰A èüÿÿÿƒëuñ‹†X&  »  À È0Ç ¸
   v ‰‹
ù  À tƒèuï‹†X&  »  À ˆÄ0Ç ¸
   ´&    f‰‹ƒúÿtâ  À tƒèuê‹†X&  »  Pñ ¸
   t& ‰‹
ù tƒèuïƒ¾ĞW  ‹–ÌW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ¶    ‰‹9Ùtƒêuóƒ¾ĞW  ‹–ÌW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ´&    v ‰‹9Ùtƒêuó‹†X&  »  Qò ¸
   t& ‰‹
ù tƒèuïƒ¾ØW  ‹–ÔW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   ¶    ‰‹9Ùtƒêuóƒ¾ØW  ‹–ÔW  À‹ŠX&  % ÿşÿ Qò Á‹	‚X&  ƒÉº
   f‰‹9Ùtƒêuó‹F~X´9Çt2¶    öƒ   @t‹Cd‹ …Àt‹H…Ét	1Ò‰Øèüÿÿÿ‹CLX´9ÇuÔ¾œR  ‰øèüÿÿÿ‰Ã…À„•   ƒÄ‰Ø[^_]Ãv Ç$   é³ûÿÿt& ‹†X&  »   ˆÇ ¸
   ´&    v ‰‹ƒúÿtâ  tƒèuê‹†X&  º
   »  ˆ$Ç t& ‰‹ƒøÿt%  =   „…üÿÿƒêuâé{üÿÿt& †U  èüÿÿÿ‰Ã…À„Vÿÿÿ‰øèüÿÿÿéJÿÿÿ´&    èüÿÿÿVS‰Ãö    @…a  ‹ƒX&  ‹ˆ Pñ ‹ƒX&  ƒÉ Pñ ¸
   ´&    v ‰
‹29ñtƒèuó‹ƒX&  ‹ˆ Qò ‹ƒX&  ƒÉ Qò ¸
   v ‰
‹29ñtƒèuóƒœR  èüÿÿÿƒU  [^éüÿÿÿfffffèüÿÿÿV‰ÆSö    …v  ‹†X&  º
   »  ˆ Ç t& ‰‹ƒøÿt
%  ƒøtƒêuè‹X&  º
   »  Á Ç f‰‹ƒøÿt
%  ƒøtƒêuè[^Ãt& èüÿÿÿW‰Á¿   VS‹€X&  ‰Óˆ€Ç ¸
   ¶    ‰:‹2ƒştƒèuò‹X&  ¿   ° Ç ¸
   t& ‰>‹ƒúÿtâ  tƒèuêƒû‹X&  Òƒâ° €Ç ¸
   ƒÂ¶    ‰‹>9útƒèuóƒû‹X&  Òâ $Iş°€Ç ¸
   Â7¿ï´&    f‰‹>9útƒèuó‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ƒû¾   ƒŞÿ€Ç ¸
   ¶    ‰2‹:9ştƒèuóƒû‹X&  Òâ;Ïó °€Ç ¸
   ÂDÃ0 ´&    f‰‹>9útƒèuó‹X&  ¿   €Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿   @€Ç ¸
   t& ‰:‹2ƒştƒèuò‹X&  ¿şâ+ D€Ç ¸
   t& ‰:‹2şşâ+ tƒèuï‹X&  ¿   H€Ç ¸
   ‰:‹2ƒştƒèuò‹X&   Ç …Û„÷   º
   ¾  ÿÿt& ‰0‹ƒû tƒêuò‹X&  ¾ ÿÿÇ ¸
   t& ‰2‹ƒûtƒèuò‹X&  ¾ ÿÿÇ ¸
   t& ‰2‹ƒûtƒèuò‹X&  ¾ÿÿÇ ¸
   t& ‰2‹û  tƒèuï‹X&  ¾&ÿÿ Ç ¸
   ‰2‹û&  tƒèuï‹‘X&  ¸
   »1ÿÿÂ$Ç ´&    f‰‹
ù1  tƒèuï[^_Ãv »
   ¾  ¶    ‰0‹ƒúÿtâ  ƒútƒëuç‹X&  º
   ¾  ˜Ç ´&    f‰3‹ƒøÿt%  ƒø„ïşÿÿƒêuäéåşÿÿ´&    ´&    èüÿÿÿV1Ò¾   S‰Ã¸
   èüÿÿÿ‹ƒX&  ˆ Ç ¸
   t& ‰1‹ƒúÿtâ  tƒèuê1Ò¸
   èüÿÿÿ‹Ch‹ˆÈ   ‹ƒX&   È ¸
   t& ‰
‹29ñtƒèuó1Ò¸
   èüÿÿÿ‹Ch‹ˆÌ   ‹ƒX&   È ¸
   t& ‰
‹29ñtƒèuó‹Ch‹ˆĞ   ‹ƒX&   È ¸
   ‰
‹29ñtƒèuó‹Ch‹ˆÔ   ‹ƒX&   È ¸
   ´&    f‰
‹29ñtƒèuó1Ò¸
   èüÿÿÿ‹Ch‹ˆØ   ‹ƒX&  H È ¸
   t& ‰
‹29ñtƒèuó‹Ch‹ˆÜ   ‹ƒX&  T È ¸
   ‰
‹29ñtƒèuó1Ò¸
   ¾   èüÿÿÿ‹ƒX&  ˆ Ç ¸
   ‰1‹ƒúÿtâ  tƒèuê1Ò¸
   ¾   èüÿÿÿ‹ƒX&   È ¸
   ´&    ‰2‹
ƒùtƒèuò‹ƒX&  º
   ¾  ˆ$ Ç t& ‰1‹ƒøÿt
%  ƒøtƒêuè‹ƒX&  ¾H   œ È ¸
   ´&    v ‰2‹
ƒùHtƒèuò‹ƒX&  ¾   üÈ ¸
   t& ‰2‹
ù   tƒèuï‹ƒX&  ¾  É ¸
   ‰2‹
ù  tƒèuï‹ƒX&  ¾  $É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  (É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  0É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  4É ¸
   ´&    f‰2‹
ù  tƒèuï‹ƒX&  ¾  ,É ¸
   ´&    f‰2‹
ù  tƒèuï‹“X&  ¸
   »  Â8É ´&    f‰‹
ù  tƒèuï[^Ã´&    t& èüÿÿÿUW‰ÇVS»   ƒìP‰D$d¡    ‰D$L‰ø‹€X&  ‰$L Ğ ¸
   ‰‹
ƒùtƒèuò‹D$º
   »  ‹€X&  ‰$ˆ Ç ‰‹ƒøÿt
%  ƒøtƒêuè1Ò¸   èüÿÿÿ‹D$‹€X&  ‰$ Ç ‹ ¨„Ù  ‹|$º   ‰øèüÿÿÿ‰øºˆ   èüÿÿÿ‹Gº  ‹ŸT&  ÇD$0    ‰$ˆˆ   D$0èüÿÿÿ‰Ç…À…Û  ‹D$0‹‹p‰$…É„s  ‹D$‰ú‰ıèüÿÿÿ‹D$0‰|$Ç   ‰D$‹ ‰$9Ås9t& 9ı}1‹¸
   t& ‰‹9Êtƒèuó‹9Â…¦  ƒÃƒÆƒÅ;,$rË;<$rš‹D$‹€4&  ¨„‹  ‹D$èüÿÿÿ‹t$º  Ü ‰ğèüÿÿÿ‹<$…ÿ°  ‰ğèüÿÿÿ‹F‹¾T&  º3  ÇD$0    ‰$ˆˆ   D$0‰|$ èüÿÿÿ…À…ı  ‹T$0‹z‹2‰Ğ¶_‰|$‰t$ˆ\$$¶_ˆ\$*¶_0ˆ\$+ƒş†™  ‹D$ÇD$   ÇD$    ƒÀ‰D$,ë‹D$‹:ƒÀ‰Æ‰D$‰Ğ9şƒâ   ‹D$‹t$ƒàüD$‹x‰|$‹xş8Segm‰<$‰t$u½xHedru´ö    …
  ‹|$9|$‚   ‹$…Ét~‹D$ ‹t$1ÿğ‰D$‹D$‰û‰ı8‹D$Ç   èüÿÿÿ9$vJ9û}Fv ‹D$‹\$(‹D$4(‰ğƒàü‹¸
   ‰‹9Útƒèuó‹9Â…t  ƒÅ9,$v9ı|½;<$r’‹T$0éÿÿÿ‹D$0‹8‹t$‹–4&  ƒâ„5  èüÿÿÿ‹t$1Ò‰ğèüÿÿÿ…ÿ‹D$Ld+    …=  ƒÄP‰ø[^_]Ã‹†X&  »   ‰$ Ç ¸
   ‰‹
ƒùÿtá  tƒèuê‹t$»2   ëv 1Ò¸
   èüÿÿÿƒë„†   ‹†X&   Ç ‹ ¨uÚ1Ò¸È   èüÿÿÿ‹t$ºˆ   ‰ğèüÿÿÿj jh   D$@PVèüÿÿÿ‰ÇƒÄ…ÀˆEÿÿÿ‹†X&  ¹
   »  Ç ‰‹ƒúÿtâ  ú   t9ƒéuä1ÿéÿÿÿ1Ò¸È   èüÿÿÿ‹D$‹€X&  ‰$ Ç ‹ ¨„oÿÿÿé»  1ÿéÙşÿÿ‹t$‹–4&  ƒât+èüÿÿÿ‹D$º  Ü èüÿÿÿé²şÿÿèüÿÿÿt& ‹|$évşÿÿé‹  fffffèüÿÿÿWVS‹°”   ‰ÃƒÃp¾X  ‰øèüÿÿÿÿ¶ X  ÿ¶üW  Sh¬  èüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_Ã´&    fèüÿÿÿVS‰Ã‹@`‹ˆ˜   …ÉtJö    …-  ‰ÙÁĞX  t3‹ƒ4Y  ‹ …Àt'‹X…Ût ¡    º   …Àt‹p…öt‰È[‰ñ^éüÿÿÿ[^Ãt& ‰È‰Ù[^éüÿÿÿ´&    ¶    èüÿÿÿWVS‹°”   ‰ÃƒÃp¾X  ‰øèüÿÿÿÿ¶ X  ÿ¶üW  Sh¬  èüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_Ã´&    fèüÿÿÿU‰ÅWVSƒì‹üW  ‹€ X  ·ğÁè‰$‰Ó‰D$‹…X&  ·úÁã‰t$‰ÖˆÇ Áî¸
   ´&    ‰‹ƒúÿt…×tƒèuî‹D$‹X&  ‰t$º
   	ÃÁ Ç ‰Æ‰‹ƒøÿt1ğ…øtƒêuì‹…X&  ‹$‹t$˜Ç f1Ò¸
   ‰‹ƒùÿt…Îtƒèuî‹|$‹X&  ¹
   	úÃÇ ´&    ‰‹ƒøÿt1ø…ğtƒéuìƒÄ[^_]Ãt& èüÿÿÿUWVS‹˜”   ‹Ch‹¨œ   …ít5»X  ‹°¨   ‰øèüÿÿÿ3³ X  ‰Ø!î3³ X  ‰³ X  è¶şÿÿ‰øèüÿÿÿ[1À^_]Ã´&    èüÿÿÿUWVS‹°”   ‹Vh‹˜Y  ‹ªœ   I‹‚¤   fƒ|ŠTE‚    …ít3¾X  ‰Ã‰øèüÿÿÿ‹† X  1Ø!è3† X  ‰† X  ‰ğè<şÿÿ‰øèüÿÿÿ[1À^_]Ã´&    ´&    èüÿÿÿUW‰ÇV‰ÖS‰ğ‰Ëƒì	Ø‹L$ ‹T$$‰$‰L$‰T$uƒÄ[^_]Ãt& ¯X  ‰èèüÿÿÿ3· X  ‰ø÷Ö#4$3· X  ‰· X  è½ıÿÿ‰èèüÿÿÿ‹D$èüÿÿÿèüÿÿÿ‰èèüÿÿÿ3Ÿ X  ‰ø÷Ó#$3Ÿ X  ‰Ÿ X  èƒıÿÿ‰èèüÿÿÿ‹D$èüÿÿÿƒÄ[^_]éüÿÿÿ´&    èüÿÿÿS‹˜”   ‹Chƒútƒút_…Òt31À[Ã´&    ‹˜   ÿ°”   1Éÿ°   ‰Øèışÿÿ1ÀY[[Ã´&    ‹ˆ   ‹ˆŒ   ÿ°”   ÿ°   ‰ØèÑşÿÿX1ÀZ[Ãv ƒ¸´   Guœ¶H|jº   ‰ØjÓâ1Éè§şÿÿXZë€v èüÿÿÿUWVSƒì‹°”   ƒútƒút|¿êÿÿÿ…ÒtcƒÄ‰ø[^_]Ã‹Fh‹˜¤   ‹œ   1ÿ…Ò‰$tİ®X  ‰èèüÿÿÿ‹$3 X  ‰ğ!Ó3 X  ‰ X  èMüÿÿ‰èèüÿÿÿ‰øƒÄ[^_]Ãt& ‹Fh‹˜    ë¦t& ‹Fh‹˜¨   ë–´&    ¶    èüÿÿÿUW¸X  VS‰Ã‰øèüÿÿÿ‹Ch‹€   ‰“üW  ‹°„   ‰³ X  ƒ¸´   Gu ¶H|½   Óå	ê‰“üW  ¶@|«Æ‰³ X  …Òtö    u‰Øè ûÿÿ[‰ø^_]éüÿÿÿ‹ƒX&  ‹Ç ‹ƒX&  ‹ˆ Ç ‹ƒX&  ‹°Ç ‹ƒX&  Ç ‹ RQVPCPh  èüÿÿÿƒÄë¦´&    ¶    èüÿÿÿUWV‰ÖS‰Ãƒú tƒú@ta[¸êÿÿÿ^_]Ã´&    ¸X  ½F  º   ‰øèüÿÿÿC‰Ÿ”   UPGph_  j Pèüÿÿÿ‰·   C‰úƒÄ[^_]éüÿÿÿ´&    ¸ØX  ½O  º@  ë®¶    èüÿÿÿ…ÉugVS‹ ‹‹Chƒ¸´   GuMö    …B  ‰ŞÆØX  tV‹ƒ<Y  ‹ …ÀtC‹H…Ét<¡    º   …Àt%‹X…Ût‰ğ‰Ù[^éüÿÿÿv 1À[^Ãv 1ÀÃt& ‰ğ[^éüÿÿÿ¸ııÿÿëâ¸íÿÿÿëÛèüÿÿÿUWxV‰ÖS‰Ã‰øƒìèüÿÿÿF‰$èüÿÿÿ‹9ÃtKnf‹‹P‹‰Q‰
‹V‰F‰0‰P‰Ç@     Ç@$    Ç@    Ç@    Ç@    ğÿE ‹9Øuº‰‰ø‰[ÇC    ÇC    èüÿÿÿ‹$ƒÄ[^_]éüÿÿÿèüÿÿÿ‹P…ÒtS1Ò‹XÓƒÂ‹É‰9Pwí[Ãv Ã´&    èüÿÿÿVpS‹X9ót0´&    ‹C…Àt"1À´&    ‹KÁƒÀ‹Ê‰9Cwí‹9óu×[^Ã´&    t& èüÿÿÿ‰ ‰@Ç@    Ç@    Ã´&    èüÿÿÿUWV‰ÖS‰Ã¨  9Át#ÇB     ÇB$    ÇB    ÇB    ÇB    ƒ”  9ÁtM‰Ï‰Ëk‰èèüÿÿÿ‹D$…Àt'‹‰p‰‰~‰3‹F +F$CğÿC‰èèüÿÿÿ‰Ø[^_]Ã‹C‰s‰>‰F‰0ëÖ‹Aƒø>~«Ã€  ‰ßë¥´&    ´&    èüÿÿÿWzV‰øS‰Óèüÿÿÿ‹9Ãt6‹3‹F‹‰B‰‹F$‰6‰vC+F ‰CÇF    ğÿK‰øèüÿÿÿ‰ğ[^_Ã‰ø1öèüÿÿÿ‰ğ[^_ÃèüÿÿÿUWVS‰Ã³¤  ƒì$‰L$‰$d¡    ‰D$ ‹ƒ8  ‰D$D$‰D$‰D$‰ğèüÿÿÿ‹«”  ƒ”  ‰D$‹M 9è„¸  ‹D$‹U‰t$‰ïƒÀ‰D$;$t4‹EƒÀ‰E‹³œ  ƒî9ğƒW  ‹‰Ï‰Í;L$„q  ‹U‰Á;$uÌ‹U ‹E‹t$‰B‰‰} ‰}ğÿ‹œ  ‰ğèüÿÿÿ…í„   ‹D$‹uÇE    ‰E E‰$‰Ç‹E9Çt	9u„Ò   ‹»t  ;4$t[‰l$‰ı‹|$ë8v )ï‰è‹V‹N‰F‹ƒ8  ‹@ÿ³X  ÿ³t  ˆ   èüÿÿÿ‹6XZ94$tÇF    9ıvÀ‰ø1ÿë¾f‹l$ƒ»@  tğ€M‹T$L$»€  ‹2‰Ğ9Êu
ë/¶    ‰Î‹J‰N‰1‰ù‰‰B‰Øj èüÿÿÿ‹‰ğ‰ò^t$9ğu×‹D$ d+    u]ƒÄ$‰è[^_]Ã‹D$‹VÇF    ‰F‹‹8  ‹F‹qÿ³X  ‰Ñÿ³t  ‰ÂÆˆ   ‰ğèüÿÿÿY^éSÿÿÿ‹t$‰ğ1íèüÿÿÿéOÿÿÿèüÿÿÿ´&    èüÿÿÿV°€  S‰ò‰Ã€”  èdûÿÿƒ¨  ‰ò[^éUûÿÿt& èüÿÿÿUW¸¼  V‰ú‰Æ”  S®h  ƒì‰<$è'ûÿÿ†¨  ‰úèûÿÿ†€  ‰úèûÿÿ†Ì  ‰D$èüÿÿÿ‹¼  9ßtYt& ‹C{9øt;¶    ‹C‹P‹‰Q‰
‹–l  ‰†l  ‰(‰P‰Ç@    Ç@    ‹C9øuË‹†L  ‰C‹9$u«‹D$ƒÄ[^_]éüÿÿÿ´&    t& èüÿÿÿUWVS‰Ãƒì‹¸8  ‹€L  ‰D$ƒ¼  ‰Â‰D$‰Øèüÿÿÿ“€  ‰D$‰Æ‹ƒx  ‰T$“h  ‰$‰T$…ö„/  9$…&  v ‹D$‹t$‰p‹‹x  …É„m  ƒÀÇ$    4õ´  ‰D$´&    v ‹ƒh  ;D$„ã   ‹ƒh  ‹‹P‰Q‰
‹L$‹Q‰A‹L$‰P‰‰‹—„   L2ü‹P¸
   ´&    f‰‹)9êtƒèuó‹„   ‹“t  ¸
   ñ‰‹)9êtƒèuóƒ$‹ƒx  ƒÆ‹$9È‡kÿÿÿ‹t$Î‰õ9Èu[‹ƒ|  1Ò÷³t  …Òuaj ‹T$‰Ø‹L$èüÿÿÿZ‰l$‹T$‰Øèüÿÿÿ‰D$…Àt‹ƒx  9$„İşÿÿƒÄ[^_]Ã‹$‹t$Æj‹L$‰Ø‹T$èüÿÿÿ‰t$Xëµ‹$‹t$Œ0•  ‹‡„   ¾
   DÈ´&    ‰‹9Ê„oÿÿÿƒîuïéeÿÿÿÇ$    ‹l$éCÿÿÿfèüÿÿÿUWVSƒì‹¨X  ‹¸t  ‹€8  ‹Z‹pB‰$Æˆ   9Ãt t& ‹C‹SUW‰Ñ‰Â‰ğèüÿÿÿ‹XZ;$uäƒÄ[^_]Ãt& èüÿÿÿWV‰ÆSö    …£  ‰ğ¼  èüÿÿÿë´&    v èüÿÿÿ‰Ú‰ğèüÿÿÿ…Àuî‹†h  ¾h  9Çt^¶    ‹h  ‹‹C‰B‰‹S‰‹K‰[‹†8  ‹@j ÿ¶X  ÿ¶t  ˆ   èüÿÿÿ‹Cèüÿÿÿ‰Øèüÿÿÿ‹†h  ƒÄ9øu¨[^_Ã´&    t& èüÿÿÿ‹ˆp  …É„  U‰ÅWVSƒì‹€8  ‰D$ö    …   ‹D$‹€   „–  Áà=   ñ  ‰•L  …¼  1ÿ‰D$é›   ´&    ‚   @j âÿ  ‰ÁÁèÿt$Áéÿt$€‹Í    ƒáàÁ‰Ñ‰Â‰ØèüÿÿÿƒÄ‹8  ‰FƒÇ‰V‹YÿµX  ‰Ñ‰Âÿµt  Ãˆ   ‰Øèüÿÿÿ‹…l  •h  ‰µl  ‰‰F‰0‹…p  ZY9ø†8  ¡   ¹(   ºÀ-  èüÿÿÿ‰Ã…À„>  ¡   ¹   ºÀ-  èüÿÿÿ‰Æ…À„(  ‹…t  ºÀ,  èüÿÿÿ‰F…À„Í   C‰‰Ú‰C‰C‹…L  ‰[‰C‰èj ‹L$èüÿÿÿ‹…X  ‹V‰6‰v‰D$‹…t  ‰T$‰D$‹…8  ‹H‰Ğ‰L$™ˆ   èüÿÿÿZ‹$„À‹L$„¥şÿÿ€=     ¸ÿÿÿÿºÿÿÿÿ…Êşÿÿ‹‘´   Æ    …Òte‰Ø‰$èüÿÿÿ‹$RPhd  èüÿÿÿ¸ÿÿÿÿƒÄºÿÿÿÿéŒşÿÿ1ÀÃt& ‰Øèüÿÿÿ‰ğèüÿÿÿ‹…p  9Ç…Å  ‹|$‡€   1ÀƒÄ[^_]Ã‹‘ˆ   ë“‹…p  ëÔ‰Øèüÿÿÿ‹…p  ëÅffffèüÿÿÿV‰ÆS‹€Ø   ‰Ó‹€8  ·  ·€  ¯ĞÒ‰‹…ÀuÇ    ‹¸    ¯Ğƒèú  évv ‰‹¯Ğƒèú  éwîÇF4   1À[ÇF8   ^Ã´&    t& èüÿÿÿS‹€Ø   Z(ÇB   ‹ˆğ  ‰˜ğ  ì  ‰B(‰J,‰[Ã´&    ´&    èüÿÿÿU¨”  W¸€  VS‰Ãƒì¶    ‰ú‰Øèüÿÿÿ‰Æ…Àt?‹ƒ8  ƒ»D  ÿ‰$t?‹ƒT  öÄu4‹ƒT  ¨t*j ‰é‰ò‰ØèüÿÿÿY9Åt8‹ƒœ  ƒÄ[^_]Ã´&    v j ‰ò‰ù‰Øèüÿÿÿ‹“œ  ^ƒú>Õ9ètƒëÏ‹VF9Ât‹F;Ft^‰ò‰Øèüÿÿÿ‹Fÿ³|  Pÿ³x  ‹L$‹‘„   „Â°  +T&  Pÿ³D  jh  Qèüÿÿÿ‹ƒœ  ƒÄ ƒø>ÿÿÿéaÿÿÿv ‹P‹H‹ƒ8  ‹@ÿ³X  ÿ³t  ˆ   èüÿÿÿXZë‚´&    èüÿÿÿ‹P¸…Òt‹PÌƒú>-Ğ  é¢şÿÿfÃ´&    ´&    èüÿÿÿS1É‰Ój èüÿÿÿ‰ØèüÿÿÿÇC    X[Ã´&    ´&    èüÿÿÿUWVSƒì‹°Ø   ‰D$‹®8  ‹†à  ‰L$·  ·½  ‰D$ÇBl    ‰$ƒÿ/†>  ƒû†5  ‹$‹BT¯ß‰ÙÉ‰J…À…  ;z„  ‹$‰z‰B‹D$‰B‹D$‹ Y  ‹¤Y  ‰Jd‰Zh‰†à  =HM12„4  i$   ‰†ä  1Éj ‹D$‰T$èüÿÿÿ‹D$èüÿÿÿ‹T$‹BTÇB    Y…Àt‹B9BL‚—   ‹Z…ÛuÇB   ‹J…Éu_‹$‰z‰B‹D$‰B‹D$‹½ Y  ‹­¤Y  ‰zd‰jh‰†à  =HM12„â   i$   ‰ä  ‹D$1É‰$èüÿÿÿ‹$…À‰Ã…Š   ÇB   1ÛƒÄ‰Ø[^_]Ã´&    ;JL†ãşÿÿ»êÿÿÿëß‹$;J…Ùşÿÿ‹\$;Z…Ìşÿÿ‹\$;à  …¼şÿÿ‹Jh‹Zd3¤Y  3 Y  	Ù…¢şÿÿé
ÿÿÿ´&    i$p  ÑèéÅşÿÿfj ‹D$1Éèüÿÿÿ‹D$èüÿÿÿ‹T$ÇB    XéXÿÿÿ´&    fi$p  ÑëéÿÿÿfèüÿÿÿW‰×VS‰Ã‹°8  ‹€¬  …À„8  ö    …a  ‹†<&  …À„ë   ğ€‹U  ‹ƒ@  …À„ß   ÿ³D  jh  VèüÿÿÿƒÄ‹ƒ@  …ÿt…À„‘  ƒøtğÿ8&  ğÿ<&  ğ€£T  ïÿ³D  jh  Vèüÿÿÿÿ³D  jh  @VèüÿÿÿÇƒD  ÿÿÿÿğ€£U  ı‹†<&  ƒÄ …ÀM†  1Òèüÿÿÿ‹–X&  ¸
   ¾   ÂL Ğ t& ‰2‹
ƒùtƒèuòj ƒ\  ¹   º   èüÿÿÿX1À[^_Ãt& 1À…ÿ”ÀPÿ³D  jh  VèüÿÿÿƒÄéÿÿÿt& ‹ƒ4  …Àt‹€ü  …À…°şÿÿƒ»@  ¸êÿÿÿuª‹x  …É…”şÿÿëšé¬  ´&    ¶    èüÿÿÿUWVS‰Ãƒìp‹³8  d¡    ‰D$l1À‹ƒ¬  …À„²  ö    …d  ‹»@  ƒÿ„¦  P  ƒÿtƒÿ…b  ¿   ğ€£T  h   jh  @l$ UVèüÿÿÿ‹D$(‰ƒD  WPjh  VèüÿÿÿƒÄ(ƒÿ…¹  ‹†<&  …À„Ó  ‹ƒ@  ‹®„   ‹–T&  ÁàŒH  „@  )Ñ)ĞQPÿ³D  jh  Vèüÿÿÿ‰Øèüÿÿÿ‹ƒ@  ƒÄƒø„  ƒø„  Çƒx     ‹ƒt  ‰ƒ|  ‰Øèüÿÿÿ‹ƒˆ  …Àt‹ƒœ  ƒø>‰ØèDùÿÿÿ³D  jh  VèüÿÿÿƒÄ…À…P  ƒÿtğÿ†8&  ğÿ†<&  ‹T$ld+    …  ƒÄp[^_]Ã´&    v …ÿ„è  ƒÿ…_  ¿   é¨şÿÿt& ƒÿ…G  ¾Ä0  À0  1ÀÇ†8'      Ç†@1      ƒçüÇ†À0      )ùÇ†<1      ƒé€Áéó«¿   éMşÿÿ´&    v Çƒx     ‹ƒ8  ƒ¸\&  „3  €xuÀ%
  @&  ‰ƒ|  éÑşÿÿ´&    j ÿ³D  jh  Vèüÿÿÿjjÿ³D  jh  VèüÿÿÿƒÄ,j jÿ³D  jh  Vèüÿÿÿjjÿ³D  jh  Vèüÿÿÿ‹†8&  ƒÄ0…À„2  h9  h8  ÿ³D  jh  VèüÿÿÿƒÄöFx…(  ‹ø  ¸   …É„k  Pÿ³D  jh  UVèüÿÿÿ‰”  †  èüÿÿÿ‹®à  ƒÄ…í„~  ƒÿ…\ıÿÿ»à  UYVY‹ƒD  „·  j Pjh#  Vèüÿÿÿ‹†<&  ƒÄ…À…3ıÿÿ¶    †  º   èüÿÿÿğ€¦4&  ï‹†X&  ½   L Ğ ¸
   ´&    v ‰*‹
ƒù„èüÿÿƒèuîéŞüÿÿ´&    f‹ƒ4  …Àt‹¨ü  …í…6üÿÿƒ»@  u‹¾x  …ÿ…ñ  ¸êÿÿÿécıÿÿ¶    »à  HM12‹ƒ8  ·€  t'iÀ   1Ò‰ƒ|  ÷³t  ƒúƒØÿ‰ƒx  éËüÿÿfiÀp  ÑøëÕ¶    €xuÀ%°C  `‡  ‰ƒ|  éüÿÿt& ¡    ¿   Ç†1      Ç†”1      ‰†   ‹†°  Ç†ˆ1      Ç†Œ1      èüÿÿÿÇ†@&      ‰†$  é{ûÿÿ´&    ¿   éiûÿÿ¶    jÿ³D  jh  VèüÿÿÿƒÄé±ıÿÿv ‹‹8  ¶Au‰Ê‰L$<ˆ$ÉˆD$ƒáƒÁ<À‰Š,'  ‰ÑƒàüƒÀ
‰D$¶$‰$‹T$<À‰‘0'  ƒà-  ƒ¹\&  ‰4'  ‹A`„–  …ÀtR‹Pd‹R…ÒtH‹R‰$…Òt>‹T$‹   Â`&  …É„ç  ‹   ‹I…É„Ö  èüÿÿÿ‹T$¶BuˆD$t& ¶D$‹‹D  ÇD$ğ°ğ°Ç$à à <‰L$Ò‰L$,â  Â  <À‰T$0ƒà  ‹L$‰D$4ÇD$8   ‰L$<‹$‰L$@ö    …Ö  D$,¹   º  P‹D$èüÿÿÿ‹ø  X¸   …É…•üÿÿ‹–€  …Òt‹’ü  …Ò…}üÿÿ1Àƒ¾Œ  …nüÿÿ‹†„  º   ‹€x  …ÀEÂéSüÿÿ´&    ‹†4&  ¨ „tüÿÿ‹†ä  èüÿÿÿÁàƒÈPÿ³D  jh  VèüÿÿÿƒÄéGüÿÿt& ö    „Qùÿÿéÿ  ¶    jéDüÿÿ…Àt8‹Pd‹R…Òt.‹R‰$…Òt$‰Ê‹   Â`&  …ÉtR‹   ‹I…ÉtEèüÿÿÿ‹D$‹“D  ÇD$` ` Ç$Ğp0‹€,'  ‰T$‰T$,À‰ÂÁàÑú	Â¸¤  ‰T$0é†şÿÿ‹$ë¶‹$èüÿÿÿ‹D$¶@uˆD$é&şÿÿö    …Á  ğ€‹U  ‹ƒ@  …À…¢   jÿ³D  jh  VèüÿÿÿƒÄğ€£T  ïÿ³D  jh  Vèüÿÿÿÿ³D  jh  @VèüÿÿÿÇƒD  ÿÿÿÿğ€£U  ı‹†<&  ƒÄ …À…Êûÿÿğ€4&  ‹–X&  ¸
   »   ÂL Ğ t& ‰‹
ƒù„›ûÿÿƒèuîé‘ûÿÿÿ³D  jh  VèüÿÿÿƒÄé[ÿÿÿèüÿÿÿéÿ  ´&    t& èüÿÿÿU¨œ  WV‰Æ€ˆ  S1Ûƒì‰D$†L  ‰$‰D$ë5ƒûtk‹…¬  …Àtƒû„˜   ‰èèüÿÿÿ‰èèüÿÿÿƒûtuƒÃÅğ  ƒûuÇ‹†À  …Àtè‹$¾Œ  …Òupèüÿÿÿ‰øÇ†À      èüÿÿÿëÃ´&    ‹x  …Ét±Ç†x      ‹–¼  …Òt‹D$èüÿÿÿë’´&    ƒÄ[^_]Ã´&    ‹D$èüÿÿÿéZÿÿÿf‰øèüÿÿÿ‹†À  ëèüÿÿÿU¨œ  ºµ  WV‰Æ@S1ÛƒìÇ$Ì  ÇD$    ‰D$‹>‹Fx‰|$‹=    Ç…¬      ‰|$Ç…4      ‰µ8  ‰@  ‰•<  ƒû„m  ƒû„¤  ‹$‹ ‰D$ƒøt‹Œd  …É„  }‰é1ÀÇE     Ç…ì      ƒçü)ùÁğ  Áéó«‰µ8  ¹¤   Ç…4      ‹D$‰@  ‰•<  ‰…X  ‹$Ç…D  ÿÿÿÿ‹@‰…H  ‹„d  ‰…p  ‹”€  …h  ‰•t  ‰ ‰…l  …\  ‰•|  ºÜ  Ç…x     èüÿÿÿ…€  Ç…P  ÿÿÿÿÇ…      èüÿÿÿ…”  Ç…¤      èüÿÿÿ…¨  Ç…¸      èüÿÿÿ…¼  Ç…Ì      èüÿÿÿ…Ô  º    1ÉÇ…Ğ  àÿÿÿ‰ ‰…Ø  …ì  Ç…Ü      ‰ ‰…ğ  …ø  j j èüÿÿÿÇ…ô      XZƒû„m  ƒû„D  ‹|$ƒÿÿ„‰   …À  ÿµ<  ÿt$hæ  j Pèüÿÿÿ‹D$ D$$1ÒÇ…à   `  øƒÄf‰…ì  F‰…¬  ¸ÿÿÿ Ç…      ‹Nh€yu‹†¨Y  ‹–¬Y  ‰…  †(  ‰…0  ‰è‰•  èüÿÿÿ‰èèüÿÿÿ…Àx[C‰ÂƒøtA’ƒ$Åğ  ‹•È  ‹•À  ‰\$‰ÃéTıÿÿv ©   …‘ıÿÿ‰èèüÿÿÿ‰Ç…ÀxƒÄ1ÿ[‰ø^_]Ãt& ‰Ç‰ğ1Òèüÿÿÿ‰øƒÄ[^_]Ãv ¨P…Tıÿÿ‰èèüÿÿÿ…ÀˆÙ   º   ¸   éqÿÿÿ´&    v ‹Fhö@…‹   Ç†ü      é şÿÿt& –(  ‹FÇ†d      R–d  Uˆˆ   †ˆ  jpÇ†h     jjRº   èüÿÿÿ·†  Ç†\  HM12Ç†d  Ğ  ƒÄiÀp  Ñø‰†`  é)şÿÿt& ¡,   ¹  ºÀ  èüÿÿÿ‰†À  …À…şÿÿé2  ‰Çéóşÿÿ´&    fèüÿÿÿU‰ÅW1ÿV¾   S˜œ  @ƒìÇD$    ‰D$‰,$éœ   ¶    ‹“¬  …Òtn‹“H  ·«ì  ‰“ä   …À…¶   ‹ƒà   ‰›(  ‰é‰úÿ0‰Øj èüÿÿÿY‰ÂX…Òˆë  ‹‹  …Éu‹‹è   ƒÿ„}  …ÿ„š  ƒÿ„Â   ‰ğƒş„…   €ƒÆÃğ  ‹<…Ä  Fÿƒş…^ÿÿÿ‹$‹¨À  …ít‰Øèüÿÿÿ…ÀˆS  ‹ƒ¬  …Àth‹ƒH  ·«ì  ‰ƒä   ‹$‹H  …À„9ÿÿÿ¶·‰ˆ  …´  ‰Íé!ÿÿÿt& ‹T$‹,$…Òtº   ‰èèüÿÿÿ‹D$ƒÄ[^_]Ãt& ¸   éIÿÿÿé  ´&    fèüÿÿÿVS‹°ø  ‰Ã…ötM‹“Ô  ‹ƒè  Ğƒø4‹ƒü  ƒø~)³L  “ô  ‰ğèüÿÿÿ‰Â…Àtj ‹Ì  ‰ğèüÿÿÿX[^Ãv ‹€€  …Àt
‹ˆü  …ÉuŸƒ»Œ  uİ‹ƒ„  ‹x  …Òu†ëËèüÿÿÿW‰ÇV¾   S˜<!  ë)´&    fiÆğ  ø‹€ğ  ¨uNƒîëğ  ƒşÿtW‹‹¬  …ÉuÖ‹ƒ4  …Àt
‹ü  …ÒuÂƒ»@  uË‹ƒ8  ‹€x  …Àu©ë¹´&    ‰Ø1Òƒîëğ  èüÿÿÿƒşÿu©[^_Ãt& èüÿÿÿH  Ø(  ‹
…Ét‹Š˜   ƒùÿuÂğ  9Âuå¹ÿÿÿÿ‰ÈÃ´&    ¶    èüÿÿÿVSƒúÿt^ˆœ  ,&  ´&    f‰Ë9‘D  u6‹±¬  …öt‰Ø[^Ã´&    ‹±4  …öt
‹¶ü  …öuàƒ¹@  tv Áğ  9Áu¶1Û‰Ø[^Ã´&    ‹±8  ‹¶x  …öu¬ëÖfffèüÿÿÿVS‰Ã‹pXƒú„€  iÊğ  ‰Ğ”ğ  ğº*sE‹Œì  ‹[P9Ùt)‹    ƒâƒø…­   ƒùÿ…¤   ‰¼  …Ò…i  1À[^Ã´&    f‹SP‰”ì  …Àuä‹–<'  †L  …Òt*ƒ¾\&  t!†l  ğ€ˆT  ğ€ˆT   1À[^Ã¶    ‹ø  …ÛuÛ‹–€  …Òt
‹Šü  …ÉuÇƒ¾Œ  uƒ‹–„  ‹’x  …Ò„oÿÿÿë¨v …Ò…š  ¸ğÿÿÿéZÿÿÿ¶    èüÿÿÿVS‰Ã‹°8  Ç€P  ÿÿÿÿ‹€@  ƒøtƒøtoğº³T  rö    …¶  [^Ãt& ‰Øèüÿÿÿ‹ƒ@  …Àuè†L  ğº¶   r>†l  ğº¶À  sÆƒ¾¼  ÿu½ğ€¦À  ÷[^éüÿÿÿt& ‹ƒT  ¨ t‡ët& ğ€¦   ÷èüÿÿÿë³´&    fèüÿÿÿUWV‰ÆSƒì\d¡    ‰D$X‹†°  èüÿÿÿ¹   l$‰$1À‰ïó«‹F~9ÇtEX´¶    ‹Cd‹@…Àt‹H…Ét	‰ê‰Øèüÿÿÿ‹CLX´9ÇuÜƒ|$<uöD$8¸   D$‰$‹†$  ;$t9ö    …Ò  ‹¾°  ‹G‹@èüÿÿÿ‹$‰øèüÿÿÿ‰Ã‹G‹@èüÿÿÿ…Ûu‹D$Xd+    uƒÄ\[^_]Ãv ö    tßéì  èüÿÿÿ´&    ¶    èüÿÿÿUWVSƒì‹HT‹XXiéğ  ‰$<+‹—Ü  ƒú„«  èüÿÿÿ‰Â…À…¬  ‹‡ğ  ´+ğ  „Àˆ§   ğº.‚œ   ƒl  ¬+œ  ‰D$ƒL  ‰D$‹‡Ü  …Àu#‹ƒ   ¨ tğº«   ƒ¦   ‹ƒÀ  ¨ ul‰èèüÿÿÿ‰Â…À…Ã   ğ€Nğº³4&  s8i<$ğ  ‰D$ÿ´;à  jh  Sèüÿÿÿ‹T$ƒÄƒÄ‰Ğ[^_]Ãv ğ€NƒÄ‰Ğ[^_]Ã´&    fğº«À  r‰‹D$èüÿÿÿ…À…ğ   ö    „kÿÿÿéO  f‹D$èüÿÿÿ…Àuö    „@ÿÿÿé%  ö    …d  ğ€£   ït& ö    …  i<$ğ  ‹„;Ü  …Àu)‹ƒ   ¨uQ‹ƒÀ  ¨t‹ƒÀ  öÄtQ´&    v ğ€&ï‰èèüÿÿÿºûÿÿÿé(ÿÿÿv ºÿÿÿÿéÿÿÿ¶    ºğÿÿÿéÿÿÿ‹D$1Òèüÿÿÿğ€£   ïëš‹D$1Òèüÿÿÿğ€£À  ïë¤ö    …:  ğ€£À  ïéBÿÿÿ´&    v èüÿÿÿUWVS‰Ãìˆ   ‹³Œ   ‹„$œ   ‰T$,‰L$0‰D$`d¡    ‰„$„   1À‹FX‰D$8‹FT‰D$H¡    %   =   „E  ‹D$8¸(  ‰øèüÿÿÿ‰ğèüÿÿÿ‰Å‰D$L‰øèüÿÿÿ…íudi|$Hğ  ‹[ ‹D$8ã   ƒ¼8ˆ  ‰|$d‰\$ u
ƒ~T„’  iT$Hğ  T$8‹‚Ô  ‹L$0‰D$D¡    …Éu3%   =   „¢  ‹„$„   d+    …ö  ‹D$LÄˆ   [^_]Ã‹\$D‹‹8&  …Éuƒºì  ÿ„·  iD$Hğ  ‹\$8ÇD$\    ‹„Ü  ƒø„«  ‹\$8‹t$dÇD$$    iT$Hğ  ¼3œ  ‰|$@¼3D  ‰|$¼3ø  Ú‰|$X¼3  ´3l  ‰t$l‰T$‰|$h‹|$Dt& \$|·l  ÇD$p    ‰\$|‰õ‹t$@‰œ$€   ‹\$Xd‹    ÇD$x    ‰T$të7t& T$p‰Øèüÿÿÿd¡    ‹â   …±  ‹ ¨…§  ‹D$‹€Ü  …ÀuD‹    ¸è  )Ğ‡   ˆˆ   ‹‡À  ¨ t‹‡À  öÄ„   ‹‡d1  9‡h1  …  ‹T$‰ğèüÿÿÿ…À…š  ‹†T  ¨„Ü  ‹D$ …À…0  ‰Ø¹   T$pèüÿÿÿ‹†°  …À…:ÿÿÿèüÿÿÿé0ÿÿÿ´&    ¡    ‰‡   ‰øèîùÿÿéaÿÿÿ´&    fì  ‡  ‰t$‰L$<  ‰\$‰Î‰Ãë8t& ‹¬  ‰ø‰$èüÿÿÿ‰èj ‹L$‹T$èüÿÿÿ‹    ‰ñ¸    èüÿÿÿX‰Ú‰èèüÿÿÿ‰Â…Àu½‹t$‹\$‹‡d1  9‡h1  „şşÿÿv ‰ıÅD1  „  ‡X1  ‰D$‹u‹D$0+D$$‰D$…ö„Ù  ‹D$DÇ$    D1  ‰D$<;t$u!éú  ´&    v ‹E‹0‰u;t$„.  ‹V‹F9Âsä‹$‹L$$)Ğ‹|$ÙL$,‰L$‹Ô  ‹|$‰L$()ß‹^9ÇGø‹<'  Ú‰\$‰T$1Û…À„Û   ‹D$‹€Ü  …À…É   ƒ¹\&  „¼   l1  9Æ„®   ƒ¹@&  BÉƒá&ƒéFˆL$7:9Á†   ‰|$T‰Ë‹|$‰l$PëQt& ‹Nj@ù9Ís9€z u3€zu-¶L$78Ju#‹L$(‹‰@&  …É…Á  ¶JƒáÀ€ù€„‘  9Ãv‰Ù1Ò)Áèüÿÿÿ‰Â…Àu£‰øF‹l$P1Û‰D$‹|$T´&    …ÿˆ   ‹D$¹   ‰úèüÿÿÿ‹T$‹D$‰ùèüÿÿÿ…À…Û   ‹D$~‹€Ü  …Àu‹L$(l1  9Æt1Ò¹ˆ1  ‘Œ1  ‹U$<$‹$ú‰U$9D$†|  „Û…t  ‹E‹X9X‚U  ;U ‚/şÿÿ‹$‹E ;l$<„Œ   t& 9E$„  j‹L$ ‰ê‹D$Dèüÿÿÿ^‹D$‹¸Ô  …ÛˆP  ‹$\$$‹D$$9D$0„  ‹\$\…Û…‚  ‹D$‹€Ü  é£ûÿÿv ö    …  Ç$òÿÿÿ‹E »òÿÿÿ;l$<…xÿÿÿ9E$u‹t$D‹†@1  ‰ÂƒÀƒâÇ„–À0      1Ò‰†@1  ‹E †1  –”1  éUÿÿÿf‹u‰uéıÿÿt& ¶J‰Íƒåƒát1É€|
ÿ…5şÿÿƒÁ9Íuî¶JƒáÄ€ùD…şÿÿ¶Jƒá€ù…şÿÿ€|* …şÿÿ€|* …øıÿÿ€|*…íıÿÿ‹\$(‰×‹l$P+|$Çƒ@&      ‹ƒ@1  ƒà‹”ƒ@0  ‰“|1  ‹„ƒÀ0  Çƒ„1      ‰ƒ€1  Çƒ`1      ‰ƒd1  Çƒh1      ‹F»   F‰D$éªıÿÿ´&    v ¶JƒáÀ€ù€„   €ùÀ…Wıÿÿ¶J	ƒáğ€ù0…Gıÿÿ‹D$(ÆD$7ºÇ€@&     B	‹~é)ıÿÿ´&    f‹E 1Û‰E$;l$<„sşÿÿj ‹L$l‰ê‹D$Dèüÿÿÿ‹    ‹L$p¸    èüÿÿÿ_éÖıÿÿv ‹$éŸıÿÿ´&    ‹$éıÿÿ´&    ‰Åğºp ]’À‰\$„À„Vûÿÿ‹D$‹ˆÜ  …É…¬   ‹E9Øt‹E;E„#  ‰èèüÿÿÿé%ûÿÿt& ö    …y  ƒ|$$ „  ¡    ‹\$$%   ‰\$L=   „c  ‹\$L…ÛHøÿÿ‰Ø‹\$`™Sé7øÿÿ¶    ¡    ¨…æ  ‹L$$…Éu°ÇD$Lüÿÿÿéÿ÷ÿÿ¶    ‹$é¯üÿÿ‰ê‰øèüÿÿÿéŠúÿÿ‹\$Dƒ»\&  •Ã¶Û‰\$\é;øÿÿ¶J	ƒáğ€ù …°ûÿÿédşÿÿ¡    ‰\$L%   =   …«÷ÿÿé¢  t& ÿt$ „8¨  j ÿt$h‹L$<‹T$8èüÿÿÿ‰D$XƒÄéw÷ÿÿèüÿÿÿéúÿÿƒ|$$ ¡    …ñşÿÿ‹D$8‹\$d„ğ  ğ€ ‹D$@ğ€ U  şèüÿÿÿ¡    é÷ÿÿ¶    ‹T$$¡    …Ò…¬şÿÿÇD$LõÿÿÿéûöÿÿÇD$Lûÿÿÿ¨„ëöÿÿér  èüÿÿÿé¢  v èüÿÿÿU‰ÑWVSƒì‰$…Ò„—  ‹B‹<$‹·Œ   ‹^T‹~Xiëğ  ı‹•ğ  Áê‰T$ƒâ‰T$u‹•ğ  ƒâttiÛğ  ƒ¼ˆ  „¸   ¡    %   =   „´  ‰ğ‰L$¾   èüÿÿÿ‹L$…À„ß   „L  ‹ …À…¾   ‹T$‰ğƒÈ…ÒEğƒÄ‰ğ[^_]Ã¶    ¨Atˆ—(  ‰L$‰Ğ‰T$èüÿÿÿ‰ğèüÿÿÿ‰D$‹D$èüÿÿÿ‹L$…É‹L$…Ë   ö     „Bÿÿÿé—  ´&    fƒ~T…>ÿÿÿ‹$”¨  èüÿÿÿ‰Ã‰ğèüÿÿÿ…À•À¶À4 ƒûunöD$tgƒÎéWÿÿÿ´&    ƒÄƒÎA[‰ğ^_]Ãv 1ö…É„ÿÿÿ‹)”ø  …ít…Òt‹$èüÿÿÿéùşÿÿ1öéòşÿÿ´&    f¸ÿÿÿÿébşÿÿ¶    	Şéñşÿÿ´&    fö    …Ñ  ¾   éÑşÿÿ´&    fèüÿÿÿUWV‰ÖS‹¸Œ   ‹OT‹_XiÑğ  Ú‹‚ğ  Áèƒàƒºˆ  uiƒùud…Àu
‹ƒĞ  ¨tƒˆ  ‰ò[^_]éüÿÿÿ´&    «(  ‰èèüÿÿÿ‰øèüÿÿÿ‰Ç‰èèüÿÿÿ…ÿuö     t½éö  ö    …  [¸êÿÿÿ^_]Ã´&    fèüÿÿÿUhüWVS‰Ã‰èsôèüÿÿÿ‰Ç‹Cô9ÆtE´&    ‹Cô‹‹PƒÀ‰Q‰
º   ¹   Ç@ø   Ç@ü"  Ç@ì   j èüÿÿÿ‹CôZ9ğuÂ[‰ú^‰è_]éüÿÿÿ´&    ¶    èüÿÿÿU‰ÕWVS‰Ãƒì‹pT‹xXiÎğ  „œ  ‰$ö    …  iÆğ  ø‹€ğ  ¨t9ö    …b  ‹CT…Àt7ƒø…~   iöğ  ş‹†ğ  ¨ tlÇ†ì  ÿÿÿÿ…íttƒÄ[^_]Ã¶    ‹‡À  ¨t~‹‡À  öÄusö    …(  1Ò‡l  ‰L$èüÿÿÿ‹‡   ‹L$¨uQ‹CTëƒ´&    v ‹$‰ê‰L$èüÿÿÿ‹L$…íuŒ„ğ  ğ€`şğ€ ‹$ƒÄ[^_]éËíÿÿv ‹‡   ¨t¾ö    …E  ‡L  1Ò‰L$èüÿÿÿ‹CT‹L$éÿÿÿt& èüÿÿÿS‰Ã‹€8&  …Àuö    …¹  [Ã´&    ‰Øèüÿÿÿƒøÿ„¤  jPjh  SèüÿÿÿƒÄëÅv èüÿÿÿVS‰Ã‹€8&  …Àuö    …ã  [^Ãt& ‰Øèüÿÿÿ‰Æƒøÿ„Î  1Ò¸d   èüÿÿÿjVjh  Sèüÿÿÿj Vjh  SèüÿÿÿƒÄ(ë¨t& èüÿÿÿU‰ÅWVSƒì‹˜Œ   ‹CT‹sX‰$ö    …ø  ¾(  ‰øèüÿÿÿƒ{TtC‰Øèüÿÿÿ‰Øèüÿÿÿi$ğ  ‹KP9Œì  „®   ‰Øèüÿÿÿ‰øèüÿÿÿ1ÀƒÄ[^_]Ã¶    ‹…Œ   èüÿÿÿ…Àt®‰ğèüÿÿÿğ€¦4&  ß‹FVh´9ÂtC‰\$‰|$‰×v ‹Ed‹@…Àt‹X…Ût‹– Y  ‹¤Y  ‰èèüÿÿÿ‹ELh´9ÇuÒ‹|$‹\$‰ğèüÿÿÿ‹†8&  …À!‰ğèüÿÿÿé2ÿÿÿt& 1Ò‰ØèüÿÿÿéDÿÿÿf‹†à  èüÿÿÿ‰Å‹†ä  èüÿÿÿÁà	èPiD$ğ  ÿ´à  jh  VèüÿÿÿƒÄë èüÿÿÿUWVS‰Ãƒìèüÿÿÿ‹°(  ‰Å‹¾8  ‡(  ‰D$èüÿÿÿ‰øèüÿÿÿ‰$…À…8  ‹¾8  ö     …  ¡   ¹\   ºÀ  èüÿÿÿ‰Å…À„ã   ‰òèüÿÿÿ‰}X‹†@  ‰ET‹‡D&  P‰—D&  ‰EP‰è‰«Œ   èüÿÿÿƒ}Tt‹D$èüÿÿÿ‹$ƒÄ[^_]Ã‹ƒŒ   èüÿÿÿ…ÀtÜ‹‡4&  ¨ u
‹‡8&  …ÀRğ€4&   ‰øwèüÿÿÿ‹GX´9Æt#‹Cd‹@…Àt‹P…Òt‰Øèüÿÿÿ‹CLX´9ÆuŞ‰øèüÿÿÿ‰øèüÿÿÿévÿÿÿ‰èèüÿÿÿ‰èèüÿÿÿ‰èèüÿÿÿÇ$ğÿÿÿéUÿÿÿÇ$ôÿÿÿö    „Aÿÿÿéi  èüÿÿÿV‰ÈS‹JX‹ZT·±  ‰p·±  Ç@   ‰pÇ@   ƒzTt1ÒÇ@MPEGÇ@   ‰P1À[^ÃiÛğ  Ù‹‘|  ‰P‹‘€  ‰P‹‘„  ‰P1À[^Ã´&    èüÿÿÿ‹BXÇAÀü›ÇAø   ÇA   ÇAGREY‹0'  ‰Q‹4'  ‰Q‹€,'  ÇA$    ‰A ‰A1ÀÇA(    ÇA,    Ã¶    èüÿÿÿW‹AV¾Ğ  S‰Ó‹zX‹Q9òOÖ¾   9òLÖƒ{Tuƒàà¾    €t‰QÛƒã Ã@  9ØOÃ[9ğLÆ^_‰A1ÀÃ¶    èüÿÿÿUW‰×VS‹rX‰Ëƒ¾\&  t‹†8&  …Àã   ‹F`…À„Î   ‹Pd‹R…Ò„°   ‹J…É„¥   ‹-   S…í„„   ‹m…ít}èüÿÿÿ…Àug‹–('  1Éf‰
Ç†\&     ‹WXÇCÀü›ÇCø   ÇC   ÇCGREY‹Š0'  ‰K‹Š4'  ‰K‹’,'  ÇC$    ‰S ‰SÇC(    ÇC,    [^_]Ã´&    èüÿÿÿë´&    f[¸ııÿÿ^_]Ã¶    [¸íÿÿÿ^_]Ã¸ğÿÿÿëÂ´&    ´&    èüÿÿÿS‰È‰Ó‹T$‹KXƒøu*€ytÀƒàÕƒÀ6‰€ytÀƒàÏƒÀ;‰B1À[Ã´&    f¸êÿÿÿ[Ã´&    fèüÿÿÿƒ9‹RXu;‹Aƒèƒøw0ÇA    ÇA    ÇAĞ  €ztÀƒà @  ‰A1ÀÃ´&    ¸êÿÿÿÃfèüÿÿÿ‹‰ÈƒúwrÁâ‰Ñ’   ‹‰   ‰‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J ‰H ‹J$‰H$‹J(‰H(‹J,‰H,‹J0‰H0‹J4‰H4‹J8‹R<‰H8‰P<1ÀÃf¸êÿÿÿÃ´&    v èüÿÿÿ‹BX‹€œY  ‰1ÀÃ´&    ¶    èüÿÿÿWVS‹‹rX…Àu9‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿ‹BX‹¤Y  ‹€ Y  ‰Q‰1ÀÃt& èüÿÿÿWVS‹‹rX…Àu9‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜ1À[^_Ã¸êÿÿÿëõ´&    ´&    èüÿÿÿVS|$fd@u]‹D$‹ …ÀuM‹rX‹FƒÆX´9Æt4t& öƒ   @t‹Cd‹ …Àt‹H…Étº   ‰Øèüÿÿÿ‹CLX´9ÆuÑ1À[^Ãt& ¨u¯ëñ¸çÿÿÿëì´&    t& èüÿÿÿS‰Ë‹JX‹‹    ƒâƒøtMw…Àt/…Ò…Ì  ƒc1À[Ã´&    ƒøuC…Ò…á  ÇC    1À[Ã…Ò…†  ÇC    1Àëët& …Ò…›  ÇC    1ÀëÓt& …Ò…°  ¸êÿÿÿ[Ãèüÿÿÿ‹BX‹¶Hw9Ês‰˜Y  èüÿÿÿ1ÀÃ¸êÿÿÿÃ´&    v èüÿÿÿWVS‹ZX‹Sh¶Cv|Ê9È†¢   ¡    ‰Î‹‹œY  ƒà9ñt~…À…ö  ‰³œY  ¶Dò1Ò‰ƒ˜Y  €?¸ÿÿÿ u‹ƒ¨Y  ‹“¬Y  ‰“¨  ‰“ˆ  ‰“x  ‰ƒ¤  ‰ƒ„  ‰ƒt  ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ1À[^_Ã´&    …À…  [^_Ãt& ¸êÿÿÿëŞ´&    fèüÿÿÿUWVS‹‹zX…ÀuX‰ø‰Îèüÿÿÿö    …(  ‹GoX´9Åt'v ‹Cd‹@…Àt‹H…Ét	‰ò‰Øèüÿÿÿ‹CLX´9ÅuÜ‰øèüÿÿÿ1À[^_]Ã¸êÿÿÿëô´&    ´&    èüÿÿÿUWVS‹D$‹L$‹rX©ÿÿÿ „  ‹–¤Y  ‹ Y  1Ê1Ã	Ú„ç   ‹–4&  ƒâ …ß   ‹–8&  …ÒÑ   öÄù‰† Y  †  ”Â‰¤Y  ˆVt¶Ò•Fuèüÿÿÿ€~t¸Ğ  Ûf‰†  ƒã fÃ@€~tÉf‰  ƒáúƒÁ€~tÒ‰,'  ƒâƒÂ€~tÀ‰–0'  ƒàÓ>  ‰†4'  ö    …A  ‹F~X´9Çt0f‹Cd‹@…Àt‹h…ít‹– Y  ‹¤Y  ‰Øèüÿÿÿ‹CLX´9ÇuÒ1À[^_]Ã¸ğÿÿÿëô¸êÿÿÿëí´&    ¶    èüÿÿÿ‰Ğ·‹@Xéüÿÿÿ´&    ´&    èüÿÿÿ‹BX‹˜Y  ‰·Òéüÿÿÿ´&    èüÿÿÿ‰Ğ·‹@Xéüÿÿÿ´&    ´&    èüÿÿÿWVS‹‹rX…Àul‹FƒÆ‰ÏX´9Æt$‹Cd‹@…Àt‹H…Ét	‰ú‰Øèüÿÿÿ‹CLX´9ÆuÜƒ$Gt¹   º  èüÿÿÿ1À[^_Ã¶    ¹   º÷  èüÿÿÿ1À[^_Ã¸êÿÿÿëÚ´&    t& èüÿÿÿWV‰ÖSƒì‹zX‹‹    ƒâƒø„Ä   w*…À„š   …Ò…]  ‹Q‰ğƒâ‰Qèüÿÿÿ1ÀƒÄ[^_Ãƒøu[…Ò…x  ÇA    ‹‡8&  …À„×   ğº·4&  ƒ¿   ‰øèüÿÿÿƒøÿ„)  Pjh  Wèüÿÿÿ‰øèüÿÿÿ1ÀƒÄë›v …Ò…“  ƒÄ¸êÿÿÿ[^_Ãt& …Ò…¯  ÇA    ƒÄ‰ğ[^_éüÿÿÿt& …Ò…Ê  ÇA    ‹‡8&  …ÀtHğº¯4&  r4‰øèüÿÿÿ‰Ãƒøÿ„C  ‰øèüÿÿÿSjh  Wèüÿÿÿ1ÀƒÄéÿÿÿv ƒÄ1À[^_Ã¸ÿÿÿÿéõşÿÿ´&    ¶    èüÿÿÿWVS‹Œ   ‹JT‹zXiÑğ  ‹”ˆ  ƒú•Ãƒú‰Ş•Ã‰ğ„Øu#ƒùu‡ˆ  ƒúº    [EÂ^_éüÿÿÿt& [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿWVS‹˜Œ   ‹KT‹{XiÑğ  ‹”ˆ  ƒú•Àƒú•Â„ĞuGƒùuB‹    ‡t  ÂĞ  èüÿÿÿiSTğ  ‹KX1Àƒ¼ˆ  u„¨  [^_éüÿÿÿ¶    [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿW‰ÊVS‹ˆŒ   ‹YT‹yXiËğ  ‹´ˆ  ƒştƒşu+‹H 1Àá   ƒşuiÛğ  „¨  [^_éüÿÿÿ¶    [¸êÿÿÿ^_Ã´&    ´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿV‰ÊS‹€Œ   ‹XX‹@TiÈğ  ‹Œˆ  ƒùtƒùu$1öƒùuiÀğ  ´¨  ‰ğ[^éüÿÿÿ´&    ¸êÿÿÿ[^Ã´&    èüÿÿÿUWVS‰Ë‹rX¹   º  ‰Øèüÿÿÿ‹~lº    ‰øèüÿÿÿƒøÿ„ï  k‰Áƒø tƒÁƒù ‡å  ‰ú‰èèüÿÿÿ‹V‹‚´   …Àu‹‚ˆ   PC0h  j Pèüÿÿÿ‹FxƒÄ   €‰CT[1À^_]ÃèüÿÿÿS‹\$…ÛtR‰Ã‰Èf%Dt<ƒú~S…Ûugƒúbƒúuk€åus·À1Éë´&    fƒÁƒù t(£Èsó¸   Óà[Ã´&    v ‰Èf% tìJöƒùv1À[Ãt& ƒúu³¸   [Ãt& ƒûuãƒú~1ÀëÜƒúu•‰È[f% @Ã¸   [Ã¶    èüÿÿÿU‰ÍWVSƒì ‹BX‹°ø  ˜L  ‰D$‰\$…ö„W  ‹\$‹ƒÄ  @‹ƒ¼  ‰$)Ğº   …ÀNÂ¯ƒÀ  º«ªªªÇE     ÇE    ÇE    ÷âƒô  ÇE    ‰D$ƒÌ  ÇE    Áê‰D$‰U¶    ‹T$‹D$èüÿÿÿ‰D$…À„e  ‹\$ƒ»Œ  …ı  ‹@‰D$…À„‰  ‹D$ƒÀ‰D$9D$u$é†  ‹D$‹@‹ ‹\$‰D$‰C9D$„4  ‹D$‹X‹x9ûsÔ‹M ‹t$)ßÇ$    ‹V‰ÈÁàDÚƒÿwlëv´&    ‹J‹ZƒïƒÂƒ$‰X1Û‰‹rü‹Jøƒæ	Ş‰H‰p‹Jè‰H‹JôÇ@    ƒáÇ@    ‹`  ‰H‹E H‰È‰M ÁàDƒÿvƒù?v˜‹D$‹X‹4$Gÿ÷ƒøCşû‰ø‹|$‰_‹|$G$‰G$ƒ} ?‰Áwc‹G‹W ‹X9XrU9Ê‡ÿşÿÿ´&    j ‹L$ ‹T$‹D$èüÿÿÿƒ} ?X†‚şÿÿ‹L$‹    ¸    Á   èüÿÿÿ1ÀƒÄ [^_]Ã‹K$‹D$;H s²j‰Â‹L$‹D$èüÿÿÿZë¼‹D$‹X‰\$‰Xédşÿÿ‹\$‹C ‰C$é|ÿÿÿ‹€€  …Àt‹˜ü  …Û…‘ıÿÿ‹\$¸êÿÿÿƒ»Œ  uŒ‹“„  ‹Šx  …É…kıÿÿésÿÿÿ‹D$‹H$‹D$;H ƒ%ÿÿÿénÿÿÿèüÿÿÿ‹BXÇAÀü›ÇAø   ÇA   ÇAGREY‹0'  ‰Q‹4'  ‰Q‹€,'  ÇA$    ‰A ‰A1ÀÇA(    ÇA,    Ã¶    èüÿÿÿUWVSƒì‹zX€töæÿËÿÿÆD  ƒyd…Œ   Ç$    1Ûf‰‰Ë1Àëv ƒ<$uƒø)f‰tCƒÀf	1ƒøt(¶Wt…ÒtƒøØhöƒıw…Òt×1Òf‰TCƒÀƒøuØƒÃ0ƒ<$tÇ$   ë¤v ÇAh    1ÀÇAl    ÇAp    ƒÄ[^_]Ã¸êÿÿÿëñfèüÿÿÿU1ÀWy
Vƒçü‰ÎSY‹jXQÇAl    ÇAp    ÇAh 	  ÇA    ‰Ù)ùÇC\    ƒÁ`Áéó«1Àf‰F‹E`…Àtm‹Hd‹I…Étc‹I…Ét\‹=   …ÿt‹…ÿtèüÿÿÿ‰Áët& èüÿÿÿ‰Á…Éu2V61À´&    fƒÃ9ÓuöVfv fƒÃ9Úuöf‰F[‰È^_]Ã¹êÿÿÿëòt& èüÿÿÿUWV‰ÎSƒì‹BX·YÇAh 	  ÇAl    ‰$ÇAp    f…Û…¢   ‹$1Û1ÿ¶ht´&    ·L~U‰ú1Àè)úÿÿZf‰D~ƒÇ	Ãƒÿuá1Ò‰4$~0‰Ö´&    ·LwU‰ò¸   èöùÿÿZf‰DwƒÆ	ÃƒşuŞ‹4$f…Ût,NF61Ò¶    fƒÁ9ÈuöNfv fƒÀ9Èuöf‰VƒÄ1À[^_]Ã‹$1É1í¶xtf‰N´&    fW‰ê‰Ù1Àè„ùÿÿZf‰DnƒÅƒıuæ1ít& W‰Ù‰ê¸   èaùÿÿYf‰Dn6ƒÅƒıuãéÿÿÿ´&    ´&    èüÿÿÿUW‰ÏVS‹ZXè­şÿÿƒ»\&  „ˆ   ‹C`…À„¥   ‹Pd‹R…Ò„‡   ‹J…É„|   ‹   w…ÒtG‹j…ít@‰òèüÿÿÿ…Àu-Çƒ\&     ‹‹('  ‹Wy‰‹Vlƒçü‰Ql)ù)ÎƒÁpÁéó¥[^_]Ãv ‰òèüÿÿÿë¾´&    ‹ƒ8&  …Àjÿÿÿ¸ğÿÿÿëÓv [¸ııÿÿ^_]Ã¶    [¸íÿÿÿ^_]Ã¶    èüÿÿÿU½   WV‰ÖS‰Ë¹   ƒìh‹RXd¡    ‰D$d1À|$ÇD$   ó«‹{¹Ğ  ‹FT9Ï‰$‹COù¹   9ÏLùƒ<$uƒàà½    €zt‰{Éƒá Á@  9ÈOÁ9èLÅ‰C·Š  9ù„=  ‹Š8&  …É›  ‹KùHM12„   iè   ÇD$   ‰l$i,$ğ  ‰D$‰|$ÇD$   Õ‰|  ‹L$‰€  ‹L$‰„  f‰‚  ‹B`f‰º  …Àt6‹Pd‹R…Òt,‹z…ÿt%‹   L$…Ò„  ‹j…í„ø   1Òèüÿÿÿ‹FX‹VT·ˆ  ‰K·ˆ  ÇC   ‰KÇC   ƒ~T„   ÇCMPEGÇC   ÇC    ÇD$    ‹D$dd+    …©   ‹D$ƒÄh[^_]Ãt& ièp  ÇD$Ğ  Ñı‰l$éùşÿÿ´&    ·Š  9Á…´şÿÿi,$ğ  ‹KÇD$    9Œ*|  …•şÿÿëiÒğ  ÇD$    Ğ‹|  ‰S‹€  ‰S‹€„  ‰Cé_ÿÿÿ1ÒèüÿÿÿéÿÿÿÇD$ğÿÿÿéFÿÿÿèüÿÿÿfèüÿÿÿ‰Â=   t2 ¸   ƒút+1Àú   º   DÂÃt& 1Àú @  ”À€Ã¸   Ã´&    v èüÿÿÿU‰ÕWV‰ÆS1Û·8f‰1Û´&    fU‰ù‰Ú1Àè”õÿÿYf‰D^ƒÃƒûuæ1Ût& U‰Ú‰ù¸   èqõÿÿZf‰D^2ƒÃƒûuã[^_]Ã´&    ´&    èüÿÿÿS‰ÁPX21ÀfƒÂ9ÚuöƒÁbv fƒÂ9Ñuö[Ãt& èüÿÿÿÇ€     ÃèüÿÿÿW¹   S‰Ãƒì\d¡    ‰D$X1À|$Ç$   ó«·CÇD$   ‰ÁÑù…ÒDÁ‰D$·C
‰D$‹ƒPşÿÿ…Àt+‹Pd‹R…Òt!‹Z…Ût‹   ‰á…Òt&‹z…ÿt1Òèüÿÿÿ‹D$Xd+    uƒÄ\1À[_Ãv 1Òèüÿÿÿëßèüÿÿÿ´&    v èüÿÿÿ‰  1ÀÃfèüÿÿÿWV‰ÆS‹€¼   ‹H|‹†($  …À  ƒúuZAıƒàıt…ÉuN‹¾0.  …ÿ„   Ç†,%     ö    …ë   ‹†%  èüÿÿÿf…Àt9´&    f1À[^_Ã´&    v Ç†,%      ö    tİéù  ´&    €¾eşÿÿ‹–%  Àf% 0f f‰¶–dşÿÿ‹†%  èüÿÿÿë¢f1Ûët& ƒÃƒû „\ÿÿÿ¡,   ¹8  ºÀ  èüÿÿÿ‰„0.  …ÀuÕ{ÿ…Û„  f‹„¾0.  èüÿÿÿÇ„¾0.      ƒïsäé  ´&    ¸ğÿÿÿé0ÿÿÿé:  ´&    fèüÿÿÿƒúwVW‰×V° şÿÿS‹ şÿÿZ´9Öt.¶    ‹Cd‹@…Àt‹…Ét‹½|  ‰Øèüÿÿÿ‹CLX´9ÆuØ[1À^_Ã´&    v 1ÀÃffffffèüÿÿÿU‰ÍWVS…Ò~A‰Æ’‰Ë<BÏt& ÿ6ƒÆh"  jSƒÃèüÿÿÿƒÄ9ûuäÆ ‰è[^_]Ã´&    v ‰Ï‰èÆ [^_]Ãt& èüÿÿÿUW‰×VS1Ûìœ   ‰D$‹„$°   ‰L$‰D$d¡    ‰„$˜   1À¸  ët& ƒÃ[‹…   …À„Å  9øuçd¡    ‰D$0D$8‰D$8‰D$<¡    ÇD$,    ÇD$4    ¨t!ÿ  …´  öÄt%  =  „  [¶…¥  „À…w  ‹D$ÇD$ €   °˜1  ‰4$°ˆ   ‹€„   ‰t$ 	  ‰D$‹D$èüÿÿÿ‹t$‹.‰l$‰l$èüÿÿÿ‰ğv‹@9Å…ò  ‹L$‰Èƒàşƒøş„~  ƒÁ‰L$‹D$P ¸
   t& ‰:‹
9Ïtƒèuó‹D$1íP$‹D$…À~A‰\$(‹|$‰t$$‹t$f‹¯¸
   ´&    ‰
‹9ÙtƒèuóƒÅƒÂ9îuÙ‹t$$‹\$(‹D$1Éx<¸
   v ‰‹…Òtƒèuó‹T$‹L$¸
   ¶    ‰
‹*9étƒèuó‹T$¸
   ´&    v ‰‹9Êtƒèuó[¶…¤  ƒà<Àƒà
ƒÀ
èüÿÿÿ‰Å¡    %€  =€  „R  ‹$¹   T$,èüÿÿÿ‹D$º
   ‰t$‹t$ ‹€X&  ˆ 1Ç ‰1‹ƒøÿt!ğ9Ætƒêuì‹t$‹    ‹9D$…  ¡    )ĞT$,‰D$‹$èüÿÿÿ¡    ;l$†¾   %  =  „	  ‹t$1ÀN$‹t$v ‹‰ƒÀƒøuğ‹7‹D$èüÿÿÿ[ö…¤  …¿  1À…ötö    …™  ¸ûÿÿÿ‹”$˜   d+    …œ  Äœ   [^_]Ã<…C  ‹D$ÇD$    °¤1  ‰4$°œ   ‹€„   ‰t$   ‰D$é|ıÿÿ¨„Jÿÿÿ‹D$èüÿÿÿPkÃÿ°¨  ‹D$ƒÀPh  èüÿÿÿƒÄéÿÿÿt& ‰è‰T$èüÿÿÿ¡    ‹T$)Ğ‰D$‹6‹$T$,èüÿÿÿ9t$„Çşÿÿ‹D$èüÿÿÿ9l$‚ß   ö    …å   ¸êÿÿÿéÿÿÿt& 1ÒD$@½
   èüÿÿÿ´&    ‹$¹   T$@èüÿÿÿ‹9L$”À…íuR„À„=  ‹$T$@½   èüÿÿÿö    „²üÿÿ¸
   )èèüÿÿÿP‹D$ƒÀPhÈ  èüÿÿÿƒÄé‹üÿÿt& „À…ë   ‰èèüÿÿÿ‰ÅézÿÿÿÇD$    ÇD$   étüÿÿ1Ò¸,  èüÿÿÿé0şÿÿö    u<¸êÿÿÿé5şÿÿkÛ‹D$èüÿÿÿPÿ³¨  ‹D$ƒÀPh4  èüÿÿÿ¸êÿÿÿƒÄéşÿÿkÛ‹D$èüÿÿÿVÿt$Pÿ³¨  ‹D$ƒÀPh„  èüÿÿÿ¸êÿÿÿƒÄéÌıÿÿ‹T$‹D$L$UèGúÿÿPWkÃÿ°¨  ‹D$ƒÀPh  èüÿÿÿƒÄé+ûÿÿéy  ‹$T$@‰L$$èüÿÿÿ‹L$$9L$uZƒı
„pûÿÿé¬şÿÿ‹T$‹D$L$UèäùÿÿPh  ë—èüÿÿÿkÃ‹°¨  ‹D$èüÿÿÿVP‹D$ƒÀPh„  èüÿÿÿƒÄéÉüÿÿ¸
   ëfƒè„Â  ‹L$‰‹9ÑuëéÂ  d¡    ‰D$0D$8‰D$8‰D$<ÇD$,    ÇD$4    W‹D$ƒÀPh*  èüÿÿÿ¸êÿÿÿƒÄé´üÿÿèüÿÿÿW‰ÇVS‹B‰Ó…À…   ‹‡„   ¹   €  ‹S ‹09òuPp‹@9ÂtF¸
   ‰‹9Útƒèuó‹ŸX&  º
   Ã@1Ç f‰‹ƒøÿt!È9Átƒêuì[^_Ã´&    ö    u.ƒK[^_Ã´&    ƒøu‹‡„   ¹   @  éuÿÿÿéå  éÿ  t& èüÿÿÿU‰ÅWVSƒì,‹@‰D$‹E…À„  ƒøtÇE    ƒÄ,[^_]Ãt& ‹E@=  u>‹]pö    …@  º.   ‰Øèüÿÿÿ‹|$‹—4&  ƒâu¸…Àt´9Ãs°é"  ¶    = …8  ‹uD‹D$‰òèüÿÿÿ‰Ã…À„^  ‹T$‹EL1ÿƒÂ‰D$‰T$“¨  ‰T$…À„J  ‰,$ë5fƒø„  j ‹L$‰ê‰Øèüÿÿÿƒ»@  Y„  ƒÇ;|$„  ‹$‹tø`ö@t‹ƒL  9Æ‚  ƒp  9Æƒ  ‹$‰ò‹Lød‰Øèüÿÿÿ‰Å¡    %  =  „«  …í„K  ‹ƒ@  ƒø„  ƒø…]ÿÿÿ‹D$‹€0&  ‰D$…À„W  ‹E …Àt]E‹u‰Â‹E9Ât	9u„Ø  9ÖtA‰l$‰Õ‰\$‹\$‰|$ë‹V‹ƒ,&  ‹»0&  èüÿÿÿ‹69õt‹N…Éuß‹\$‹l$‹|$j ‹€  ‰ê‰ØƒÇèüÿÿÿX;|$…÷şÿÿt& ‹,$‹    ‹Ğ  ¸    èüÿÿÿ‹D$º   ¹   j ¼1  èüÿÿÿƒ»P  ÿZ„Ôıÿÿj ƒ\  ¹   º   èüÿÿÿXé·ıÿÿf‹ƒ4  …ÀtZ‹°ü  …ötP‹M …ÉtIU‹u‰Ñ‹U9Ñt	9u„½  9ñt-‰l$‰Íë‹ƒ4  ‹V  èüÿÿÿ‹69õt‹N…Éuà‹l$j ‰ê‹€  ‰ØèüÿÿÿZéşÿÿt& ‹E …À„Ë   ƒô  ‰D$èüÿÿÿ‹“ì  ƒì  9Â„Ÿ   ‹³ì  FØ‰t$èüÿÿÿ‰D$…À„   ‹VD‰ğ‹uM9ÎtS‰\$ ‰Ó‰l$$‰Í‰|$(‰Çë%;G$w‹D$‹VØèüÿÿÿ‹FGDÃ‹69õt‹N…ÉuÕ‹D$‹\$ ‹l$$‹|$(‹PD;“ä  sj‹    ƒø  ÂĞ  èüÿÿÿ‹D$èüÿÿÿj ‹€  ‰ê‰Øèüÿÿÿ^éıÿÿv j ‹L$‰ê‰ØèüÿÿÿXéıÿÿv ‹D$èüÿÿÿéğüÿÿ´&    v ‹t$ÇFD    èüÿÿÿ¹   ‰F‹F‰V‹‰B‰Fº   Ç   ÇF"  ÇFô   j èüÿÿÿXéHÿÿÿf‹N…É„işÿÿ‹V  èüÿÿÿéWşÿÿv ‹N…É„bıÿÿ‹D$‹V‹t$‹€,&  èüÿÿÿéGıÿÿéû  t& èüÿÿÿU‰ÕW‰ÇVSƒìTd¡    ‰D$P1À…Ò„ì  ƒú…£  ‹‡„   @  ‰D$1ö1Ûô1  ë´&    ƒÃƒÆtƒûF„J  1‹…ÒuèŒ7ä1  Ç    ‰$…É„)  kÃt‹L$‰\$ƒÁ øÇ€ü1      ‰¨ 2  1À‹‹$‰T@ƒÀƒøuì‹\$‹T$‹‹$‰A ‹B‹Q ‰A$9Â„  këtı‹… 2  …À„j  ƒø…İ   ‹…$2  =  „¤   = …S  ‹,$‹EL‹MHPÿƒú‡  ƒ}Dÿ„ú   ƒø´7D2  •À¶À,Å   1À´&    f—T&  ‹‰ƒÀ9èrëkÛtö„ü1  „  ‹D$Pd+    …  ‹—Ô1  ‹$ƒÄT¸    [^_]éüÿÿÿ´&    ‹T2  Æ ‹µ,2  …ö…™   ö…ü1  tm´&    ‹D$Pd+    …Ã   ƒÄT[^_]Ã´&    ö    …†  kÃtÇ„ü1     éÇşÿÿ‹‡„   €  ‰D$éşÿÿt& kÛtö„ü1  u›‹D$Pd+    ub‹$ƒÄT‰ø[^_]é=øÿÿt& ‰ò‰øèüÿÿÿ·T&  ‰Ø¹ü   ‰òèüÿÿÿÆƒü    º  Ü ‰øèüÿÿÿö…ü1  …áşÿÿ‹$‰øèñ÷ÿÿéÒşÿÿèüÿÿÿGPh$  èüÿÿÿXZéÿÿÿfèüÿÿÿéfñÿÿ¶    èüÿÿÿWVS‹L$‹|$…É~D$$‰úˆv ‹püƒÀƒÂ‰rü9ØuğW‹T$‹D$è"ñÿÿZ[^_Ã´&    ¶    èüÿÿÿUWVSƒìd¡    ‰D$1À‹\$0‹t$4‹L$8…Û„  ƒùó  T$<‰å…É~"ƒÂ1À‰å´&    v ‹züƒÂ‰|… ƒÀ9È|ïU‰ò‰Øè¥ğÿÿZ‹T$d+    uƒÄ[^_]Ãèüÿÿÿ´&    v èüÿÿÿV‰ÆS‹˜8  ¸   ‹“  öƒ  u1À…Ò•ÀRPjÿ¶D  jh	  Sèüÿÿÿº   ƒÄöƒ  u‹‹  1Ò…É•Â…Àt[^Ã´&    v ÿ³  Rj ÿ¶D  jh	  SèüÿÿÿƒÄ…ÀuĞj ‹ƒ  ÁèPjÿ¶D  jh	  SèüÿÿÿƒÄ[^Ã¶    èüÿÿÿS‹L$‹˜8  ú¡   „ä  ‡¾   ú™   „ò  wpú•   „  v2ú—   …  ÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã´&    fú   …´   ÿ1j j j j ÿ°D  jh  SèüÿÿÿƒÄ$[Ãfú   „¼  úŸ   …¨   ÿqÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ [Ãt& úĞ   „”  †¾   úÚ   „¢  úÜ   …~   ÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ú‘   …­  ÿ1ÿqÿ°D  jh  SèüÿÿÿƒÄ[Ã´&    ú›   …x  ‹‹IƒâÁá	Ê‰“  [é»ıÿÿv úÙ   …Z  ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãfú½   „  vúÅ   …  1À[Ãú¹   uÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãú»   tÒé*  v ÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãv ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿqÿqÿqÿ1ÿ°D  jh  SèüÿÿÿƒÄ [Ãt& ‹‰“  ‹Q‰“  [éÁüÿÿÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ã¶    ÿ1ÿ°D  jh  SèüÿÿÿƒÄ[Ãé*  é*  é*  é*  é*  ffffffèüÿÿÿUWVSƒì<‰$‰T$d¡    ‰D$81Àƒù…ˆ  ‹z‰ĞƒÀ‰|$‰D$ 9ø„p  ‹D$‹h…í„a  ‹x‰|$èüÿÿÿ‹$ƒ¸\&  „W  ‹D$ÇD$    8?ÿÿÿu	‹@È‰D$‹$€xu‹D$ÿƒçXEÇ  ƒı†E  º   ët& ƒÂƒÃ9Õ„
  €;ÿuí€{ uç€{ uá¶Kƒá¿€ù°uÕ)Ğ9Ç‡ğ  v 1ÒÇD$    ƒÃ1ö÷÷‰l$‰Å´&    f€{üÿ…  €{ı …ü   €{ş …ò   ¶Cÿƒà¿<°…ã   ‹$‰\$,‹@`…ÀtE‹Pd‹R…Òt;‹
…Ét5‹   …Ò„K  ‹   ‹‰T$…Ò„7  ‹L$T$(èüÿÿÿ¶    ‹T$4…Ò„„   ‹L$‰ÈÁà$‰@'  ‹T$(‰D'  ‹T$0‰H'  ‹$‰ÈÁà„P'  ‹T$,‹
‰‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J‰H‹J ‰H ‹J$‰H$·R(f‰P(ƒD$t& ƒÆû9õ‡ãşÿÿ‹D$‹<$‹l$Ÿ@'  ‰D$…À„œ   ‹L$‹D$‰ÚÁá‰H‹D$èüÿÿÿ‹$‹€<'  …À…Á  ‹$‹€8'  ‰D$ƒÀ‹<$‹L$‰‡8'  ‹|$‹GA )è‰A ‹‰D$;D$ …ıÿÿ‹D$8d+    …8  ƒÄ<[^_]Ã´&    ¸   9Ç†şÿÿ‹$˜@'  ‹<$‹D$‹t$Ç‡@'      Ç‡H'      Ç‡D'      Ç@@   ‹‰‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C‰F‹C ‰F ‹C$‰F$‹C(‰F(‹C,‰F,‹C0‰F0‹C4‰F4‹C8‰F8‹C<‰F<‹—<'  …Ò„òşÿÿ‹<$ÇD$    ÇD$    ‹‡8'  ÇD$   ƒà‰D$$‹´‡@0  1À‰l$ƒÃ1ÿ‰Åëp‹L$«Á‰L$‰Ğèüÿÿÿ‰ÂkÅ+ƒÅˆT,‹D-‰‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S‰P‹S ‰P ‹S$‰P$·S(f‰P(ƒÇƒÃ@9|$~8‹Sğ…Òtí‹Køƒ{ô AuAúƒøqÿÿÿ‹L$ƒè «Á‰L$éiÿÿÿ¶    ‰èÇ  º‹l$ÇFD fÇF$ÑÇFÓúÿÿÇF  ½ÇF „€ÇF! ]ÇFc§ÿÿƒø$„  ‰Á€‹T$‹|$‰ËƒDAƒàüƒÀ·ØƒÀ
K
ƒÃ ¶ÍÇF itv0‰V$‰~(‹|$ˆFˆN‰øÁèƒàƒÈ!ˆF‰øÁèˆF‰øÁèƒÈˆF‰øÁèˆF‰ø‹|$$D ˆF‹$‰œ¸À0  é?ıÿÿ‹<$ÇD$    ÇD$    ‹‡8'  ƒà‰D$$‹´‡@0  ‹D$…ÀIşÿÿÇ  º»,   1É1ÒÇFD f¸   1ÿÇF$ÑÇFÓúÿÿÇF  ½ÇF „€ÇF! ]ÇFc§ÿÿé%ÿÿÿT$(èüÿÿÿéÎûÿÿ‹¸,'  ‹T$1Û‰|$ÿB‰|$„´   ƒï‰l$‰ıë=t& ‹ˆœ  z‰Æƒçü‰Šœ  ‰ÑÂ   )ù)ÎÁ   Áéó¥ƒÃ¤  ;\$th€xüÿub€xı u\€xş uV¶Hÿƒá¿€ù uJ‹‰
9İu¡‹ˆ  z‰Æƒçü‰Š  ‰ÑÂ”  )ù)ÎÁ”  Áéó¥¶ˆ“  iÉ‰
‰J‰JëŠ‹l$iD$@  ‹|$‹t$‰G‹<$‹—8'  ‰Tü‹‡8'  ‰D$ƒÀé½ûÿÿ¹  V,F$»0  ÇF ITV0èüÿÿÿ¸   ¹   éôıÿÿ‰èéìùÿÿèüÿÿÿfèüÿÿÿUWV‰ÆSƒì‹€4&  ‹Vh¨ „1  Bp‰$‹Fd…ÀtD‹Pd‹R…Òt:‹Z…Ût3‹<$‹   ·W…É„n  ‹y…ÿ„c  j 1ÉèüÿÿÿY´&    f‹Fn1ÒX´9Åuë_´&    ‹CLX´9ÅtN‹Fh‹@…Àt#ƒ   t%‹Cd‹@…Àt‹x…ÿt‹$1É‹Pj ‰Øèüÿÿÿ‰ÂX…Òt»úııÿÿt³ƒÄ‰Ğ[^_]Ã‹†X&  ‹˜ Ç ‹$‰ßƒçÏ‹@…À„±   ƒøtd‰øƒÈ 9Øtg‰Ã‹X&  ‰ßº
   Ï   Á Ç ´&    ‰9‹ƒøÿt1Ø¨0tƒêuìƒÄ1Ò[‰Ğ^_]Ã‹†˜Y  @D‚L‰$éÀşÿÿv ‰øƒÈ9ØuœƒÏ ‹†X&  ‰ıº
   Í   ˆ Ç v ‰)‹ƒøÿt
1ø¨0„lÿÿÿƒêuèébÿÿÿv 9ßt½‰ûéTÿÿÿt& j 1ÉèüÿÿÿZé¡şÿÿèüÿÿÿWVS‹X`…Ût9‹Sd‹R…Òt/‹2…öt)‹Hh‹=   ‹€œY  ‹TÁ …ÿt‹?…ÿtj 1É‰ØèüÿÿÿZ[^_Ãt& j ‰Ø1ÉèüÿÿÿX[^_ÃèüÿÿÿUWVS‰Óƒì‹‚X&  ‹°1Ç ‹‚X&  #²È1  ‹¸D1Ç ‹‚X&  #ºÌ1  Ä0Ç ‹ #‚Ğ1  …öu7‰ò	ú‰$…ÿuc…À…Œ   ‹,$…í…Z  ‹$1À…Ò•ÀƒÄ[^_]Ã´&    ‹’X&  ª1Ç º
   ´&    ‰u ‹M ƒùÿt…Ît¢ƒêuì‰ò	ú‰$…ÿt‹“X&  ªD1Ç º
   ´&    ‰} ‹M ƒùÿt…Ïtƒêuì…Àt/‹‹X&  º
   ©Ä0Ç ´&    ‰E ‹M ƒùÿt…Ètƒêuì	$‹    â€  ú€  „C  …öt÷Æ   uuæ   u]…ÿ„ÿÿÿ÷Ç   u-ç€   „ÿÿÿj ¹   ƒ˜1  º   èüÿÿÿYéëşÿÿt& j ƒ¤1  ¹   º   èüÿÿÿ^ë¹¶    1Ò‰Øèüÿÿÿë˜t& º   ‰Øèüÿÿÿézÿÿÿ‹    â€  ú€  …ZÿÿÿéC  fèüÿÿÿ‰Á‰Ğ‹‘4  …Òu}V‹±”   S‹X&  ÃÄ ‹ã    »ÿÿ  EÓƒH(p‰P<‹–X&  ‹šÄ ‰Úsşƒâƒú”Â¶ÒƒÂæı   ¾   DÖ‰ÖƒÎƒãEÖ‰P4‹‘L  ‰P81À[^Ã´&    1ÀÃ´&    ¶    èüÿÿÿÇ€4     1ÀÃ´&    ´&    èüÿÿÿUWVSƒì‹¨”   ‹…X&  ‹° Ä ‹…X&  ‹¸Ä ‹…X&  Ä ‹ ¹H  ©    ¸G  DÁ‹ğ     ƒæ‰$PSh    èüÿÿÿ‰ø¶Ôƒâÿ4•   ShL   èüÿÿÿ‰ğƒÄ„À„\  é"  ´&    ¶    èüÿÿÿV‰Á¾S‹€X&  Ä ¸
   ‰2‹ûtƒèuï‹X&  ¾şâ+ Ä ¸
   ´&    f‰2‹ûşâ+ tƒèuï‹X&  ¾­'R Ä ¸
   ´&    f‰2‹û­'R tƒèuï‹X&  ‹$Ä ‹‰X&  âÿÿÿ ¸
   Ê   VÁ$Ä ¶    ‰‹9Útƒèuó[^ÃèüÿÿÿVSƒì‹˜”   ö    …×#  ‹ƒX&  Ä …Òtl‹‹ƒX&  0ö€ÎŒˆÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹Ä ‹‹X&  âÿÿ ÿ¸
   Ê   ÁÄ f‰‹9ÚtƒèuóƒÄ1À[^Ãv ‹‹ƒX&  0íÄ ¸
   v ‰
‹29ñtƒèuó‹ƒX&  ‹ˆÄ ‹“X&  áÿÿ ÿ¸
   ÂÄ ‰
‹9Ùt¥ƒèuóƒÄ1À[^Ã´&    t& èüÿÿÿUWVSƒì‹˜”   ‹q‰D$‹€8  ‰Å‰$å ù  …ö…œ  ‹q‰Êƒş…‡  ÇA   ÇA   ‹ƒX&  ‹¸tÄ Áï‹ƒX&  çğ  ‹ˆtÄ Áé‹ƒX&  ƒá	ù‹¸pÄ ‹ƒX&  Áïçğ  pÄ ‹ Áèƒà	ø‹z…í‰<$¿   D÷‹z4$‰ıÁå9Å‚$  9Ç‡$  ,õ    9ÍŒ$  9ñŒ$  ‹…Ò„   Áà1Ò÷÷¨  ğÿ‰ÈÁà	™÷ş¾   ‰D$¸  +D$%ÿ  ‰D$ÿ€  †w  ö    …'$  ‹ƒX&  Ä ‹ ‰é0À¶Ñ	Â‹ƒX&  ˆÄ ¸
   v ‰‹99útƒèuó‹ƒX&  ‹Ä ‰è0ö% ÿ  	Â‹ƒX&  ˆÄ ¸
   v ‰‹99útƒèuó‹ƒX&  ‹Ä ‰èâÿÿ ÿ%  ÿ 	Â‹ƒX&  ‰ÕÄ ¸
   t& ‰*‹
9Ítƒèuó‹ƒX&  ‹Ä ‹D$‹»X&  0Ò÷ØÄ ¶À	Ğº
   ‰‹99øtƒêuó‹ƒX&  ‹Ä ‹D$0ö0À	Â‹ƒX&  ˆÄ ¸
   t& ‰‹99útƒèuó‹ƒX&  ‹Ä ‹‹X&  âÿÿ ÿ¸
   	òÁÄ ¶    ‰‹9Útƒèuó1ÀƒÄ[^_]Ã´&    f¾  	 ÿÀ   ‡xşÿÿƒÿaöæ   Æ  
 ébşÿÿ¸êÿÿÿëÁ¸êÿÿÿëº´&    t& èüÿÿÿWVS‹P‹JÔ‹P(ú	˜ „  waú 	˜ „µ   ú	˜ …™   ‹@|‹±X&  À‹–Ä ‹‰X&  ¶À0öÁà	ÂÁÄ ¸
   t& ‰‹9Útƒèuó[1À^_Ãt& ú	˜ uH‹@|‹¹X&  ‹— Ä ‹‰X&  Áàâÿÿ ÿ%  ÿ 	ĞÁ Ä º
   ´&    ‰‹9Øt­ƒêuóë¦[¸êÿÿÿ^_Ã´&    ‹@|‹¹X&  ƒÀ€‹—Ä ‹‰X&  ¶À0Ò	Â¸
   ÁÄ ¶    ‰‹9Ú„YÿÿÿƒèuïéOÿÿÿ´&    v ¶x|‹±X&  ?‹ Ä ‹¹X&  ¶Ò0Û	Óº
   · Ä t& ‰‹>9ûtƒêuó‹@|‹±X&  À‹– Ä ‹‰X&  ¶À0öÁà	ÂÁ Ä ¸
   ´&    ‰‹9Ú„Éşÿÿƒèuïé¿şÿÿ´&    v èüÿÿÿUWV¾   Sƒì‹˜”   ‰$‰Øèüÿÿÿ‹ƒX&  º
   ˆ Ä ¶    ‰1‹ƒøÿt%   =   tƒêuæ‹ƒX&  ‹¨  Ä ‹ƒX&  ‰ï‰îº
   ƒÏæşÿ  ˆ  Ä t& ‰9‹ƒøÿt	1è©şÿ  tƒêué‹ƒX&  º
   ˆ  Ä ´&    ‰1‹ƒøÿt·À9ğtƒêuë‹ƒX&  XÄ ‹ ‹»X&  %ÿşÿáº
   XÄ ´&    ‰‹19ğtƒêuó‹»X&    º
   XÄ t& ‰‹19ğtƒêuó‹ƒX&  \Ä ‹ ‹»X&  %ÿşÿáº
   \Ä ´&    ‰‹19ğtƒêuó‹»X&    º
   \Ä t& ‰‹19ğtƒêuó‹ƒX&  ¾
 4Ä ¸
   t& ‰2‹
ù
 tƒèuï‹ƒX&  ‹°<Ä ‹ƒX&  ‰õ‰÷º
   åŸ ƒÏˆ<Ä ƒÍf‰9‹ƒøÿt	%Ÿ 9Åtƒêué‹ƒX&  ‰÷º
   æŸ ƒçşˆ<Ä t& ‰9‹ƒøÿt	%Ÿ 9Ætƒêué‹ƒX&  ‹ˆÄ ‹ƒX&  É  Ä ¸
   ¶    ‰
‹29ñtƒèuó‹ƒX&  ‹ˆÄ ‹ƒX&  ƒÉÄ ¸
   v ‰
‹29ñtƒèuó‹ƒX&  ¾ €  ¤Ä ¸
   t& ‰2‹
ù €  tƒèuï‹ƒX&  1ö¤Ä ¸
   t& ‰2‹
…Étƒèuó‹ƒX&  ‹ Ä ‹ƒX&  âÿÿûÿÊ   ˆ Ä ¸
   f‰‹19òtƒèuó‰Øèlöÿÿ‹ƒX&  ‹ Ä ‹ƒX&  âğç÷ÿÊ ˆ Ä ¸
   v ‰‹19òtƒèuó‹ƒX&  ‹ˆ Ä ‹ƒX&  É  P  Ä ¸
   ‰
‹29ñtƒèuó‹ƒX&  ‹ŒÄ ‹ƒX&  0ö€Î ˆŒÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ¾.%@Ä ¸
   t& ‰2‹
ù.%@tƒèuï‹ƒX&  ‹Ä ‹ƒX&  â   ÿ€Î]ˆÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ¾(fxÄ ¸
   t& ‰2‹
ù(ftƒèuï‹ƒX&  ÔÄ ‹ ¶Ğ<ä‡—   ƒúvJ¸ä   )ĞÑøƒÀÁà	‰Ã‰ÁÁû‹<$‹—0  ‰JD‰ZH‹—0  ‰‚€   ƒÄ‡À   [^_]éüÿÿÿ´&    ‹ƒX&  ‹ÔÄ ‹‹X&  0Ò¸
   ƒÊÁÔÄ ¶    ‰‹9Útƒèuó¹ ş  1Û¸ ş  ë‰t& ‹ƒX&  ‹ÔÄ ‹‹X&  0Ò¸
   €ÊäÁÔÄ ¶    ‰‹9Útƒèuó¹ .  1Û¸ .  é>ÿÿÿfèüÿÿÿè¶úÿÿ1ÀÃv èüÿÿÿ‹T  …Òt	1ÀÃ¶    Ç€T     è‰úÿÿ1ÀÃ¶    èüÿÿÿWV‰ÎS‰Ã‰Ğâüÿ  ƒàÂ  Ä Å    ‹ƒX&  Ğ‹8¸ÿ   “X&  Óà÷Ğ!Ç‰ğ¶ğ‰øÓæ	ğ‰Á¸
   ´&    ‰
‹9Ùtƒèuó[1À^_Ãt& èüÿÿÿUW‰×‰ÊV‰ÆSƒì‹D$‹\$‰$‰øƒç%üÿ  ı    ‹¾X&    Ä Ç‹/¿ÿ   ¶Ò¶Û†X&  ÓçÓâ¾
   ÷×Óã!ï	ú¶<$Óçv ‰‹ƒùÿt1ù…ÙtƒîuìƒÄ1À[^_]Ã´&    v èüÿÿÿWVS‹¸4  …ÿu^‰Ã‹€”   ‰Ö‹ˆX&  ‹‰Ä ‹R8ÁéƒáğƒútRw(…ÒuD¶Éhÿ   º	  Qèüÿÿÿ‹F8‰ƒL  XZ‰ø[^_Ãƒútƒúu#ƒÉëÍ1ÿ[^‰ø_Ã´&    ƒÉë·v ƒÉë¯¿êÿÿÿëÆ´&    t& èüÿÿÿUWVS‹°è  ‰Ã‹¸ì  ‹“X&  ‰ğ% °  ƒøÀÂœÄ %   ı   ‹âÿÿÿ 	Â‹ƒX&  ˆœÄ ¸
   t& ‰‹)9êtƒèuó‹ƒX&  ‹ˆ Ä Áéº  ‰ØáŸ   èüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰ØƒÉ`¶Éèüÿÿÿ‹ƒX&  ÷Æ ù  „¯   ‰òÄ €ö 	ú„¥  ö €  	ş„  ‹0‹ƒX&  æ ÿÿÿÎö   ˆÄ ¸
   ´&    ‰1‹ƒúÿt€úötƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   tƒêuæ‹ƒX&  ë¶    ‰ò„Òujæ  ÿ …   Ä ‹ ‰ÁÁé©   u[^_]Ã¶    ‰Îáï   jº  Qƒæï‰Øèüÿÿÿ‰ñ‰Øº  ƒÉj¶ÉQèüÿÿÿƒÄ[^_]Ã´&    ‹°Ä ‹ƒX&  Îÿ   º
   ˆÄ v ‰1‹ƒøÿt
¶À=ÿ   tƒêuè‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ ´&    ‰1‹ƒøÿt%   ?=   „ÿÿÿƒêuâ‹ƒX&  éÿÿÿ´&    ‹0‹ƒX&  æ ÿÿÿÎ÷   ˆÄ ¸
   ‰1‹ƒúÿt€ú÷tƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   „qşÿÿƒêuâégşÿÿt& ‹0‹ƒX&  æ ÿÿÿÎø   ˆÄ ¸
   ´&    f‰1‹ƒúÿt€úøtƒèuí‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ t& ‰1‹ƒøÿt%   ?=   „áıÿÿƒêuâé×ıÿÿt& ‹°Ä ‹ƒX&  Îÿ   º
   ˆÄ v ‰1‹ƒøÿt
¶À=ÿ   tƒêuè‹ƒX&  ‹°Ä ‹ƒX&  æÿÿÿ º
   Î   ˆÄ ´&    ‰1‹ƒøÿt%   ?=   „QıÿÿƒêuâéGıÿÿ´&    ¶    èüÿÿÿ‹€”   èĞûÿÿ1ÀÃ´&    ¶    èüÿÿÿUW‰×V‰ÎS‰Ãƒìd¡    ‰D$1ÀÇ$    ÇD$    ÇD$    ö    …K$  Gÿƒø‡Š  Ç$   Oï1íƒş„
  ‡7  ƒş„.  ƒş…  ÇD$   ƒáÏƒÉ´&    ¶Éh÷   º  ‰ØQèüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰Øƒáù	é¶Éèüÿÿÿ‹ƒX&  ‹ˆ Ä XZÁé‹l$…í„›  ‹D$…À„  ƒÉ¶Éjº  ‰ØQèüÿÿÿ‹ƒX&  Ä ‹ ]%   ÿY‹l$‰Â€ÎP…ít‹L$…Ét0P  ‰Â1É½   ‹Œƒøt2‡½  ƒø„\  …À„¬  …Ét€Î ƒù„«  ƒÁ‹ŒƒøuÎ¸@  Óà	Â…Éuß€æ¿ëãƒş†şşÿÿéf$  ´&    fƒşuÇD$   ƒá?éÛşÿÿv ƒş…
  ƒá?ÇD$   ƒÉ@é¼şÿÿt& ÿÿ  †¬   ‰ú‰ù‰øâ ğ  á   %  ğ ÷Çÿ ÿ……$  ê   ú p  ‡…$  é   ù   ‡…$  -  p =   ‡…$  ÁùÁúÇ$   ½   	ÑÁøÇD$   ÇD$   	Áƒş…öıÿÿƒáÏÇD$   ƒÉ éşÿÿƒáıƒáûéişÿÿt& ‰ø‰ú%ğ   â   ÷Çğÿÿ……$  ƒèƒøp‡…$  ª üÿÿı   ‡…$  ÁøÇ$   Hğúÿ     ê   ƒá?½   ÇD$   Áú	ÑéXıÿÿ¶    ‰èÓà	Âƒù…Uşÿÿ‹ƒX&  ˆÄ ¸
   ´&    ‰‹)9êtƒèuó‰»ğ  ‰Ø‰³ô  èüÿÿÿ‰Øèiøÿÿ1À‹T$d+    u`ƒÄ[^_]ÃfƒÉéÿÿÿ´&    ÁıƒáÏÇD$   	é½   éÀüÿÿ¶    ÇD$   ƒáÏéàüÿÿ‰èÓà	Â…É…¥ıÿÿ€æïé¦ıÿÿèüÿÿÿéf$  èüÿÿÿ‹ˆD  ‹€”   éüÿÿ´&    v èüÿÿÿ‰Ñ‹@  ‹€”   éøûÿÿ´&    èüÿÿÿS‹€X&  ·Ò”  Ä ¸
   t& ‰
‹9Ùtƒèuó1À[Ã´&    ´&    èüÿÿÿU·ÒWVS‰Ëƒì‹€X&  ‹|$‹t$Ç$
   Œ  Ä !şƒşÿ•À‰Å´&    f‰‹ƒøÿu‰ê„Òu!ø9Ætƒ,$uåƒÄ1À[^_]Ã´&    t& èüÿÿÿ‹€X&  ·Ò„  Ä ‰1ÀÃ¶    èüÿÿÿ‰Á‰Ğ‹‰X&  %üÿ  „  Ä ‹ ƒâÕ    ÓèÃ¶    èüÿÿÿ‹€X&  ·Ò„  Ä ‹ Ã´&    èüÿÿÿU‰Õ·ÒW‰ÏVS‰Ã‰è‹t$‹‹X&  %üÿ  „  Ä ‹ ƒåí    Óè‰Á‰Ø[!ù	ñ^_¶É]é¤ôÿÿt& èüÿÿÿS·Ò‹˜X&  Â  Ä Ó‹!ÙX&  L$¸
   ¶    ‰
‹9Ùtƒèuó1À[Ã´&    ´&    èüÿÿÿUWVS‰Ãƒì\‹€è  ‹³ì  ‹»X&  ‰Á€åO—œÄ 	ñ„í   ‹
‹»X&  áÿÿÿ º
   É   ÇœÄ ¶    ‰‹/9étƒêuó‰Ççÿÿ ‰|$(„ì   „À„ù  ÇD$„   ½ ‚  ¹<‚
 ÇD$   ]ÇD$$&   ÇD$ 0  ÇD$  $ ÇD$   0ÇD$   ÇD$    ÇD$ <   Ç$  
 ÇD$H    ÇD$D   ÇD$@   ÇD$<0   ÇD$8C  ÇD$4&   ÇD$0]   ÇD$,„   é8  v ‹
‹»X&  áÿÿÿ º
   É   ÇœÄ ‰‹/9é„ÿÿÿƒêuï‰Ççÿÿ ‰|$(…ÿÿÿ‰Â€ö	ò„ğ  €ôÇD$z   	ğÇD$$   ƒøÇD$   ÿÇD$   ç   ÇD$   &Ç   ZƒøÇD$  P ‰|$ÿç   ºÇ$   Ç   fƒøÇD$D   ‰|$ÿç   ƒøÇD$@   í‰|$ å ıÿÿÇD$<&   Å |  ƒøÇD$8á  ÿÇD$4   ƒçºÇD$,z   ƒÇfƒøÉ‰|$HáıÿÿÁ | ƒøÀƒàƒÀZ‰D$0‹ƒX&  ‹Ä ‹ƒX&  ¶Ò‹¸Ä ‹ƒX&  çÿÿÿÄ ‹ ¶ôö    …%  …ötO¸Èô´÷é‰Ç‰Ö…Òt@ÇD$L    ‰ğº¹ˆbñ÷â‰T$PÁê	iÂ  )Æ‰ò‹t$PÁî	‰ø¿  ÷÷‰ò‹t$L…ö…¤$  ‹ƒX&  ‹pÄ ‹ƒX&  0ÒT$ˆpÄ ¸
   ´&    ‰‹19òtƒèuó‹ƒX&  ‹ˆpÄ ‹ƒX&  0ípÄ ¸
   t& ‰
‹29ñtƒèuó‹ƒX&  ‹pÄ ‹ƒX&  âÿÿ ÿÊ  - ˆpÄ ¸
   ´&    v ‰‹19òtƒèuó‹ƒX&  ‹pÄ ‹ƒX&  âÿÿÿ T$ˆpÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  0ÒT$$ˆtÄ ¸
   ´&    ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  0öT$ˆtÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  âÿÿ ÿT$ˆtÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹tÄ ‹ƒX&  âÿÿÿ T$ˆtÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  0ÒƒÊˆxÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  0ö€ÎˆxÄ ¸
   ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  âÿÿ ÿT$ˆxÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹xÄ ‹ƒX&  âÿÿÿ T$ˆxÄ ¸
   t& ‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  0ÒT$ ˆ|Ä ¸
   ‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  0ö	êˆ|Ä ¸
   f‰‹19òtƒèuó‹ƒX&  ‹|Ä ‹ƒX&  âÿÿ ÿ$ˆ|Ä ¸
   t& ‰‹19òtƒèuó‹|$(ƒÿÒƒÂƒÿÀ‰“  ƒàƒÀ÷ßÉ‰ƒ  ‹ƒX&  á   ‹|Ä âÿÿÿ ¸
   	Ê‹‹X&  Á|Ä ´&    f‰‹9ÚtƒèuóƒÄ\[^_]Ãv €ôÇD$„   	ğÇD$   ]ƒøÇD$$&   ÿÇD$ 0  ç   ÇD$  $ Ç  € ƒøÇD$   0‰|$ÿç    ƒøÇD$@   ‰|$ÿƒçLÇD$<0   ƒÇ:ƒøÇD$8C  í‰|$ å ;  ÇD$4&   Å B  ƒøÇD$0]   ÿÇD$,„   ç  şÿÇ  
 ƒø‰<$ÿ1Éƒç ƒø’Á‰|$H‰L$DÉáL;şÿÁ:B
 éõúÿÿÇD$z   ½ ‚  ¹<‚
 ÇD$   ZÇD$$   ÇD$   ÇD$   ÇD$   &ÇD$   ÇD$    ÇD$ <   Ç$  
 ÇD$H    ÇD$D   ÇD$@   ÇD$<&   ÇD$8á  ÇD$4   ÇD$0Z   ÇD$,z   éWúÿÿfèüÿÿÿWVS‹°4  ‹˜”   …öu‹°<  ‹¸8  1Î1×	ş„   ‰Ö‰8  ö    ‰ˆ<  Ç€4      	Î„ê   ‰Öö @  	Îtf‰Öö   	Î„>  ‰Öö   	Î„Î   ‰Öö   	Î„.  ‰Öö   	Î„>  öÆ°…%  „Ò…=  â  ÿ ÷ÚÒ‰×ƒçë	t& ¿   ö    …;&  1ö‹ƒX&  ‹ˆ Ä ƒáĞº   ‰Ø	ùƒÉ ¶Éèüÿÿÿ‹ƒX&  ‹ˆ Ä Áéº  ‰Øƒáü	ñ¶Éèüÿÿÿ‰Øèüÿÿÿ‰ØèÀìÿÿ[1À^_Ã´&    v ¿   ë†´&    f1ö¿   ö    …Y&  ‹ƒX&  ‹ˆ Ä áğ   º   ‰ØƒÉèüÿÿÿ‹ƒX&  ‹ˆxÄ Áéº{  ‰Øáù   èüÿÿÿé1ÿÿÿt& ¾   ¿   ë›t& 1ö¿   ë´&    ¿   éóşÿÿ¶    ¿   éãşÿÿ¶    1ö¿   é[ÿÿÿt& èüÿÿÿUWVS‰Ã‹€X&   Ä ‹ %ÿÿ  «°   Çƒğ     ºà  ‰ƒ   ‰è³p  Çƒô     Çƒø  €»  Çƒü     Çƒ      Çƒ     èüÿÿÿ‹ƒ   ‰›D  ÁèPCPƒ   hñ  j Pèüÿÿÿ1Éº	   ‰ğj Çƒ@     èüÿÿÿ¹ 	˜ º0  ‰ğj h€   j jj hÿ   j j èüÿÿÿ¹	˜ º0  ‰ğƒÄ8j j@j jj jj j èüÿÿÿ¹	˜ º0  ‰ğƒÄ j j@j jj jj j èüÿÿÿ¹	˜ º0  ‰ğƒÄ j j j jj jjÿj€èüÿÿÿ¹	˜ º    ‰ğƒÄ j j j h  j hÿÿ  j j èüÿÿÿ¹		˜ º    ƒÄ ‰ƒà  ‰ğj j j jj jj j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ¹	˜ º    ‰ğƒÄ j h €  j h  j hÿÿ  j j èüÿÿÿ‹»¬  ‰³  ƒÄ …ÿuC‰êèüÿÿÿ‰Ç…Àt‰ğèüÿÿÿ‰ø[^_]Ã´&    f‰ØèùÛÿÿ‰ø[^_]ÃfèüÿÿÿVú }  ‰Ö•ÂşD¬  S‰Ã•À„Âtş€»  …ı  ƒ»ô  v2şD¬  „  ş€»  „r  ş }  „Æ   ‰³ø  1À[^Ãv şD¬  „$  ş€»  „€  ş }  uÒ¹0º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹  ºø  ‰Øèüÿÿÿ¹  º 	  ‰Øèüÿÿÿ¹  º	  ‰Øèüÿÿÿ¹  º	  ‰Øèüÿÿÿ¹p   º'  ‰Øèüÿÿÿ¹ÿ é‚   ¶    ¹ º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹÷º 	  ‰Øèüÿÿÿ¹÷º	  ‰Øèüÿÿÿ¹÷º	  ‰Øèüÿÿÿ¹`   º'  ‰Øèüÿÿÿ¹ÿ/ º,  ‰Øèüÿÿÿ¹ø. º(  ‰Øèüÿÿÿéœşÿÿf¹º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹­'R º  ‰Øèüÿÿÿ¹ªOº 	  ‰Øèüÿÿÿ¹ªOº	  ‰Øèüÿÿÿ¹ªOº	  ‰Øèüÿÿÿ¹V   º'  ‰Øèüÿÿÿ¹ÿ_ º,  ‰Øèüÿÿÿ¹ø“ º(  ‰Øèüÿÿÿéüıÿÿf¹º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹ò¡b º  ‰Øèüÿÿÿ¹Ymº 	  ‰Øèüÿÿÿ¹Ymº	  ‰Øèüÿÿÿ¹Ymº	  ‰Øèüÿÿÿ¹X   º'  ‰Øèüÿÿÿ¹ÿ’ º,  ‰Øèüÿÿÿ¹øK º(  ‰Øèüÿÿÿé\ıÿÿf¹ º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹tvº  ‰Øèüÿÿÿ¹ €ºø  ‰Øèüÿÿÿ¹UUº 	  ‰Øèüÿÿÿ¹UUº	  ‰Øèüÿÿÿ¹UUº	  ‰Øèüÿÿÿ¹`   º'  ‰Øèüÿÿÿ¹ÿ? éªşÿÿ¶    ¹$º  ‰Øèüÿÿÿ¹şâ+ º  ‰Øèüÿÿÿ¹ò¡b º  ‰Øèüÿÿÿ¹Í`ºø  ‰Øèüÿÿÿ¹…sº 	  ‰Øèüÿÿÿ¹…sº	  ‰Øèüÿÿÿ¹…sº	  ‰Øèüÿÿÿ¹d   º'  ‰Øèüÿÿÿ¹ÿa é²şÿÿ¸êÿÿÿé.üÿÿ´&    t& èüÿÿÿVS‹P‹ZÔ‹P(ú	˜ „œ  vRú	˜ „N  ú		˜ …Ş  ƒ»ô  ‹p|‡²   1À…ö¹ıÿÿÿºÓ  •À1öÀP‰ØèüÿÿÿX‰ğ[^Ã´&    ú	˜ „Ä   ú	˜ …Œ  ‹@|‰ÆÁş=ÿ€  ~9h€   ¹   ‰ØƒæºÕ  èüÿÿÿVºÕ  ‰Ø1ö¹€ÿÿÿèüÿÿÿX‰ğZ[^Ãt& j ¹   ºÕ  ‰Øèüÿÿÿ¸€ÿÿÿ‰ñ)È¶ÀPë¿¶    º  ‰Øèüÿÿÿ…ö„ì   ‰Ájº  ‰Øáï   Qèüÿÿÿ‰Ø¹   ºÓ  èüÿÿÿ[^1öé'ÿÿÿv ‹@|¹ä   ‰ÂÁú	=ÿ/  ~¹‰   )ÑÉ¶É‰Ø1öºÔ  èüÿÿÿ‰ğ[^Ãt& ‹@|º€ €1ö@Áá‰È÷êÊÁùÁú)ÑºÛ  A0¹Àÿÿÿ¶ÀP‰Øèüÿÿÿ‰ğY[^Ãt& ‹@|º€ €1ö@Áá‰È÷êÊÁùÁú)ÑºÙ  A0¹Àÿÿÿ¶ÀP‰Øèüÿÿÿ‰ğ[[^Ãt& ƒÈjº  ¶È‰ØQèüÿÿÿZYéPşÿÿ¾êÿÿÿéFşÿÿfèüÿÿÿSº  ‰Ãèüÿÿÿº  ‰Áj‰Øáï   Qèüÿÿÿº  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿ¹   ºÓ  ‰ØèüÿÿÿƒÄƒ»ô  vy¹p8ºĞ  ‰Øèüÿÿÿ‹“ø  ‰Øèùÿÿº  ‰Øèüÿÿÿº  ‰Áj‰Øáş   Qèüÿÿÿƒ»ô  YXw[Ãv º  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿXZ[Ãv ¹ºĞ  ‰Øèüÿÿÿë…´&    ¶    èüÿÿÿV‰ÖS‹˜”   ƒ»ô  v6º  ‰Øèüÿÿÿº  ‰Áj‰Øáï   Qèüÿÿÿ¹   ºÓ  ‰ØèüÿÿÿYXº  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿ‰ò‰Øè(øÿÿº  ‰Æ‰Øèüÿÿÿº  ‰Áj‰Øáş   QèüÿÿÿƒÄƒ»ô  v#º  ‰Øèüÿÿÿº  ƒÈj¶È‰ØQèüÿÿÿXZ‰ğ[^ÃfffffffèüÿÿÿUºc  WV‰ÆS¾   ƒìd¡    ‰D$1À‹FÇD$    ˆˆ   D$èüÿÿÿ‰D$‰|$…À…ƒ(  ÇD$   jÿ¹   º   ‰ğh0„  èüÿÿÿ¹ö   º  ‰ğhÿ   hö   èüÿÿÿ¹   º   ‰ğjÿh0„  èüÿÿÿ¹   º   ‰ğèüÿÿÿ‹D$,‹x‹ ‰|$‰D$ƒÄ…À„©   1ÿ´&    ‹$1Û¶8Áá	ùÉ   ‰Ífº   ‰é‰ğèüÿÿÿ¸Æ§  èüÿÿÿº   ‰ğèüÿÿÿ9ÅtH1èöÄ?uƒÃƒû
uÉƒl$…%ÿÿÿhc  ÿt$h'  èüÿÿÿ‹D$ èüÿÿÿƒÄÇD$ûÿÿÿéâ'  fƒÇ;|$…kÿÿÿ‹D$‹ëv 1Éh   É   º   ‰ğh   èüÿÿÿÿt$hc  ÿt$h4'  èüÿÿÿº   ‰ğ‹\$,èüÿÿÿƒÄ%ÿÿÿ º      ‰Á‰ğèüÿÿÿº   ‰ğèüÿÿÿ‰Ââ   ÿú   uÌ‰Â©ÿ?  tv º   ‰ğèüÿÿÿ©ÿ?  uí‰Â‹{‹…Û„u&  ¶€æ?Áà   9Âu:1íë´&    ¶/€ä?Áá	éÉ   9Èuº   ‰ğƒÅèüÿÿÿ9İu×éu&  éÿ'  ‰Â‰Èéÿ'  ffèüÿÿÿUJWzƒçüV)ùS‹°”   1ÀƒÁ`‰ÓÁé‹®è  ÇB    ÇB^    ó«1Àf‰º  ‰ğèüÿÿÿ¨tUå ù  ¿$  uTt& ‰ú‰ğèüÿÿÿ‰ÂƒàÀê·„ `  ¶Ò·”`  f‰„{ø÷ÿÿf‰”{È÷ÿÿƒÇ	Âf	ÿ5  u¼[1À^_]Ãt& ‰ú‰ğèüÿÿÿ‰ÂƒàÀê·„ `  ¶Ò·”`  f‰„{ş÷ÿÿf‰”{Î÷ÿÿƒÇ	Âf	ÿ0  u¼ë®´&    v èüÿÿÿS‹˜”   ‰Øèüÿÿÿ¶‹  º  ‰Øèüÿÿÿ‰Ø¹.   º  èüÿÿÿ1À[Ãt& èüÿÿÿU‰ÕWVSƒì‹°”   d‹    ‰T$1Ò‰à\$‹¾è  ç ù  t& Æ  ƒÀ9Øuö‰ğèüÿÿÿ¹2   º  ‰ğèüÿÿÿ¹   º  ‰ğèüÿÿÿ¶  º  ‰ğèüÿÿÿE…ÿ…®  ƒÅt& 1Ò1ÉƒÀf‰P.f‰Hş9èuíD$ë´&    fú u€ƒÀƒÅ9ØtQ·U0fú „  ‡ß   fƒú„ı   fú u€	·U fú „Ü   ‡   fƒúu¨€ƒÀƒÅ9Øu¯»$  …ÿ„Ş   ¶    ¶Œæûÿÿ‰Ú‰ğƒÃèüÿÿÿû1  uä‰Ú1É‰ğƒÃèüÿÿÿû5  uê¹   º<  ‰ğèüÿÿÿ¹   ºt  ‰ğèüÿÿÿ‹D$d+    …Ü   ƒÄ1À[^_]Ã´&    ffú @…ÿÿÿ€@éÿÿÿt& fú @…*ÿÿÿ·U €fú …'ÿÿÿv €`éÚşÿÿ€éÿÿÿ´&    €éõşÿÿ´&    ¶Œãûÿÿ‰Ú‰ğƒÃèüÿÿÿû5  uä¹   º<  ‰ğèüÿÿÿ¹&   é9ÿÿÿU¶    1ÉƒÀf‰H.1Éf‰Hş9ĞuíÇE^    ƒÅÇE    é:şÿÿèüÿÿÿfèüÿÿÿU‰Á‰ĞWVSƒì‹R‹±”   ¶
„Éu€zÿtÇ@    Ç@    ƒÄ1À[^_]Ãf€zÿuà¶Zˆ\$€ûUt€û‘uÎ¶Z‹¾  ƒã‰|$z‰İ¶Z‰<$ˆ\$‰ë€û„Ì   wJ€û„±   €û…˜   º @  ‰P‹T$1Éƒâ?T$€|$U”Á‹<$‰P‰‰xƒÄ1À[^_]Ã¶    1ö€û	uY‰D$‹$¶\2¶|2	¶›`  ¶¿`  ‰İÁã	ıƒç	é‰õ	ûƒÆÑıˆ(ƒşuËáğ   ‹D$º   „uÿÿÿ´&    Ç@    1Ò1Ééuÿÿÿº   éSÿÿÿ¶    ¶Z‰ÙÀé1Ë‰ÙÀé1Ù‰ËĞë1ÙƒátÃ¶J	‰ÊÀê1Ê‰ÑÀé1Ê‰ÑĞé1Êƒât¦º   é	ÿÿÿffffffèüÿÿÿVº  Ü ¾   S‰Ãèüÿÿÿ‹“„   ‰Ø1Éh   èüÿÿÿ‹ƒ„   „   X¸
   f‰2‹
ƒùtƒèuò‹ƒ„   ¾   ˆ   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   Ä   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   È   ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾     ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾     ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   D  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   H  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   Ä  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾   È  ¸
   t& ‰2‹
ƒùtƒèuò‹ƒ„   ¾      ¸
   t& ‰2‹
ƒù tƒèuò‹ƒ„   ¾      ¸
   t& ‰2‹
ƒù tƒèuò‹ƒ„   ¾@   D  ¸
   t& ‰2‹
ƒù@tƒèuò‹ƒ„   ¾@   H  ¸
   t& ‰2‹
ƒù@tƒèuò‹ƒ„   ¾€   „  ¸
   t& ‰2‹
ù€   tƒèuï‹ƒ„   ¾€   ˆ  ¸
   ´&    f‰2‹
ù€   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   „  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   ˆ  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   Ä  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   È  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾    „  ¸
   ´&    f‰2‹
ù    tƒèuï‹ƒ„   ¾    ˆ  ¸
   ´&    f‰2‹
ù    tƒèuï‹ƒ„   ¾ @  Ä  ¸
   ´&    f‰2‹
ù @  tƒèuï‹ƒ„   ¾ @  È  ¸
   ´&    f‰2‹
ù @  tƒèuï‹ƒ„   ¾ €    ¸
   ´&    f‰2‹
ù €  tƒèuï‹ƒ„   ¾ €    ¸
   ´&    f‰2‹
ù €  tƒèuï‹ƒ„   ¾   „  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   ˆ  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   Ä  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   È  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾     ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   D  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾   H  ¸
   ´&    f‰2‹
ù   tƒèuï‹ƒ„   ¾@Ü €   ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü À   ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ    ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ Ü @  ¸
   ´&    f‰2‹
ù Ü tƒèuï‹ƒ„   ¾@Ü À  ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü    ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ @  ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ 	Ü €  ¸
   ´&    f‰2‹
ù 	Ü tƒèuï‹ƒ„   ¾@	Ü    ¸
   ´&    f‰2‹
ù@	Ü tƒèuï‹ƒ„   ¾€	Ü @  ¸
   ´&    f‰2‹
ù€	Ü tƒèuï‹ƒ„   ¾À	Ü €  ¸
   ´&    f‰2‹
ùÀ	Ü tƒèuï‹ƒ„   ¾ 
Ü À  ¸
   ´&    f‰2‹
ù 
Ü tƒèuï‹ƒ„   ¾@
Ü @  ¸
   ´&    f‰2‹
ù@
Ü tƒèuï‹ƒ„   ¾€
Ü €  ¸
   ´&    f‰2‹
ù€
Ü tƒèuï‹ƒ„   ¾À
Ü À  ¸
   ´&    f‰2‹
ùÀ
Ü tƒèuï‹ƒ„   ¾ Ü    ¸
   ´&    f‰2‹
ù Ü tƒèuï‹ƒ„   ¾@Ü €  ¸
   ´&    f‰2‹
ù@Ü tƒèuï‹ƒ„   ¾€Ü À  ¸
   ´&    f‰2‹
ù€Ü tƒèuï‹ƒ„   ¾ÀÜ    ¸
   ´&    f‰2‹
ùÀÜ tƒèuï‹ƒ„   ¾ Ü @  ¸
   ´&    f‰2‹
ù Ü tƒèuï‹“„   ¸
   ¾` Ü ´&    ‰2‹
ù` Ü tƒèuï‹“„   ¸
   »   Â`  ‰‹
ƒùtƒèuò[^ÃfffffffèüÿÿÿWVS‹ô  ‹ZD…Ûty‹³8  ö    …«(  ‹ƒ4  ¿      èüÿÿÿ‹“4  ƒª   u,ö    …Ï(  Æ(  ‰ğèüÿÿÿ1Ò‰Øèüÿÿÿ‰Ç‰ğèüÿÿÿ‹ƒ4    èüÿÿÿ‰ø[^_Ã¿êÿÿÿëó´&    t& èüÿÿÿUWVSƒì‹°ô  ‹^D‰t$…Û„j  ‹»8  ö    …ä(  ·(  ‰ğèüÿÿÿ‰øèüÿÿÿ‰Å‰$‰ğèüÿÿÿ…í…)  ‹Gh‹ ƒø–   …À‰—   ‹D$‹@…À„
  ‹ƒ4    èüÿÿÿ‹“4  ‹‚   H‰Š   …Àu1ö    …)  ‰ğèüÿÿÿğ€‹T  ‰Øèüÿÿÿ‰$…Àx|‰ğèüÿÿÿ‹ƒ4    èüÿÿÿ‹$ƒÄ[^_]Ã´&    v ƒø	…iÿÿÿ‹‡X&  ‹ˆ  Õ ‹‡X&  É  O   Õ ¸
   t& ‰
‹*9é„6ÿÿÿƒèuïé,ÿÿÿ´&    v ö    …2)  ‹ƒ4  ƒ¨   ‹ƒ4  ‹€   …À…Vÿÿÿğ€£T  ïéIÿÿÿÇ$êÿÿÿéTÿÿÿ´&    fèüÿÿÿUºv  W‰ÇVSƒìd¡    ‰D$‹‡$  ‹€ØıÿÿÇ$    ‹˜8  ‹Cˆˆ   ‰àèüÿÿÿ‰Å…À…)  ‹$‹Hşƒù>‡G)  ¨…G)  ‹·È   1Ûë=t& ¸rI  f‰D$…öt¹   T$‰øèüÿÿÿ‹·È   ‹$ƒÃ‰Ğ9v*´&    ‹J¶ˆD$<VtqwO<Tt³<UuKƒÃ‰Ğ9wŞº]  f‰T$…öt¹   T$‰øèüÿÿÿ‹$èüÿÿÿ‹D$d+    uBƒÄ‰è[^_]Ã<Wt‹¶DˆD$…ö„zÿÿÿ¹   é\ÿÿÿ¹1Àÿÿf‰L$…ö…DÿÿÿéXÿÿÿèüÿÿÿt& èüÿÿÿUW‰ÇVSƒì0‹Ÿ4  d¡    ‰D$,‹‡8  ‰D$…Û„%  ‰;«(  ¹    º  Çƒü      hÀ  ‹@‰D$ˆ   P‰èèüÿÿÿY‰ÆX…öˆ  ‰»Ô  ƒ  ÇƒØ     ÇƒÜ     Çƒà  ğä  Çƒä  Pä  Çƒ     ‰$èüÿÿÿ‰Æ…Àˆ«  ‹$SÇC   ÇC    ‰Ñ‰C(‰ÈÇC,   ‰T$‰êèüÿÿÿ‰Æ…Àˆl  S‹‹¸  ‹$‰T$èüÿÿÿ‰Æ…ÀˆD  K‹$‰Î‰L$‹‹¸  ‰òèüÿÿÿ‰Æ…Àˆ  ‹‹Ä  ‹T$‹$èüÿÿÿ‰Æ…Àˆá  ‹‡4  ‰D$‹‡8  ‰D$‹@h‹ ƒø„H    ƒø„I  ƒø	…€   ¸à  èüÿÿÿ‰Á…À„Å  ‹D$°œR  ¸|  ‰òèüÿÿÿ…À„¬  ‹L$‰ˆ  ¸  èüÿÿÿ‰D$…À„¾  ‹D$‰ñº`   ‹€ˆ  h  ‹t$ èüÿÿÿZ…Àu
¸  èüÿÿÿ‹T$‹‚ˆ  …À„d  Ç€      ‰Ğ‹’ˆ  (  èüÿÿÿ‰Æ…ÀˆÈ   ‹D$º
   ‹ˆX&  Á$Ç v ¸€ € ‰‹ƒøÿt%€ € =€   tƒêuá‹$“Œ  ‰èèüÿÿÿ‹l$ƒÅUhÀ*  èüÿÿÿk‡t  d¹d   1ÒÁè
÷ñ‹‡t  Áè
RP‹‡4  ÿ·p  ÿ·<  ÿ°(  Uhà*  èüÿÿÿƒ  ¹¤   º•  ƒÄ$èüÿÿÿÇƒü     ëm´&    v ‹D$‹€ˆ  ‹°   …Òtèüÿÿÿ‹“È  ‹$èüÿÿÿ‹‹¼  ‹T$‹$èüÿÿÿ‹‹¼  ‹T$‹$èüÿÿÿ‹D$èüÿÿÿ‹$èüÿÿÿ‰èèüÿÿÿ‹D$,d+    …l  ƒÄ0‰ğ[^_]Ãfƒø§   ƒø…nşÿÿ¸t  èüÿÿÿ‰Á…À„Ú  ‹D$°U  ¸ä  ‰òèüÿÿÿ…À„d  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„=  ‹D$T$ ‹€ˆ  èüÿÿÿ…À„A  ‹ˆØ  …É„äıÿÿº|  èüÿÿÿéÕıÿÿt& …ÀˆÈıÿÿ¸”  èüÿÿÿ‰Á…À„±  ‹D$°œR  ¸ˆ  ‰òèüÿÿÿ…À„v  ‹L$‰ˆ  ¸¹  èüÿÿÿ‰D$…À„v  ‹D$‰ò¹   ‹t$‹€ˆ  èüÿÿÿ…À…Tıÿÿ¸¹  èüÿÿÿéEıÿÿt& ¸,  èüÿÿÿ‰Á…À„Ş  ‹D$°U  ¸´  ‰òèüÿÿÿ…À„¦  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„?  ‹D$T$ ‹€ˆ  èüÿÿÿ…À„  ‹ˆØ  …É„¶üÿÿº`  èüÿÿÿé§üÿÿhï  jèüÿÿÿ¸à  èüÿÿÿ^‰ÁX…É…üÿÿéĞ)  h  jèüÿÿÿ¸t  èüÿÿÿ^‰ÁX…É…şÿÿéî)  ´&    f¸,  èüÿÿÿ‰Á…À„è  ‹D$°U  ¸ø  ‰òèüÿÿÿ…À„­  ‹L$‰ˆ  ¸Q  ‰t$ ÆD$$aÇD$(    èüÿÿÿ‰Á…À„Ç  ‹D$T$ ‹€ˆ  èüÿÿÿ…Àt'‹ˆØ  …É„Êûÿÿº˜  èüÿÿÿé»ûÿÿ¾êÿÿÿéıÿÿ¸Q  èüÿÿÿé¢ûÿÿ¸à  èüÿÿÿ‹D$Ç€ˆ      ‹D$ƒÎÿƒÀPh+  èüÿÿÿXZé‡üÿÿ¶    ¸,  èüÿÿÿ‹D$Ç€ˆ      ëÄh;  jèüÿÿÿ¸,  èüÿÿÿ‰ÁXZ…É… şÿÿé,*  ´&    ¸”  èüÿÿÿ‹D$Ç€ˆ      éyÿÿÿh£  jèüÿÿÿ¸”  èüÿÿÿ‰ÁXZ…É…-ıÿÿéş)  t& ¸t  èüÿÿÿ‹D$Ç€ˆ      ¸,  èüÿÿÿ‰Á…À„e  ‹D$°U  ¸Ì  ‰òèüÿÿÿ…À…Rüÿÿ¸,  èüÿÿÿ‹D$Ç€ˆ      éçşÿÿ¸,  èüÿÿÿ‹D$Ç€ˆ      éÊşÿÿh;  jèüÿÿÿ¸,  èüÿÿÿ^‰ÁX…É…öıÿÿéZ*  t& h_  jèüÿÿÿ¸Q  èüÿÿÿ‰ÁXZ…É…şÿÿéJ*  hÉ  jèüÿÿÿ¸¹  èüÿÿÿ‰D$$ZY…À…füÿÿé*  ´&    h_  jèüÿÿÿ¸Q  èüÿÿÿ^‰ÁX…É…¡ûÿÿéJ*  ´&    fh_  jèüÿÿÿ¸Q  èüÿÿÿ^‰ÁX…É…ŸüÿÿéJ*  ´&    fh  jèüÿÿÿ¸  èüÿÿÿ‰D$$YZ…À…ùÿÿéÀ)  h;  jèüÿÿÿ¸,  èüÿÿÿ‰ÁXZ…É…yşÿÿéx*  èüÿÿÿfffffèüÿÿÿUWVSƒì‹\$‰$…Û„  ‰×öÂ…ÿ   ‹$‰ÅÁå	Åƒ|$†¿   ÷Ç   „  ¸
   t& f‰/f‹f9Õtƒèuğƒl$ƒÇƒ|$†ñ   ‰é‹t$·Åf‰l$Áá	Á‰ğø‰Åt& ‰ê¸
   )ò´&    ‰
‹9ÙtƒèuóƒîƒşwÛ‹D$‹L$·l$ƒèƒáÁè|‡ƒù††   ¸
   v f‰/f‹f9ÕtƒèuğAşƒÇ‰D$‹D$…Àt¶$¸
   t& ˆŠ8ÊtƒèuóƒÄ[^_]Ã‰Ê¸
   t& ˆŠ8Êtƒèuóƒl$ƒÇéÜşÿÿƒ|$‹L$‡ÿÿÿézÿÿÿ‰L$ë–‹L$ébÿÿÿv èüÿÿÿVS‰Ã‹€X&  °1Ç ¸
   ¶    ‰‹ƒùÿt…Êtƒèuî‹ƒX&  1Ç ‹ 	Â‹‹X&  ¸
   ‰“È1  Á1Ç ´&    ‰‹9Útƒèuó[^ÃèüÿÿÿS‹˜X&  ‹‹1Ç ÷Ò!Ê‹ˆX&  ‰È1  ¸
   Á1Ç v ‰‹9Útƒèuó[ÃèüÿÿÿVS‰Ã‹€X&  °D1Ç ¸
   ¶    ‰‹ƒùÿt…Êtƒèuî‹ƒX&  \1Ç ‹ 	Â‹‹X&  ¸
   ‰“Ì1  Á\1Ç ´&    ‰‹9Útƒèuó[^ÃèüÿÿÿS‹˜X&  ‹‹\1Ç ÷Ò!Ê‹ˆX&  ‰Ì1  ¸
   Á\1Ç v ‰‹9Útƒèuó[ÃèüÿÿÿS‹˜X&  ‹‹X1Ç ÷Ò!Ê‹ˆX&  ¸
   ÁX1Ç ´&    f‰‹9Útƒèuó[ÃèüÿÿÿS‹˜X&  ‹‹ø Ğ Áê€åàâ   	Ê‹ˆX&  ¸
   Áø Ğ ´&    ‰‹9Útƒèuó[ÃèüÿÿÿW‰×VpSVh
   èüÿÿÿ[1ÛXöÃuSVh   èüÿÿÿƒÄ¶Ph1   èüÿÿÿ‰ØZYƒàƒøuh9   èüÿÿÿXCû   u½[^_Ãÿ4$h    èüÿÿÿXZé|   ÿ4$h=   èüÿÿÿY[éd   ÿ4$h$   èüÿÿÿ‹†L&  []éC  ƒøtƒøtÇF,ÿÿÿÿ‰â‰èèAÿÿÿé   ÇF,  ‰è‰âƒÅè)ÿÿÿ¶D$P¶D$P¶D$	P¶D$PUhD   èüÿÿÿƒÄé   ÿt$h   èüÿÿÿ‹³X  Y1À_1Òéì  ÿt$hl  ëßÿt$hD  ëÔjVhl   èüÿÿÿƒÄÇD$ôÿÿÿé9  ÇD$íÿÿÿÿt$ÿt$h@	  èüÿÿÿ‹D$ èüÿÿÿ‰ØèüÿÿÿƒÄé9  ÿ3h¬   èüÿÿÿ‰Øèüÿÿÿ]Xé9  Rÿt$hP  èüÿÿÿƒÄº   ‰Øèüÿÿÿ‰Øèüÿÿÿ‹ƒT&    Ü ‰ƒ„   ‰Øèüÿÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ‰D$…À„À  ÿt$hÀ  èüÿÿÿ_]ƒ»T&   t.ö    tÿt$h$   èüÿÿÿY^‹ƒT&  èüÿÿÿÇƒT&      j ¸    h   ‹“H&  ‹‹L&  èüÿÿÿXZ‹ƒÔ1  èüÿÿÿéïşÿÿÿt$h@  èüÿÿÿ_]ÇD$ôÿÿÿéÒşÿÿÿ³L&  ÿ³H&  ÿt$hx  èüÿÿÿƒÄö    „Ç  ÿt$h˜  èüÿÿÿY^éÇ  ÿ3ÿt$h¼  èüÿÿÿƒÄÇD$ûÿÿÿétÿÿÿÿt$h”  èüÿÿÿ^_‹Ch…À„%  éQ	  ‹<$‹K‹wRP¶D$ P‰ğƒàÿ±L  P‰ğÁèƒàP‹G¶@tP¶ƒP&  P·A"Pÿt$(h€  èüÿÿÿƒÄ,ö    …¿  ‹ƒH&  ‹“L&  é—  ÿt$h¡   èüÿÿÿXZéf	  ÿvDÿv(ÿ6Wÿt$hÜ   èüÿÿÿƒÄé?  ÿ3ÿt$h  èüÿÿÿƒÄÇD$ûÿÿÿé–şÿÿPÿt$h@  èüÿÿÿ‹|$¹@   º   ‰øèüÿÿÿº   ‰øL$$èüÿÿÿƒÄé~  ÿ3ÿt$hÜ  èüÿÿÿƒÄÇD$ûÿÿÿé:şÿÿRÿt$ht  èüÿÿÿƒÄéxıÿÿ‹|$Wh  èüÿÿÿWh`  èüÿÿÿWh¼  èüÿÿÿWhô  èüÿÿÿƒÄ ÇD$ôÿÿÿéÄıÿÿÿqÿt$hÒ   èüÿÿÿ‹ChƒÄ…À…Q	  éØ  ÿpÿt$h´   èüÿÿÿƒÄéBşÿÿ‹Chö@@t$º@   ‰Øèüÿÿÿ…À„€   ÿt$hğ  èüÿÿÿY^ö    tÿt$h@  èüÿÿÿXZ‰Øèüÿÿÿ‰D$…ÀtNÿt$h`  èüÿÿÿ]Xéæüÿÿh   ÿ³L&  ÿ³H&  ÿt$hÔ  èüÿÿÿ‹ƒH&  ‹“L&  ƒÄé—  ƒK|@ëŠ‹shöF„Í  ‰ØT$4èüÿÿÿ‹D$`ƒøÿ„„  =‹$ „é   ‡¶   =ÿ# „Ø   ‡¥   =Ã# „Ç   ‡”   =›# „¶   ‡ƒ   =_# „¥   wv=7# „˜   wi=-# „‹   w\=y" „„  wO=o" „w  wB=Q" „j  w5==" „]  w(=! tN=9! w=$! w.…À„M  =  „Ë   Pÿt$h   èüÿÿÿ1ÀèüÿÿÿƒÄëˆÛŞşÿº Óê€âtÓ1Àèüÿÿÿ‰Ch‹P‰Sx‹P‰Sl‹€Ä   ‰CpRÿt$hÂ  èüÿÿÿƒÄƒ|$@„  ƒ»\  ÿ„˜  ƒ»`  ÿ„š  ‹ƒ¤Y  ‰Ç» Y  u8‹T$D‰Ñáÿ·ÿ ùÿ·ÿ …´  ö    …f  ºÿÿÿ 1À‰“ Y  ‰ƒ¤Y  ;sht‰Ø{èüÿÿÿ‹Cp´FL9ø…G  ‹sh‹F…ÀtPÿt$hÙ  èüÿÿÿ‹shƒÄƒ~ uÇD$íÿÿÿ‰Øèüÿÿÿéúÿÿ‰Øèüÿÿÿ‰Øèüÿÿÿ‹C1Éº    ‹€L  Sÿt$h€   èüÿÿÿ‰D$ƒÄ…ÀtPÿt$hÔ  èüÿÿÿƒÄë¨‹ƒ¤Y  ƒ Y  uÇƒ Y     Çƒ¤Y      ƒ»\  ÿ‹Chu~‹³ Y  ‹»¤Y  ‹¬   ‹ˆ°   !ò!ù	Ê…µ  #°¸   #¸¼   	÷…Á  ‹°°   ‹¬   ‰÷	×t4‰“ Y  ‰³¤Y  „Ò„Á  Çƒ Y     Çƒ¤Y      ‹´   ‰“\  ƒ»`  ÿu1Ò€xp •Â‰“`  1É€|È …]  ˆKv€xL …a  1ÒˆSw1Ò9Ñ„~  €|Ğ…z  ‰“œY  ¶TĞ¿   ‰,$‰İ‰“˜Y  ‹\$‹p…ş…}  ƒÃÿƒû uî‰ë‹,$öC|tº   ‰Øèüÿÿÿ‰C`‹Ch‹P…Òt
‰Øèüÿÿÿ‰Cd÷ƒ Y   ù  „  ÆCu1Ò€{u ‰è”Âèüÿÿÿƒ»`     ‹ƒ\  Kx   …Àˆ¾  fÇD$ ÿ ‰D$$ÇD$,    ÇD$(   ƒøGº    {¸    EÂ‰D$0‹Cp´FL9Ç„Q  ‹Fd‹@…Àt‹H$…ÉtT$ ‰ğèüÿÿÿ‹vLƒîLëÑ1ÒkÒ‹”´   ‰“\  ƒÂ„Dşÿÿé…şÿÿº   ëÛƒÁƒù„—şÿÿé‡şÿÿ€xXÒƒÂé“şÿÿöÆ°t(Çƒ Y     Çƒ¤Y      é5şÿÿ1Òé†şÿÿƒÂékşÿÿâ  ÿ „şÿÿÇƒ Y    @ Çƒ¤Y      éşÿÿƒÿ@„zşÿÿwƒÿ t,wƒÿtKw
ƒÿtƒÿt‰Ú‰èèüÿÿÿ…À…Oşÿÿ	}|éGşÿÿº    ‰èèüÿÿÿ…À…3şÿÿƒM| é*şÿÿÆCtémşÿÿƒM|éşÿÿ‹ƒ\  …ÀxJfÇD$ ÿ ‰D$$ÇD$(   ÇD$,    é‡şÿÿƒ|$$Gu#‹ƒ\  ÇD$    ‰D$‹Cp´FL9Ç…  ‹ƒ Y  ‹“¤Y  ‰ƒ¨Y  5ÿÿÿ 	Ğ‰“¬Y  uÇƒ Y     Çƒ¤Y      ‰Øèüÿÿÿ‰D$…Àt'Pÿt$hø  èüÿÿÿƒÄ‹C‰Ú‹€L  èüÿÿÿéßûÿÿ‰Øèüÿÿÿ‰D$…Àt Pÿt$h	  èüÿÿÿº   ‰ØèüÿÿÿƒÄë¼ÿslÿt$hâ  èüÿÿÿƒ´Y  Çƒ°Y  àÿÿÿ‹    ‰ƒ´Y  ‹°Y  ‰ƒ¸Y  ¸    Çƒ¼Y      èüÿÿÿƒÄé9  ÿt$h€  èüÿÿÿY_éùúÿÿ‹Fd‹@…Àt‹H(…ÉtT$‰ğèüÿÿÿ‹vLƒîLéÊşÿÿÿt$hè  èüÿÿÿY_é[úÿÿ‹D$@‰ƒ\  éYúÿÿ1Àƒ|$4 •À‰ƒ`  éQúÿÿö†   @t‹Fd‹ …Àt‹H…Ét	1Ò‰ğèüÿÿÿ‹vLƒîLéúÿÿ„Òt-ö    tÿt$h<  èüÿÿÿ‹ƒ¤Y  Y_‹“ Y  ƒÊé/úÿÿöÆ°t-ö    tÿt$h`  èüÿÿÿX‹ƒ¤Y  Z‹“ Y  €Îéıùÿÿâ  ÿ t0ö    tÿt$h„  èüÿÿÿ_X‹ƒ¤Y  ‹“ Y  Ê  @ éÅùÿÿÿt$h¨  èüÿÿÿ‹“ Y  _‹ƒ¤Y  Y€Îé¡ùÿÿÿt$h  èüÿÿÿXZé…ùÿÿ¸	   èüÿÿÿéòøÿÿÿt$h¬  èüÿÿÿXZétùÿÿCPhş  èüÿÿÿ‰ØèüÿÿÿZY…ÀtCğ€‹6&  @¸úÿÿÿé  CPhş  èüÿÿÿ‰ØèüÿÿÿZY…À„Á  ğ€‹6&  @¸úÿÿÿé  é  ¸4  èüÿÿÿ¸,  èüÿÿÿ¸4  èüÿÿÿ¸,  èüÿÿÿ¸¼  èüÿÿÿ¸´  èüÿÿÿ¸´  èüÿÿÿ@Phy  èüÿÿÿXZéÇ  ¸   èüÿÿÿ@Ph—  èüÿÿÿXZéò  @Phè  èüÿÿÿXZé’  ÿ4$h  ‹D$ƒÀPh¬  èüÿÿÿ‹D$@‰D$‹ ‰D$ƒÄé˜%  ‹D$¿ûÿÿÿƒÀPhŒ  èüÿÿÿXZél'  h  ‹D$¿ûÿÿÿƒÀPh  èüÿÿÿƒÄél'  ‹|$h3  _ShÔ  èüÿÿÿShø  èüÿÿÿ‰ø1Ò¿ôÿÿÿèüÿÿÿƒÄél'  W‹D$(¶T$.ÁàÁâ	Ğ¶T$/	ĞPh3  ‹D$ƒÀPh`  èüÿÿÿ‹D$DƒÄ‹8éV'  V‹|$GPh  èüÿÿÿ‹D$<èüÿÿÿ‰ø1Ò¿ûÿÿÿèüÿÿÿƒÄél'  ÿt$‹|$GPh  èüÿÿÿ‹D$<èüÿÿÿ‰ø1Ò¿ûÿÿÿèüÿÿÿƒÄél'  ‹\$h  ¿ôÿÿÿƒÃShD  èüÿÿÿShh  èüÿÿÿƒÄél'  ‹|$‹$DÿPWÿt$4h<  èüÿÿÿƒÄé¡&  CPhÜ  èüÿÿÿXZéO)  CPhD  èüÿÿÿXZét/  ‰L$PRÿ³<  ÿt$hl  èüÿÿÿ‹E‹U L$,‰B‰‹D$0‰|$0‰M ‰E‰8ğÿ‹œ  ƒÄ‹L$é¼2  ÿ°<  ‹€8  ƒÀPh¸  èüÿÿÿƒÄéÓ7  ÿµ<  ‹D$ƒÀPh  èüÿÿÿ‰èèüÿÿÿ¸ôÿÿÿƒÄéÌ:  ‹D$XSh4  èüÿÿÿhj  She  èüÿÿÿ¸ôÿÿÿƒÄéÌ:  ‹½t  ‰Ë¾d   1Ò¯ßkÃdÁë
Áè
÷öRSWQÿµ<  ‹|$(_Shì  èüÿÿÿ‹€   ‹…p  ƒÄ”–  Áâú   zÿÿÿ‰L  …À„À:  éÔ8  FPhÀ  èüÿÿÿ‹ƒ@  ZYé@  FPhš  èüÿÿÿXZé¹?  FPhd  èüÿÿÿZYéóH  ÿ4$ÿt$jPRÿt$$‹D$ƒÀPh(  èüÿÿÿƒÄ évG  Fÿ³<  Ph   èüÿÿÿƒÄégA  Rÿt$hÂ  èüÿÿÿƒÄé~M  Fÿ¶È  ¿ôÿÿÿPh  èüÿÿÿƒÄéæM  ‰D$ÿt$hì  èüÿÿÿY_‹T$‹D$…ÀEĞ‰T$é¬O  ÿ³<  Qÿt$h@  èüÿÿÿƒÄé¬O  ‹$¿   ‹¬°|  kÅd™÷ÿ¿d   ™÷ÿ‰è¿   R™÷ÿP‹D$ÿ´°`  ÿ³<  Qÿt$h  èüÿÿÿƒÄé¬O  ‰T$Uÿ³<  ÿt$hÄ  èüÿÿÿ‹T$Çƒ¬      ƒÄéQÿÿÿ‹$‹”°`  ‹ƒ<  …Òt#‹<$ÿ´·|  RPQÿt$hd  èüÿÿÿƒÄé¬O  PQÿt$h@  èüÿÿÿƒÄé¬O  ƒÆVhQ  èüÿÿÿ1ÀY[éàR  ƒÆVh˜  èüÿÿÿ^X¸êÿÿÿéàR  ƒÆPVhä  èüÿÿÿ¸ğÿÿÿƒÄéàR  ƒÆÿ³<  Vh  èüÿÿÿƒÄéÍS  ÿ4$PFPh4  èüÿÿÿƒÄéèT  ƒÆVht  èüÿÿÿXZéU  i$ğ  ÿ´Ø  CPhL  èüÿÿÿƒÄéÑV  CPhØ  èüÿÿÿXZéæU  CPhü  èüÿÿÿ_XéuW  CPh(  èüÿÿÿZYéğU  CPh¬  èüÿÿÿY_é¸V  iD$Hğ  ‹\$8ÿ´Ø  GPhn  èüÿÿÿƒÄéù_  iD$Hğ  ‹\$8‹„Ô  ÿt$LiT$Lğ  ‹\$<ƒÀÿ´Ø  ÿt$8Ph   èüÿÿÿƒÄé!`  iD$Hğ  ‹\$8ÿ´Ø  GPhƒ  èüÿÿÿ¡    ƒÄéQ`  ‹D$»òÿÿÿÿ°Ø  W‹D$0ƒÀPhè  èüÿÿÿƒÄÇ$òÿÿÿé#]  iD$Hğ  ‹|$8ÿ´Ø  Gÿt$4Phˆ  èüÿÿÿƒÄéèW  ÿ²Ø  ‹D$HƒÀPh°  èüÿÿÿ¡    ƒÄéeX  G‰L$Ph|  èüÿÿÿXZ‹L$éàa  G‰L$Ph½  èüÿÿÿY]‹L$é	b  ƒÇÿt$¾   ÿµØ  WhH  èüÿÿÿƒÄéDb  CPh¨  èüÿÿÿXZéÃc  WƒÃÿ³œ  ShH  èüÿÿÿƒÄéd  G‰L$Phü  èüÿÿÿYX‹L$é^e  G‰L$Ph0  èüÿÿÿXZ‹L$éÛe  G‰L$PhÔ  èüÿÿÿXZ‹L$éşd  ÿ´Ø  G‰L$PhØ  èüÿÿÿ‹L$ƒÄéßd  CPh\  èüÿÿÿYXéf  ƒÃShõ  èüÿÿÿXZéf  CPhŒ  èüÿÿÿY^é_f  ƒÃSh  èüÿÿÿXZélf  i$ğ  ÿ´Ø  FPhØ  èüÿÿÿƒÄéæf  Gÿ¶<  Ph  èüÿÿÿƒÄéqh  ‹…  …Àt<ƒÇPWh¼  èüÿÿÿ‹D$èüÿÿÿƒÄÇ$úÿÿÿéĞh  ƒÇWhà  èüÿÿÿXZéÇh  ‹…è   ë¼ƒÁQh  èüÿÿÿYXéán  ƒÁQhL  èüÿÿÿYXéán  ƒÁPQh3  èüÿÿÿ¸êÿÿÿƒÄéËn  ƒÁQh(  èüÿÿÿXZéÅn  ƒÁQhp  èüÿÿÿXZéán  CVQPh”  èüÿÿÿ‹ShƒÄé–o  ƒÃShQ  èüÿÿÿX1ÀZép  GÿvPhÀ  èüÿÿÿƒÄéTp  Fÿ¶¤Y  ÿ¶ Y  Phì  èüÿÿÿƒÄéq  èüÿÿÿUWVSì„   ‹ZXd¡    ‰„$€   1À{ÿslhx  Wh~  èüÿÿÿƒÄöC|tT$4‰Øèüÿÿÿ‹Ckp´FL9èt‹Fd‹ …Àt‹…Òt‰ğèüÿÿÿ‹vLƒîLëÛ·“œY  L$4‰Ø³X  «Ü)  èüÿÿÿ·“˜Y  ‰Ø‰áèüÿÿÿD$8PWh›  èüÿÿÿD$PWh²  èüÿÿÿ‰ğèüÿÿÿÿ³ X  ÿ³üW  Wh  èüÿÿÿ‰ğ³L  ƒÄ(èüÿÿÿ‹ƒ4&  ºu  ¨ ¸o  DÂPWhÉ  èüÿÿÿƒ$  ‰úèüÿÿÿÿ³4&  WhÚ  èüÿÿÿƒÄƒ¾üşÿÿ t4‹NÀ…Ét-‹Q‹VÄ¯ÑÁê
R¯FÈ1ÒkÀd÷ñPÿv¤ÿvŒWhH  èüÿÿÿƒÄÆğ  9õu¹ÿ³”1  ÿ³1  ÿ³Œ1  ÿ³ˆ1  Wh  èüÿÿÿƒÄ‹„$€   d+    tèüÿÿÿÄ„   1À[^_]ÃGPhô  èüÿÿÿX¸³ÿÿÿZé$s  GPh¸  èüÿÿÿ¸³ÿÿÿ^_é$s  G‰$Ph(  èüÿÿÿY[‹$és  G‰$Php  èüÿÿÿY[‹$é9s  PGPh3  èüÿÿÿ¸êÿÿÿƒÄé$s  G‰$Ph  èüÿÿÿ_X‹$é¬s  G‰$PhL  èüÿÿÿXZ‹$éÌs  ¸ˆ  èüÿÿÿ¸€  èüÿÿÿîô  Vh0  èüÿÿÿX1ÀZé~ƒ  Ç†,%      îô  Vh|  èüÿÿÿ¸ôÿÿÿY[é~ƒ  †şÿÿPh¼  èüÿÿÿXZécƒ  kÃÿ°¨  ÿt$$‹D$ƒÀPh   èüÿÿÿƒÄé]‡  P‹D$ƒÀPh@  èüÿÿÿ¸êÿÿÿƒÄé@ˆ  kÛÿ³¨  V‹D$ƒÀPh´  èüÿÿÿ¸ûÿÿÿƒÄé@ˆ  kÃÿ°¨  ‹D$ƒÀPhh  èüÿÿÿƒÄéB†  ƒÇÿr@PWhä  èüÿÿÿƒÄéŒ  ‹CƒÇR‹…¤  PPWh  èüÿÿÿƒÄéŒ  ƒèP‹D$ƒÀPh_  èüÿÿÿƒÄéqŒ  SÿuD‹D$ƒÀPhK  èüÿÿÿƒÄéŒ  öEºF  ¸@  ÿu DÂPV‹D$ƒÀPh  èüÿÿÿƒÄéqŒ  ÿu@‹D$ƒÀPhô  èüÿÿÿƒÄéqŒ  Vÿ³<  ÿt$$hL  èüÿÿÿƒÄ…ít4¡    %  =  …§  ÿu ÿ³<  ÿt$$h   èüÿÿÿƒÄé§  ÿ³<  Vÿt$$ht  èüÿÿÿƒÄéB  ‹,$ÿu ‹D$ƒÀPhè  èüÿÿÿƒÄé_  P‹D$ƒÀPhÄ  èüÿÿÿƒÄéqŒ  PGPhÄ  èüÿÿÿƒÄéÌ’  Gÿµ$2  Phô  èüÿÿÿƒÄéÌ’  ‹­¤  oRPPUh€  èüÿÿÿ¡    ƒÄ¨„ù’  ¨„ù’  ‹$L$º   ƒÀDèÌ„  P‹D$ÿp\ÿp@ÿp$ÿp hu  Uhì  èüÿÿÿƒÄ éù’  CVPh™  èüÿÿÿ¹   T$HƒÄé[”  Vjh~  èüÿÿÿ1ÀƒÄéˆ”  ƒÃRSh¶  èüÿÿÿ1ÀƒÄéÑ•  PCWVPh¼  èüÿÿÿƒÄéË¢  ¸M  PSht   èüÿÿÿ‹D$ƒÄƒèƒø‡  ÿ4$Shœ   èüÿÿÿƒÄÿµø  Sh!  èüÿÿÿ‹…X&   Ä ‹ Áè‰D$‹…X&  ‹Ä ‹…X&  Ä ‹ Áè‰D$‹…X&  ‹°Ä ‹…X&  Ä ‹ Áè‰D$‹…X&  ‹¸Ä ‹…X&  ÁïĞÄ ‹ Áè‹­ô  ‰D$¶ÂƒÄ€úş„  =ÿ   „  ƒø„  ‡  ƒø„  ‡ø  ƒø„  ƒø„æ  ƒø„¥  ‡×  ƒø„¶  ‡È  ƒø„»  ‡¹  …À„Á  ƒèºf  ¸»  DÂPSh8!  èüÿÿÿ¶D$ƒÄ=ÿ   „–  =ş   „Ÿ  =ı   „î  ƒø„  ‡  ƒø„&  ƒø„	  ƒø„4  ‡ú  ƒø„9  ƒø„è  ƒø„;  ‡Ù  ƒø„6  ƒø„Ç  ƒø
„.  ‡¸  ƒø	„)  ƒø
„¦  ƒø„!  ‡—  ƒø„  ƒø„…  ƒø„  ‡v  ƒø„  ƒø„d  ƒø„  ‡U  ƒø„  ƒø„C  ƒø„ú  ƒøºÌ  ¸»  DÂPSh`!  èüÿÿÿºš  ¸–  ƒÄöD$DÂPShˆ!  èüÿÿÿº¥  ¸  ƒÄö$DÂPSh°!  èüÿÿÿ‰ò‰ñƒæÀê¶ÉBÿ‰L$ƒÄ<†2  h­  ShØ!  èüÿÿÿƒÄö$„Y  ¸­  PSh "  èüÿÿÿƒÄ…í„S  USh("  èüÿÿÿƒÄ‹L$‰Èƒà€á„X  ¸­  PSh„"  èüÿÿÿƒÄƒşuB÷Ç   ºÁ  ¸·  EÂƒçPSh¬"  èüÿÿÿƒÄƒÿ;  ¸­  PShÔ"  èüÿÿÿƒÄƒÄ1À[^_]Ã¶ğ‹µ   éDüÿÿ‹<$‰øÁøƒàP‰øÁøƒàPShÌ   èüÿÿÿƒÄéLüÿÿ¸¯  é_ıÿÿ¸»  éUıÿÿ¸¢  éKıÿÿ¸{  évşÿÿ¸”  é7ıÿÿ¸e  ébşÿÿ¶ÀˆT$ÿ4…`  ShØ!  èüÿÿÿ¶T$ƒÄ€ú…·şÿÿÿ4µ   Shü"  èüÿÿÿƒÄé»şÿÿ‹µ@  é şÿÿ¸N  éşÿÿShX"  èüÿÿÿXZéªşÿÿ¸„  é·üÿÿ¸9  éâıÿÿ‹…à  é¡şÿÿ¸»  éÌıÿÿ¸v  éüÿÿ¸4  é¸ıÿÿ‹½À  é¾şÿÿ¸r  émüÿÿ¸(  é˜ıÿÿ¸m  éYüÿÿ¸ø  é„ıÿÿ¸a  éEüÿÿ¸   épıÿÿ¸  éfıÿÿ¸  é\ıÿÿ¸  éRıÿÿ¸  éHıÿÿ¸ì  é>ıÿÿ¸å  é4ıÿÿ¸Ş  é*ıÿÿ¸×  é ıÿÿ¸Ñ  éıÿÿ¸Ç  éıÿÿ…Ò¹Ê  ¾Ñ  ‰$DÎƒÀpQPhÙ  èüÿÿÿ‹T$ƒÄéÉ¥  ÿ4$W‹D$ƒÀpPh$#  èüÿÿÿ¸ŞÿÿÿƒÄé;©  ÿt$Uÿt$W‹D$ƒÀpPhH#  èüÿÿÿƒÄéâ§  ƒ   QRPh€#  èüÿÿÿƒÄéî¶  Ã   VShè#  èüÿÿÿ¸êÿÿÿƒÄéº  Ã   WSh¼#  èüÿÿÿ¸êÿÿÿƒÄéº  ¬Ğ¿@B ³   ‰L$L™÷ÿRPVhä$  èüÿÿÿ‹L$\ƒÄö    „b¿  Qÿt$Lÿt$Lÿt$Lÿt$@h  ÿt$Tÿt$Tÿt$ThĞ  ÿt$TVh %  èüÿÿÿƒÄ4éb¿  ‰L$Xƒ   VWR‰T$\‰D$XPh$  èüÿÿÿ‹T$d‹L$lƒÄ…ö„b¿  ö    „¿  ‰Ğ1Ò‰|$P¹Èô´ÇD$T    ¤ÂÁàD$PT$TiúÈô´÷áú¬Ğ1Ò÷ö¾@B ™‰Ç÷şRPÿt$ThL$  èüÿÿÿ‹L$hƒÄö    „¿  ‰ø‰L$P¹   ™÷ù™÷ş‰ø¾ z R™÷şPÿt$Tht$  èüÿÿÿ‹L$`ƒÄö    „¿  jyjÿt$Th¤$  èüÿÿÿ¡    ‹L$`ƒà‰D$\¸Èô´ƒÄ÷é‰Ö‰Ç1Ò…ö„K¿  é*¿  ‰úƒÀp1ö¶ÒRPh´%  èüÿÿÿƒÄéXÅ  ‰úƒÀp¶ÒRPh´%  èüÿÿÿƒÄéàÅ  Shc  ÿt$h”&  èüÿÿÿ‹D$$º   ‹h   ‰ğh   É   èüÿÿÿƒÄh € ƒÉÿº  ‰ğèüÿÿÿ¹    º	  ‰ğèüÿÿÿ¹   º	  ‰ğèüÿÿÿ¹‡¶ Vº$  ‰ğèüÿÿÿ¹ö   º  ‰ğhÿÿ ?hö   èüÿÿÿ¹   ºÌ	  ‰ğèüÿÿÿ‹†X&   Ç ‹ ƒÄöÄ…â   ‹†X&   Ç ‹ ‰ÂƒâÏ¨ „ù   ‹†X&  ‰×¹
   Ï    Ç ‰8‹ƒûÿt€ã0tƒéuí‰ĞƒÈ ‹X&  ‰ÇÏ   “ Ç »
   ‰:‹
ƒùÿt1Á€á0tƒëuëº  ‰ğèüÿÿÿº  ÿ hÿÿ ?P‰Á‰ğèüÿÿÿ‹D$èüÿÿÿXZ‹D$d+    …³   ‹D$ƒÄ[^_]ÃRPhc  ÿt$hÄ&  èüÿÿÿƒÄé’şÿÿ€äû¹
   ‰Â‹†X&   Ç ‰‹ƒûÿt	€ç„÷şÿÿƒéuééíşÿÿ‹¾X&  Ê    Ç ¿
   ‰‹ƒûÿtƒã0ƒû „ÿÿÿƒïuæéÿÿÿhc  ÿt$hp&  èüÿÿÿƒÄÇD$êÿÿÿé<ÿÿÿèüÿÿÿÿ°0  ·€  PFPh\'  èüÿÿÿƒÄétä  FPhŒ'  èüÿÿÿXZé¥ä  ÿ°0  ·€  PGPh´'  èüÿÿÿƒÄé å  GPh(  èüÿÿÿ]Xé£å  ƒÇWhä'  èüÿÿÿXZé×å  ƒÇWhD(  èüÿÿÿZYéIæ  ƒÃP½¬ÿÿÿhv  Sh@)  èüÿÿÿ‹D$èüÿÿÿÇD$    Sh˜(  èüÿÿÿShô(  èüÿÿÿƒÄ é„ç  ƒÃhv  Shp(  èüÿÿÿSh˜(  èüÿÿÿShô(  èüÿÿÿƒÄé„ç  h *  èüÿÿÿXéìé  hĞ)  èüÿÿÿ‹D$Ç€ˆ      Yébî  h*  èüÿÿÿYéï  hp)  èüÿÿÿ‹D$Ç€ˆ      ^ébî  h )  èüÿÿÿXéìé  h0*  èüÿÿÿ‹D$Ç€ˆ      Xébî  h`*  èüÿÿÿYéìé  h0*  èüÿÿÿ‹D$Ç€ˆ      Yébî  h0*  èüÿÿÿ‹D$Ç€ˆ      Xébî  èüÿÿÿVS‹˜4  ‹€8  ƒÀPh¤  èüÿÿÿXZ…Ûtyƒ»ü   tp³  ‹“   ‰ğèüÿÿÿƒŒ  èüÿÿÿ‹‹¼  S‰ğèüÿÿÿ‹‹¼  S‰ğèüÿÿÿCèüÿÿÿ‰ğèüÿÿÿ‹ƒˆ  èüÿÿÿ‹ƒˆ  èüÿÿÿƒ(  [^éüÿÿÿ[^ÃèüÿÿÿSh  hd	  èüÿÿÿƒ=    XZvjh	  èüÿÿÿ[ƒËÿXëX=    ÿ  vhĞ	  Ç        èüÿÿÿY¹"  º    ¸    èüÿÿÿ‰Ã…Àth 
  »íÿÿÿèüÿÿÿZëh'  èüÿÿÿX‰Ø[Ã¸    éüÿÿÿ                                                                                   enc_pcm_bufs    enc_vbi_bufs    enc_yuv_bufs    enc_idx_bufs    enc_mpg_bufs    enc_ts_bufs enc_pcm_bufsize enc_yuv_bufsize enc_idx_bufsize enc_mpg_bufsize enc_ts_bufsize  enc_pcm_buffers enc_vbi_buffers enc_yuv_buffers enc_idx_buffers enc_mpg_buffers enc_ts_buffers  cx18_first_minor    cx18_pci_latency    retry_mmio  mmio_ndelay debug   ntsc          secam        pal      cardtype                     radio                   tuner                           ñ  z[  ÿÿÿÿÿÿÿÿ                                                R  Z  d  strnlen strscpy R  n  x  ‚    š                  	                        
                             ¦  $
  Q        y           0          †                                                                                                   ÿÿÿ     G               4    »  ƒ"$          „  z[}f                         À  Œ
  Q        q           0          †                                                                                                   ÿÿÿ     G               4    »  ƒ"$          „  z[}'o                         Ø  Œ
  Q        Q           0                                                                                                                          ÿÿÿ     G               4    »  c
23
       B   „  z[y                         ô    Q        1           0         p                                                          ğ                        ğ        @  ÿ ÿ     &               4   S  S	"3	           „  z[ñ	                          +     Q        {           0         p                                                                                               ÿÿÿ     G               4    ½  f	26          „  z[TXC3                         h     Q        {           0         p                                                                                               ÿÿÿ     G               4    ½  f	26          „  z[«                         C    Q        Q           0                                                                                                                          ÿÿÿ     G               4    S  „3$           „	  z[[ á                         Œ  °  Q       ß                                                                               0  0  0      
   (                                              4       s##                                          	   Y  °  Q       ß                                                                               8  8  8      
   (                                                      ‚"D                                               Y  °  Q       ß                                                                               0  0  0      
   (                                              4       ‚"D                                           __fortify_strlen    strnlen strscpy                             ­  ³  ¼  Ã  Ï  Ú  ã  ó           L    qcx18_firmware_init                              `                              °)               ,                                              `       À                              `+                                                                          @-           +                                                          à(                                                               ;  `=  €;  0=                  µ              
      ÿÿÿÿ                                 P %              7      ÿÿÿÿ       C                                                                                                              HM12 (YUV 4:1:1)                HM12                         MPEG                            MPEG                          UYVY 4:2:2                      UYVY                ÿÿÿÿ       ÿÿÿÿ   ÿÿÿÿÿÿÿÿÿÿÿÿstrnlen strscpy                 Ğv  l                          i              j      |                                             Ğj       ~                              pj              ğz      à|                              pv  v  °u      @u                      °t  @t   m          ğq   m                                                  0r  r  0o                      ğk  @l              x  àr   n                  Pr  Àm  @m          P{  g                                                   n  D¬  €»   }  `„   ƒ  P‚  ƒ               Ì       æ    @   õ    @               /       E       \       Ì       0       t       Š       ¢  	     À       P       Ú       õ       	       +	       C	       [	      x	       “	       °	       Ç	       Ş	       õ	       
       t       +
       E
        c
  !     ˜  "     
  #     –
       ­
      Å
      Ù
        ñ
                                   5  7  ;  ?  C              ù  4  Ç  Ì                     &  1  f  A  M  Y  e  h  l  p  t  v  4  Ç  Ì  x  ‹    ­  ­  ­  à%  &  ¦  ¹  @&  Ô  Û  æ  ñ  4  Ç  Ì  Ñ  ×  Ş  å  ì             ü  M        	      !  +  1  7  >  E  I  M  Q  W  [  _     À  °  `  @                                 °¦                                                                          €º          €Ä                          °¥                                               º           ¤  ¶          €£  Ğ±                                      @¤           °  °                                      ©          0Í                          ğxpğ´<4´°80°ğxpğÒZRÒ––’’ÒZRÒĞXPĞ””ĞXPĞğxpğ´<4´°80°ğxpğáiaá¥-%¥¡)!¡áiaáÃKCÃ‡‡ƒƒÃKCÃÁIAÁ……	ÁIAÁáiaá¥-%¥¡)!¡áiaáàh`à¤,$¤ (  àh`àÂJBÂ††‚
‚ÂJBÂÀH@À„„€ €ÀH@Ààh`à¤,$¤ (  àh`àğxpğ´<4´°80°ğxpğÒZRÒ––’’ÒZRÒĞXPĞ””ĞXPĞğxpğ´<4´°80°ğxpğ        @                    adapter_nr                À  cx18_ext_init  cx18_reset_ir_gpio  cx18_start_v4l2_encode_stream  cx18_stop_v4l2_encode_stream  cx18_claim_stream  cx18_release_stream  cx18-alsa 6%s: eeprom dump:
 6%s: eeprom %02x: c %02x c
 6%s:  info: Removing Card
 6%s: Removed %s
 cx18 tveeprom tmp cx18-%d 6%s: Initializing card %d
 6%s: Ignore card
 6%s: User specified %s card
 6%s: Autodetected %s card
 3%s: Defaulting to %s card
 &cx->serialize_lock &cx->gpio_lock &cx->epu2apu_mb_lock &cx->epu2cpu_mb_lock %s-in %s &cx->cap_w &cx->mb_apu_waitq &cx->mb_cpu_waitq &cx->dma_waitq cx18 encoder 3%s: Invalid EEPROM
 6%s: Autodetected %s
 6%s: %s 6%s: Initialized card: %s
 4%s: Retry loading firmware
 1.5.1 cx18 6cx18:  End initialization
 xc3028-v27.fw Tuner 1 Line In 1 Line In 2 S-Video 1 S-Video 2 Composite 1 Composite 2 Component 1 Leadtek WinFast DVR3100 H Leadtek WinFast PVR2100 Toshiba Qosmio DVB-T/Analog Conexant Raptor PAL/SECAM Analog TV capture supported
 GoTView PCI DVD3 Hybrid Compro VideoMate H900 Hauppauge HVR-1600 rc-hauppauge 6%s:  i2c: i2c init
  #%d-%d 6%s:  i2c: i2c exit
 tuner tveeprom cs5345 cx23418_DTV cx23418_AV gpio_mux gpio_reset_ctrl ir_z8f0811_haup v4l-cx23418-cpu.fw 3%s: Mismatch at offset %x
 v4l-cx23418-apu.fw gpio-mux gpio-reset-ctrl %s %s 3%s: Max buffers = %zu
 include/linux/dma-mapping.h 6%s:  info: Stop Capture
 encoder MPEG 6%s: Disabled %s device
 &s->waitq %s %s 3%s: DVB failed to register
 TS encoder YUV encoder VBI encoder PCM audio encoder IDX encoder radio 6%s:  info: Start Read VBI
 6%s:  info: EOS %s
 6%s:  info: User stopped %s
 include/linux/thread_info.h 6%s:  file: Encoder poll
 6%s:  ioctl: close() of %s
 6%s:  info: Mute
 6%s:  info: Unmute
 6%s:  file: open %s
 6%s:  ioctl: Unknown cmd %d
 6%s:  info: Input unchanged
 Radio TV 1.5.1 6%s: Version: %s  Card: %s
 6%s: Video Input: %s
 6%s: Audio Input: %s
 6%s: Tuner: %s
 6%s: Status flags: 0x%08lx
 cx18 Radio Tuner cx18 TV Tuner cx18 PCI:%s  %#010x 4%s: unknown cmd %x
 stale good 6%s:  info: %x %s
 6%s: FW version: %s
 incoming 3%s: cx == NULL (cmd=%x)
 3%s: args too big (cmd=%x)
 4%s: Unknown cmd %x
 CX18_CPU_SET_CHANNEL_TYPE CX18_EPU_DEBUG CX18_CREATE_TASK CX18_DESTROY_TASK CX18_CPU_CAPTURE_START CX18_CPU_CAPTURE_STOP CX18_CPU_CAPTURE_PAUSE CX18_CPU_CAPTURE_RESUME CX18_CPU_SET_VIDEO_IN CX18_CPU_SET_VIDEO_RATE CX18_CPU_SET_VIDEO_RESOLUTION CX18_CPU_SET_FILTER_PARAM CX18_CPU_SET_MEDIAN_CORING CX18_CPU_SET_INDEXTABLE CX18_CPU_SET_AUDIO_PARAMETERS CX18_CPU_SET_VIDEO_MUTE CX18_CPU_SET_AUDIO_MUTE CX18_CPU_SET_MISC_PARAMETERS CX18_CPU_SET_RAW_VBI_PARAM CX18_CPU_SET_CAPTURE_LINE_NO CX18_CPU_SET_COPYRIGHT CX18_CPU_SET_AUDIO_PID CX18_CPU_SET_VIDEO_PID CX18_CPU_SET_VER_CROP_LINE CX18_CPU_SET_GOP_STRUCTURE CX18_CPU_SET_ASPECT_RATIO CX18_CPU_SET_SKIP_INPUT_FRAME CX18_CPU_SET_SLICED_VBI_PARAM CX18_CPU_GET_ENC_PTS CX18_CPU_SET_VFC_PARAM CX18_CPU_DE_SET_MDL_ACK CX18_CPU_DE_SET_MDL CX18_CPU_DE_RELEASE_MDL CX18_APU_START CX18_APU_STOP CX18_APU_RESETAI CX18_CPU_DEBUG_PEEK32 0 APU CPU EPU HPU  not  automatic detection mono stereo dual tri mono with SAP stereo with SAP dual with SAP tri with SAP forced mode not defined EIAJ A2-M A2-BG A2-DK1 A2-DK2 A2-DK3 A1 (6.0 MHz FM Mono) AM-L NICAM-BG NICAM-DK NICAM-I NICAM-L IF FM Radio BTSC detected chrominance unknown audio standard forced audio standard no detected audio standard yes no running stopped undefined system DK system L enable disable 6%s:  info: %s output
 %s %03x Chroma autodetect mono/language A language B language C analog fallback language AC language BC language AB BG DK1 DK2 DK3 I L FM Radio (4.5 MHz) FM Radio (5.5 MHz) S-Video MONO2 (LANGUAGE B) MONO3 (STEREO forced MONO) STEREO DUAL1 (AC) DUAL2 (BC) DUAL3 (AB) FM radio 0x0 NTSC-M NTSC-J NTSC-4.43 PAL-BDGHI PAL-M PAL-N PAL-Nc PAL-60 0x9 0xA 0xB SECAM 0xD 0xE 0xF v4l-cx23418-dig.fw dvb-cx18-mpc718-mt352.fw cx18 s5h1409_attach symbol:s5h1409_attach mxl5005s_attach symbol:mxl5005s_attach s5h1411_attach symbol:s5h1411_attach tda18271_attach symbol:tda18271_attach zl10353_attach symbol:zl10353_attach xc2028_attach symbol:xc2028_attach mt352_attach symbol:mt352_attach &dvb->feedlock 6%s: unregister DVB
 xc3028-v27.fw         @   à  `  P  p      p     €  0       @  p  P  °  à  €     Ğ   $  à(  0)  °)   *   +  `+  à+   ,  @-  ğ-  À.  P/  à/  €0  °0   1   1  Ğ1  02  `4  4  `5  `7  À7  €8   ;  €;  À;   =  0=  `=  ?  0A  àI  àJ   O  €P  Q  °Q  ğQ  €R  S  PT  PU  W  a  €c  0d  °d   f  Pf  Àf   h  i  j  pj  Ğj  ğk  @l  l   m  @m   m  Àm   n   n  0o  `o  0p  °p  ğq  r  0r  g  Pr  àr  @t  °t  @u  °u  v  pv  Ğv  `w  x  ğz  P{  |  à|   ~     P     ‚  @‚  P‚   ƒ  ƒ  `„  Ğ„  0…  ‹  PŒ   ‘  À“  Ğ“   ”  °”  p•  p˜  pŸ  @¡   ¡  €£   ¤  @¤  à¤  °¥  °¦  ©  P«  °   °  Ğ°  @±  Ğ±  p²  ¶  °¶  €º   º  Àº   »  p»  »  À»  à»  0¼  €¼  €Ä  €Æ  ğÈ  0Í  PÏ  PĞ  0Ñ  °Ó  °Ô  ğÔ  `×   Ù  Pä  ğä  æ  àç  š*  àğ  Pò  Àò   ó  pó  °ó  ğó  6%s:  info: Stopping all streams
  6%s:  info: releasing enc_mem
 6%s: eeprom PCI ID: %02x%02x:%02x%02x
 3cx18: cannot manage card %d, driver has a limit of 0 - %d
    3cx18: v4l2_device_register of card %d failed
 6%s:  info: Stream type %d options: %d MB, %d buffers, %d bytes
   4%s: pal= argument not recognised
 4%s: secam= argument not recognised
   4%s: ntsc= argument not recognised
    3%s: Unknown user specified type, trying to autodetect card
   6%s: Autodetected Hauppauge card
  3%s: Unknown card: vendor/device: [%04x:%04x]
 3%s:               subsystem vendor/device: [%04x:%04x]
   3%s: Please mail the vendor/device and subsystem vendor/device IDs and what kind of
   3%s: card you have to the linux-media mailinglist (www.linuxtv.org)
   3%s: Prefix your subject line with [UNKNOWN CX18 CARD].
   3%s: Unable to create incoming mailbox handler thread
 6%s:  info: base addr: 0x%llx
 6%s:  info: Enabling pci device
   3%s: Can't enable device %d!
  3%s: No suitable DMA available, card %d
   3%s: Cannot request encoder memory region, card %d
    6%s: Unreasonably low latency timer, setting to 64 (was %d)
   6%s:  info: cx%d (rev %d) at %02x:%02x.%x, irq: %d, latency: %d, memory: 0x%llx
   6%s:  info: attempting ioremap at 0x%llx len 0x%08x
   3%s: ioremap failed. Can't get a window into CX23418 memory and register space
    3%s: Each capture card with a CX23418 needs 64 MB of vmalloc address space for the window
 3%s: Check the output of 'grep Vmalloc /proc/meminfo'
 3%s: Use the vmalloc= kernel command line option to set VmallocTotal to a larger value
    6%s: cx23418 revision %08x (A)
    6%s: cx23418 revision %08x (B)
    6%s: cx23418 revision %08x (Unknown)
  3%s: Could not register A/V decoder subdevice
 4%s: Could not register GPIO reset controllersubdevice; proceeding anyway.
    6%s:  info: activating i2c...
 3%s: Could not initialize i2c
 6%s: Unknown EEPROM encoding
  3%s: Unknown model %d, defaulting to original HVR-1600 (cardtype=1)
   3%s: tveeprom cannot autodetect tuner!
    6%s:  info: Worldwide tuner detected
  6%s:  info: PAL tuner detected
    6%s:  info: NTSC tuner detected
   6%s:  info: SECAM tuner detected
  6%s: No tuner detected, default to NTSC-M
 3%s: Failed to register irq %d
    3%s: Error %d setting up streams
  3%s: Error %d registering devices
 3%s: Error %d on initialization
   6cx18:  Start initialization, version %s
  3cx18:  Exiting, cx18_first_minor must be between 0 and %d
    6cx18:   Debug value must be >= 0 and <= 511!
 3cx18:   Error detecting PCI card
 Simultaneous DVB-T and Analog capture supported,
	except when capturing Analog from the antenna input.
 Experimenters and photos needed for device to work well.
	To help, mail the linux-media list (www.linuxtv.org).
    Experimenters needed for device to work well.
	To help, mail the linux-media list (www.linuxtv.org).
   Yuan MPC718 MiniPCI DVB-T/Analog    Hauppauge HVR-1600 (Preproduction)  Simultaneous Digital and Analog TV capture supported
   6%s:  info: Preparing for firmware halt.
  3%s: %s: couldn't stop CPU to load firmware
   3%s: Unable to open firmware %s
   3%s: Did you put the firmware in the hotplug firmware directory?
  6%s: loaded %s firmware (%zu bytes)
   3%s: unable to open firmware %s
   3%s: did you put the firmware in the hotplug firmware directory?
  6%s:  info: load segment %x-%x
    6%s: loaded %s firmware V%08x (%zu bytes)
 3%s: Could not start the CPU
  6%s: GPIO:  direction 0x%08x, value 0x%08x
    6%s:  info: Resetting IR microcontroller
  6%s:  info: GPIO initial dir: %08x/%08x out: %08x/%08x
    6%s:  info: Resetting XCeive tuner
    4%s: Skipped %s, MDL %d, %d times - it must have dropped out of rotation
  6%s:  info: Deallocating buffers for %s stream
    6%s:  info: Allocate %s stream: %d x %d buffers (%d.%02d kB total)
    3%s: Too many buffers, cannot fit in SCB area
 %s %s: rejecting DMA map of vmalloc memory
 3%s: Couldn't allocate buffers for %s stream
  6%s: ignoring gop_end: not (yet?) supported by the firmware
   6%s:  info: Start encoder stream %s
   6%s:  info: Setup VBI h: %d lines %x bpl %d fr %d %x %x
   6%s:  warning: Error starting capture!
    3%s: Couldn't allocate cx18_dvb structure for %s
  3%s: Couldn't register v4l2 device for %s (device node number %d)
 6%s: Registered device %s for %s (%d x %d.%02d kB)
    6%s: Registered device %s for %s
  6%s: Registered device %s for %s (%d x %d bytes)
  4%s: MPEG Index stream cannot be claimed directly, but something tried.
   6%s:  info: Stream %d is busy
 6%s:  warning: Release stream %s not in use!
  6%s:  info: dualwatch: change stereo flag from 0x%x to 0x%x.
  6%s:  info: dualwatch: changing stereo flag failed
    6%s:  warning: IDX capture start failed
   6%s:  info: IDX capture started
   6%s:  warning: VBI capture start failed
   6%s:  info: VBI insertion started
 6%s:  warning: Failed to start capturing for stream %s
    6%s:  file: read %zd bytes from %s
    6%s:  warning: Stream %s not initialized before read
  6%s:  warning: copy %zd bytes to user failed for %s
   6%s:  file: read %zd from %s, got %zd
 6%s:  info: Could not start capture for %s (%d)
   6%s:  file: Encoder poll started capture
  6%s:  file: Encoder mmap started capture
  6%s:  info: close stopping capture
    6%s:  info: close stopping embedded VBI capture
   6%s:  info: close stopping IDX capture
    3%s: Can't find valid task handle for mute
    3%s: Can't find valid task handle for unmute
  3%s: Failed to initialize on %s
   6%s:  warning: nomem on v4l2 open
 6%s:  ioctl: V4L2_ENC_CMD_START
   6%s:  ioctl: V4L2_ENC_CMD_STOP
    6%s:  ioctl: V4L2_ENC_CMD_PAUSE
   6%s:  ioctl: V4L2_ENC_CMD_RESUME
  6%s:  info: Changing input from %d to %d
  6%s:  info: v4l2 ioctl: set frequency %d
  6%s:  info: Switching standard to %llx.
   6%s: GPIO:  direction 0x%08x, value 0x%08x
    6%s: Stream %s: status 0x%04lx, %d%% of %d KiB (%d buffers) in use
    6%s: Read MPEG/VBI: %lld/%lld bytes
   3%s: Can't find valid task handle for V4L2_ENC_CMD_PAUSE
  3%s: Can't find valid task handle for V4L2_ENC_CMD_RESUME
 6%s:  info: disabled insertion of sliced VBI data into the MPEG stream
    4%s: Unable to allocate buffers for sliced VBI data insertion
 6%s:  info: enabled insertion of sliced VBI data into the MPEG PS,when sliced VBI is enabled
  6%s:  api: %s	cmd %#010x args%s
   4%s: Unknown RPU (%d) for API call
    3%s: mbox was found stuck busy when setting up for %s; clearing busy and trying to proceed
    6%s:  api: waited %u msecs for busy mbox to be acked
  6%s:  irq: sending interrupt SW1: %x to send %s
   6%s:  warning: sending %s timed out waiting %d msecs for RPU acknowledgment
   6%s:  warning: woken up before mailbox ack was ready after submitting %s to RPU.  only waited %d msecs on req %u but awakened with unmatched ack %u
   6%s:  warning: failed to be awakened upon RPU acknowledgment sending %s; timed out waiting %d msecs
   6%s:  api: waited %u msecs for %s to be acked
 6%s:  api: mailbox error %08x for command %s
  4%s: Unhandled RPU (%d) for command %x ack
    6%s:  warning: Possibly falling behind: %s self-ack'ed our incoming %s to EPU mailbox (sequence no. %u) while processing
  4%s: Got DMA done notification for unknown/inactive handle %d, %s mailbox seq no %d
   4%s: Fell behind! Ignoring stale mailbox with  inconsistent data. Lost MDL for mailbox seq no %d
  6%s:  dma: DMA DONE for %s (MDL %d)
   4%s: Could not find MDL %d for stream %s
  6%s:  dma: %s recv bytesused = %d
 4%s: Unknown CPU to EPU mailbox command %#0x
  4%s: Unknown APU to EPU mailbox command %#0x
  4%s: Unable to find blank work order form to schedule incoming mailbox command processing
 6%s:  warning: Possibly falling behind: %s self-ack'ed our incoming %s to EPU mailbox (sequence no. %u)
   6%s:  api: %s: req %#010x ack %#010x cmd %#010x err %#010x args%s
 CX18_CPU_SET_STREAM_OUTPUT_TYPE CX18_CPU_SET_SPATIAL_FILTER_TYPE    CX18_CPU_SET_SCENE_CHANGE_DETECTION CX18_CPU_SET_USERDATA_PLACE_HOLDER  6%s:  irq: received interrupts SW1: %x	SW2: %x  HW2: %x
   BTSC/EIAJ/A2-M Mono (4.5 MHz FMMono)    6%s: Video signal:              %spresent
 6%s: Detected format:           %s
    6%s: Specified standard:        %s
    6%s: Specified video input:     Composite %d
  6%s: Specified video input:     S-Video (Luma In%d, Chroma In%d)
  6%s: Specified audioclock freq: %d Hz
 6%s: Detected audio mode:       %s
    6%s: Detected audio standard:   %s
    6%s: Audio muted:               %s
    6%s: Audio microcontroller:     %s
    6%s: Configured audio standard: %s
    6%s: Configured audio mode:     %s
    6%s: Specified audio input:     Tuner (In%d)
  6%s: Specified audio input:     External
  6%s: Preferred audio mode:      %s
    6%s: Selected 65 MHz format:    %s
    6%s: Selected 45 MHz format:    %s
    6%s: Configured audio system:   %s
    3%s: %dx%d is not a valid size!
   6%s:  info: decoder set size %dx%d -> scale  %ux%u
    6%s:  info: decoder set video input %d, audio input %d
    3%s: 0x%06x is not a valid video input!
   3%s: 0x%04x is not a valid audio input!
   6%s:  info: PLL regs = int: %u, frac: %u, post: %u
    6%s:  info: Video PLL = %d.%06d MHz
   6%s:  info: Pixel rate = %d.%06d Mpixel/sec
   6%s:  info: ADC XTAL/pixel clock decimation ratio = %d.%03d
   6%s:  info: Chroma sub-carrier initial freq = %d.%06d MHz
 6%s:  info: hblank %i, hactive %i, vblank %i, vactive %i, vblank656 %i, src_dec %i, burst 0x%02x, luma_lpf %i, uv_lpf %i, comb 0x%02x, sc 0x%06x
  6%s:  info: changing video std to fmt %i
  automatic standard and mode detection   MONO1 (LANGUAGE A/Mono L+R channel for BTSC, EIAJ, A2)  MONO4 (NICAM ANALOG-Language C/Analog Fallback) 3%s: unable to open firmware %s
   6%s: verified load of %s firmware (%d bytes)
  3%s: verification of %s firmware load failed: expected %#010x got %#010x
  3%s: unable to load firmware %s
   6%s: loaded %s firmware (%d bytes)
    6%s:  info: Stop feed: pid = 0x%x index = %d
  6%s:  info: Stopping Transport DMA
    6%s:  info: Start feed: pid = 0x%x index = %d
 3%s: Failed to initialize firmware starting DVB feed
  6%s:  info: Starting Transport DMA
    6%s:  info: Failed to start Transport DMA
 3%s: Unable to open firmware file %s
  3%s: The MPC718 board variant with the MT352 DVB-T demodulator will not work without it
   3%s: Run 'linux/scripts/get_dvb_firmware mpc718' if you need the firmware
 3%s: Firmware %s has a bad size: %lu bytes
    3DVB: Unable to find symbol s5h1409_attach()
  3DVB: Unable to find symbol mxl5005s_attach()
 3DVB: Unable to find symbol s5h1411_attach()
  3DVB: Unable to find symbol tda18271_attach()
 3DVB: Unable to find symbol zl10353_attach()
  3DVB: Unable to find symbol xc2028_attach()
   3DVB: Unable to find symbol mt352_attach()
    6%s: DVB Frontend registered
  6%s: Registered DVB adapter%d for %s (%d x %d.%02d kB)
    3%s: frontend initialization failed
   p  ¢  Å  ¸  é  L0  1  2  ï2  3  ”  Ë?  @  @  @  T@  “A  ŸB  ¦B  ÈD  ÷H  %I  ]I  vI  ¤R  S  %S  ¹S  ïS   T  T  8T  ¡U  ÛU  V  
V  HV  `V  ¼V  W  MW  bW  yW  ¢_  +a  3a  ­e  ²e  Vg  i  Rs  ás  ®å  næ  firmware=xc3028-v27.fw version=1.5.1 license=GPL description=CX23418 driver author=Hans Verkuil parm=cx18_first_minor:Set device node number assigned to first card parm=enc_pcm_bufs:Number of encoder PCM buffers
			Default is computed from other enc_pcm_* parameters parm=enc_pcm_bufsize:Size of an encoder PCM buffer (kB)
			Default: 4 parm=enc_pcm_buffers:Encoder PCM buffer memory (MB). (enc_pcm_bufs can override)
			Default: 1 parm=enc_vbi_bufs:Number of encoder VBI buffers
			Default is computed from enc_vbi_buffers parm=enc_vbi_buffers:Encoder VBI buffer memory (MB). (enc_vbi_bufs can override)
			Default: 1 parm=enc_yuv_bufs:Number of encoder YUV buffers
			Default is computed from other enc_yuv_* parameters parm=enc_yuv_bufsize:Size of an encoder YUV buffer (kB)
			Allowed values are multiples of 33.75 kB rounded up
			(multiples of size required for 32 screen lines)
			Default: 102 parm=enc_yuv_buffers:Encoder YUV buffer memory (MB). (enc_yuv_bufs can override)
			Default: 2 parm=enc_idx_bufs:Number of encoder IDX buffers
			Default: 63 parm=enc_idx_bufsize:Size of an encoder IDX buffer (kB)
			Allowed values are multiples of 1.5 kB rounded up
			(multiples of size required for 64 index entries)
			Default: 2 parm=enc_idx_buffers:(Deprecated) Encoder IDX buffer memory (MB)
			Ignored, except 0 disables IDX buffer allocations
			Default: 1 [Enabled] parm=enc_mpg_bufs:Number of encoder MPG buffers
			Default is computed from other enc_mpg_* parameters parm=enc_mpg_bufsize:Size of an encoder MPG buffer (kB)
			Default: 32 parm=enc_mpg_buffers:Encoder MPG buffer memory (MB). (enc_mpg_bufs can override)
			Default: 2 parm=enc_ts_bufs:Number of encoder TS buffers
			Default is computed from other enc_ts_* parameters parm=enc_ts_bufsize:Size of an encoder TS buffer (kB)
			Default: 32 parm=enc_ts_buffers:Encoder TS buffer memory (MB). (enc_ts_bufs can override)
			Default: 1 parm=mmio_ndelay:(Deprecated) MMIO accesses are now never purposely delayed
			Effectively: 0 ns parm=retry_mmio:(Deprecated) MMIO writes are now always checked and retried
			Effectively: 1 [Yes] parm=cx18_pci_latency:Change the PCI latency to 64 if lower: 0 = No, 1 = Yes,
			Default: Yes parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
			  4/0x0004: mailbox
			  8/0x0008: dma
			 16/0x0010: ioctl
			 32/0x0020: file
			 64/0x0040: i2c
			128/0x0080: irq
			256/0x0100: high volume
 parm=ntsc:Set NTSC standard: M, J, K parm=secam:Set SECAM standard: B, G, H, D, K, L, LC parm=pal:Set PAL standard: B, G, H, D, K, I, M, N, Nc, 60 parm=cardtype:Only use this option if your card is not detected properly.
		Specify card type:
			 1 = Hauppauge HVR 1600 (ESMT memory)
			 2 = Hauppauge HVR 1600 (Samsung memory)
			 3 = Compro VideoMate H900
			 4 = Yuan MPC718
			 5 = Conexant Raptor PAL/SECAM
			 6 = Toshiba Qosmio DVB-T/Analog
			 7 = Leadtek WinFast PVR2100
			 8 = Leadtek WinFast DVR3100 H
			 9 = GoTView PCI DVD3 Hybrid
			 10 = Hauppauge HVR 1600 (S5H1411)
			 0 = Autodetect (default)
			-1 = Ignore this card
		 parm=radio:Enable or disable the radio. Use only if autodetection
			fails. 0 = disable, 1 = enable parm=tuner:Tuner type selection,
			see tuner.h for values parmtype=enc_pcm_bufs:int parmtype=enc_vbi_bufs:int parmtype=enc_yuv_bufs:int parmtype=enc_idx_bufs:int parmtype=enc_mpg_bufs:int parmtype=enc_ts_bufs:int parmtype=enc_pcm_bufsize:int parmtype=enc_yuv_bufsize:int parmtype=enc_idx_bufsize:int parmtype=enc_mpg_bufsize:int parmtype=enc_ts_bufsize:int parmtype=enc_pcm_buffers:int parmtype=enc_vbi_buffers:int parmtype=enc_yuv_buffers:int parmtype=enc_idx_buffers:int parmtype=enc_mpg_buffers:int parmtype=enc_ts_buffers:int parmtype=cx18_first_minor:int parmtype=cx18_pci_latency:int parmtype=retry_mmio:int parmtype=mmio_ndelay:int parmtype=debug:int parmtype=ntsc:string parmtype=secam:string parmtype=pal:string parmtype=cardtype:array of int parmtype=radio:array of int parmtype=tuner:array of int firmware=v4l-cx23418-apu.fw firmware=v4l-cx23418-cpu.fw firmware=v4l-cx23418-dig.fw firmware=dvb-cx18-mpc718-mt352.fw parm=adapter_nr:DVB adapter numbers parmtype=adapter_nr:array of short srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                 ¤ÿ ¼              ¤ÿ À               ¤ÿ Ä   0           ¤ÿ È   @           ¤ÿ Ì   P           ¤ÿ Ğ   \           ¤ÿ Ô   l           ¤ÿ Ø   |           ¤ÿ Ü   Œ           ¤ÿ à   œ           ¤ÿ ä   ¬           ¤ÿ è   ¼           ¤ÿ ì   Ì           ¤ÿ ğ   Ü           ¤ÿ ô   ì           ¤ÿ ø   ü           ¤ÿ ü             ¤ÿ                ¤ÿ ¸   4          ¤ÿ ´   @          ¤ÿ    L          ¤ÿ     T          ¤ÿ \  d          ¤ÿ l  t          ¤ÿ x  €          ¤ÿ Œ             ¤ÿ ¨  ¼          ¤ÿ Ä  €          $ÿ Œ                      D  @                                   "  à  P  @                                                                                                                                     ÿÿÿÿÿÿÿÿÿÿÿÿ?   ÿÿÿÿÿÿÿÿ   f                                - -- --                      ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿşÿ  B C şÿa ` şÿ    şÿ  C şÿ  a ` şÿ                            º  @               Ğ      º  @               Ğ      º  @               Ğ              ²                   ²               P  Ğ     æ          ²           @                                                                            o             ƒo           à«   à«               c    R  $ô    È   ü     ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿŒ:  ~  F		˜]  ¡  é 	             GNU  À       À               GNU óÅß™-GÖÙÔ”ºçÜ•|LÄ‡‡        Linux                Linux                                           cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                                !              "              #             ñÿ              '             A            	 Y       2     n       c     ‹   @        —   c   B     ¨   ¥   ]     ¾   P      É           ! _
  ü        a  Ì        	
  ø        ²	  ô        [	  ğ        	  ì        ­  è        ®  Ğ          È        Å  Ä        w  À        )  ¼        X  ä          à        «  Ü        T  Ø        ı  Ô        ã               €    ! Ì     €     j     €     ›         N          ×           ! ß           ! ç           ! ï           ! ÷           !            ! 	          !           !   ¸            ›    +           2    c     O           \      ”     l      
     {           “  à  @                ·  %        Î  1        é  L        ÿ  `   D       ¤   g     ;    F     Z  Q  _     y  °  \     •    _     ´  k  g     Ğ  Ò  ³     ï  …  _       ä  ?     *  #  °     I  Ó       h  a  g     „  È  G     £    _     Â  n  d     İ  Ò  E     û    \       s  a     4  Ô  d     N  8  ^     n  –  í     ƒ  ƒ	  %     —  ¨	  4     ¬  Ü	  :     ¿  
  í    ×    d     ì  g  ;       ¢       !           6           O  ¼       o          „            Ö       ½  (        Ò           ë  ğ         <           0        9  
       Y  P        n  @        ‡  $       ¦  d        º  P        Ò  =       õ  x          \        )  Z       L  Œ        d  l        €  w       £           »  |        ×  ”       ú  ´          Œ        .  ±       P  È        g  œ        ‚  Í       ¥  Ü        ½  ¬        Ù  ê       ü  ğ        	  ¼        0	         S	         k	  Ì        ‡	  $       ª	         Â	  Ü        Ş	  A       
  ,       
  ì        5
  ^       W
  @       n
  ü        ‰
  z       ­
  T       Æ
         ã
  ˜         h                  =  ¶       [  |       n  4       c  ´        …  Î       ¤         ¸  @       ¬         ! Ğ  ç       é  ¤       ÷  L       	  ú       !  ¸       .  T       ?  \       S         l  Ì       z  d       Œ  l       ¡  %       ¸  à       Ä  t       Ô  x       ç  9         ô         €  	     )  Œ       >         I  X       b         p          ‚  ¨       ”         œ  t       µ         Ã  ¼       Õ  Ä       ç         ï           ñÿü  `  (       <                 ,  4       7  ,       B          O         i     ä     €   	  ä     š     ä     ©     ä     º     ä     Ô     ä     ò     ä          ä     '     ä     >   
  ä     X  4       e  „         „       ˜  „       µ  „       Î  „       ä  „       ô  „	                            ñÿ  0  o     &     o     2    %     >  @  %     J  à        Y          `         i  (       ,  ¼       7  ´       €  <  )     “            e       ±           ñÿÁ  z       Ù    ¢    “         ñ         	  ¬       !           ñÿ-            J  "          i  $         	 †  à(  G     ™  1       ±  °)  G     Æ   *  ü     Ñ   +  X     á  `+  r     ï  à+  ¹     ş   ,         @-  £     &           2  @        @  F       [  `  0     n  `  0          ,     ‘         £  À  P     µ           ñÿÂ  à/  Ÿ     Ú  [  L     ò  §  "                  É  Ì     0           ñÿ?  #          g  A          ‘  0         	 ¹  B          à  _          	  <         	 0   ;  u     =  €;  1     J  À;  9    l  0=  "     {  `=  .    Š  •  0     ¬  Å  Z     Ï  À  Œ     à  ¤        ! è  `  $     û              8     &  W      A           ñÿP  `          l  r          Š            	 ¦  s          Ä  ‡          ä           	   m  M       º       2  PT  ó     A  Ö  /     U    x     m  }        ›  _     ™  ú  2     ­  ,  |     Ä  ¨  *     Ó  Ò  *     ä  ü  $     ù     j                ñÿ  i  x     -  j  Z     @  pj  Z     U  Ğj      h  ğk  G     {  @l  N     Œ  l  †     ¢     À     ¬   m       ¹  @m  Q     Ê   m       Õ  Àm  Q     â   n  u     ï   n         Š  p       0o  &     +  ú  2     =  ,       S  E  "     c  ğq       s  r       €  0r         g  Æ    Ÿ  Pr  „     ¬  àr  S    ½  -  ¼     Ó  @t  a     â  °t       ğ  @u  a     û  °u  X       v  X       pv  X        Ğv       .  é       A  ˆ       ,  €       L  `w  ª     d  x  à    u  `          ğz  Z     ”  P{  ¾     ª  |  Ì     Ä  à|  2    à   ~  Ú     ú     N         Ü               ñÿ,  P‚  ¦     B   ƒ       T  ƒ  G    j  ı  Y     …  `„  c        |       ¨           ñÿ·  Ğ„  \     Â  0…  `    Ğ         Ù  V  “     ì  ‹  ¼     ÷  é  =       ¤         &  1    )  W        C  ÷  7     R  °”  º     h  .       {           ñÿ†           ñÿ“           ñÿ            ñÿ«  G       Á           ñÿĞ  €£  “     à   ¤       ğ  @¤  “          @       `  {    &  `  <     0     @     :  @        D  à        N  À       X  à¤  Ğ     e  °¥  õ     v  Û#  -     Œ  °¦  Õ    œ  $  G     ±  ©  ¶    À  P«  >    Ó  °       á   °  *     ñ  Ğ±  ”       p²        ¶       "  °¶  Ğ    ,  O$  Y     ;  €º       S   º       k  ¨$  —    ‚  €Ä  ü      ?&  :     £  à        ¯  0       À     0     Ô  À  ,     æ  °       ø  `  P     
  @            @     *           ñÿ:  ğÈ  4    J  0Í      _           ñÿr  y&  6    	  È       †           ñÿ”  `          `       ¬           ñÿ·           ñÿÂ  Pä  •     Õ  ¯(  9     í  ğä  —      è(  c       æ  L    1  K)  y     ¶   À       M  |  
     `         z  ¤        ! ‚  ä       š  |       ¡  ˆ       º           Ò  ´       í  `       ô  ø          ˜          Ì       ,   Ä)  Ö     C   ä  "     [   @  <     v     $        *  #     ®   0       Á   €       Ø   Œ       ï            ñÿù            ñÿ!  M  #     !  p  +     2!  ›  `     H!  û       `!    	     u!    
     ˆ!    7     Ÿ!           §!          ¯!  °ó  ?     È!             Û!  pó  ?     ğ!             ı!             "     ×     "             %"             5"             N"             b"             w"             „"             •"   =  !     «"             »"          ! Æ"             Ø"  `4  +     ê"             ı"  »  *     
#           #              #             )#             7#  @¡  `     I#             [#             h#             x#             „#             #             œ#  °d  L    ®#             Ã#             Ô#             ê#  àI  ÿ     ÿ#             $             7$             H$             V$             ^$      
     m$             }$             $             ”$             š$  À»       ¨$             ¸$  àJ      Ë$  `×  ”    ã$  °Ó  ö     ø$  `  í     	%   Ù  A    %  Àº  1     &%   »  e     <%  à  •     J%             [%             q%             {%  0d  s     ‹%             š%  Ğ  D    «%             Â%             Ñ%             è%             ü%  €c  §     &             $&             :&             H&             Y&             n&             y&           …&  PĞ  Ñ     š&   ¡  Ş    «&   $  ¶    ¾&             Ğ&             ì&  €¼  ş    ş&             '             ''  PU  6    ›  0A  ¤    :'             S'  p  Ñ    /             s  0)  s     e'             ‚'             –'             ®'             ¿'             È'             Ù'  à»  L     è'             ö'             (             #(             +(  0Ñ  |    :(  <       Q(  àğ  m    `(  à  @     …(             •(  °0  D     ¤(             ·(  °  )    Å(  P  `     Ò(  P  F     ã(  ‚  ,     ø(  p˜  ş    )             )             +)  @‚       :)  Àf  _    J)  À.  Š     ])             m)             †)             ›)             ¢)             °)             Å)             Ï)  €  {     â)  0¼  A     ò)             *             *             $*             /*             H*             ]*             o*   1       *  Ğ°  k     *  àç  õ    Ÿ*             ¹*   ó  p     Í*             à*             í*  PÏ  ó     +  ğó  G     +             !+             )+             :+             R+   f  M     \+             ”  €R  
    j+             î  S  ·     ƒ+             +             £+  ˆ       ²+             Ã+             Ğ+             ì+   ‘  ¾    ,             ,  4  Å     $,             µ
         ! <,  02  (      ?  “    O,  p  ö    g,  €  ¯     |,  a  ç    ,  W  ı	    ,  Ğ1  _     «,   h  o    º,             È,             Ş,  pŸ  Ï    ğ,             -             -             *-   ”  †     4-   1  ¢     õ"             B-             R-  €Æ  n    `-             h-  0p  q     y-  €8  w    ‹-             “-  PŒ  «    ¨-  ğÔ  n    ½-  Àò  ?     Ò-  à  {     æ-  °Ô  ;     K          ! ø-             .             .             $.  ğ-  Ã     3.             K.             Z.  P/       p.             ˆ.             š.             °.             º.  p»       Ñ.             â.  Q  ›     ù.  Pò  p     /             /  °Q  3     ,/  `5  ş    =/  À7  ´     N/  š*  £     b/  €0  )     p/  €P       Œ/             /             º/             Ë/             à/             ù/             0             '0             :0             P0             i0             v0     b     0             ¨0             Á0             Û0  À“  
     ä0             ø0  p  #     1             1             01             Q1  Ğ“  C     b1             q1             ˆ1             •1             £1   O  w    ¹1     Á    É1             Ğ1             æ1             ÷1  Pf  k     2             2  °p  3    )2  ğQ  Š     ?2  @±  †     T2             ^2             s2             Š2             ™2             ª2  `o  Ç     ·2  `7  \     Ñ2  p•  ó    ß2             ğ2             ú2             3              cx18-driver.c __kstrtab_cx18_ext_init __kstrtabns_cx18_ext_init __ksymtab_cx18_ext_init request_module_async cx18_eeprom_dump.constprop.0 cx18_remove cx18_remove.cold cx18_read_eeprom.cold cx18_probe cx18_instance __key.6 __key.7 __key.8 __key.9 __key.10 __key.11 __key.12 __key.13 cx18_probe.cold ctrl.5 cx18_init_on_first_open.cold module_start cx18_pci_driver module_cleanup __UNIQUE_ID_firmware420 cx18_pci_tbl __UNIQUE_ID_version419 __UNIQUE_ID_license418 __UNIQUE_ID_description417 __UNIQUE_ID_author416 __UNIQUE_ID_cx18_first_minor415 __UNIQUE_ID_enc_pcm_bufs414 __UNIQUE_ID_enc_pcm_bufsize413 __UNIQUE_ID_enc_pcm_buffers412 __UNIQUE_ID_enc_vbi_bufs411 __UNIQUE_ID_enc_vbi_buffers410 __UNIQUE_ID_enc_yuv_bufs409 __UNIQUE_ID_enc_yuv_bufsize408 __UNIQUE_ID_enc_yuv_buffers407 __UNIQUE_ID_enc_idx_bufs406 __UNIQUE_ID_enc_idx_bufsize405 __UNIQUE_ID_enc_idx_buffers404 __UNIQUE_ID_enc_mpg_bufs403 __UNIQUE_ID_enc_mpg_bufsize402 __UNIQUE_ID_enc_mpg_buffers401 __UNIQUE_ID_enc_ts_bufs400 __UNIQUE_ID_enc_ts_bufsize399 __UNIQUE_ID_enc_ts_buffers398 __UNIQUE_ID_mmio_ndelay397 __UNIQUE_ID_retry_mmio396 __UNIQUE_ID_cx18_pci_latency395 __UNIQUE_ID_debug394 __UNIQUE_ID_ntsc393 __UNIQUE_ID_secam392 __UNIQUE_ID_pal391 __UNIQUE_ID_cardtype390 __UNIQUE_ID_radio389 __UNIQUE_ID_tuner388 __UNIQUE_ID_enc_pcm_bufstype387 __param_enc_pcm_bufs __param_str_enc_pcm_bufs __UNIQUE_ID_enc_vbi_bufstype386 __param_enc_vbi_bufs __param_str_enc_vbi_bufs __UNIQUE_ID_enc_yuv_bufstype385 __param_enc_yuv_bufs __param_str_enc_yuv_bufs __UNIQUE_ID_enc_idx_bufstype384 __param_enc_idx_bufs __param_str_enc_idx_bufs __UNIQUE_ID_enc_mpg_bufstype383 __param_enc_mpg_bufs __param_str_enc_mpg_bufs __UNIQUE_ID_enc_ts_bufstype382 __param_enc_ts_bufs __param_str_enc_ts_bufs __UNIQUE_ID_enc_pcm_bufsizetype381 __param_enc_pcm_bufsize __param_str_enc_pcm_bufsize __UNIQUE_ID_enc_yuv_bufsizetype380 __param_enc_yuv_bufsize __param_str_enc_yuv_bufsize __UNIQUE_ID_enc_idx_bufsizetype379 __param_enc_idx_bufsize __param_str_enc_idx_bufsize __UNIQUE_ID_enc_mpg_bufsizetype378 __param_enc_mpg_bufsize __param_str_enc_mpg_bufsize __UNIQUE_ID_enc_ts_bufsizetype377 __param_enc_ts_bufsize __param_str_enc_ts_bufsize __UNIQUE_ID_enc_pcm_bufferstype376 __param_enc_pcm_buffers __param_str_enc_pcm_buffers __UNIQUE_ID_enc_vbi_bufferstype375 __param_enc_vbi_buffers __param_str_enc_vbi_buffers __UNIQUE_ID_enc_yuv_bufferstype374 __param_enc_yuv_buffers __param_str_enc_yuv_buffers __UNIQUE_ID_enc_idx_bufferstype373 __param_enc_idx_buffers __param_str_enc_idx_buffers __UNIQUE_ID_enc_mpg_bufferstype372 __param_enc_mpg_buffers __param_str_enc_mpg_buffers __UNIQUE_ID_enc_ts_bufferstype371 __param_enc_ts_buffers __param_str_enc_ts_buffers __UNIQUE_ID_cx18_first_minortype370 __param_cx18_first_minor __param_str_cx18_first_minor __UNIQUE_ID_cx18_pci_latencytype369 __param_cx18_pci_latency __param_str_cx18_pci_latency __UNIQUE_ID_retry_mmiotype368 __param_retry_mmio __param_str_retry_mmio __UNIQUE_ID_mmio_ndelaytype367 __param_mmio_ndelay __param_str_mmio_ndelay __UNIQUE_ID_debugtype366 __param_debug __param_str_debug __UNIQUE_ID_ntsctype365 __param_ntsc __param_str_ntsc __param_string_ntsc __UNIQUE_ID_secamtype364 __param_secam __param_str_secam __param_string_secam __UNIQUE_ID_paltype363 __param_pal __param_str_pal __param_string_pal __UNIQUE_ID_cardtypetype362 __param_cardtype __param_str_cardtype __param_arr_cardtype cardtype_c __UNIQUE_ID_radiotype361 __param_radio __param_str_radio __param_arr_radio radio_c __UNIQUE_ID_tunertype360 __param_tuner __param_str_tuner __param_arr_tuner tuner_c cx18-cards.c cx18_card_list input_strs.3 cx18_get_input.cold __func__.2 __func__.1 input_strs.0 cx18_get_audio_input.cold cx18_card_hvr1600_esmt cx18_card_hvr1600_samsung cx18_card_h900 cx18_card_mpc718 cx18_card_cnxt_raptor_pal cx18_card_toshiba_qosmio_dvbt cx18_card_leadtek_pvr2100 cx18_card_leadtek_dvr3100h cx18_card_gotview_dvd3 cx18_card_hvr1600_s5h1411 cx18_i2c_std cx18_pci_leadtek_dvr3100h cx18_pci_leadtek_pvr2100 cx18_pci_toshiba_qosmio_dvbt cx18_pci_cnxt_raptor_pal cx18_pci_gotview_dvd3 cx18_pci_mpc718 cx18_pci_h900 cx18_i2c_nxp cx18-i2c.c cx18_setscl cx18_setsda cx18_getscl cx18_getsda hw_devicenames hw_bus hw_addrs cx18_i2c_register.cold init_cx18_i2c.cold __func__.0 exit_cx18_i2c.cold cx18-firmware.c cx18_halt_firmware.cold cx18_firmware_init.cold __UNIQUE_ID_firmware360 __UNIQUE_ID_firmware359 cx18-gpio.c __kstrtab_cx18_reset_ir_gpio __kstrtabns_cx18_reset_ir_gpio __ksymtab_cx18_reset_ir_gpio gpiomux_log_status cx18_reset_ir_gpio.cold resetctrl_log_status gpio_write gpiomux_s_radio gpiomux_s_std gpio_reset_seq resetctrl_reset gpiomux_s_audio_routing gpiomux_ops resetctrl_ops cx18_reset_tuner_gpio.cold resetctrl_core_ops gpiomux_core_ops gpiomux_tuner_ops gpiomux_audio_ops gpiomux_video_ops cx18-queue.c cx18_queue_flush.part.0 cx18_queue_get_mdl.cold cx18_stream_free.cold __already_done.0 cx18_stream_alloc.cold cx18-streams.c __kstrtab_cx18_start_v4l2_encode_stream __kstrtabns_cx18_start_v4l2_encode_stream __ksymtab_cx18_start_v4l2_encode_stream __kstrtab_cx18_stop_v4l2_encode_stream __kstrtabns_cx18_stop_v4l2_encode_stream __ksymtab_cx18_stop_v4l2_encode_stream buffer_setup buffer_queue _cx18_stream_load_fw_queue.part.0 buffer_release buffer_prepare cx18_stop_v4l2_encode_stream.cold cx18_start_v4l2_encode_stream.cold cx18_stream_info __key.0 cx18_v4l2_enc_fops cx18_videobuf_qops cx18_streams_setup.cold cx18_streams_register.cold cx18-fileops.c __kstrtab_cx18_claim_stream __kstrtabns_cx18_claim_stream __ksymtab_cx18_claim_stream __kstrtab_cx18_release_stream __kstrtabns_cx18_release_stream __ksymtab_cx18_release_stream cx18_claim_stream.cold cx18_release_stream.cold cx18_dualwatch cx18_dualwatch.cold cx18_start_capture.cold cx18_v4l2_read.cold cx18_v4l2_enc_poll.cold cx18_v4l2_mmap.cold cx18_stop_capture.cold cx18_mute.cold cx18_unmute.cold cx18_v4l2_close.cold cx18_v4l2_open.cold cx18-ioctl.c cx18_g_fmt_vid_cap cx18_g_fmt_vbi_cap cx18_try_fmt_vid_cap cx18_s_fmt_vbi_cap cx18_g_pixelaspect cx18_g_selection cx18_enum_fmt_vid_cap formats.0 cx18_g_input cx18_g_frequency cx18_g_std cx18_s_tuner cx18_default cx18_try_encoder_cmd cx18_try_encoder_cmd.cold cx18_s_audio cx18_s_input.cold cx18_s_frequency.cold cx18_s_std.cold cx18_enum_input cx18_g_audio cx18_enumaudio cx18_log_status cx18_g_tuner cx18_encoder_cmd cx18_encoder_cmd.cold cx18_streamoff cx18_streamon cx18_dqbuf cx18_qbuf cx18_querybuf cx18_reqbufs cx18_querycap cx18_querycap.cold __func__.3 select_service_from_set cx18_g_enc_index mapping.1 cx18_try_fmt_vbi_cap cx18_g_sliced_vbi_cap cx18_g_fmt_sliced_vbi_cap cx18_try_fmt_sliced_vbi_cap cx18_s_fmt_sliced_vbi_cap cx18_s_fmt_vid_cap cx18_ioctl_ops cx18-controls.c cx18_s_video_encoding cx18_s_audio_mode cx18_s_stream_vbi_fmt cx18_s_stream_vbi_fmt.cold cx18_s_audio_sampling_freq freqs.0 cx18-mailbox.c u32arr2hex cx18_api_call api_info cx18_api_call.cold mb_ack_irq mb_ack_irq.cold rpu_str cx18_in_work_handler.cold cx18_api_epu_cmd_irq.cold cx18_vapi.cold cx18_set_filter_param cx18_api_func.cold cx18-vbi.c cx18-audio.c cx18-video.c cx18-irq.c cx18_irq_handler.cold cx18-av-core.c cx18_av_g_tuner cx18_av_s_radio cx18_av_log_status fmt_strs.0 cx18_av_log_status.cold CSWTCH.75 CSWTCH.77 CSWTCH.76 CSWTCH.78 CSWTCH.79 cx18_av_init cx18_av_s_stream cx18_av_s_stream.cold cx18_av_set_fmt cx18_av_set_fmt.cold cx18_av_s_ctrl cx18_av_initialize cx18_av_reset cx18_av_load_fw cx18_av_s_tuner input_change cx18_av_s_frequency set_input set_input.cold cx18_av_s_video_routing cx18_av_s_audio_routing cx18_av_std_setup.cold cx18_av_s_std cx18_av_s_std.cold cx18_av_ops cx18_av_ctrl_ops cx18_av_general_ops cx18_av_tuner_ops cx18_av_audio_ops cx18_av_video_ops cx18_av_vbi_ops cx18_av_pad_ops cx18-av-audio.c set_audclk_freq cx18_av_audio_s_ctrl cx18-av-firmware.c cx18_av_loadfw.cold cx18-av-vbi.c lcr2vbi.1 biphase_tbl.0 cx18-scb.c cx18-dvb.c cx18_dvb_stop_feed cx18_dvb_stop_feed.cold cx18_dvb_start_feed cx18_dvb_start_feed.cold yuan_mpc718_mt352_init yuan_mpc718_mt352_init.cold hcw_s5h1411_config hauppauge_tda18271_config __key.3 yuan_mpc718_mt352_demod ctrl.1 hauppauge_hvr1600_config hauppauge_hvr1600_tuner gotview_dvd3_zl10353_demod ctrl.0 leadtek_dvr3100h_demod ctrl.2 yuan_mpc718_zl10353_demod cx18_dvb_register.cold __UNIQUE_ID_firmware361 hauppauge_tda18271_std_map __UNIQUE_ID_adapter_nr360 __UNIQUE_ID_adapter_nrtype359 __param_adapter_nr __param_str_adapter_nr __param_arr_adapter_nr cx18-io.c cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 cx18_sw2_irq_disable_cpu videobuf_streamoff cx18_sw2_irq_disable v4l2_fh_exit dvb_dmxdev_init cx18_get_input free_irq is_vmalloc_addr cx2341x_handler_set_busy __check_object_size videobuf_poll_stream video_ioctl2 release_firmware cx18_out_work_handler alloc_workqueue cx18_debug pci_enable_device cx18_flush_queues __msecs_to_jiffies cx18_av_read __this_module memmove snprintf queue_work_on cx18_video_set_io __stack_chk_guard __symbol_put i2c_bit_add_bus v4l2_fh_del v4l2_fh_add finish_wait cx18_stop_capture dma_unmap_page_attrs request_firmware __pci_register_driver cx18_streams_cleanup v4l2_ctrl_handler_init_class dma_sync_single_for_device videobuf_reqbufs memcpy_fromio iounmap cleanup_module param_array_ops prepare_to_wait memcpy kfree cx18_av_read4 videobuf_waiton cx18_streams_setup cx18_av_decode_vbi_line cx18_av_g_sliced_fmt cx18_read_eeprom cx18_init_scb cx18_av_write4 cx18_av_write4_expect exit_cx18_i2c v4l2_subdev_init prepare_to_wait_event __wake_up cx18_vb_timeout videobuf_dqbuf cx18_init_memory _raw_spin_lock_irqsave _raw_spin_lock kmem_cache_alloc_trace videobuf_to_vmalloc cx18_v4l2_mmap __x86_indirect_thunk_edx pci_unregister_driver fortify_panic __release_region dvb_register_adapter __fentry__ init_module cx18_av_s_clock_freq cx18_irq_handler cx18_firmware_init dev_driver_string v4l2_device_register_subdev cx18_av_std_setup videobuf_vmalloc_free dma_map_page_attrs cx18_start_capture __x86_indirect_thunk_ecx cx18_i2c_register v4l2_ctrl_handler_log_status kmalloc_order_trace v4l2_ctrl_handler_setup schedule_timeout schedule __stack_chk_fail cx18_av_and_or video_devdata i2c_new_scanned_device cx2341x_handler_setup strnlen cx18_av_loadfw cx18_av_audio_ctrl_ops cx18_memset_io __mod_pci__cx18_pci_tbl_device_table i2c_del_adapter _cx18_mdl_swap __v4l2_ctrl_s_ctrl init_cx18_i2c cx18_find_hw cx18_service2vbi cx18_get_service_set cx18_process_vbi_data __symbol_get init_wait_entry cx18_set_funcs cx18_v4l2_close cx18_gpio_register dvb_dmx_release __x86_indirect_thunk_edi request_threaded_irq memchr tveeprom_read videobuf_read_stream mod_timer cx18_halt_firmware cx18_av_and_or4 v4l2_fh_is_singular destroy_workqueue mem_section mutex_lock __x86_indirect_thunk_ebp pci_read_config_word videobuf_querybuf cx18_queue_init cx18_av_write cx18_dvb_register v4l2_ctrl_subscribe_event cx18_sw2_irq_enable v4l2_event_pending v4l2_fh_init cx18_av_audio_set_path cx18_setup_page dvb_net_init ioremap dvb_dmx_swfilter video_unregister_device cx18_mute videobuf_qbuf __x86_indirect_thunk_esi __mutex_init dvb_dmxdev_release cx18_cxhdl_ops jiffies_to_msecs current_task _raw_spin_unlock_irqrestore cx18_api_epu_cmd_irq param_ops_short cx18_unload_queues dma_sync_single_for_cpu cx18_queue_get_mdl cx18_init_on_first_open cx18_get_audio_input cx18_v4l2_enc_poll cx18_v4l2_read cx18_dequeue cx18_v4l2_open _copy_to_user __init_waitqueue_head cx18_audio_set_io v4l2_device_register dvb_frontend_detach param_ops_string cx18_vapi _cx18_enqueue dvb_net_release cx18_av_probe sprintf cx18_s_frequency cx18_stream_alloc strscpy cx18_in_work_handler cx18_av_s_sliced_fmt cx18_sw1_irq_disable cx18_msleep_timeout cx18_av_s_raw_fmt mutex_unlock videobuf_iolock init_timer_key cx18_gpio_init __video_register_device __const_udelay cx18_reset_tuner_gpio dvb_unregister_frontend videobuf_streamon pci_write_config_byte ktime_get cx18_av_write4_noretry cancel_work_sync cx18_stop_all_captures cx18_sw1_irq_enable __warn_printk cx18_find_handle cx18_load_queues cx18_stream_free cx18_dvb_unregister cx18_buf_swap cx18_stream_rotate_idx_mdls v4l2_ctrl_new_std videobuf_queue_vmalloc_init v4l2_ctrl_g_ctrl cx2341x_handler_init cx2341x_handler_set_50hz v4l2_ctrl_handler_free v4l2_device_unregister pci_disable_device dvb_register_frontend __x86_indirect_thunk_ebx dma_set_mask cx18_expand_service_set tveeprom_hauppauge_analog autoremove_wake_function v4l2_subdev_call_wrappers cx18_api v4l2_i2c_new_subdev cx18_get_card pci_read_config_byte videobuf_mmap_mapper schedule_timeout_uninterruptible cx18_vapi_result iomem_resource dvb_unregister_adapter dvb_dmx_init param_ops_int cx18_streams_register cx18_init_power strchr pci_write_config_word _raw_spin_unlock cx18_unmute video_device_release_empty cx18_s_std cx18_handle_to_stream cx18_av_write_expect __kmalloc __SCT__might_resched v4l2_event_unsubscribe kmalloc_caches __request_region cx18_s_input _cx18_mdl_sync_for_device cx18_api_func __request_module system_wq videobuf_mmap_free flush_work    # 	        Û    ˜ (    A   # ]   ê d     o   Ş u   ê |        ¦ Ÿ   • ¶   ¥ İ   ¥ ó   İ ú   V   Y   ş    (  á <  ê C    H   n  Å s  ! {  ¶ §   ¶  ´ Â    Ç  1 Î  µ Ü   á  # í  í û  u   6 a  # …  ô ¬  Ö ±   Ä    É  ’ é  S    »      ô 2    ;    D    I  8 Q  # f  ô t    }    ‘  4 ¸  ‡ Ä    Ú    â  ñ ì    ñ  1 ö    ü        /    :    E    P    [    f    q    |    ‡    ’        ¨    ³    È    8  ê ?    {    „    ‹    ’          À     %    ?  À ”        >    â        ~    ß  À ù  
  ş  1 	  
  	  1 %	    *	  1 0	  
  5	  1 ;	  
  @	  1 F	  
  K	  1 s	    {	    ˜	    ©	  q ´	    ¹	    ¾	  q É	    Î	    Ó	  q Ø	    İ	    è	  q ñ	    ù	  ñ 
    
  é 
    P
  “ •
  s Ÿ
  Ú ¤
  ² °
    Á
        #  … .    3    8  … C    H    M  … X    ]    b  … À  ê Ç    Ï  ë ×    ç  ¹ ï    ö  Å û      ×     1  ] M  Ì g  Á v    ~      ê —    ¡  i ¯    Ó    Ş    è  
  í  1 õ        F    M    œ    ¦    â    ó    ø    ı        	  À   
    1 1    D  ô W    \    a  8 q  # €  ô ¹  ' Á    İ  Š ê  Š ü  Š   '     "  Š /  Š A  Š a  ½ y  / Ğ  Ø 
  Ğ    +  ô Q   b  8 q  #     ¡  # Ö    ã  = î    ÿ    	  ’   # ß    æ  = ñ          ’ 1  # ¡  #   # A  # q  # Š      ô     ¶    ×  ¿ ğ  ô   ¿ C  ¿ g  ¿ µ  = À    Ñ    Ü  ’ é    %  ; =  8 Q  # c   •  Í ¥  Í ±  # À  ê Ç            %    ,    d  ï ¾  = Ç    Í    á    ç   n   ¶   ş   ş  /   ö ¿  ö Ğ  B á  # ë  ê ò    d  B q  B   # ‹  ê ’      # Ñ  #