// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for a cx23417 mpeg encoder via cx23885 host port.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *    (c) 2008 Steven Toth <stoth@linuxtv.org>
 *      - CX23885/7/8 support
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/drv-intf/cx2341x.h>

#define CX23885_FIRM_IMAGE_SIZE 376836
#define CX23885_FIRM_IMAGE_NAME "v4l-cx23885-enc.fw"

static unsigned int mpegbufs = 32;
module_param(mpegbufs, int, 0644);
MODULE_PARM_DESC(mpegbufs, "number of mpeg buffers, range 2-32");
static unsigned int mpeglines = 32;
module_param(mpeglines, int, 0644);
MODULE_PARM_DESC(mpeglines, "number of lines in an MPEG buffer, range 2-32");
static unsigned int mpeglinesize = 512;
module_param(mpeglinesize, int, 0644);
MODULE_PARM_DESC(mpeglinesize,
	"number of bytes in each line of an MPEG buffer, range 512-1024");

static unsigned int v4l_debug;
module_param(v4l_debug, int, 0644);
MODULE_PARM_DESC(v4l_debug, "enable V4L debug messages");

#define dprintk(level, fmt, arg...)\
	do { if (v4l_debug >= level) \
		printk(KERN_DEBUG pr_fmt("%s: 417:" fmt), \
			__func__, ##arg); \
	} while (0)

static struct cx23885_tvnorm cx23885_tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
	}, {
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
	}, {
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
	}, {
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
	}, {
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
	}, {
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
	}, {
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
	}, {
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
	}, {
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
	}, {
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
	}, {
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
	}
};

/* ------------------------------------------------------------------ */
enum cx23885_capture_type {
	CX23885_MPEG_CAPTURE,
	CX23885_RAW_CAPTURE,
	CX23885_RAW_PASSTHRU_CAPTURE
};
enum cx23885_capture_bits {
	CX23885_RAW_BITS_NONE             = 0x00,
	CX23885_RAW_BITS_YUV_CAPTURE      = 0x01,
	CX23885_RAW_BITS_PCM_CAPTURE      = 0x02,
	CX23885_RAW_BITS_VBI_CAPTURE      = 0x04,
	CX23885_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	CX23885_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum cx23885_capture_end {
	CX23885_END_AT_GOP, /* stop at the end of gop, generate irq */
	CX23885_END_NOW, /* stop immediately, no irq */
};
enum cx23885_framerate {
	CX23885_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	CX23885_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum cx23885_stream_port {
	CX23885_OUTPUT_PORT_MEMORY,
	CX23885_OUTPUT_PORT_STREAMING,
	CX23885_OUTPUT_PORT_SERIAL
};
enum cx23885_data_xfer_status {
	CX23885_MORE_BUFFERS_FOLLOW,
	CX23885_LAST_BUFFER,
};
enum cx23885_picture_mask {
	CX23885_PICTURE_MASK_NONE,
	CX23885_PICTURE_MASK_I_FRAMES,
	CX23885_PICTURE_MASK_I_P_FRAMES = 0x3,
	CX23885_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum cx23885_vbi_mode_bits {
	CX23885_VBI_BITS_SLICED,
	CX23885_VBI_BITS_RAW,
};
enum cx23885_vbi_insertion_bits {
	CX23885_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	CX23885_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum cx23885_dma_unit {
	CX23885_DMA_BYTES,
	CX23885_DMA_FRAMES,
};
enum cx23885_dma_transfer_status_bits {
	CX23885_DMA_TRANSFER_BITS_DONE = 0x01,
	CX23885_DMA_TRANSFER_BITS_ERROR = 0x04,
	CX23885_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum cx23885_pause {
	CX23885_PAUSE_ENCODING,
	CX23885_RESUME_ENCODING,
};
enum cx23885_copyright {
	CX23885_COPYRIGHT_OFF,
	CX23885_COPYRIGHT_ON,
};
enum cx23885_notification_type {
	CX23885_NOTIFICATION_REFRESH,
};
enum cx23885_notification_status {
	CX23885_NOTIFICATION_OFF,
	CX23885_NOTIFICATION_ON,
};
enum cx23885_notification_mailbox {
	CX23885_NOTIFICATION_NO_MAILBOX = -1,
};
enum cx23885_field1_lines {
	CX23885_FIELD1_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD1_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD1_MICRONAS = 0x0105, /* 261 */
};
enum cx23885_field2_lines {
	CX23885_FIELD2_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD2_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD2_MICRONAS = 0x0106, /* 262 */
};
enum cx23885_custom_data_type {
	CX23885_CUSTOM_EXTENSION_USR_DATA,
	CX23885_CUSTOM_PRIVATE_PACKET,
};
enum cx23885_mute {
	CX23885_UNMUTE,
	CX23885_MUTE,
};
enum cx23885_mute_video_mask {
	CX23885_MUTE_VIDEO_V_MASK = 0x0000FF00,
	CX23885_MUTE_VIDEO_U_MASK = 0x00FF0000,
	CX23885_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum cx23885_mute_video_shift {
	CX23885_MUTE_VIDEO_V_SHIFT = 8,
	CX23885_MUTE_VIDEO_U_SHIFT = 16,
	CX23885_MUTE_VIDEO_Y_SHIFT = 24,
};

/* defines below are from ivtv-driver.h */
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/* Firmware API commands */
#define IVTV_API_STD_TIMEOUT 500

/* Registers */
/* IVTV_REG_OFFSET */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC)
#define IVTV_REG_SPU (0x9050)
#define IVTV_REG_HW_BLOCKS (0x9054)
#define IVTV_REG_VPU (0x9058)
#define IVTV_REG_APU (0xA064)

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
| MIWR# | MIRD# | MICS# |MIRDY# |MIADDR3|MIADDR2|MIADDR1|MIADDR0|
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|MIDATA7|MIDATA6|MIDATA5|MIDATA4|MIDATA3|MIDATA2|MIDATA1|MIDATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
#define MC417_MIWR	0x8000
#define MC417_MIRD	0x4000
#define MC417_MICS	0x2000
#define MC417_MIRDY	0x1000
#define MC417_MIADDR	0x0F00
#define MC417_MIDATA	0x00FF

/* MIADDR* nibble definitions */
#define  MCI_MEMORY_DATA_BYTE0          0x000
#define  MCI_MEMORY_DATA_BYTE1          0x100
#define  MCI_MEMORY_DATA_BYTE2          0x200
#define  MCI_MEMORY_DATA_BYTE3          0x300
#define  MCI_MEMORY_ADDRESS_BYTE2       0x400
#define  MCI_MEMORY_ADDRESS_BYTE1       0x500
#define  MCI_MEMORY_ADDRESS_BYTE0       0x600
#define  MCI_REGISTER_DATA_BYTE0        0x800
#define  MCI_REGISTER_DATA_BYTE1        0x900
#define  MCI_REGISTER_DATA_BYTE2        0xA00
#define  MCI_REGISTER_DATA_BYTE3        0xB00
#define  MCI_REGISTER_ADDRESS_BYTE0     0xC00
#define  MCI_REGISTER_ADDRESS_BYTE1     0xD00
#define  MCI_REGISTER_MODE              0xE00

/* Read and write modes */
#define  MCI_MODE_REGISTER_READ         0
#define  MCI_MODE_REGISTER_WRITE        1
#define  MCI_MODE_MEMORY_READ           0
#define  MCI_MODE_MEMORY_WRITE          0x40

/*** Bit definitions for MC417_CTL register