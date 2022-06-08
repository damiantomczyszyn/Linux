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

	/* sets bpl size */
	cx_write(AUD_INT_A_LNGTH, buf->bpl);

	/* This is required to get good audio (1 seems to be ok) */
	cx_write(AUD_INT_A_MODE, 1);

	/* reset counter */
	cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
	atomic_set(&chip->count, 0);

	dprintk(1, "Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer\n",
		buf->bpl, cx_read(audio_ch->cmds_start+12)>>1,
		chip->num_periods, buf->bpl * chip->num_periods);

	/* Enables corresponding bits at AUD_INT_STAT */
	cx_write(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	/* Clean any pending interrupt bits already set */
	cx_write(AUDIO_INT_INT_STAT, ~0);

	/* enable audio irqs */
	cx_set(PCI_INT_MSK, chip->dev->pci_irqmask | PCI_MSK_AUD_INT);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5)); /* Enables Risc Processor */
	cx_set(AUD_INT_DMA_CTL, 0x11); /* audio downstream FIFO and
					  RISC enable */
	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev, audio_ch);

	return 0;
}

/*
 * BOARD Specific: Resets audio DMA
 */
static int cx23885_stop_audio_dma(struct cx23885_audio_dev *chip)
{
	struct cx23885_dev *dev = chip->dev;
	dprintk(1, "Stopping audio DMA\n");

	/* stop dma */
	cx_clear(AUD_INT_DMA_CTL, 0x11);

	/* disable irqs */
	cx_clear(PCI_INT_MSK, PCI_MSK_AUD_INT);
	cx_clear(AUDIO_INT_INT_MSK, AUD_INT_OPC_ERR | AUD_INT_DN_SYNC |
				    AUD_INT_DN_RISCI1);

	if (audio_debug)
		cx23885_sram_channel_dump(chip->dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);

	return 0;
}

/*
 * BOARD Specific: Handles audio IRQ
 */
int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask)
{
	struct cx23885_audio_dev *chip = dev->audio_dev;

	if (0 == (status & mask))
		return 0;

	cx_write(AUDIO_INT_INT_STAT, status);

	/* risc op code error */
	if (status & AUD_INT_OPC_ERR) {
		pr_warn("%s/1: Audio risc op code error\n",
			dev->name);
		cx_clear(AUD_INT_DMA_CTL, 0x11);
		cx23885_sram_channel_dump(dev,
			&dev->sram_channels[AUDIO_SRAM_CHANNEL]);
	}
	if (status & AUD_INT_DN_SYNC) {
		dprintk(1, "Downstream sync error\n");
		cx_write(AUD_INT_A_GPCNT_CTL, GP_COUNT_CONTROL_RESET);
		return 1;
	}
	/* risc1 downstream */
	if (status & AUD_INT_DN_RISCI1) {
		atomic_set(&chip->count, cx_read(AUD_INT_A_GPCNT));
		snd_pcm_period_elapsed(chip->substream);
	}
	/* FIXME: Any other status should deserve a special handling? */

	return 1;
}

static int dsp_buffer_free(struct cx23885_audio_dev *chip)
{
	struct cx23885_riscmem *risc;

	BUG_ON(!chip->dma_size);

	dprintk(2, "Freeing buffer\n");
	cx23885_alsa_dma_unmap(chip);
	cx23885_alsa_dma_free(chip->buf);
	risc = &chip->buf->risc;
	dma_free_coherent(&chip->pci->dev, risc->size, risc->cpu, risc->dma);
	kfree(chip->buf);

	chip->buf = NULL;
	chip->dma_size = 0;

	return 0;
}

/****************************************************************************
				ALSA PCM Interface
 ****************************************************************************/

/*
 * Digital hardware definition
 */
#define DEFAULT_FIFO_SIZE	4096

static const struct snd_pcm_hardware snd_cx23885_digital_hw = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,

	.rates =		SNDRV_PCM_RATE_48000,
	.rate_min =		48000,
	.rate_max =		48000,
	.channels_min = 2,
	.channels_max = 2,
	/* Analog audio output will be full of clicks and pops if there
	   are not exactly four lines in the SRAM FIFO buffer.  */
	.period_bytes_min = DEFAULT_FIFO_SIZE/4,
	.period_bytes_max = DEFAULT_FIFO_SIZE/4,
	.periods_min = 1,
	.periods_max = 1024,
	.buffer_bytes_max = (1024*1024),
};

/*
 * audio pcm capture open callback
 */
static int snd_cx23885_pcm_open(struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	if (!chip) {
		pr_err("BUG: cx23885 can't find device struct. Can't proceed with open\n");
		return -ENODEV;
	}

	err = snd_pcm_hw_constraint_pow2(runtime, 0,
		SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0)
		goto _error;

	chip->substream = substream;

	runtime->hw = snd_cx23885_digital_hw;

	if (chip->dev->sram_channels[AUDIO_SRAM_CHANNEL].fifo_size !=
		DEFAULT_FIFO_SIZE) {
		unsigned int bpl = chip->dev->
			sram_channels[AUDIO_SRAM_CHANNEL].fifo_size / 4;
		bpl &= ~7; /* must be multiple of 8 */
		runtime->hw.period_bytes_min = bpl;
		runtime->hw.period_bytes_max = bpl;
	}

	return 0;
_error:
	dprintk(1, "Error opening PCM!\n");
	return err;
}

/*
 * audio close callback
 */
static int snd_cx23885_close(struct snd_pcm_substream *substream)
{
	return 0;
}


/*
 * hw_params callback
 */
static int snd_cx23885_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *hw_params)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct cx23885_audio_buffer *buf;
	int ret;

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	chip->period_size = params_period_bytes(hw_params);
	chip->num_periods = params_periods(hw_params);
	chip->dma_size = chip->period_size * params_periods(hw_params);

	BUG_ON(!chip->dma_size);
	BUG_ON(chip->num_periods & (chip->num_periods-1));

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (NULL == buf)
		return -ENOMEM;

	buf->bpl = chip->period_size;
	chip->buf = buf;

	ret = cx23885_alsa_dma_init(chip,
			(PAGE_ALIGN(chip->dma_size) >> PAGE_SHIFT));
	if (ret < 0)
		goto error;

	ret = cx23885_alsa_dma_map(chip);
	if (ret < 0)
		goto er