// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for CX23885 analog audio capture
 *
 *    (c) 2008 Mijhail Moreyra <mijhail.moreyra@gmail.com>
 *    Adapted from cx88-alsa.c
 *    (c) 2009 Steven Toth <stoth@kernellabs.com>
 */

#include "cx23885.h"
#include "cx23885-reg.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>

#include <asm/delay.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>

#include <sound/tlv.h>

#define AUDIO_SRAM_CHANNEL	SRAM_CH07

#define dprintk(level, fmt, arg...) do {				\
	if (audio_debug + 1 > level)					\
		printk(KERN_DEBUG pr_fmt("%s: alsa: " fmt), \
			chip->dev->name, ##arg); \
} while(0)

/****************************************************************************
			Module global static vars
 ****************************************************************************/

static unsigned int disable_analog_audio;
module_param(disable_analog_audio, int, 0644);
MODULE_PARM_DESC(disable_analog_audio, "disable analog audio ALSA driver");

static unsigned int audio_debug;
module_param(audio_debug, int, 0644);
MODULE_PARM_DESC(audio_debug, "enable debug messages [analog audio]");

/****************************************************************************
			Board specific functions
 ****************************************************************************/

/* Constants taken from cx88-reg.h */
#define AUD_INT_DN_RISCI1       (1 <<  0)
#define AUD_INT_UP_RISCI1       (1 <<  1)
#define AUD_INT_RDS_DN_RISCI1   (1 <<  2)
#define AUD_INT_DN_RISCI2       (1 <<  4) /* yes, 3 is skipped */
#define AUD_INT_UP_RISCI2       (1 <<  5)
#define AUD_INT_RDS_DN_RISCI2   (1 <<  6)
#define AUD_INT_DN_SYNC         (1 << 12)
#define AUD_INT_UP_SYNC         (1 << 13)
#define AUD_INT_RDS_DN_SYNC     (1 << 14)
#define AUD_INT_OPC_ERR         (1 << 16)
#define AUD_INT_BER_IRQ         (1 << 20)
#define AUD_INT_MCHG_IRQ        (1 << 21)
#define GP_COUNT_CONTROL_RESET	0x3

static int cx23885_alsa_dma_init(struct cx23885_audio_dev *chip,
				 unsigned long nr_pages)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct page *pg;
	int i;

	buf->vaddr = vmalloc_32(nr_pages << PAGE_SHIFT);
	if (NULL == buf->vaddr) {
		dprintk(1, "vmalloc_32(%lu pages) failed\n", nr_pages);
		return -ENOMEM;
	}

	dprintk(1, "vmalloc is at addr %p, size=%lu\n",
		buf->vaddr, nr_pages << PAGE_SHIFT);

	memset(buf->vaddr, 0, nr_pages << PAGE_SHIFT);
	buf->nr_pages = nr_pages;

	buf->sglist = vzalloc(array_size(sizeof(*buf->sglist), buf->nr_pages));
	if (NULL == buf->sglist)
		goto vzalloc_err;

	sg_init_table(buf->sglist, buf->nr_pages);
	for (i = 0; i < buf->nr_pages; i++) {
		pg = vmalloc_to_page(buf->vaddr + i * PAGE_SIZE);
		if (NULL == pg)
			goto vmalloc_to_page_err;
		sg_set_page(&buf->sglist[i], pg, PAGE_SIZE, 0);
	}
	return 0;

vmalloc_to_page_err:
	vfree(buf->sglist);
	buf->sglist = NULL;
vzalloc_err:
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return -ENOMEM;
}

static int cx23885_alsa_dma_map(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	buf->sglen = dma_map_sg(&dev->pci->dev, buf->sglist,
			buf->nr_pages, DMA_FROM_DEVICE);

	if (0 == buf->sglen) {
		pr_warn("%s: cx23885_alsa_map_sg failed\n", __func__);
		return -ENOMEM;
	}
	return 0;
}

static int cx23885_alsa_dma_unmap(struct cx23885_audio_dev *dev)
{
	struct cx23885_audio_buffer *buf = dev->buf;

	if (!buf->sglen)
		return 0;

	dma_unmap_sg(&dev->pci->dev, buf->sglist, buf->nr_pages, DMA_FROM_DEVICE);
	buf->sglen = 0;
	return 0;
}

static int cx23885_alsa_dma_free(struct cx23885_audio_buffer *buf)
{
	vfree(buf->sglist);
	buf->sglist = NULL;
	vfree(buf->vaddr);
	buf->vaddr = NULL;
	return 0;
}

/*
 * BOARD Specific: Sets audio DMA
 */

static int cx23885_start_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_audio_buffer *buf = chip->buf;
	struct cx23885_dev *dev  = chip->dev;
	struct sram_channel *audio_ch =
		&dev->sram_channels[AUDIO_SRAM_CHANNEL];

	dprintk(1, "%s()\n", __func__);

	/* Make sure RISC/FIFO are off before changing FIFO/RISC settings */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* setup fifo + format - out channel */
	cx23885_sram_channel_setup(chip->dev, audio_ch, buf->bpl,
		buf->risc.dma);

	/* sets