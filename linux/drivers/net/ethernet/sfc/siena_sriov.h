DING,
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

/* Read and write modes 