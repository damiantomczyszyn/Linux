/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#ifndef _CX23885_REG_H_
#define _CX23885_REG_H_

/*
Address Map
0x00000000 -> 0x00009000   TX SRAM  (Fifos)
0x00010000 -> 0x00013c00   RX SRAM  CMDS + CDT

EACH CMDS struct is 0x80 bytes long

DMAx_PTR1 = 0x03040 address of first cluster
DMAx_PTR2 = 0x10600 address of the CDT
DMAx_CNT1 = cluster size in (bytes >> 4) -1
DMAx_CNT2 = total cdt size for all entries >> 3

Cluster Descriptor entry = 4 DWORDS
 DWORD 0 -> ptr to cluster
 DWORD 1 Reserved
 DWORD 2 Reserved
 DWORD 3 Reserved

Channel manager Data Structure entry = 20 DWORD
  0  IntialProgramCounterLow
  1  IntialProgramCounterHigh
  2  ClusterDescriptorTableBase
  3  ClusterDescriptorTableSize
  4  InstructionQueueBase
  5  InstructionQueueSize
...  Reserved
 19  Reserved
*/

/* Risc Instructions */
#define RISC_CNT_INC		 0x00010000
#define RISC_CNT_RESET		 0x00030000
#define RISC_IRQ1		 0x01000000
#define RISC_IRQ2		 0x02000000
#define RISC_EOL		 0x04000000
#define RISC_SOL		 0x08000000
#define RISC_WRITE		 0x10000000
#define RISC_SKIP		 0x20000000
#define RISC_JUMP		 0x70000000
#define RISC_SYNC		 0x80000000
#define RISC_RESYNC		 0x80008000
#define RISC_READ		 0x90000000
#define RISC_WRITERM		 0xB0000000
#define RISC_WRITECM		 0xC0000000
#define RISC_WRITECR		 0xD0000000
#define RISC_WRITEC		 0x50000000
#define RISC_READC		 0xA0000000


/* Audio and Video Core */
#define HOST_REG1		0x00000000
#define HOST_REG2		0x00000001
#define HOST_REG3		0x00000002

/* Chip Configuration Registers */
#define CHIP_CTRL		0x00000100
#define AFE_CTRL		0x00000104
#define VID_PLL_INT_POST	0x00000108
#define VID_PLL_FRAC		0x0000010C
#define AUX_PLL_INT_POST	0x00000110
#define AUX_PLL_FRAC		0x00000114
#define SYS_PLL_INT_POST	0x00000118
#define SYS_PLL_FRAC		0x0000011C
#define PIN_CTRL		0x00000120
#define AUD_IO_CTRL		0x00000124
#define AUD_LOCK1		0x00000128
#define AUD_LOCK2		0x0000012C
#define POWER_CTRL		0x00000130
#define AFE_DIAG_CTRL1		0x00000134
#define AFE_DIAG_CTRL3		0x0000013C
#define PLL_DIAG_CTRL		0x00000140
#define AFE_CLK_OUT_CTRL	0x00000144
#define DLL1_DIAG_CTRL		0x0000015C

/* GPIO[23:19] Output Enable */
#define GPIO2_OUT_EN_REG	0x00000160
/* GPIO[23:19] Data Registers */
#define GPIO2			0x00000164

#define IFADC_CTRL		0x00000180

/* Infrared Remote Registers */
#define IR_CNTRL_REG	0x00000200
#define IR_TXCLK_REG	0x00000204
#define IR_RXCLK_REG	0x00000208
#define IR_CDUTY_REG	0x0000020C
#define IR_STAT_REG	0x00000210
#define IR_IRQEN_REG	0x00000214
#define IR_FILTR_REG	0x00000218
#define IR_FIFO_REG	0x0000023C

/* Video Decoder Registers */
#define MODE_CTRL		0x00000400
#define OUT_CTRL1		0x00000404
#define OUT_CTRL2		0x00000408
#define GEN_STAT		0x0000040C
#define INT_STAT_MASK		0x00000410
#define LUMA_CTRL		0x00000414
#define HSCALE_CTRL		0x00000418
#define VSCALE_CTRL		0x0000041C
#define CHROMA_CTRL		0x00000420
#define VBI_LINE_CTRL1		0x00000424
#define VBI_LINE_CTRL2		0x00000428
#define VBI_LINE_CTRL3		0x0000042C
#define VBI_LINE_CTRL4		0x00000430
#define VBI_LINE_CTRL5		0x00000434
#define VBI_FC_CFG		0x00000438
#define VBI_MISC_CFG1		0x0000043C
#define VBI_MISC_CFG2		0x00000440
#define VBI_PAY1		0x00000444
#define VBI_PAY2		0x00000448
#define VBI_CUST1_CFG1		0x0000044C
#define VBI_CUST1_CFG2		0x00000450
#define VBI_CUST1_CFG3		0x00000454
#define VBI_CUST2_CFG1		0x00000458
#define VBI_CUST2_CFG2		0x0000045C
#define VBI_CUST2_CFG3		0x00000460
#define VBI_CUST3_CFG1		0x00000464
#define VBI_CUST3_CFG2		0x00000468
#define VBI_CUST3_CFG3		0x0000046C
#define HORIZ_TIM_CTRL		0x00000470
#define VERT_TIM_CTRL		0x00000474
#define SRC_COMB_CFG		0x00000478
#define CHROMA_VBIOFF_CFG	0x0000047C
#define FIELD_COUNT		0x00000480
#define MISC_TIM_CTRL		0x00000484
#define DFE_CTRL1		0x00000488
#define DFE_CTRL2		0x0000048C
#define DFE_CTRL3		0x00000490
#define PLL_CTRL		0x00000494
#define HTL_CTRL		0x00000498
#define COMB_CTRL		0x0000049C
#define CRUSH_CTRL		0x000004A0
#define SOFT_RST_CTRL		0x000004A4
#define CX885_VERSION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_