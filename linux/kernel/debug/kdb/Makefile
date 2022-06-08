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
