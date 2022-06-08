ION		0x000004B4
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
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /* SPDX-License-Identifier: GPL-2.0-or-later */
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
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;
	*num_planes = 1;
	sizes[0] = lines * VBI_LINE_LENGTH * 2;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;

	if (vb2_plane_size(vb, 0) < lines * VBI_LINE_LENGTH * 2)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, lines * VBI_LINE_LENGTH * 2);

	cx23885_risc_vbibuffer(dev->pci, &buf->risc,
			 sgt->sgl,
			 0, VBI_LINE_LENGTH * lines,
			 VBI_LINE_LENGTH, 0,
			 lines);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;
	*num_planes = 1;
	sizes[0] = lines * VBI_LINE_LENGTH * 2;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	unsigned lines = VBI_PAL_LINE_COUNT;

	if (dev->tvnorm & V4L2_STD_525_60)
		lines = VBI_NTSC_LINE_COUNT;

	if (vb2_plane_size(vb, 0) < lines * VBI_LINE_LENGTH * 2)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, lines * VBI_LINE_LENGTH * 2);

	cx23885_risc_vbibuffer(dev->pci, &buf->risc,
			 sgt->sgl,
			 0, VBI_LINE_LENGTH * lines,
			 VBI_LINE_LENGTH, 0,
			 lines);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
			struct cx23885_buffer, vb);
	struct cx23885_buffer *prev;
	struct cx23885_dmaqueue *q = &dev->vbiq;
	unsigned long flags;

	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	if (list_empty(&q->active)) {
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		dprintk(2, "[%p/%d] vbi_queue - first active\n",
			buf, buf->vb.vb2_buf.index);

	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		spin_lock_irqsave(&dev->slock, flags);
		list_add_tail(&buf->queue, &q->active);
		spin_unlock_irqrestore(&dev->slock, flags);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
			buf, buf->vb.vb2_buf.index);
	}
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_vbi_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vbiq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}


const struct vb2_ops cx23885_vbi_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"
#include "cx23885-video.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kmod.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/div64.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include "cx23885-ioctl.h"
#include "xc2028.h"

#include <media/drv-intf/cx25840.h>

MODULE_DESCRIPTION("v4l2 driver module for cx23885 based TV cards");
MODULE_AUTHOR("Steven Toth <stoth@linuxtv.org>");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------ */

static unsigned int video_nr[] = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };
static unsigned int vbi_nr[]   = {[0 ... (CX23885_MAXBOARDS - 1)] = UNSET };

module_param_array(video_nr, int, NULL, 0444);
module_param_array(vbi_nr,   int, NULL, 0444);

MODULE_PARM_DESC(video_nr, "video device numbers");
MODULE_PARM_DESC(vbi_nr, "vbi device numbers");

static unsigned int video_debug;
module_param(video_debug, int, 0644);
MODULE_PARM_DESC(video_debug, "enable debug messages [video]");

static unsigned int irq_debug;
module_param(irq_debug, int, 0644);
MODULE_PARM_DESC(irq_debug, "enable debug messages [IRQ handler]");

static unsigned int vid_limit = 16;
module_param(vid_limit, int, 0644);
MODULE_PARM_DESC(vid_limit, "capture memory limit in megabytes");

#define dprintk(level, fmt, arg...)\
	do { if (video_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: video:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------- */
/* static data                                                         */

#define FORMAT_FLAGS_PACKED       0x01
static struct cx23885_fmt formats[] = {
	{
		.fourcc   = V4L2_PIX_FMT_YUYV,
		.depth    = 16,
		.flags    = FORMAT_FLAGS_PACKED,
	}
};

static struct cx23885_fmt *format_by_fourcc(unsigned int fourcc)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(formats); i++)
		if (formats[i].fourcc == fourcc)
			return formats+i;
	return NULL;
}

/* ------------------------------------------------------------------- */

void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count)
{
	struct cx23885_buffer *buf;

	if (list_empty(&q->active))
		return;
	buf = list_entry(q->active.next,
			struct cx23885_buffer, queue);

	buf->vb.sequence = q->count++;
	buf->vb.vb2_buf.timestamp = ktime_get_ns();
	dprintk(2, "[%p/%d] wakeup reg=%d buf=%d\n", buf,
			buf->vb.vb2_buf.index, count, q->count);
	list_del(&buf->queue);
	vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
}

int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm)
{
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		.format.code = MEDIA_BUS_FMT_FIXED,
	};

	dprintk(1, "%s(norm = 0x%08x) name: [%s]\n",
		__func__,
		(unsigned int)norm,
		v4l2_norm_to_name(norm));

	if (dev->tvnorm == norm)
		return 0;

	if (dev->tvnorm != norm) {
		if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
		    vb2_is_busy(&dev->vb2_mpegq))
			return -EBUSY;
	}

	dev->tvnorm = norm;
	dev->width = 720;
	dev->height = norm_maxh(norm);
	dev->field = V4L2_FIELD_INTERLACED;

	call_all(dev, video, s_std, norm);

	format.format.width = dev->width;
	format.format.height = dev->height;
	format.format.field = dev->field;
	call_all(dev, pad, set_fmt, NULL, &format);

	return 0;
}

static struct video_device *cx23885_vdev_init(struct cx23885_dev *dev,
				    struct pci_dev *pci,
				    struct video_device *template,
				    char *type)
{
	struct video_device *vfd;
	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	vfd->lock = &dev->lock;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		 cx23885_boards[dev->board].name, type);
	video_set_drvdata(vfd, dev);
	return vfd;
}

int cx23885_flatiron_write(struct cx23885_dev *dev, u8 reg, u8 data)
{
	/* 8 bit registers, 8 bit values */
	u8 buf[] = { reg, data };

	struct i2c_msg msg = { .addr = 0x98 >> 1,
		.flags = 0, .buf = buf, .len = 2 };

	return i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg, 1);
}

u8 cx23885_flatiron_read(struct cx23885_dev *dev, u8 reg)
{
	/* 8 bit registers, 8 bit values */
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };

	struct i2c_msg msg[] = {
		{ .addr = 0x98 >> 1, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = 0x98 >> 1, .flags = I2C_M_RD, .buf = b1, .len = 1 }
	};

	ret = i2c_transfer(&dev->i2c_bus[2].i2c_adap, &msg[0], 2);
	if (ret != 2)
		pr_err("%s() error\n", __func__);

	return b1[0];
}

static void cx23885_flatiron_dump(struct cx23885_dev *dev)
{
	int i;
	dprintk(1, "Flatiron dump\n");
	for (i = 0; i < 0x24; i++) {
		dprintk(1, "FI[%02x] = %02x\n", i,
			cx23885_flatiron_read(dev, i));
	}
}

static int cx23885_flatiron_mux(struct cx23885_dev *dev, int input)
{
	u8 val;
	dprintk(1, "%s(input = %d)\n", __func__, input);

	if (input == 1)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) & ~FLD_CH_SEL;
	else if (input == 2)
		val = cx23885_flatiron_read(dev, CH_PWR_CTRL1) | FLD_CH_SEL;
	else
		return -EINVAL;

	val |= 0x20; /* Enable clock to delta-sigma and dec filter */

	cx23885_flatiron_write(dev, CH_PWR_CTRL1, val);

	/* Wake up */
	cx23885_flatiron_write(dev, CH_PWR_CTRL2, 0);

	if (video_debug)
		cx23885_flatiron_dump(dev);

	return 0;
}

static int cx23885_video_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s() video_mux: %d [vmux=%d, gpio=0x%x,0x%x,0x%x,0x%x]\n",
		__func__,
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0, INPUT(input)->gpio1,
		INPUT(input)->gpio2, INPUT(input)->gpio3);
	dev->input = input;

	if (dev->board == CX23885_BOARD_MYGICA_X8506 ||
		dev->board == CX23885_BOARD_MAGICPRO_PROHDTVE2 ||
		dev->board == CX23885_BOARD_MYGICA_X8507) {
		/* Select Analog TV */
		if (INPUT(input)->type == CX23885_VMUX_TELEVISION)
			cx23885_gpio_clear(dev, GPIO_0);
	}

	/* Tell the internal A/V decoder */
	v4l2_subdev_call(dev->sd_cx25840, video, s_routing,
			INPUT(input)->vmux, 0, 0);

	if ((dev->board == CX23885_BOARD_HAUPPAUGE_HVR1800) ||
		(dev->board == CX23885_BOARD_MPX885) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1250) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_IMPACTVCBE) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1255_22111) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1265_K4) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_QUADHD_DVB) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR1850) ||
		(dev->board == CX23885_BOARD_HAUPPAUGE_HVR5525) ||
		(dev->board == CX23885_BOARD_MYGICA_X8507) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_HC81R) ||
		(dev->board == CX23885_BOARD_VIEWCAST_260E) ||
		(dev->board == CX23885_BOARD_VIEWCAST_460E) ||
		(dev->board == CX23885_BOARD_AVERMEDIA_CE310B)) {
		/* Configure audio routing */
		v4l2_subdev_call(dev->sd_cx25840, audio, s_routing,
			INPUT(input)->amux, 0, 0);

		if (INPUT(input)->amux == CX25840_AUDIO7)
			cx23885_flatiron_mux(dev, 1);
		else if (INPUT(input)->amux == CX25840_AUDIO6)
			cx23885_flatiron_mux(dev, 2);
	}

	return 0;
}

static int cx23885_audio_mux(struct cx23885_dev *dev, unsigned int input)
{
	dprintk(1, "%s(input=%d)\n", __func__, input);

	/* The baseband video core of the cx23885 has two audio inputs.
	 * LR1 and LR2. In almost every single case so far only HVR1xxx
	 * cards we've only ever supported LR1. Time to support LR2,
	 * which is available via the optional white breakout header on
	 * the board.
	 * We'll use a could of existing enums in the card struct to allow
	 * devs to specify which baseband input they need, or just default
	 * to what we've always used.
	 */
	if (INPUT(input)->amux == CX25840_AUDIO7)
		cx23885_flatiron_mux(dev, 1);
	else if (INPUT(input)->amux == CX25840_AUDIO6)
		cx23885_flatiron_mux(dev, 2);
	else {
		/* Not specifically defined, assume the default. */
		cx23885_flatiron_mux(dev, 1);
	}

	return 0;
}

/* ------------------------------------------------------------------ */
static int cx23885_start_video_dma(struct cx23885_dev *dev,
			   struct cx23885_dmaqueue *q,
			   struct cx23885_buffer *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* Stop the dma/fifo before we tamper with it's risc programs */
	cx_clear(VID_A_DMA_CTL, 0x11);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH01],
				buf->bpl, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000011);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x11); /* FIFO and RISC enable */

	return 0;
}

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	*num_planes = 1;
	sizes[0] = (dev->fmt->depth * dev->width * dev->height) >> 3;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);
	u32 line0_offset, line1_offset;
	struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, 0);
	int field_tff;

	buf->bpl = (dev->width * dev->fmt->depth) >> 3;

	if (vb2_plane_size(vb, 0) < dev->height * buf->bpl)
		return -EINVAL;
	vb2_set_plane_payload(vb, 0, dev->height * buf->bpl);

	switch (dev->field) {
	case V4L2_FIELD_TOP:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, 0, UNSET,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_BOTTOM:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, UNSET, 0,
				buf->bpl, 0, dev->height);
		break;
	case V4L2_FIELD_INTERLACED:
		if (dev->tvnorm & V4L2_STD_525_60)
			/* NTSC or  */
			field_tff = 1;
		else
			field_tff = 0;

		if (cx23885_boards[dev->board].force_bff)
			/* PAL / SECAM OR 888 in NTSC MODE */
			field_tff = 0;

		if (field_tff) {
			/* cx25840 transmits NTSC bottom field first */
			dprintk(1, "%s() Creating TFF/NTSC risc\n",
					__func__);
			line0_offset = buf->bpl;
			line1_offset = 0;
		} else {
			/* All other formats are top field first */
			dprintk(1, "%s() Creating BFF/PAL/SECAM risc\n",
					__func__);
			line0_offset = 0;
			line1_offset = buf->bpl;
		}
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl, line0_offset,
				line1_offset,
				buf->bpl, buf->bpl,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_TB:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				0, buf->bpl * (dev->height >> 1),
				buf->bpl, 0,
				dev->height >> 1);
		break;
	case V4L2_FIELD_SEQ_BT:
		cx23885_risc_buffer(dev->pci, &buf->risc,
				sgt->sgl,
				buf->bpl * (dev->height >> 1), 0,
				buf->bpl, 0,
				dev->height >> 1);
		break;
	default:
		BUG();
	}
	dprintk(2, "[%p/%d] buffer_init - %dx%d %dbpp 0x%08x - dma=0x%08lx\n",
		buf, buf->vb.vb2_buf.index,
		dev->width, dev->height, dev->fmt->depth, dev->fmt->fourcc,
		(unsigned long)buf->risc.dma);
	return 0;
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(vb->vb2_queue->drv_priv, buf);
}

/*
 * The risc program for each buffer works as follows: it starts with a simple
 * 'JUMP to addr + 12', which is effectively a NOP. Then the code to DMA the
 * buffer follows and at the end we have a JUMP back to the start + 12 (skipping
 * the initial JUMP).
 *
 * This is the risc program of the first buffer to be queued if the active list
 * is empty and it just keeps DMAing this buffer without generating any
 * interrupts.
 *
 * If a new buffer is added then the initial JUMP in the code for that buffer
 * will generate an interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct cx23885_buffer   *prev;
	struct cx23885_dmaqueue *q    = &dev->vidq;
	unsigned long flags;

	/* add jump to start */
	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&q->active)) {
		list_add_tail(&buf->queue, &q->active);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.vb2_buf.index);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		list_add_tail(&buf->queue, &q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.vb2_buf.index);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_video_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x11);
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static const struct vb2_ops cx23885_video_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */
/* VIDEO IOCTLS                                                       */

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = dev->fmt->fourcc;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * dev->fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_fmt *fmt;
	enum v4l2_field   field;
	unsigned int      maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -EINVAL;

	field = f->fmt.pix.field;
	maxw  = 720;
	maxh  = norm_maxh(dev->tvnorm);

	if (V4L2_FIELD_ANY == field) {
		field = (f->fmt.pix.height > maxh/2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_BOTTOM;
	}

	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		maxh = maxh / 2;
		break;
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_SEQ_TB:
	case V4L2_FIELD_SEQ_BT:
		break;
	default:
		field = V4L2_FIELD_INTERLACED;
		break;
	}

	f->fmt.pix.field = field;
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct v4l2_subdev_format format = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
	};
	int err;

	dprintk(2, "%s()\n", __func__);
	err = vidioc_try_fmt_vid_cap(file, priv, f);

	if (0 != err)
		return err;

	if (vb2_is_busy(&dev->vb2_vidq) || vb2_is_busy(&dev->vb2_vbiq) ||
	    vb2_is_busy(&dev->vb2_mpegq))
		return -EBUSY;

	dev->fmt        = format_by_fourcc(f->fmt.pix.pixelformat);
	dev->width      = f->fmt.pix.width;
	dev->height     = f->fmt.pix.height;
	dev->field	= f->fmt.pix.field;
	dprintk(2, "%s() width=%d height=%d field=%d\n", __func__,
		dev->width, dev->height, dev->field);
	v4l2_fill_mbus_format(&format.format, &f->fmt.pix, MEDIA_BUS_FMT_FIXED);
	call_all(dev, pad, set_fmt, NULL, &format);
	v4l2_fill_pix_format(&f->fmt.pix, &format.format);
	/* set_fmt overwrites f->fmt.pix.field, restore it */
	f->fmt.pix.field = dev->field;
	return 0;
}

static int vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "cx23885", sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
			    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_DEVICE_CAPS;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		cap->capabilities |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			cap->capabilities |= V4L2_CAP_TUNER;
		break;
	}
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
	struct v4l2_fmtdesc *f)
{
	if (unlikely(f->index >= ARRAY_SIZE(formats)))
		return -EINVAL;

	f->pixelformat = formats[f->index].fourcc;

	return 0;
}

static int vidioc_g_pixelaspect(struct file *file, void *priv,
				int type, struct v4l2_fract *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	bool is_50hz = dev->tvnorm & V4L2_STD_625_50;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	f->numerator = is_50hz ? 54 : 11;
	f->denominator = is_50hz ? 59 : 10;

	return 0;
}

stat