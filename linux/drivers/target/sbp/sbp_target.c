nnel *audio_ch =
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
		goto error;

	ret = cx23885_risc_databuffer(chip->pci, &buf->risc, buf->sglist,
				   chip->period_size, chip->num_periods, 1);
	if (ret < 0)
		goto error;

	/* Loop back to start of program */
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP|RISC_IRQ1|RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	substream->runtime->dma_area = chip->buf->vaddr;
	substream->runtime->dma_bytes = chip->dma_size;
	substream->runtime->dma_addr = 0;

	return 0;

error:
	kfree(buf);
	chip->buf = NULL;
	return ret;
}

/*
 * hw free callback
 */
static int snd_cx23885_hw_free(struct snd_pcm_substream *substream)
{

	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);

	if (substream->runtime->dma_area) {
		dsp_buffer_free(chip);
		substream->runtime->dma_area = NULL;
	}

	return 0;
}

/*
 * prepare callback
 */
static int snd_cx23885_prepare(struct snd_pcm_substream *substream)
{
	return 0;
}

/*
 * trigger callback
 */
static int snd_cx23885_card_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	int err;

	/* Local interrupts are already disabled by ALSA */
	spin_lock(&chip->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		err = cx23885_start_audio_dma(chip);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		err = cx23885_stop_audio_dma(chip);
		break;
	default:
		err = -EINVAL;
		break;
	}

	spin_unlock(&chip->lock);

	return err;
}

/*
 * pointer callback
 */
static snd_pcm_uframes_t snd_cx23885_pointer(
	struct snd_pcm_substream *substream)
{
	struct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	u16 count;

	count = atomic_read(&chip->count);

	return runtime->period_size * (count & (runtime->periods-1));
}

/*
 * page callback (needed for mmap)
 */
static struct page *snd_cx23885_page(struct snd_pcm_substream *substream,
				unsigned long offset)
{
	void *pageptr = substream->runtime->dma_area + offset;
	return vmalloc_to_page(pageptr);
}

/*
 * operators
 */
static const struct snd_pcm_ops snd_cx23885_pcm_ops = {
	.open = snd_cx23885_pcm_open,
	.close = snd_cx23885_close,
	.hw_params = snd_cx23885_hw_params,
	.hw_free = snd_cx23885_hw_free,
	.prepare = snd_cx23885_prepare,
	.trigger = snd_cx23885_card_trigger,
	.pointer = snd_cx23885_pointer,
	.page = snd_cx23885_page,
};

/*
 * create a PCM device
 */
static int snd_cx23885_pcm(struct cx23885_audio_dev *chip, int device,
	char *name)
{
	int err;
	struct snd_pcm *pcm;

	err = snd_pcm_new(chip->card, name, device, 0, 1, &pcm);
	if (err < 0)
		return err;
	pcm->private_data = chip;
	strscpy(pcm->name, name, sizeof(pcm->name));
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_cx23885_pcm_ops);

	return 0;
}

/****************************************************************************
			Basic Flow for Sound Devices
 ****************************************************************************/

/*
 * Alsa Constructor - Component probe
 */

struct cx23885_audio_dev *cx23885_audio_register(struct cx23885_dev *dev)
{
	struct snd_card *card;
	struct cx23885_audio_dev *chip;
	int err;

	if (disable_analog_audio)
		return NULL;

	if (dev->sram_channels[AUDIO_SRAM_CHANNEL].cmds_start == 0) {
		pr_warn("%s(): Missing SRAM channel configuration for analog TV Audio\n",
		       __func__);
		return NULL;
	}

	err = snd_card_new(&dev->pci->dev,
			   SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1,
			THIS_MODULE, sizeof(struct cx23885_audio_dev), &card);
	if (err < 0)
		goto error_msg;

	chip = (struct cx23885_audio_dev *) card->private_data;
	chip->dev = dev;
	chip->pci = dev->pci;
	chip->card = card;
	spin_lock_init(&chip->lock);

	err = snd_cx23885_pcm(chip, 0, "CX23885 Digital");
	if (err < 0)
		goto error;

	strscpy(card->driver, "CX23885", sizeof(card->driver));
	sprintf(card->shortname, "Conexant CX23885");
	sprintf(card->longname, "%s at %s", card->shortname, dev->name);

	err = snd_card_register(card);
	if (err < 0)
		goto error;

	dprintk(0, "registered ALSA audio device\n");

	return chip;

error:
	snd_card_free(card);
error_msg:
	pr_err("%s(): Failed to register analog audio adapter\n",
	       __func__);

	return NULL;
}

/*
 * ALSA destructor
 */
void cx23885_audio_unregister(struct cx23885_dev *dev)
{
	struct cx23885_audio_dev *chip = dev->audio_dev;

	snd_card_free(chip->card);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-av.h"
#include "cx23885-video.h"

void cx23885_av_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			   container_of(work, struct cx23885_dev, cx25840_work);
	bool handled = false;

	v4l2_subdev_call(dev->sd_cx25840, core, interrupt_service_routine,
			 PCI_MSK_AV_CORE, &handled);

	/* Getting here with the interrupt not handled
	   then probbaly flatiron does have pending interrupts.
	*/
	if (!handled) {
		/* clear left and right adc channel interrupt request flag */
		cx23885_flatiron_write(dev, 0x1f,
			cx23885_flatiron_read(dev, 0x1f) | 0x80);
		cx23885_flatiron_write(dev, 0x23,
			cx23885_flatiron_read(dev, 0x23) | 0x80);
	}

	cx23885_irq_enable(dev, PCI_MSK_AV_CORE);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-av.h"
#include "cx23885-video.h"

void cx23885_av_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			   container_of(work, struct cx23885_dev, cx25840_work);
	bool handled = false;

	v4l2_subdev_call(dev->sd_cx25840, core, interrupt_service_routine,
			 PCI_MSK_AV_CORE, &handled);

	/* Getting here with the interrupt not handled
	   then probbaly flatiron does have pending interrupts.
	*/
	if (!handled) {
		/* clear left and right adc channel interrupt request flag */
		cx23885_flatiron_write(dev, 0x1f,
			cx23885_flatiron_read(dev, 0x1f) | 0x80);
		cx23885_flatiron_write(dev, 0x23,
			cx23885_flatiron_read(dev, 0x23) | 0x80);
	}

	cx23885_irq_enable(dev, PCI_MSK_AV_CORE);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_AV_H_
#define _CX23885_AV_H_
void cx23885_av_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#ifndef _CX23885_AV_H_
#define _CX23885_AV_H_
void cx23885_av_work_handler(struct work_struct *work);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2006 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <media/drv-intf/cx25840.h>
#include <linux/firmware.h>
#include <misc/altera.h>

#include "xc2028.h"
#include "netup-eeprom.h"
#include "netup-init.h"
#include "altera-ci.h"
#include "xc4000.h"
#include "xc5000.h"
#include "cx23888-ir.h"

static unsigned int netup_card_rev = 4;
module_param(netup_card_rev, int, 0644);
MODULE_PARM_DESC(netup_card_rev,
		"NetUP Dual DVB-T/C CI card revision");
static unsigned int enable_885_ir;
module_param(enable_885_ir, int, 0644);
MODULE_PARM_DESC(enable_885_ir,
		 "Enable integrated IR controller for supported\n"
		 "\t\t    CX2388[57] boards that are wired for it:\n"
		 "\t\t\tHVR-1250 (reported safe)\n"
		 "\t\t\tTerraTec Cinergy T PCIe Dual (not well tested, appears to be safe)\n"
		 "\t\t\tTeVii S470 (reported unsafe)\n"
		 "\t\t    This can cause an interrupt storm with some cards.\n"
		 "\t\t    Default: 0 [Disabled]");

/* ------------------------------------------------------------------ */
/* board config info                                                  */

struct cx23885_board cx23885_boards[] = {
	[CX23885_BOARD_UNKNOWN] = {
		.name		= "UNKNOWN/GENERIC",
		/* Ensure safe default for unknown boards */
		.clk_freq       = 0,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = 1,
		}, {
			.type   = CX23885_VMUX_COMPOSITE3,
			.vmux   = 2,
		}, {
			.type   = CX23885_VMUX_COMPOSITE4,
			.vmux   = 3,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1800lp] = {
		.name		= "Hauppauge WinTV-HVR1800lp",
		.portc		= CX23885_MPEG_DVB,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = 0,
			.gpio0  = 0xff00,
		}, {
			.type   = CX23885_VMUX_DEBUG,
			.vmux   = 0,
			.gpio0  = 0xff01,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = 1,
			.gpio0  = 0xff02,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = 2,
			.gpio0  = 0xff02,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1800] = {
		.name		= "Hauppauge WinTV-HVR1800",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_ENCODER,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_PHILIPS_TDA8290,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.tuner_bus	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.amux   = CX25840_AUDIO8,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1250] = {
		.name		= "Hauppauge WinTV-HVR1250",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
#ifdef MT2131_NO_ANALOG_SUPPORT_YET
		.tuner_type	= TUNER_PHILIPS_TDA8290,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.tuner_bus	= 1,
#endif
		.force_bff	= 1,
		.input          = {{
#ifdef MT2131_NO_ANALOG_SUPPORT_YET
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.amux   = CX25840_AUDIO8,
			.gpio0  = 0xff00,
		}, {
#endif
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0xff02,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0xff02,
		} },
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP] = {
		.name		= "DViCO FusionHDTV5 Express",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1500Q] = {
		.name		= "Hauppauge WinTV-HVR1500Q",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1500] = {
		.name		= "Hauppauge WinTV-HVR1500",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61, /* 0xc2 >> 1 */
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1200] = {
		.name		= "Hauppauge WinTV-HVR1200",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1700] = {
		.name		= "Hauppauge WinTV-HVR1700",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1400] = {
		.name		= "Hauppauge WinTV-HVR1400",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP] = {
		.name		= "DViCO FusionHDTV7 Dual Express",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP] = {
		.name		= "DViCO FusionHDTV DVB-T Dual Express",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H] = {
		.name		= "Leadtek Winfast PxDVR3200 H",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200] = {
		.name		= "Leadtek Winfast PxPVR2200",
		.porta		= CX23885_ANALOG_VIDEO,
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61,
		.tuner_bus	= 1,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	= CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2,
			.amux	= CX25840_AUDIO8,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_COMPOSITE1,
			.vmux	= CX25840_COMPOSITE1,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_SVIDEO,
			.vmux	= CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		}, {
			.type	= CX23885_VMUX_COMPONENT,
			.vmux	= CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
			.amux	= CX25840_AUDIO7,
			.gpio0	= 0x704040,
		} },
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000] = {
		.name		= "Leadtek Winfast PxDVR3200 H XC4000",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_XC4000,
		.tuner_addr	= 0x61,
		.radio_type	= UNSET,
		.radio_addr	= ADDR_UNSET,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	= CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3,
		}, {
			.type	= CX23885_VMUX_COMPOSITE1,
			.vmux	= CX25840_COMPOSITE1,
		}, {
			.type	= CX23885_VMUX_SVIDEO,
			.vmux	= CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		}, {
			.type	= CX23885_VMUX_COMPONENT,
			.vmux	= CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
		} },
	},
	[CX23885_BOARD_COMPRO_VIDEOMATE_E650F] = {
		.name		= "Compro VideoMate E650F",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6920] = {
		.name		= "TurboSight TBS 6920",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6980] = {
		.name		= "TurboSight TBS 6980",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TBS_6981] = {
		.name		= "TurboSight TBS 6981",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TEVII_S470] = {
		.name		= "TeVii S470",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBWORLD_2005] = {
		.name		= "DVBWorld DVB-S2 2005",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_NETUP_DUAL_DVBS2_CI] = {
		.ci_type	= 1,
		.name		= "NetUP Dual DVB-S2 CI",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1270] = {
		.name		= "Hauppauge WinTV-HVR1270",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1275] = {
		.name		= "Hauppauge WinTV-HVR1275",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1255] = {
		.name		= "Hauppauge WinTV-HVR1255",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1255_22111] = {
		.name		= "Hauppauge WinTV-HVR1255",
		.porta		= CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1210] = {
		.name		= "Hauppauge WinTV-HVR1210",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_MYGICA_X8506] = {
		.name		= "Mygica X8506 DMB-TH",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
			},
		},
	},
	[CX23885_BOARD_MAGICPRO_PROHDTVE2] = {
		.name		= "Magic-Pro ProHDTV Extreme 2",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
			},
		},
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1850] = {
		.name		= "Hauppauge WinTV-HVR1850",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_ENCODER,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.tuner_addr	= 0x42, /* 0x84 >> 1 */
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN4_CH2 |
					CX25840_VIN8_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_COMPRO_VIDEOMATE_E800] = {
		.name		= "Compro VideoMate E800",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1290] = {
		.name		= "Hauppauge WinTV-HVR1290",
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_MYGICA_X8558PRO] = {
		.name		= "Mygica X8558 PRO DMB-TH",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_LEADTEK_WINFAST_PXTV1200] = {
		.name           = "LEADTEK WinFast PxTV1200",
		.porta          = CX23885_ANALOG_VIDEO,
		.tuner_type     = TUNER_XC2028,
		.tuner_addr     = 0x61,
		.tuner_bus	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_COMPOSITE1,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH1 |
				  CX25840_VIN6_CH2 |
				  CX25840_VIN8_CH3 |
				  CX25840_COMPONENT_ON,
		} },
	},
	[CX23885_BOARD_GOTVIEW_X5_3D_HYBRID] = {
		.name		= "GoTView X5 3D Hybrid",
		.tuner_type	= TUNER_XC5000,
		.tuner_addr	= 0x64,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2,
			.gpio0	= 0x02,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX23885_VMUX_COMPOSITE1,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_SVIDEO_LUMA3 |
				  CX25840_SVIDEO_CHROMA4,
		} },
	},
	[CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF] = {
		.ci_type	= 2,
		.name		= "NetUP Dual DVB-T/C-CI RF",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.num_fds_portb	= 2,
		.num_fds_portc	= 2,
		.tuner_type	= TUNER_XC5000,
		.tuner_addr	= 0x64,
		.input          = { {
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE1,
		} },
	},
	[CX23885_BOARD_MPX885] = {
		.name		= "MPX-885",
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_COMPOSITE1,
			.amux   = CX25840_AUDIO6,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = CX25840_COMPOSITE2,
			.amux   = CX25840_AUDIO6,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE3,
			.vmux   = CX25840_COMPOSITE3,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		}, {
			.type   = CX23885_VMUX_COMPOSITE4,
			.vmux   = CX25840_COMPOSITE4,
			.amux   = CX25840_AUDIO7,
			.gpio0  = 0,
		} },
	},
	[CX23885_BOARD_MYGICA_X8507] = {
		.name		= "Mygica X8502/X8507 ISDB-T",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.input		= {
			{
				.type   = CX23885_VMUX_TELEVISION,
				.vmux   = CX25840_COMPOSITE2,
				.amux   = CX25840_AUDIO8,
			},
			{
				.type   = CX23885_VMUX_COMPOSITE1,
				.vmux   = CX25840_COMPOSITE8,
				.amux   = CX25840_AUDIO7,
			},
			{
				.type   = CX23885_VMUX_SVIDEO,
				.vmux   = CX25840_SVIDEO_LUMA3 |
						CX25840_SVIDEO_CHROMA4,
				.amux   = CX25840_AUDIO7,
			},
			{
				.type   = CX23885_VMUX_COMPONENT,
				.vmux   = CX25840_COMPONENT_ON |
					CX25840_VIN1_CH1 |
					CX25840_VIN6_CH2 |
					CX25840_VIN7_CH3,
				.amux   = CX25840_AUDIO7,
			},
		},
	},
	[CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL] = {
		.name		= "TerraTec Cinergy T PCIe Dual",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TEVII_S471] = {
		.name		= "TeVii S471",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_PROF_8000] = {
		.name		= "Prof Revolution DVB-S2 8000",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR4400] = {
		.name		= "Hauppauge WinTV-HVR4400/HVR5500",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_NXP_TDA18271,
		.tuner_addr	= 0x60, /* 0xc0 >> 1 */
		.tuner_bus	= 1,
	},
	[CX23885_BOARD_HAUPPAUGE_STARBURST] = {
		.name		= "Hauppauge WinTV Starburst",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_AVERMEDIA_HC81R] = {
		.name		= "AVerTV Hybrid Express Slim HC81R",
		.tuner_type	= TUNER_XC2028,
		.tuner_addr	= 0x61, /* 0xc2 >> 1 */
		.tuner_bus	= 1,
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   = CX25840_VIN2_CH1 |
				  CX25840_VIN5_CH2 |
				  CX25840_NONE0_CH3 |
				  CX25840_NONE1_CH3,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN8_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_VIN7_CH3 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO6,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN1_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_NONE0_CH3 |
				  CX25840_NONE1_CH3,
			.amux   = CX25840_AUDIO6,
		} },
	},
	[CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2] = {
		.name		= "DViCO FusionHDTV DVB-T Dual Express2",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_IMPACTVCBE] = {
		.name		= "Hauppauge ImpactVCB-e",
		.tuner_type	= TUNER_ABSENT,
		.porta		= CX23885_ANALOG_VIDEO,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN4_CH2 |
				  CX25840_VIN8_CH1 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_DVBSKY_T9580] = {
		.name		= "DVBSky T9580",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_T980C] = {
		.name		= "DVBSky T980C",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S950C] = {
		.name		= "DVBSky S950C",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_TT_CT2_4500_CI] = {
		.name		= "Technotrend TT-budget CT2-4500 CI",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S950] = {
		.name		= "DVBSky S950",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_S952] = {
		.name		= "DVBSky S952",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_DVBSKY_T982] = {
		.name		= "DVBSky T982",
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR5525] = {
		.name		= "Hauppauge WinTV-HVR5525",
		.porta		= CX23885_ANALOG_VIDEO,
		.portb		= CX23885_MPEG_DVB,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.force_bff	= 1,
		.input		= {{
			.type	= CX23885_VMUX_TELEVISION,
			.vmux	=	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
				  CX25840_VIN8_CH1 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_VIEWCAST_260E] = {
		.name		= "ViewCast 260e",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN5_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH2 |
					CX25840_VIN5_CH1 |
					CX25840_COMPONENT_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_VIEWCAST_460E] = {
		.name		= "ViewCast 460e",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN4_CH1,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPONENT,
			.vmux   = CX25840_VIN7_CH3 |
					CX25840_VIN6_CH1 |
					CX25840_VIN5_CH2 |
					CX25840_COMPONENT_ON,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_COMPOSITE2,
			.vmux   = CX25840_VIN6_CH1,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_DVB] = {
		.name         = "Hauppauge WinTV-QuadHD-DVB",
		.porta        = CX23885_ANALOG_VIDEO,
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_DVB_885] = {
		.name         = "Hauppauge WinTV-QuadHD-DVB(885)",
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type   = TUNER_ABSENT,
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC] = {
		.name         = "Hauppauge WinTV-QuadHD-ATSC",
		.porta        = CX23885_ANALOG_VIDEO,
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type	= TUNER_ABSENT,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC_885] = {
		.name         = "Hauppauge WinTV-QuadHD-ATSC(885)",
		.portb        = CX23885_MPEG_DVB,
		.portc        = CX23885_MPEG_DVB,
		.tuner_type   = TUNER_ABSENT,
	},
	[CX23885_BOARD_HAUPPAUGE_HVR1265_K4] = {
		.name		= "Hauppauge WinTV-HVR-1265(161111)",
		.porta          = CX23885_ANALOG_VIDEO,
		.portc		= CX23885_MPEG_DVB,
		.tuner_type     = TUNER_ABSENT,
		.input          = {{
			.type   = CX23885_VMUX_TELEVISION,
			.vmux   =	CX25840_VIN7_CH3 |
					CX25840_VIN5_CH2 |
					CX25840_VIN2_CH1 |
					CX25840_DIF_ON,
			.amux   = CX25840_AUDIO8,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   =	CX25840_VIN4_CH2 |
					CX25840_VIN6_CH1 |
					CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
	[CX23885_BOARD_HAUPPAUGE_STARBURST2] = {
		.name		= "Hauppauge WinTV-Starburst2",
		.portb		= CX23885_MPEG_DVB,
	},
	[CX23885_BOARD_AVERMEDIA_CE310B] = {
		.name		= "AVerMedia CE310B",
		.porta		= CX23885_ANALOG_VIDEO,
		.force_bff	= 1,
		.input          = {{
			.type   = CX23885_VMUX_COMPOSITE1,
			.vmux   = CX25840_VIN1_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_NONE0_CH3,
			.amux   = CX25840_AUDIO7,
		}, {
			.type   = CX23885_VMUX_SVIDEO,
			.vmux   = CX25840_VIN8_CH1 |
				  CX25840_NONE_CH2 |
				  CX25840_VIN7_CH3 |
				  CX25840_SVIDEO_ON,
			.amux   = CX25840_AUDIO7,
		} },
	},
};
const unsigned int cx23885_bcount = ARRAY_SIZE(cx23885_boards);

/* ------------------------------------------------------------------ */
/* PCI subsystem IDs                                                  */

struct cx23885_subid cx23885_subids[] = {
	{
		.subvendor = 0x0070,
		.subdevice = 0x3400,
		.card      = CX23885_BOARD_UNKNOWN,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7600,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800lp,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7800,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7801,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7809,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7911,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1250,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xd500,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7790,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500Q,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7797,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500Q,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7710,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7717,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1500,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x71d1,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1200,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x71d3,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1200,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8101,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1700,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8010,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1400,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xd618,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xdb78,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6681,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f21,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f39,
		.card	   = CX23885_BOARD_LEADTEK_WINFAST_PXDVR3200_H_XC4000,
	}, {
		.subvendor = 0x185b,
		.subdevice = 0xe800,
		.card      = CX23885_BOARD_COMPRO_VIDEOMATE_E650F,
	}, {
		.subvendor = 0x6920,
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6920,
	}, {
		.subvendor = 0x6980,
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6980,
	}, {
		.subvendor = 0x6981,
		.subdevice = 0x8888,
		.card      = CX23885_BOARD_TBS_6981,
	}, {
		.subvendor = 0xd470,
		.subdevice = 0x9022,
		.card      = CX23885_BOARD_TEVII_S470,
	}, {
		.subvendor = 0x0001,
		.subdevice = 0x2005,
		.card      = CX23885_BOARD_DVBWORLD_2005,
	}, {
		.subvendor = 0x1b55,
		.subdevice = 0x2a2c,
		.card      = CX23885_BOARD_NETUP_DUAL_DVBS2_CI,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2211,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1270,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2215,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x221d,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2251,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2259,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255_22111,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2291,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2295,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2299,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x229d,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f0,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f1,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1255,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f2,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1275,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f3,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f4,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x22f5,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1210, /* HVR1215 */
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8651,
		.card      = CX23885_BOARD_MYGICA_X8506,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8657,
		.card      = CX23885_BOARD_MAGICPRO_PROHDTVE2,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8541,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1850,
	}, {
		.subvendor = 0x1858,
		.subdevice = 0xe800,
		.card      = CX23885_BOARD_COMPRO_VIDEOMATE_E800,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x8551,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1290,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8578,
		.card      = CX23885_BOARD_MYGICA_X8558PRO,
	}, {
		.subvendor = 0x107d,
		.subdevice = 0x6f22,
		.card      = CX23885_BOARD_LEADTEK_WINFAST_PXTV1200,
	}, {
		.subvendor = 0x5654,
		.subdevice = 0x2390,
		.card      = CX23885_BOARD_GOTVIEW_X5_3D_HYBRID,
	}, {
		.subvendor = 0x1b55,
		.subdevice = 0xe2e4,
		.card      = CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF,
	}, {
		.subvendor = 0x14f1,
		.subdevice = 0x8502,
		.card      = CX23885_BOARD_MYGICA_X8507,
	}, {
		.subvendor = 0x153b,
		.subdevice = 0x117e,
		.card      = CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL,
	}, {
		.subvendor = 0xd471,
		.subdevice = 0x9022,
		.card      = CX23885_BOARD_TEVII_S471,
	}, {
		.subvendor = 0x8000,
		.subdevice = 0x3034,
		.card      = CX23885_BOARD_PROF_8000,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc108,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-4400 (Model 121xxx, Hybrid DVB-T/S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc138,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-5500 (Model 121xxx, Hybrid DVB-T/C/S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc12a,
		.card      = CX23885_BOARD_HAUPPAUGE_STARBURST, /* Hauppauge WinTV Starburst (Model 121x00, DVB-S2, IR) */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xc1f8,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR4400, /* Hauppauge WinTV HVR-5500 (Model 121xxx, Hybrid DVB-T/C/S2, IR) */
	}, {
		.subvendor = 0x1461,
		.subdevice = 0xd939,
		.card      = CX23885_BOARD_AVERMEDIA_HC81R,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7133,
		.card      = CX23885_BOARD_HAUPPAUGE_IMPACTVCBE,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x7137,
		.card      = CX23885_BOARD_HAUPPAUGE_IMPACTVCBE,
	}, {
		.subvendor = 0x18ac,
		.subdevice = 0xdb98,
		.card      = CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x9580,
		.card      = CX23885_BOARD_DVBSKY_T9580,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x980c,
		.card      = CX23885_BOARD_DVBSKY_T980C,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x950c,
		.card      = CX23885_BOARD_DVBSKY_S950C,
	}, {
		.subvendor = 0x13c2,
		.subdevice = 0x3013,
		.card      = CX23885_BOARD_TT_CT2_4500_CI,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0950,
		.card      = CX23885_BOARD_DVBSKY_S950,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0952,
		.card      = CX23885_BOARD_DVBSKY_S952,
	}, {
		.subvendor = 0x4254,
		.subdevice = 0x0982,
		.card      = CX23885_BOARD_DVBSKY_T982,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0xf038,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR5525,
	}, {
		.subvendor = 0x1576,
		.subdevice = 0x0260,
		.card      = CX23885_BOARD_VIEWCAST_260E,
	}, {
		.subvendor = 0x1576,
		.subdevice = 0x0460,
		.card      = CX23885_BOARD_VIEWCAST_460E,
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6a28,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB, /* Tuner Pair 1 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6b28,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_DVB, /* Tuner Pair 2 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6a18,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC, /* Tuner Pair 1 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x6b18,
		.card      = CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC, /* Tuner Pair 2 */
	}, {
		.subvendor = 0x0070,
		.subdevice = 0x2a18,
		.card      = CX23885_BOARD_HAUPPAUGE_HVR1265_K4, /* Hauppauge WinTV HVR-1265 (Model 161xx1, Hybrid ATSC/QAM-B) */
	}, {
		.