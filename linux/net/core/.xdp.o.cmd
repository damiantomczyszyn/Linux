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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 Â
t´>¤íç\J$ßõíXdL¢=×èí¬CV=p©Î<‡]–ó^qˆûÚyøBæXçòŽB”±ýt=m® 2Ÿ°"“·6D¼b‹O1b3z=8„,ÆÄBp5e/þóâO¤f¯×±IüŠ=ÌJ¬œ;v&/oÓ)H`J‘TÚ‘?>š‚•fî£!ûhª÷†S”²‹ð5ËÍÔ$¹7)2%`|À¨¥-Üo0$õÁyµqÖŠrp>¸Î¼ëf˜ˆÝmY·J»…Ê\YsìbÉ„uÇCÞ…Ãw^ëó™K]3#Cð}X’9XV¯t>Þ"Noo#%üås/íºÔ­¬ÙñÔíiq«`r@ 7›^]õ¡sA~Ð0~Ý”/(À·PÝ+P` ÷ø ¥zê0E€y\éMØ‡J^¦Ì”–‹Ò¬Vi'¾ kS©{¸²Ëbç¨R|)8ä»Á»¼²Î7r¹vzW†‚³%H(>~a.‘_Ï•ó"´«·‚·WÆ¹B?0låù@P[éªüxDÛ`äT$:ÚŒ‘Ë|!¦†ä#¶.Æ­§}_æ’SúÃ‹vds~E÷äjWë¿™Ú8è<-´,Î*Ê¡ËÞwöJó-ÐÁ|AÖpÈ¦!Uît
†±‡¯Â’WhÅð ¨tó/ý»Ó%MÖ›„k^/ñøu$yRnÆºd" Õ)•.ìØŒÞÄõ+¥[Á¢-	'†ÓXÄn€$ÊÜÿ³ILº¯8\B¤¡+6Q‡Ú¢¥a<HÔrŠútX"bÉ38£
«ñó#Ä9ë†(	ô²^Ž\t¥~'ÖdÓUâ1c(›¿ã|ÚV§µM§øA²Ç‹î6ÄVÎZùŽGüf¥ßf\‰>ž{IŠQA(z×ÿXÜY$Côl;í³ Ì9X¿“†à4§hƒÿòkºTel†´©ÃöqÌë æC©)Á^¨G•àeJÿQ³–,s(ª¶µca-+[ý„ZÂ¿Ë÷þ»ŒÞå)M{<8›EFb’·Ã-¹ýû{XµâxÜ×“i²ùÊ}<xnŒ“þ¸éÉÊ-I‘oT©Âüç^Ë„ ZDrÜµ¼ãc‰‡ÛPÑÈ£³î@ÿX#×é:šW–•ÀOòeŒ¤9ªÌU¢¹%—£É¦ý;å®ËìVÕ"Ý°Ž$ÄU$*Û>Ésó«¥‘~I¼äá„¼D¶š·öSBožÝc=%Æs’5}»¸ˆÙ°±Ôi¸ð|µ¤Zñ»?§UŒòl°®üº\4Ù1’Í÷Ó”Š÷ÓëžÍä›q×áGÓ%Jô¦µ Œ>£§¡Zn–Ù:­é{‚BŒÈ.T5Ù9…ˆ™Áú—_	gÿŠÑþµ¼§¿ÇéõŠg}&bŠ-åÿ›€ÙC…G]Ša×ûJò¸6‰©ƒ¾¯•ÑW&$ï€#NÂ0Èqç‚¦Óš\Œµx¿JÒ ¬U%÷FX(øzª¶aÜ¸ Ðå17xnsu¡\#V¡Ê'ò›Píážˆ¿k
ÒÛ¤¯Äü3¯·ç»Ý3A	Ô¢>|	Î6Anæñù¡ù5g‹†		dƒû²¿ ä&G«—/K¢Ï1ÎÍo×„‘¹nbÍwFˆ€_FŸE8|Ï¤Éâb~¼åÎƒ®iŽ„ê2fg¿ëºÃ8öÔ¤ ß¨? ÍA×¥>áXË/–~÷\À6¨ÍÕ«¢k÷ò‹*Á²¨C¯Ö‚Q!,ÖÔÉ,Ü¿	-/§jÎ¿õóÖiTØú|5Ñ:dJQ !tqy3›y5‡îyÎfDãPÖ^Uº?W—~A°œÃ%·Ù'“‡íŠ+™&‡t£oQlW¸¯ ºEK¬¡ï€ÑL¢7+ÒÆeo ì·WT—ë!µYÙ“G®§5ÏäËµ#å”’?;w-ðDpRi¯ÞèÎpIéhwy^0œ‚å\®Žñz°[£‚åZ(‚‘OÜsi€¶ë‡h4Rèß}Ú[ÁJ=Jˆ·N’–[–h^äƒ­T;ÓšùÂ´Ú|ÒÉ6Û	Ð!d¹ÊjG-vs…’™Ö¶ê:óæ'¬ \»¯ce¡«2ƒeüÊÜßa{W¯
ÉÜây>	ŒòU¥Z™´òˆ „ËáŽîg'AöÍCy#qû¢;–¿÷ëïÙ‚M7(‘A	N¸©WN~wÒ´SB,ë{¨ *ß*5½éX U¨¹ÜàV	ËXIÞ
Š¹€3ÈtèCdÖ‰@¾ãj¾­S¹¼RZŠ+s<»Bå_HiBÃE£«UÈ²Ñøé°:ú0k„ß÷ÔàðÝ	¿JÚNÿ¥ƒ‹M'…'­I^‚¢†wÌ®H+&£kaÔ!˜[å¹¨9ˆ*óÁî_7æn…8F‡nßé³æe%nâ,·œó‰“h³'¶ßVØx´Kã¨
1ûúÀ"g°ªCCõüO+7rþû‹¯¨¨l.;’	²ÇZ·•<ôNP™ÇûiîÞÍrõ™¤é8ã¿gP ­°»¸X@âÿ Ü"¯`ØsÒ“øâG ¨yfî=§ }ï“¢é9m™˜ÓÀ/Cý<Íú¿¡€
)4{ó´o2«»7½uïc±€ ‘Ä*Ûäþ©¹“²FW[1(q¨!uÏD´Ø±´Ž?÷T·Tþ*+O„cŒT­A>8#¼Z‡h®ú$ñü£-ºûó]NµYÓ8™ã!w/u9'>¿£N gÿtÆ2I3E¬1·†ÌTíÕî…¥yjx®F"Ÿ¾báôÖÖG™w¾ã§‘ØÐì”zþŒ·¿«€JxTCÕXãÈ‰ÔAL›lQþÃü"¥YrS_›´T(| 0üWSLø½”,Xs›,õ2gÓImB6ó4Ü4&7Ëþ|„‹úˆ»A{±±äkÖóßp¶pE+­Ý«xÁ\žûÙØš^ªÄ–"ö›1ç™9R6ã’AsU¾´~í±Již©&
¹ßa¿Oyx€‘–IìOÌÄ²ä¤×ê¢«Aö©ã 
«˜a‡i¤Ã¨›5²_œŠåÉ«uf—>Ì¦Xo±Jy¸dRéàÿŠhüË°ãÄT*ªßk‘~vÀ|Ô[§ÐUÉ|UÑ‹;Ù%kK4¶}Ve©«øZè*ðÎµYç‡
DÎ’N@fRZ76“„RUóø—mc‚&Ò:Ç[Cµ•FDEÌ×±'Ø¦	ýó4*M\Gû_½±p`/@øéÑ¤57#ï‚è~'#«J[šÜµFÑÎ—‘Û’¶ÈÑÑ§áÇ§ 7t~¾ì…€ŸÀ8ª	ôÚóÛ;è“e˜À6<‚£ÒÿX6£÷Uy÷Gl<sÕbå3ÅtU
!r³—7ÎCdè½¼AF›fÅc«~	:uìÉ[Âr¢ž¥7°EzÇPUŠ¹9öç'æë‰…[$‹‰;–=Îí#¯ÔF£P #yI´-8Ùð{b~H¥é©ÚÞ–#nržØŒŒZ¾y^'ä’¬`äƒ€“6k€¡õüL0((>|›yD»<uLâ~^oô™*¤™ú2`_M‘sTB—LìÀj×“‹1#ÚÀ[G±
®#æ0ðÓÐÃw› â*õíL}ù¾íþØÈ:]'O½“ïl”bbkKðØPž»}í‡\Fì±‡È¾èD^gÈœ#¡qj'qŽ_NqpˆÐ?5Wä“¥\š5‰Æ”ëYoÓXòSy<ºPFÒAŽ³À<J}
 H÷¼8×ÕVãøc!-—&Œ½wUµ®—ˆÊjgí›ò‹!8¸ªÅÅæw!"Á´DT_ik¥*à<ëz„Ó	%[Ð9ÅÏ¯ˆœšv•VžÎ)Ð•WÅÑa„¿¶„.ÔÓ„k·:xµ[™–	É×õjZÄg[¬S°CýËåÙÏÖ¥ÕJ+$|eç›·ek´8³9? áàªTEü½xÕ‰ø@vÎL«;çŒšÊ½Uõõ|Ÿ¡¿ƒÅaIMnÝGÛJTBS3šešäœþ!S¯­ñH ˆá%àKô±6„á8ï²Ó«épä‚³AêV>Õ•]¸j[ùí”èŠ4@2ýoJ³Ð+Ðw«4!¸kgÿ£Ú\Uhîsù†ß×Kh¼_4Z}@7à/K<Ò²õõÞ„|ùóÄ6(fß¸†ÂBI½ê°§=LkÄ‚òY-Ñ†©Šµl¨èˆûkî"‹ðf&žiûï%ìaíŒ¾Ž‰#²Ïˆ?àyfÜ:ØÇK76OM3U•FK„¥kº+±ZU…Ø=UIEÉUkùÆ80‚°'ò7é,¢zš^+7´IïÔSE„QÈØ/
âCa1Oƒõ¡Á³=vÆ°Òå¨7‹tÒ¿]F¹×¿$”†¢+aî²Åµ'ö@p\è¥x%ßý%š„.}bÕOîa»©lvkˆäþðø9…t%I¯'Óñ'‰±XæV}2Ø câ›
`òC+õ­Uë#sÈ`(ïj®+uÛ~CÓ9?“·»£"ˆ«öÏ(·ˆOKÚø˜;.+¹d*Ð½Ãè¹NDîa´¤ÿÍû‚i±·Ö
ÇKã²¢½+$ƒpÎyW¹±­ñF†ç@Œ‡¾³?<pç¯§ª›µM¢c3±)ÖŸÉßêQÂCyÐ©äà‹{íñ·¶—9°MG/•I4ìÊ,Cx0qWÝ4­„ãgú²ì‘Ò.Yžrn{Ö¿ÊSªuÄ÷,˜,¡æáÎ£uÞ6- eœÿO_£jsÿ×…cô²’ð,Ä¿òù\Ì¯¸ù9c˜Q§ræéø?‰(÷<=þF©/¾¹g½!þ¶@üÏ\’b õQ@·AsôàŒ„]9Hk›˜PP†Säf`7IÖ¶‡lÇéÜý×D¢FV:"äz¦œÊ·½¡	X(íŽ$µkµõœ‡Áái2 Í?ãk¸f‹øF!Ë tÌ@„WMÐUMüWIÇMS'Pb:s³}a÷ïj|v‰¯(‚.—ÄÍ¾ƒ(¨61Ÿ¼ÅJÊgïg KO¢èï¦¼™H¦î˜
4¤m	k4´³“n²Tˆïú˜‡`É0[®õ
góJjÁôƒûU·Û1~ï¡i÷¨O9‘¿
AŠjÑ}ÎåÒãØ£±ˆøÄI‹¯m,VöÏ>.º%cÀ 1eÿäy²íÝ'….µèŽS¿ÚRÏr„P,[
µ±ˆ¾XZˆG^ Ì0 ˜s¼‘<[õ•Ù#ˆ³ÿ=Èm‚ZhusþæÏ’Ùéb‡×…YsT¢èy|ŸÕTâ½Xÿ`’wÈ£P
…t¤Ågu|E“|ÂÜº½pªÎÌ²‹éãe„I§ËõõKÖ7ÞG—ºÊRB(â%2Ÿ½‹ºÐ“ðñLÈX»ÈcÖtR£ü*H¾v­4^Wª½1ð3-šùð•‡%8¬yÝVÛw–od·hºÃæ¦&ûvxß‚çÆA»,Qód ÍhNRïšÃ.…¯pÍ5ÿ4u²Šï|ò‹šQ­„{ª»æþ,}ô.‰uãÉhÁà–.,Ì†&Ê¾x@Î€˜LQ:çfeÉs7Ïx =ŽM U
ñfžD¿»sùAÔÉaNìÞÔy
ŽÀ@´Š55†ò5ù®Isó¿»Ëèô‘ENYY oä„î5C•ÜÃ÷H¾Pƒ,Á°…NÀu:ÿíÙã+¶XÂ_ƒ©y^X+ô2M¸´v„~XÔ,:ßdgüô¥HÏJr:jþ•@øí´h£Ë´Wåµµ I@ŽE(d#}¦‡îØê¼‰›K%Í¡€ ýGPÿ{Þ*jò±NòqTgµ#è[¿ˆJèÖê¦o=táÚ:ó¤Ÿî0hÍ)èkdØ!ÔyTõëÒ}{HãáÛR	ƒN¥~ˆhØp¨‚áy+ÛÕÚN’O<í’èG#]ÏSQ”‰ÁüÝT¶ºÊROìP¦»+ëøUÕ_ƒìŸnÊ[†ßôÏ’|ZmígÁ–L‘Ðòâkœ¸1“¬3ïu¢Â¾„u‘sk<‘ÓšÙÆ*àqÆ¨8vÑWÞóÞ0I„f?:³±n¸(Ð²^# ù‚k£®Âæ"¯yÚ_u7x…óÃ­' ÒRìÏþ+ˆ!uïT¯[u ˆ)yw"žx‰¶Ýd	åq`âû_k¨/¤}þ÷1Y‹ýÚ“‘€Ïd·@¬ÜËÎÓ\Þ_ëÒûç¡¾ÐSù’$ñ»Zë)-¹~· ½ ¦ù‘çäùíhŒuà v®‰×Ð‘GfJñ€Ÿ Ø]›} ;©¡ÆÇ÷Gé›ãUÀ›ÿÿHÌr@š÷–^,!qÍ¡á­ªgÚÁº6êà©áÓ¶)©Îw Ñ[Íèf]sÆia¨Q·d§×£’Q±šGYS„ A¦÷ &TSGUò F!ñMÂtß(œ¦AvŒXJ—†Û}éî„¾‹‚L‡Äâ×'š* ©­ê°ã‹+±/ç2Ü“AÐ ¶Â
(.À`W/ðÎC0FktmV,¡oÍ_¯Þ‡ñ€o…°Göã•ç±Ÿ‡¥Éh›ìNyw“Žbñ(PD–fN#~@ÝŠüY¾jE>â‹ë”ÀTóë>Ô¼¬°ðmÀG`±#Þp”7ÿÊº˜rð’w£CT›Ú[£) ð	§aâ_MY}þ©þõ'…)KÌ¡K®~ñéÃ»'"ÐÎ]'éÓ[¶¡ì#é6˜ŠØ‹î‰Ñä´½Í»KXC¥çûKâ°Ô†Ýç0°öý3~ï„érØwžì`é™Ïd>ÍgÐù,~_ÛîÕ¹œï_>Âº)’½ò¿úaD·¯¤[yšæÜ¦=Þv¦±Ý/«äç8ö6òî®oS f	äd×? %€(P€8ÒbœÎÁà.jkqŒV`ƒ‡T[,÷/"ÒZª7Œyj³½¿ƒ	fy Ç¢;™“>×,@.•Wî“éxùè¡cR³¶4t¸ëMS”…gº+ßž% i"¥¶z©ä!<>(âÈ¶¿J¼•Ç›°Ù«S`ÝŠF»·…EJÆ•p½wÞ¢÷(W°¦Î®µkdÊì±#r nWg•Ú/¼wÁ‘dfPúÁŸ0%u$`ß¿?-¿á~S²ÒN÷8wÙ.ùm…ÝÅ)Íf{·HÏ e¥ºÿÃv’Wp.ŠöfJÌDß•Ýû®ñ6;YÝ”˜TƒîtÚDs·’'Z¬’>ƒ€Ê¦?«Öi¤ÒAù
‰×OêÏ¹êÕ¡•gÝÙ°Þr	9!¢úÐ„®°v‰+­ =ƒ¡éx}íkbŒkõÄï¤ê+q)t@i€[Š¦ªŒQ¯+“"‹vÃk:~þ”`÷•¾¦•ªS“män§£˜6x¸Q yf-_é'ëÏ/=²^Qnã¿0Ã84ëièý·_ìd²)(-ÄÇ;:&É4“õîê–öZÂ—bÈs¿l¼ÂW¦7dÍ@'?áµ={Ï€ á·$hÝ¤U¾È?Ù¯BÄá£Õ^øÉHª“­–ÌÀAêÑÍ§LÜíx[ÀwÚÌ3ðè“_Àÿ(èèKÆyo:}¶ãªNÒ´ÿD’îýÚ–âUÂØ§¢ÂþþuFö ½:,ŽœüÌÖd9‡ð»}ŒÛe»YWáÁßÒê:ä.û3çÐESÉ$— Øsf2Ñ¹6Ã5g}u2¥ac—u2ýIØàB;ª^0qÒj^ñ0HÆft‹9{£(G«,\³ôÖÁé'b¯zéÈ®²}s'ýSâæC¨Z©å 4Ø³ßôn`bé­ñqdÛÕ“ðŠª8\G .eÊ4E¬_ÍKªn2Éil¾jö7shv›XqX%l‹VÛê¨Ì	ªfª¨v\Æ)Ö(èÝM±é>±?*a;–½¢KÐ:×hµ$T×©Ùç/&&ÆŠÛH—¿ïò¼Rõ@•{d¥~mœèBÞèÆijedY¨(	—kN˜<š•/<§Ž³’vŠò¦zù¡—œ¶fÊÜD%¶É#o>‹±õCïðcF1â?´ð ÿt$a¥¯Š1¾g¾D5uÚõhIßJ€êÄ @5
âæÔ@gæ!ÂI;f’µO_Àâ‰â?Œ¦7žäzòNZ®‘zÄ‚¸7(|Ç"\Bð«PÃC¹0ÒjËÈ†U]ˆ¦ÞÇ­3S•ÚŠ\±	 |9vÁ«³ˆ?I
Y”Yˆ1’mþ¢¨°œ8-J¡u58·½«@¶T×	ì#`™ý\·©[!ceM_¸¬}{ß§iÒ)óO×0™%ƒæØ„Õ:dG¯æiOHáYN³gOCÑè2É¤ôÀ±|×áÉÂ¾Í²R#ËÓ`¤Ôlô›U¬Ô¡=eXD¥|¯]oæLmç)‚o]`‚ ÉlšlÕyG7§r‘ÛKŽ8/­ZŠ¯8hå¼¦ßØ»©Y«:‚ä›³t…£4¾Ê…I	q™°9%vìØ‚³ãÃRå#üåô%¬É¥£YäÅß”oPýØ¦j‘Ñ¾“VTmêEÂKñxØKd(´Gïñïh*”šÌp¿‘Tøa’O_RªÈïž1¤ýãOîjÜNçú¡Í«l®¿¥0¶;mO/,"kÙSø‰âƒƒ8ÈÄ›[X¥"Â!Øhé=ƒc@ÝNQ 6’Û	ÜpZ E½3@Ï×^'æ%U¨]¡«ÉeG Ìõ'OkD÷²[@Þ}ï© sÖxškOj£—xåòsT%p"˜SMZ7ÊB|
ÅmLaê!'¦¾þÁôsQ5‰ü:Ô5|¸Ï³×|ç+È$š/'w|;ÚòoÈ QÀJwå»„5[½IÔMô4–hah•w~×b|Æ@¢ª€H‡å”d3=êµ
Ç[ÑGŽh`Z y9ßkê)`Bœ4‹"À^Ììü“òÙŽûíú¥Ç z\ÚÿAÏýÌ¢òªÃV›´? ¾{v|[®Ä zòrˆäud](ò™É„â@EmÂ-ƒJ¾—yÆÊ<ZœÚÎ{<åŸ½rØ‹>5…ê)ÒBÒã7ÏÓz Œ!Ã34tpUƒ9L­„7²Ÿî­1{zï­ä¡=D~»¡¯ÿfË*
Yó›+Ñ fÏk§Ž©$XLPxbá®4Kø*0|8“Ì²4Óxw@É%Š²†g~ªGõPà©?g×.ê_º·0í|›ð­¼1<7çE3Åt›m³ ´²œ™Ÿ}ö±tÏKÐôÁ±¤ÏÄ‡\EÂnº_îÉJ›ÔŒ7óW°ìÐ•¯¯Ñõx}µ‘¡ Ã„Ž½‹þÉ‚i)Í˜•º–píÉu¾â’ÁƒDa:ÃÔØpy·õ²"Ð·rsë.ºCè=:x”iÈT´Òœó”4ë†ãéŠgaletÀ»¸_þÃôæsÜ]$K¤;êm€ƒdeòŽdàü ÅöÔ@GÛðxãi»ÇìLýêw6ÀÇÐôs¦Kqœg^°!*¼–Ç›h„µø»4X¨{\Ô%9t­qBç4Î6øí“8ú¼zÖ³Çú¡Á+P‹Îžgmº}VŸìhZ Nß¥é@8¹«¢óezÈ×èäÜ÷üM?Î/ê§óJÔ#lÝýdïLÇëÁ7Ö6’Î˜E$€Kñl¯›ˆ2¯Ljí?°€æ½ÇêFSÓAàôt¬çÊ_yÕÑX-†pUÁÞîïOÿ~—m6ú5EÅÇW­ó¬çø©K¦ÙK¤Ìgúqî¤GÚœ'6³/¾Áu/ñÃ†p_õAHßuùôØQ:¥FÚð€±™‘JSŠÕ†b"ºÖð÷º d°ÄšëÿËNŒ’Ï.š1c“l€ˆ¦(G e""°Ðëj–
æ¹ß› ô |]úànFÝ².†ulÌ¥ÛÀVQ!èÍþç©›óâ’(”4‹ S‚[ï"gx>B”ÔrÝÿÆ òÌÙ†ûÞ4„Hä+¬Ø …’À§êŠˆUÃIKœµ{¯‰¥kÍ?â—í
¶á!£Cp©5y{è@¤·Ú¤Â§sxSò£ ”5 ëg ¡?2ål†ì±^SË*ä¨;›%Ê¥·èåEÆä18oÖ”ÒÆƒ#=Ã„¹^²31ÿº{©–L<Ø%Éæ³èø+éUèNe\JkBãýìtî²ùò|æå–¾n0ùÞeßÐg-ø'Ø‘r9ˆBtv¼W¥¡Í(6[ß€ö8@’„ÝŒZa®/®•,Ú_²—!×»0àß›èã®3õ
¤õÞãWÍƒáÎŠ‰®ã2 ]2¾¯Aù=ÆøßFì|<
{o%]÷]Ž%¡}«À?Vt|§RLªdáì¿Ó"ó‰V¸‡4¹‡[VaÕ k²Xp‚,Á-,…sG¢©<­a¨÷ëñÛ\œ¢pÔšùZìµxâà¥0vÛuŸ¡9]üêÛ|êL_ãDïªó‚âÌf» 1wÍ_‚#tLNÝ3ó·¨}’žpÌ„Ãc-oKv™-LbÆü °ÙA{˜Kè7¨…µyß0Ñ/g·§­²:„ƒJÞX`Eã4zžµƒAîlxý±ÉYÌ…æ;Ø!]y˜V•Œ~»Wvîä7v‰çÿŠ+¬£$UÀ£¬&,?|`•N >Ö|I æ@>öAø*º©_Q*uÑƒß¶D¨®Ûá¾êÚ-ï”äòµƒ/ìí[b?•äÊ´b|¶@÷L;™;ÜÆUêÝ‰B.ùœ¢%,&ç* M)-EpçÚ¡ÁãƒœÑ`é”ýa\Ðz­œÉžôXÚMJoúvÀêò¯”„>KÊTÓ%¢CÐ¢
åvçötµ,pËü]}Ìë³ŸÕÃvü%šû€¢õïö‰¦Å=	i¡R/CDŸÆ
Éð¤//n‘€dÕ»Í0‰?	Ææ¹vîáƒ¢ÝÇ!áò>Š±¾†¨ÖÚIP¦“ÊáV³G/ÚrgHŒÐl'È	w\j3pvŒ¥pZç•œ;„ßÊæS‘ËÞÚ_PLÖÚb–‚Æ¼m¥´9:*x‚¾È«¬rsÝˆ÷ñ«w<¹Út¼y×eqrøžI ¥Àˆõq5—~¢æÖü«ðpë’¶ÕÛ"îŽeëîx²Ø+L÷Ë'¾TÕ»79³sÃ^ ô¶>1µ0:±NÆ,~I‡Ë€ä-Oëâ±„ókÔGdC¤þCoõ³ª$RWW\¼ÉÉš]n¡x¹ç°]cÖ	ß–NÓžsèsÚKÃÉªäZm| xßA¸ ~DÔ8eÝ[Ç˜Swz™Ó&å$ŒùÝ½Ü)ÔúÚjâÄ.½œpWä ú	ãêO­lo+*-‰­M}Õ¹”‹£÷•
%S{'ã†€k`t]žžÖEÕ§PP¤hR|v`ˆH·í5bÕwQ©þ'Ëª¶+Ïäç@YÎñü&OlÓ‹|•U§£-¡ñœ¢ì;™Ìü<Ž-‹ÛÖ8›¿ÓÆ÷lBŠõë³	jÝÊ¶ÿSs¤+ìvÕÄ$ÌttYŠB?¯¶ž:ØCðÖ€”½M`3²ÓË½1ƒ3_NødŒ§»]6M6µtÂtdù»œwZäLc€‹pà¯1É½;ù8çÊÕÖò’}Ž|p|àŒ–›;Gêò¡¸'½®ü$N¤ÏôJ<¨ô~cg£âˆŽJµsßÑpIxÊM®&I¢£²>sÒkcìÀ½B¯-‡ñ}sêiˆœ†,ž<ÌÆèSå¼ÙÊ{µm¦Ò…5“aqmÍ£-Bë58Þ|f¯ÜìðXåùàyöóßík^±“nW¨Üt‰[ÿrÿñG‘ýb5H™`ÂÁ·_$“¥Óö7
û7ð\dqZó‡;Ñàñ=8­]Äô!O&Ûö¤¾Þ•.Ý\¯BSY±Ùâ½YÀ“™‰Kœ6¸<5ÅâPÁ{«ñ|G`ëp’Êüò¢þG†!œ†õÆ­­aš¼û°a=C/
ÎMôJ›F=¤¦”[ˆÇyáM"Âk#³á@åèOÑ§À‹¦>­‰<“(hä? Æ÷Ž}fûé¬ÆaÍEÝõÒóð…HmEÙ,0~D…“Rg;y‹Ž~ÁÜ ×½Ë†àwQ-É¢]©îÐêGô·"?*RÉ´$úâ"Ñ,ËÅ	ÈÛlJä?Ã]ò-Z¹ÐÖÓÇ¤,#9·–ÂHKŒ¯ÅÿpÐâðí›É\6U<zOdÝë¡áKwæµ™±/ÞËæ¤F\\Ðr ª *¶3Ýœ@Ô\ér7äùó,îjòcsœ¶Y_åžÊ>úûwììõÙõÅ%ãvsóˆ¸'Ç ÿD^9‰2RyØ@!ÝÆlèÛ¾ÿf}âàRß•‚ý–?:„¯óaxpa0{"E›Ñ¹\|‹–·V´ÄÑ=š’	\µ¡õ7Tø;e>¾;V—"(½y‰ÀÅÇ=óµÅJjµ¶Ð¯CÅ~8!×F™ßÁM6aC:U™-¤NoÂådJF#°‚¦¿†
Äø#Y;ï–;ò# ˜þ¤ú ô¥r”ó‹«çitŽAÛÒJßx¨~*	:©Áô~`À)Ä™Ä&ˆâD§HV+lˆžRÁ*Ì¨ÿ¼snZTq:cˆúXÇ.¥!t}ó~ Ž,‚˜ý'qª›è´Ÿ†°$Í0SCˆËMä›è‹õ
zÇ©%T$Ô¼5Tôbªj ³©Í|ºÅ©€yNšä“.m~d óÇpò^Ý$ÊÇ¸í¨ä“{Ã	¾¨Š¿ŠkþBÝM1)ü‹’||úÿ×T"q­00ºÍ[“@{„›0¡R ³×CWÆu,—qT»Äáð4i€|åRºãÍ±tÊóÓaQ>Ç{ìA™ôR¤åéÔÌ~8ØnwÒPNã6B—‰PJ'L­PbWÜž>ƒg»'|#º´ÝœM,+Åbh-aú<x¯ÇÆ)V—ëþÿ'¼³
NèS¤ï;ê–ß.ÇÎvL>ãnÉÞãH ùµÊ8ƒ+H-»ål€Åü¾b"y,ÎD”uÑ(üJØ¹ôãB‡EUDW•žh¥·ôk$:é¸¨¢¨ŸöÃÝ")[vØóû®é2¸È÷œöøy¾¯[*½¬)C&ˆÚþ•WaM\¤ûÂ€çAQMSih•·8z˜+ƒL"4r!vÏ1H
ÀTØ¦xþ'ž6ü¾öªÿ1†ñ^Û€{w¦4Î©cŒ†F£·äF½!œÁývø»ñûÜöøœP
Åþîº9áu3¥°9Æ®LbÞñ3Je`Âå`m÷V¿”Än•A$ß§#0x&wÊ¬3²<[hr¬F wë?‚;Nû<òGÔel/'ºÓ¯€bÇ­W÷1-‰j¦Ÿó«˜Ýj¹êµø¬ÐüÑFèsLq™ä÷˜V¿ák{Ðp²JF
äËð/= Ï4Ÿ&·ºžÏ5ˆÞ×³vsé<&¯YÝ#{HíûÒp«¢¹]M®˜–__Ó¦­ð4	Ä)éLK™~­7+!´Z$ùWvWK«ßµh 4\;¥õû/Iì±ƒz·~‰”©Í©ˆåÃÇÏf³mû©hX„âlÒFrÐŠ€cÉ£él°<³_˜š¨Q¥!Å¼™B	W…L6¥ç,$ƒ8f´05..%eñï<¿Ä7»¹T ÷€}UæÝ˜I½“6»vv´EÇè%,•ØÙ•þ²³;^M)[~òüDJ×B¦#FŽ†²¢T¾R{NÒZ¨!ÉBH!ªö´pâLae´ Kp‹íT¨o"W•®@ú¸¹UÎGþÜÇ¯¹×”ïŽ{*Þ†|4Ø2– [;ÿ$mÒ9 åúõ0:¬›nZJ–àñ§ÊzÊ¥„wÅ›ð˜¾úa
ó-þ€oÏÐI¬6ÕIi³mrß5‹I ¨ýëªò)…ÛÍpuÀ9dŠÏ,™hÊòdÜMßß¦…‡ÊpL·x‰(zÝÈ~Äé+ë$÷JñD´X!T¯KûÚDëd·!Ú`¢ïÆß¢|ák“c2¾æ¹ÿ?<µ}9¡7F˜mO¶ÔÜt K&ðBT{-Ä‰ù-sc—<â}µÒÿôÄC©AM×è	U!"Yj\â÷£ÇÃÊsbAÙµÖæê\e¨nÌo»òè-ŠH>øµÿÝl¬üüÖã
_U•6Lr;ç+Ò½\÷i-öòo†'î…€RÒB]#œÞË¤²˜Å•þUýc¨XÓ˜°˜{ÎeA5ß³B„ÕwØfYqƒ2fÝÛq&3}³8*Wf 4ÐÝ×5íßì§äkñ»jèrR²Ý›˜õÜ#.¡)XÀáI¤ž—ÓcfÃ4Ôû³ûL£ßŸ€­ê”Êë4kh{]¤!â+Cú[<–^dÅk!£Öéyë¾ Ü4©p%«D¿8JùtŽ£D¸¯…+²ž§ÅÐ»Õë73œ¼"YœÉê£O]‚NbÏ^ž"Ó\[è‡.C)¯ƒUÀZé ðâóå< ³“¨–âš|êFyøpšö[¸ZÀòƒ¤/êÍïyOç¾-Gý‰[<Q‰ÃTØ(ê¦t|P‚Yêö¨JÑOuÔóÂ*¾ùëDEx°\×¹IË—ó(ªg`øtíuÓ
k'aOÎÖo2L—!6‚*ë9#Òè ì–0Ü³0bdtw'³½Qï$>ONxKk
9ºþKhþ±¿÷Â£‰ü`fÿÂ’7›'mó}{«à2SÉ¡|Ò¿­ò95)?	}5î=VÖ‰æˆ‚(cmL)ÿ«+¡,æt‘å¢•éBÎŒâ†8°Ø¢.¥“9Û†#++Ìô\½&Ä-egÍ_³¦ ï	U‡è?Ú(K'µ"§:‘“|àAÌûò(à~–j4ŠÞš·lDŠ¹÷(ûLLÐ{$\
bÜN×ÚçY@"b’™›Žg>’zQr‹Hd3Ã#R‡+µFˆ/ì,¦àüån±6Xdh€ûÔlN€Œ`Ë†á35\±âž°"Ç÷ d"B‚ƒ=ÇÞ†º7³¬NCB2´Œ¯fÂØÛ™Fv¥±Æ¢á	õ×P’‚¢ø‰ü‘6²¢4Œ^Ígåsäo1Ð{:ëy¦?î*—$û]^/*£µ>}ÕÚ†$ëê ’ èJ	sÁËOÓ;Ûmâd ÀÇ¾zÃ0âD‹aòÝçO>€§š,ÀEeïf{Fp<3Ïÿ4ûs%ŽŒY  <³)•Û\^Ô¿oäï##Zå­Js"IôºÚÖ$P’ÁðgÑv¦]8:êÎ]ž ÐÿûÚB)Ý‘&+Ù{åFÅü)%V)|Û1Æ{l›å{«d~V’!&Ä‰›™´øÎ´ÑØ“ã¸—0ý­Wz·9þãvÈùí{ØP…MxÜH
Ôé‡C
-	G{Ò´µÎ’Æ”y¶ÇŠT!–=Ã×ªÎë[>°Û¿Åw%KÑ7:?å u“Ù`tQß@72¸!ÜRà`L‹œñŸµCèHÍcNû¿‘GÍøn˜DAI}iÀðØC«“¹¯jJ÷åY>XøxG(7›§è¹j?%Èˆ ù•º,ª°Ï+µÞB…óù¶—:LA@Ð’sÏl:²¢Ù— ›±¡A,ÐùK¹jÆmùw°QàÚVw÷â¤GêþzÀ¿¢ÔO$Ý‚÷ÏY½ÝkÒó-û#½ïÇÏ›®þ'$¦°/sSl>øãg‹ŠãD¹÷÷ëèÙ+MÏ
‚ã±¶Pi«gã²Ë!<qŽ=‹ûm°ú•Þ·è“Æzš€¯ÀN¤W~£)Åxla¨_HPÉšª=”[åŒ|¬	½¡Ò3ŒÌ2üI®x*îÛð5Wòe·sÓšÐË¨Cd§ðZiOÖ8evXY^êL$_q°…¦©øž¾™`àH}?ŠY*$[9Òt{Ûh!ïnf&%"¥Œ@N'XJ‡Ú®ýôÒ4ßAIò=.çx¸ƒw¥1¤ÌLA¾¾®Œ¶–å°Çe†ŒáÞGE»øÑO›Ì¥ªw_:éHÖ§ÁävV¨¹Ø^ÀK¹¡¦²Çm]å	{ŸF &­Ú‘G`
~07œžœX·oÍ¨õ 8øè®åš…Üi)Ô&
SUluûöJÈ29Ù‚{°ÂÍ UåÆ˜í
¼L‘ÁÛÅs·)x¦Àk¼·ÚèN9k.ø¿‡),ÛTMß5&‹ÖÃ‡Yò±˜,²Z‘Õ?¬^°ò+C–€îó <ù¶ûúHS½bbš‚8d¹×û«oÓßrw(Wû}ÛhÐéž®–ý
[¼¦:izÆà¡ñvéÂz_¥þƒn¨¶C¯å¾Ÿk+zìÉíLÊæ²,Ùàÿ¥B²fÒ£ÿ¨Œ°-5í¸­¦Z ùÓ¦eå Œ<®²ëè3…§ÉCpÖ:§€dÃ4¨÷Y±5¤«TñBËJŠ¯u„5ÛèY‹i¢M.‹‡	€)®×&Æ0âÞõµÇTûsÃÄ4.~ÇPß›«yˆá–®88pUÛÛ$³É&D–Sù™Yë0û¶âŸZÅ×%q5x›<Ô¨û;ÿ›k‡jð  óþôÝÜË„¸V³Çæ/¨1SÇªªl2¾@¡³xYŽ!Î®­š)MA‰–žpÌ‘•ƒpýå1’¤5dØN½=ÐôÙ8®ÂŒë¬ÀÓ;1:5§ƒwIh7èÓÅ±³]©ìÚ&è6™¤îÐ‹ÿ)µ’C0µ‘MÇ%ÒŸšº±(Ñvx‡}ÇùSap2gGÐ@Š,}›z¢hE•è¾“¢$Ú†›h\ás_È×ê[¤/jÃ[¼1þ1OÈXMëf5®‘_vzA}l>¡ŠiOa”ßb'ßy~™ÕZðêHÄwÛ…¾O_ù k”EúÂëº—0²÷
[Lý€ÇaO¯Ö\o0E™Œ3³mñTÆ<¥EÙ¨æ?|×RÕ~Ã±ó§'§ù™]&˜ û?€J\<)pØ†eÀG;·WŽì1ªc^_D´|ÕSNÍP\è	aþo¨–Œ®<ÑíÛu/P>6¯¹lóV{œ=Ï={6¥iü²$|º~ìÛÇuß`:Y’÷c¥©»`w‚¯tÜMÄ‡4åüKþl
‹÷Ùÿ÷3ô)Qáüy¯­N³$¾ÿ+c
}Ð ª¡í‹os_w´õ¥n—ìŠÍØ%b›‡¨šk|èºû°®è+öñ¨~àÄ.Ûÿž$ÿDL«k˜"þöÏ"Ô|}f§dYÕ{©Ã¬ì;ÇP;€“¨sJ|î¡ìé&Œ©J—uÓÄOŒ^O¾N¸B«èß¬5ê’]¢TV;u0_kÊêP¦>åCp¢ÊIåÞEmÙº<­öQô„¬V`@$F9ÊÂÂ¡.­j‹¶Ò¿Ù G.…ÚŒzÛœÑ[žeœ§â=”IS[Š‹ÃŒÓã‘Èmô6ú™YŸ‰mÊJC‡«%¥F‰ZÚ½ãG‚!\ÞÍøÍ¯«¬µž>Ch˜ŸCSÕ¡À§¶Rÿ“Öz[R¡/ÐÊ¾WõZBžâDÅe(”E¶Ì® –	¸›^ÔùtÛc>? /kõyµ"‹*—ÞÌ˜ôdõÏ“U¯W©%ß¡Èæ’Ë:(S}-úG÷Hçª5«V?˜‡@‡5”· ƒ#µjÝÒ¤íêKQe¨ÆÌÀ8uÅèÒqùÁNL‚/‡/šd£æºæ•yàR›Bš•¨aK—`QÎÔ“Bûâ?ÖH¶~»ßŸ>Íê(zeI=GŸœ*"¿BÊ-Ë8Þ[Ü/ìržv0MOfª}\ñÓJ`‘Ëž,Ú Ize©6c±”0à5s¬+@7šð¼äõ-—Î–Ïõ÷ƒ- NM71K›}™^ÆôÌ&Bñ-”»®†\YöQ±9˜GÖóDqy…2íòipìÜ­ÚX©úÈ$Rt¡$äÿ•Ð\ó‡ª/")zÅCØÖ¬°v@çVDV‚së”kñß3)'ÄšË:9£kÇªõ9DˆèÑ¢°Ì9B•ÓøÀ­ÞB¦IŠç“c–Ý>/gfÆˆrg/½”à)˜,3y=ØI¨µ…FƒoÐe²º…NM-HäHf™ÒŒG¢ÅÈU‡­¶pÁÀƒgÜ]D®,çã þYgwgßÈéºû4Ïh”ãìf/òÆÇ0sH#'f¾qŒÔ:ÜS• Àw &q_Ä¦£:DŠÈËÄø}fÀJ<¸ÂùW™Š†Lžá¸•Ò—×°8çà“¬Ž³©VŽ™¶@‡Ð^?9fóï½ïRÖyß‘©™wÀVáÍÅ‹òw†.+£ÿ¯Mˆ½²9æ$4YšvÚ‹&CÚ±B©˜Bu:H¼•
Dà˜¾¾¦Maªë"<OØÏ©Úø1¶iJ4›¾ƒ¬kÅØz<2OÍl‘¾5ý‰Å‰)J“£Ó[Êä~´¼5/"çC^çüµÍÏ¤ÖÝ@‹ý“•zL&Op¶…¬ùødú‹Û<hsQüpÁ1eo±1­&3„žä7°Ø?À{(	šÜ]È¢ ª‹·&<6 Xç½sævÙæÄ½(«ß€JGøçKÂÿ,´QA}û¢
¦¸[æˆ?Ufö,žGe¥ pŽa_Ï¾¿WÖ0î„§â2ÊPFNÓ(z¡Ö${iœHoT2q¬IÇÓùÂzòYŽ"ÑAŠ<§mnéÆ—¥±Gš{ä›é´¾‰µR9¼‘ì³¶Ã…ºà­¤4Ý(ô[øÒdœ²eóWªÌÔ£×©JÛ‰yPs¸àÁQÁl”yù[gŠè¢¯ù÷iûßìW¦¬òÊ^'Ð»‹©ÏhÿÈ!€M{	Æ¦C)‹µö´ó²ÅÓòè„¤s%þ€ ¢D—R4êÀÍ¾x‚:ójŒsK\ñD2Tf
çñ’(>$ßzÊUFúÉ•equBNŽe"#A&_YygguH¦h-2›±éãFx‡wû¯_-ÛÿEs¶62îƒ®¨½@BQ£[ÚÐf$„A_s·¹6ÇÏÛª~]’UÇÞÝmcÙ
ªÔ>«·¡ÂÄä;ñ8g¶ÜÈºÙ²o–pfÑ€ÙÒbc\šD¶ØŒñvs«œ›`ÍŽ¶ýX¨'k£íÊ¯ë¬{Û}&ÑS
éniú:õF™÷LœTºbQvˆ²ÕèIªÍ&23Píœg~›<ZIöZ?|–ò&¹Ò¹œtÒÀÊ}¦—féIoÈàùWžìê úM3Ì.‰£ÜâÃ òÁ{€èÙ/ãa½·^.‹©îæx#ËÊE¥¸ŸA6ÿ¯æ\+B~Z¹Ø—ÁÄæÔòÝð©õÖ*C^ÞŸuâ©*Y®ˆ£Yi‘•·ºd‹ý7§¨Æ²žEþƒ>¦ YómIúxàókºUÄ“‘ÐÂ)›Ð­0çôRø|šÛ#Ž‰ ›\Zí ¤ã!kÝ4ÔSb¦`…·6ãþ”¾¯¡„†h‚g;  ‚­Ù—-.†ª×x:-bæ%ko?öÂÃ‹µPX¤î#ôªôSd’ÿ;	Ü_¶KŒù “—Ío1óqÄGýJ”u©Bõ{[7‰Ç‰ý„pŒÚ¬ ÍÈa@©Ôd?Åv…ä}ðý¥;Û½ë^ÏIõ
q JîêR­ígØËmÕ
ßwój„ÎºÇ¡!ç âÓ[ÁyUËiŒ@à`ÂN>Í;o'FÉ±¥~Ô]]…šžŒL…te6//\ m6’¤°7‡¼ü¬´7»¯ÕFõÅå•TÁ¤õU‰0Ë„£jœM5ìå»
ìy¡,8Ñ»ðbiÀú*>¿´D+«•Ð¸#˜_Ýa'×£IKßê1vUÖÔ)#6ðFqßªÝø¸QLqèêxC¾ÛƒÒ‹1jY¶šî Ð(âS½˜LLHXÊk‘D;ÊD„Hy¶‚ÂÐ#ÏºçÄ„à°^ÀÜ7|¹&R<3<¬º’kH¦Fý«Êä^st’x8~G9ùÐx{s3t#\ÁzBQâ÷U¦É4æL*CS÷¶v´tnmÀUsm¯ÎFç’FË¹„¶Iþ2â"ÙÚþÓ4áé«¶þÚŸŠÙ	<ƒËH…ÐÆÀ!·JÒ¹XR/Gd[œ¶WÍ^\WˆÙòvº¢lz5&¿®EVÚÌºÑœ²DôÍž¤y£èâ½x¯±se]º(“¢4æ£W7Õí‘êhoZ'âËÅ1HÁ5J· ±C"ª–r|Mi_‰‡Ï…Uâ*ôëÿžñS'ê
§}³äÄŒFT='<SB°µW¼	ÖÏ±c#ƒ–M£N:d+»+©VFiÆ[úÒ ÝÑRy-¤…©8Ðµ¢hg5{¡4`ÆÂ–ìyq†X¸GSž‚Qþæ®(E´:~èùdTEL5C¹Ùãzœ²E°”WR¤„Øzøèe¦öéÄ3Ží§<ÁÓ>íõ.T’Õg[h1`šl[oWÓzé²YÖçUáhí1NF™fam’MšžÿrØß´¿DàX(=í±E¸è<Ñ ô‰¼Ê*ÇåP—ÿi›á`ÄR=£êIëfvÐÞà™íAm—£³>Ð¡äXÉØsÜ.æcL$ÓÓ»*¨äKòÌ›J€É@Pƒh‚n Üpvæé¬â	s~	`¸º)IAËŠËæÉ]oÚ¡éÂ¹h†WKòÁ¿Ê@TqrwfKj<¾c‚£:[{´Q°†î)9Ý‰Sƒ}D¦—ã¶§Ô”¶ç!¯Å»	Ô;Œañö˜O’î€¾¿ýC$ÃÆ\Æ#Ù¥#TU_v6ééÑ]²ë]-ŽJoy-É3¨<¤À1b¸IojaŒ¬ÍEÔ·Y1…Å.ÌZö•SÖgY	ÁÜ¥ÛFÜeÆmô¡ª]d(à-¹`½Š:;Õ ©w°N»Ò„J#y÷ä²ÞC¸Ë9½å·±(‰4)t»ŸƒÑ}¥!ïú¢.[¨¥[÷Éå#^782Ó¹E\F9zs ©sûos)–Æs[þ>
â¡˜Ý?[#?¨ Ùà¯(L<Õ°sf»¹ð¹ô2‹T™¿¾ÃÊ¨©^H‹;½ÔÎ@
ªÅüüósU’ó={=WV!<…yP	•ÓÒ¡ÆO”ésn.xT)cÅ°‰T¥L3«»ËÄ|ñd8DsªŒíÂÐY2é[]©Øm«’>ÞúãSdYë×KÊ<Æ«kŠÀÄµ`ž =ì,ÐdoD§¯ûªƒ©ø)Tuü_Ñ2`¬h-_Ñ÷óKiAå€•ÎáI34Ý¬êvRØ{ýþ'+»>Õ­óg„æöÿibáLÌ ®4ßâ	ûCâ:ŒXBGˆó€»†^ù
Æ–‡Òg
Ú\~Óúš Í~z3:ê¢cQ2jwàRæ©¯ÉöM¤’}ecJ ·jÃ’Ü3 ëöäÝ 'ü©[{mâ–GŽž¿c¦§zÊÊ!bd°lªÊ§ñìXË¼eE‘0lô ØÒ ®Ìyh*äUîì*e_hí×:LÈVû²pÉ[ím3™áŽ^ Þ‘NpD„Hâïµ³<~÷MX®¦–Æ]sL<“¥‡Ø=¿ð:¤)¼Íx1ÜL…GÏR±íçP¯•zp‘ü[Š7GÊzr_“h#˜ýÂàr?V_<é}ùHmcç”x„ªÖ{‰§¼£L™¤+íÀËwø¬Â+I}zECŽ®ßÜ¿¹«éªÚÜDXTÞím‚À‹7Ag8Û°õqÌÁ‚Fþ“·±8A—.œ‰œˆå¦ÏS&à¿„Íë<€Â!q[¤ü¢Ú¢y¦Võ™CL(	?½Ìxÿ»->Iõù®ü ãŽÞÛ[ó¼z©o¸—c›1}6Wô©ù(ø'4ªËa§B)JaÙK«]ÄI¥Â‰IÝoTî%8S`!Ýc ¶rSéœ~h™§¤¿ t¤9°t	8—]bZ_;:”Æû­éÆâ)!ï[/,¢„¨Òý Wµ Ô1¾u2µQO¹Xh©ÕÄm†æöWjéÂÝ\5m’-3A=_0ø€¬
7¦PTS-£Šý¨îº¸œuÈ&fßZˆ²]2‡åx®ª-z=¾‡% -ÆNG·4évÊl9p•œZÛKfÂM<GÄÓ#Úü©wÚ‹ DÔÌ®+Ñ5+ÊüÙ	igY1Å¶ÍÎùµ¼ q,=áR?ƒC,Ntðªq–
JJËa.”ÙŸë3“1¡¡Ê(ŒŽÈ×¯ŽØ®°˜•Lñ1Ý:ºT‰?~j€ˆL"&¯3È÷ªâ+“Sb9a"91Àõ)’†¨Î#oºèDV1ÄT{ñ¤À­¹BN‡æJë²[×Œ‚N¾"»«µÃt­ 0ô	}Ý“ö+®z¸†	d´«8Æ²²Ê/ïÇGÕm•×ÜøÕ›„dú“¼6‡\Úñê34í¶‰ÖwBf‹#ÞR‚ÉMâÛ¥î« ÇÀøFn1ud*|°óŸTŽÎFm*š÷Õ„¼nÙj›­¹‰R:Ï¬lù}®{Ô«Ë²ÙKã€¼íyñWºœC	ðøè"€Î‚±÷ƒ†A*Ó®f~C=°XäÝ}òÓW/‰¢¤eEŠ‡Ošÿ_ –ûiÓ97	`àWÓF	á^Ì·UæXn*)û™›hÂC%¿úS-Þm9lèèšÀþd^è¼õ¨TnclN»Üf¯‰DŒ@F©Š™£÷Yèã5#’^Äðo+¸Ûñ2¨íBvü©4D¡³»î	½ =¯>Ÿä
aã'˜*fûP'+ZG¨9	[¹êv%!cƒ¸ÃÛá·]ÁâÈ×'Ó´”h1®-ÅFÖ%E§Søç)uJÙ*ÜçÜ (°áHü\mØÝÊµë‘M_^’@µ9PV2+Ï·mèoó¹ìs€4wÖþW×ŠÅ§g°’™ Ž6H1ï1‡¯Iµ‘i’Æxr‚çŸ!ê—«ü[­aÎdBãÃ¸þ( `‹Ê¼[KµÅâÜ„òz¼D„z('¦i&ƒ.9¦H`}úü¾LÙ
OiýÜ“1)ÍYc­"Øá¾–ÐÇXñy•Ý)=}Ñ¹"¯fÝÛTDç´v7‰6ÎTvn),ì”mÞƒk;ïlgX£ÇÂØ}ò¶
éðßg\%+È5*dØp%}¿+EñË(¥I;fàTK8ÉÐx•Ï‰EØÌc+	ú(;¦OtÑ·ƒ¿žµŽ]½Ó Â¥ÆïFÃ¬-íz¤F¥œk¯Ö@þ²ë½?lÂ4%q§r|j"cVÙJ!ƒHl¥¿ì;‚AñOpo—àîRZôÑF7(Ø‡ÌbS%eúÃÖú§tDn:ÉCÎelâÿåQ'ˆòjM' _kh>ø4€)…&iðt“/®¾ÞAéWË—Û	|0!…ü‚óˆ¶ý]XÁS‘m„@0ðGòmÝs½g¤°¿Ì ŒhÙ£6‡ÿ%Ò©ÇÉªÜ‹*KŠzÎÁµäéÄ¯_ö!”ÿ-‘´Ï`s sÂb;Æœ(\ÜØPÓÒHºàÖ*¾¨ÞTý¬û`Ö
\ž%p¹‚ê¤ÉT›×SLŸ*{CÞŒy	8½Š\÷l·™±ÚåÂW -q_xÍ•òž?zf2Nµ—Ó#ÀÆ¿2¢Ž@þ~ÐòÁ«€P•ý#1CëÏEy!É~×CdŽÐ{ÅÓm`Ù>ßŽÿÓpï®…•ËvI‹Û<¯bü–.Ìž½yµâYª$-Œy¨ÆÅÚ@a¿Zc]Ù±ÇhèÛÏŒL	yzè æÞyãÊêc_µ;1ÈÀÓ™/DJÕ%LÂÞÞ@¸àÕÍY`ZS,s+þ€-Ãc+„QrQaFèª}ŠÌ|õ)ÕÚƒ çi'©eÿ¦ï=¼ºüHü™Ò†–É6_JÿuÓåxrïXßÈcQ„G‘›@Hê*EƒZÔÌ‚uû°o`ªò±Ÿ„¨ïXýÚÖ5aÁÌ7P’Âë-fCëxÆÁ½yêâCÂØÙÐUO9tø¬¨eÎ°•8PUl¿/«b'%Û³ª7$œÊ¼;VÄÎâ81]Îó”°m<ìyAÀü#ëÐ)ä6ÝèíŽF¹œÓ]DJ¥Ð®kHMáÛKÔÿ£–üXËŽ´,ýÊe+F…º®gˆûåÈŸs‹oE8uáçºÆIÇa†ð‚¿[ìÒŒ”»Óø|Áxc6IV›ª¬F‘øuüÌ£K¥|F½Òõ»[â÷…zß²çJëÀÑÛî¤¸Ø3ùA,N–®åp¡	“C¸‘Y{Å¯¿&Î4âf‘5_(óV›ÕÁ£àÆÕøˆb‚)ôCe¸EwÆYB7Ð‹ª·H%¢÷Ä ïki”ø¹åñŽVŸ(?îÃ«Ÿö·0±¸q5ìµoHwú•-b$E£‡Aæv\Ò¿ì»æ–ÇñhñD9]Ëm¿Å™<ä!6œå·œ£gëñTÐ¦áUþ3E*7;mWÒOÔ3ESÿdÄ%?ÕÅ¥ŸuªºéÉÁ“×A;*lÑùgÓšjÎàÊëù´Âyª0Ã
tYÑ4oÔ»_Í‡x„
æ£Y™gÄ.F—çÑåŒ½J‘´ÕYü€­ëîQXQ¥í`@)`{¢%àéšÌ6`¥q¤sÅ®
L>÷‚ H¸ê^/ãKëø!…xZ”eAU4çO¹VþJ *§ÒÔ=Ú‡ÕDBHø””< PÃŠzÏ›ò£F~ÜmlŒ¤*©³–Š	|’Î;Ÿ†£‘!m!Šº=‹ï 9×ðî˜ºw×@…«}¼¢©Í…,§=uÇ`ãn‚Mƒ´êf 2|©]·ÃdµpqÆæÓçéÝP*éHntÑÿ	Å6á¤ñÎ@¤ÿý\à«yÇ‚Ïè”' ÀáŒ€–ÐŽÝîRceŒG-o_yK%ã†ÇÀS…¯C¬Ç‡iÍb7ì–”)‘IB¶ÐÍC ÿråÎÈ1'n¼ù½kí”ñ©cÚ¨ˆ;\…Þ®}D­ªªâÿ§ì`nEó9/ŒÕ¢'åxoáÄ'y÷ýt#›ÔT‚é2Ûa	OßXå{*v,Í>ÝžìÈ™Ën”Í	ÁT³&+ÎòRªëÙÕî˜å×—Äx÷/®Šaz¥2mT“"6€P|]kW¼§ã­œ1ò¼ Ö¬º/Ù(gczÚ´l”?¨Q­‘-ÞÙ
Â£Ûò$·Ž{º>ŒÓ(ú—+¸pC{ {Ý)ÿ;ùô×-)ØÅŒU€jä@µñ´ÚåâEÌ0¼ªþÚüA¥ÃR¶k}ñYÐ!°Ïo4wÇ€t6e”&ˆ%‚˜4¸R[Ñ.=O½÷ã)g¯àP\¥Cy)µ
­ŽúµýÛî¸ä dJËãÓ™Ñ²E)ã«;™,òr—âC‡Öå N°ÿõVYeBð~4ÑçÓ_ÔŸþ„=ŽÞ<FÔ˜GD½­>r<4røkr9Õdïú3í=
R%Ïé¿ä•«©¦_‘OÐ;5S‘•Øb}ÊEú©(¡ñ^Ú}úÒ%³ë(ç/%‚d¡ZÇ%I^Ï¤¬™’w^WxˆtfÙ½´®ŠäÀŠaºÃ²ïñð.3= %_[”£€]ÂÀÏ	'ƒ¡¨8Ñå5t$Þ±ÎiÎh5(_Óî\þö|Ü<À·Ë„¸(˜;•	Û.U›©Zh’J”ò ÚúÜ¼çÒi¤fé‰“ÂT½Å¬ €Å°q0?¾@¤r3ñnoe>²–kóKj»ü8V.6€n)]ß’új>¾F&rƒðædeé;ânFIr\EÜt)^¢T
¯~¯F«'!öòü8HKÒÉÎê÷ïàŸnWnè=íŠ.èˆ;>ÑûØæ™_­q|Úgßáµþ`ùbteªWC®?FaÛ˜ÉXÄý/Vñ¢úïc”Hm]8%.#FBm®\=­¿E¢öa±¤pÏ¸QçS1S†öÒru‘Ò‰4“#PÐ7Þ±ì,>8lò½P)ÀPB	|8—ü6|‘Õv¹p3ºeÜè™ð?µ.·3;xÍ¸ÀÈDZöhÝøFia5F–×ærÓÓêhƒáßì˜·Òž»„‹~‰½þž¶/ÒqrvýIˆ·–PŽ@¤K=¡óU®Wô®‰ Õ4¹~PB¸pÃb¡Ø‹^Iüƒ*Ä·:’ŽGN5mú×€ôYê>6„‰Ì4´yàí¬»ßf÷à[XW¸”yU|¸ÿøÎDÀ^Î#›ß×ËB¨W•VžÛr§z9œèL)EB­½o F^ê€§nP&Utâ£;{zS|öõ÷ÄÎî~EÍfðŒ‰ƒ2ÌÐŽ¹†D^™.ˆT(¬dÅê±÷È#ZMí´K‘E4Ë*óÅ{®ýd£Z‰·rÍ5õuxc7qñné~[„”çZ|ûúvg$ÏÞUŒÙÃ&*Ìœß#`Ö‘G0+Xæ£)œà:*:;+Ü@âúŒ…¤øõ©è&JV\ÿ)"6µ“m\æ fŽËŽ;íeÂ›‚´¯ôÄÄKtåºcŸ‚´;£ 4Ô×C“Ÿš*ž—%7i…?£¾Bœàç¸6xm<ƒ€IÂ‰rÉh{ke.e€tò•5ÎÍµk_¬‹ý/ß°4±ÇØ¼c÷×c;ŽíCH-–U­·íŒ·ýé…Ã¨×r^qöKÎ‡¶kðý`
›fÔq({ä4ëƒÄªÈ%Ê§däoë39é7ó.¨îán55·‰`cÀÂ´[n©¬uø7dÐ¼W»5ãÌ§\ÛƒÖ,¡ü¬Žo0a<éü˜¡èÞÏLµì¸pöL	ŽkR®Ó½aúÌä|ñ†—¹á.Y£Õmbb,O†?îÞ¼%moðþ)OÍäÃ~Ò¼y·¤Yb@2µ«¢¡}’ŸV”%5~Ê,Ó†]é'Ð¹1ö”›ô©àËD¼¶SâbHÏ/#žMîU8¨1ßö¡_¾¹qq€1³AµW/÷xàáwã_g¾A€«8þò(I†EBëœ²–VúÓtN£#ì¨9ÅÎp~1³d#‘6ª‘ ÊŸ[/ÆAaøÑ}KT“ßé0´•9y „Ëæ/|$<m¦	•ˆcÖ\åMàA-F4ß»28
4‘Â'½ÊéðftQ ËÒœñój|rç`pnÛ§é»b¦ßŠ8¡üæï¾¼l89`ƒq:¯0h\äá–¡DÚ37]Ù|-ŠZá9L‚½›t9¬Œù_kÛiž8	Î$¹U$vþ¡ÆÜ0s/	·‡dHr`å?c:pPjHÒë¶M›­\YÏ]‰ÂFšÆô z74¹>?ÖíŒN	)šw.¿)ù¸ª¢+w‚ÇÖ!_GŠÒ÷Ù„Ã ø¬ü£³T4Éšm4kìÚú?¶¦p7Ã¦xË‘C§ßŸÊ1- àµqžë±á(#
—§8¡·kÜÙB·ù{í[Ý²n
r
ô®>w•ô½nÂS>©VUeL;èvV‡ìÓpA2îyì¹G¼Ó.óBXnþ£8ûÞk-ãè$Q·&•¬þ 3Õ4Óîåcq¼àÿ$m8èƒ‘R6$’‰Òè­šG(x[†k3À8½­RÓ*±iÄ¯k¤´WCÐìöàÒk~¬ëh&¼´0Ëõ’áA›Áï	tp:H{ißüöÆ-ë
{|IÊ<öö÷”Æsy6•Ç]'½·Ðú êý5ÌË4vFà	‘Ã½Í7š)d[ò;û+õVniSÎ#(¶uw”ÿ’Põ‚r*Ø±Û# c'LÌ0T†¾‚=ÉþkÊµÚß€"^:o"²²Ú]?_|h m?´ÏåˆuLÀo ~½Ó‘IS!p­M#­]kÁ8ÔiN}Ç ‘\PT_VÚ¼è~aÙ“v!?ÊŒöjÎàÂórÒ`¼<%x%.”[z{zÈk«»<G…Kä¡ðý!'Éa;y /Bk°4$_avÚ¿døývAI;ª±ø=[ï[ºôÁ%ê´¹f¸9Þ'[Âà4¸ÄÊF+/©êA‹ãj@Ì^2Vr$°HŸ…üãñ½ÿˆCòÖP(2X ;óÚ¤±“î¨óê‹b9ncØ>áyi1å’–·AÎ=A &è¨ÏâÏÜb¹8÷M}xG€2yú{³ÝdX«À)ìMn2uãæX8Ô3Wqê`…«Ü"‹ÉõAxâÚ[µÌÏÒçzÖµ4<ÇŸ¢_¾ýÄ.¥ÝÄù1Â(ù1s÷9ÛW™›eâÅŽ#n.|có^ÆÍgÉÒµ	ëVñ¨ka7TDEk~±¾ažle¡+us0ÿýòäàý¶ìD<(¥*ÓZŒˆY_™óÅðú`=ŠþÑÙ‰TË‹ƒÄõ°p+ ÉÀo}úâÂle•œ«“š!ÈRÕ›N—Û½Æ“¦ejA)DŸ0èˆoöËˆÀ*/ËµdÁÛf¸}`ÿ¿6žèž÷®ãCšóTËMÍIÔ'>îvg?·v°DQ‰îÐìÖ‚N•—dkçS	žÂ¼›ç¯<cÿì`‘dÕt’…ìË2ÔÉM®>vµŽ›¤”î¸$ós[MŒdãŒhÒ
¦/ñJ‘)øØ‹9R/Á,Œ¯ÓM,E~¹4A¯«µÜ*\š¥¿¨v _.z]"WG5£CŠóíQ5ÏS;–-ÀªÛ›
úSµ=óÖË µ¶÷Ó:ô–>è8ôJ¤â6œIÍ•Åb‡7<_$"²|Ô©"2îc^?ÊÏÎú×âÏådæ>$Óæ`}‚3	J"¬¥¥ÈƒG‰ñ¢#õ«GHÛð*‹ïÉ½ákÛýÛÒÎãYØ€Ÿ«P:Z[óP÷î»m¾GÇ´$èÌba³kÚµzˆ‚…êä´ãákz •£il¹A~øºžGU²ülùóÛ‰Tâjür¤)•D(ñ«1nsí\¤bAjLŸ`'EWƒn—¹”|~pðA<ÛÉ†Úž÷Ú ò†ËˆØÆû2YÎÚ¨¾áªHKlœ6OÈV}1†¶ë2—ãmqN±yø‹+¢ºânÆÎ—Ü)ßs2•¾ ûF=ì§«×Òµ|Ô:$KõÊË*$v¹zÈMŠ<¯ö=øñû€r@‰b€ ]ø¹¸?Nç,¼=v¿íÒ'¹Q"Ç¸)<Ûû°Á!n9Öî%¢uäÍ ƒôäL¶"ksb^Ÿï4Ù×K¤Ír);(âéaCkF öÑuêdê=¾;Šs”½Cwä¹œ|sq|mIŽØ£×ÍP•[¤æÊY¿­{^:¦ª]¼Z•>ÝêÚÓB
Ô×M_,ibÇr£ñ—-yò:”Ö¹Ý™€;i‘”€Ú6$9¸&Ûc`sÚK…^Óqä}‚Áß:øÌÎmÏtÐäààê,;®HYþbhmiñÝQVxLùné,(øŒ‚|¤ÌŠ»äp•ö?„04‚}ê¦¨¶ÖïªÞùÙöíT-P?·Q:•Ö7–Wïã·GÞÕ÷ã ˜Ÿ2Ý}1pÓ2%B˜È=~&½5 /jñyHÔ¤^­”g´îíÍèÿ¸ìàï þ¦”L¨™al¤¾ÞÈdµàÎå¿Az/étDÓš²É“éJÒmûË0-.P€WM€X aµVÄp´r´PíÆ*ËÿÐÀ©*C"­
8db€D/H5Ó¤²iõú_»ødCß‰¨-mn¼_©jØƒ£ÿ§\TÑI‹¨ªFAñÉqþC1qØV¿(>
*rùó·q)OÑ]ÇœÜÑxÉÑÑ¡žC¾ð¹ñ½ù3Ù
©êEqÇÒuBÌÖÄ|ù<g§âHJßüŸ1=¿ql×f(Ýª€ïI,ï&àþjàÈÎÐdòª2^_LË	ÔqtŠ	d>B
µ¾~<æwsûÿ9êZýqb\9Ûyã„}€x#‘!ºï©êD5ò¤
oÑúë+W\~V£8¿¢ì›ÕUO¼|%W`MŒäÂ89iìL5:·¸ýŒ1j>lŽÞˆêËÇý.^®t8›X ¾â›otÐ\Ä®ð%v¦«í©L|éqò<<– ª7y3à)å•Ûë0ô	öµé°Ü5Zm	–›ã[LOd±H$±ÞQehŸê*4¯&-¤ÔtjðEsozt[v~¸ÿÑ¯Hß)7Rxúƒ„Ióëz’Gg.PNÂ7Ôùƒ
`
o*’;&t˜	$¿¾¼dÅåÚRd|_8Ó™4ÐKýF¼…ºOÇýåt‰k«ˆgùc–-Røœ˜˜†6)Ãû†=ŒÐª‚~n;¥)„w´í_q‘sÿ ÍKf—GÓaˆ€Î—LSçŽÈŒíÔáJnnVtÎ¨:-”dOø“G×B‡Ö¬DWªJóWj‘E	úÉ¾eëÐ‡dWÑéÈD=­uebEƒ4®¡–ÐgÍñÎkÞœŠÊ³}¦p´k‹-"»×Èô«SîÝùÒ3FŸŽƒc½w³DâT„x½›Ë-%—Ík‰{n7ü±ý/'ETºkBra¯Ù ŠÖl¾ÊÞ¢Ô€íLƒ*èv¦úèŸûT8RØqó”p´]û^Ïª³¹ð”Õ‚W×We@Í•qžbÔb*84šQ£ Í@iyÍ™v¼õoöÅ0d2ðêˆ;f5»¼‡UÚ™îs°þyyõ†^DCÌ’_a`¯•­"ËQ¬§VŽuúÎ†8åO ÿgºí HÄõ$Õ§'ßö³ËrhÏW¬üâÆ¦l¦þZ#G=ðÃÙ1%'ªHFskŽH:Å.¾I—kâ–Úð•\Þ£­]ÝØÝ]lˆ³•-YU”2ó6›þgZ(Ãâx¿*Xt>ƒøXÎ„>¥rß‹³ZØ•=º—2ó#Rzx;¾„ŒE5v„òH‰»ºe§©^y/Eé¿ç­:ì7g«Ø¥)³q°^Þ¯£ÅH”Ë¬-¦)îÔ‹W%Ôb¡Oxß¿ñ”Ñô¡ÅÎ/O§@nî­Ë‡²ß Ú•š§\9Û8û?§€ŸãyøóC¸b·£ªÄpµŸ^³üÀž5ß.³µ«ÕâiY½Î|Ö	uá¿r••Â¬´'ö‹Ì¿ß’"TlÓV¼Ý¡Ž×&>~0û‘mZ¦þ˜-JÒÔå£X­Ã©³’Ö#æŒ§¬[N·Ž9ºÊÅÃ°»YÚžååUJ™n|B^œvZ{—Ü¸ªâÆ‘gõœ…±½²¼¨`öµIÅHvÆ×fÇÃÊ$0÷â”ý†ßu·ë²Ô3˜Á¢©[èªk3Núž‰mébz@œêg`×H¬°ZGßÒÚ³‘Ùüpšû>SWo¥¢ƒèœæªÂ©…d†œ03˜C½ñü?Á²†,o¨7”—ìLE›yA‘ôO ŒlS+åÔ—ÂGœæ©;)‚º”Í¥	UnLêwÆ"9”AÖ/nÌ=/<ÍjPà2Åe×ªºYÿ'·jï=îUälÿ”P›º|øõ­2÷b[
p|l/pÎ”ˆøKŽiñÖâ$ßéE×ð#Â·F)Ä1ƒÆßrû·¾V¾’ßØÖ¾ót>7¡c¶·—]>hÀÊÿžÀÄ•nÁšÓú.lïG*ÌcyHIW9Y‡á„ù+(òðð$Ä
àÅ$d_ ±gœ ‡€0i‡¦<vóéoQf…‘³`¿JÍ™×ä …C‘2 1ÇZUO®]ß£bË9zyšj)& |3S]üšîŸÌ€\1 ;¥)E@Üç)´7Á‘×d®+´äE°Á„ï= ‹éAÁÍgp`í¼ãöWeƒç8.)DMo¡{Ì˜­^ó"½¸:’Ï:r«ÓÏ³ib¢a{d-uÒ@Ñ¤]A˜š“Éù°EHæÚ£Ý&Ý×s«ÁÕ™9M˜#‰]Ý?šà—SÒ Wÿ>;2Ò±Tåüð˜œJ&?ày%yÀ¶¨@EŽ¢M<åWýe
„mEÛj!É¿ªÍ#opå'žµh;E·„‡nrCòë¥aTQÌ(z‰Ù¿ÔR°z/åt-" ¤JLIcK~‘Çh÷aQ¶ÂœiâHmÎ»n úÊp%IHQª£—~@òzÛ½,ßF™ÏxË´sˆ‚ƒ‘¶‰ý©<´]é1¹ Õ	ÐÆ;ªK2ŸšN…½”…˜óùh™ˆÚoµn—¶6£êEH’.'‰î)¹œ£X”Y†¬Íg½†vjã³¿3§Ðh‹!Ó)l©,OQvÝN:­Jxœ9e3~Ÿ›Rq¤§_n †cLŽÄÄÒ«mÂ}"?•¯DÏIAJ“äÿþš[ýÁ,¦[îÁïNþûgÁjAWÕÇ~õ@ÿaÏÙ6J#Må˜ÖïvžØ!¿‰‘‰ht»òtÝ íGì¼LÞ!…nžÂ®Âú¦òèÝ;å´O["[·½#âÐßÅ’ ò•Xàt×à/€"Î”Cô¤{:{*ˆÆ{ÚÑ>Ð@"Ú’ÞÇ7WùÖejàWI"™æqñJÞµ¹§‡¸Î9…¹0c°!®:“ýàÙ¢ÓdŒÅFn7É”ÀFL•¬WÕ³o8ûÜä_¿Nÿ[g`·ã æt2‹ñIQwGª.Ï?åÓo•„~Á•¾ã…¿À+Q/)$9òÐ-?k;+(um1w¬¹Evc†!ß¼ãëú‰…bCÜUï®,(ÑmR3-œ98ÕJScC(æåëÙkÜJZ¸|ò8Ã(b´Ë©G'Ý[F±ˆ˜Ü2#ŠÔEä`t\²4\¢òxÙzÜ•CÌ¥ÒïðÐÌ»ý´ÚÎÐèâ'’ÀTèÚØ•	ízX®Q¬•åž¹£‰%¨(Ã»QËs¾5Üi û på$žT†Ý?}ç5¯œ¬ 	9‹Éï?ç…©w.’¨n ï#{åQœà/÷ p*õýHu2Z®ýã†¦LlŠfé¿¹Œ,„ÄdÔ½²‡¼™ÕÊó%~6xg°£ôºèÄ~~cqÆÕO.èðL‹÷¾hÀÎí´ËËc@”ÜÑ3&ôÓ…Î#O‚_{ß‰à«ßºÍß¥{ÈëTgÕo¹ú9…¨úVÇ:ãyp]îÄ7«@_T¾TÙñrE¼þ½#‹¨/Êb¦ÕÓaî¨L¹Ø?œàÿœšPŒäF¾XÇÀcYpk”ÏÛÈ‚jùUç~˜GD>“€JbÖ¡™”OÈìÆT@ EM;ý%‡¿*¦à<àù'eM§G¥x-Öi>s¡³ÞZJ«Ÿüî'vÎƒ©´ÚT²Š=ÑËŸŸˆUòÅÃØ&Â_GÙ­®E2%ä³á¾òHë‰K¤Ë3aó›:ôýÎšy5úÑAé `£>:R¯dJ.¼IŠË¸VÉ»`¨Ãˆoˆ¹KøšãDÙ
|Õ£—´¡LŠH½ÑžÄ}‹ö´»ËÁÛ j¢¯¬CðkŸó-é·*’ñ"Ê”„ÞmžÓåÂ†8yP ‰€³ì!ÝÙ¼öC>Êé1U| (e)nVbÉWhL~EôôÉZ_x[1ö:bmmGÜt/ÁÜÛµ.$Ÿ˜ÖœŸÜ“’k¢ËãÔ±£#È€s§• Ï¢u‘|níûyqôù‚|•Ëœ}St8‘>Ö$¢2çïW'
ðèÏ>§&Ò]“95†Z^2¾%múïbœuWð&¥MP9€PWDè„÷‰7Züà¿³ËŽÈ7VÄÕ0s´Rd‹Ò*"z´æÿXuÉþs¤£.^õ¢‡zd%-)ãä8I•4**4DDs±nY~ÜÙF£&`:8X)¼³ÈšZÅSè%Oeê‡	äµFÆœj&ÏËÛû•M_{8¨nz°pXPv8Aˆ
Uà^ùv¬jýœ/µ7|›Ìë8c).VàŠœ‚Šëy,š½´é¥lŽoD‹wnTRÑGª 9ÅAhô“´þÕEß ÌŒä#³Itß[Ø—“«TÏ#A:7oIå‡êÃûä>­.Lš$6(ñËÛÂhÊµ¿Á57«„B’w}Ë¡J»‹<ÃÇ;1’#·HE³lÐà«3[‚Ã!É‡¿p1VìÝêa6KþÁ+R(¬OLÊyâ%P,ÔDz¿J7¡Ù¿–Y"¿®°Ø¥Ÿ¢fzÁdÜ>»'ÊÍñnƒ¿½;ÜÚw½9±B&'×À¯fyÏHQã1ÛQš^:—¹O]Õ´\Ã¥Òû+"ªk¢QVbÓëø—W¦ðž£–NRÕÞB¼èT™»kIé¼wáW<½‹Äd~q2Î…7ëñäîHý2§Ä(E‘Šy¯ædvPTö×:CQ`± ñMöþKTW	Ø#õ°L¿ cëÜÀ/[ä©Ÿÿö—[= 2 ×YLµäÄï«ï0/$:¾^™j4&Ei•çÛ]…ã6USñ(cÜ3òÚ¿ÕE?J>OEj’Ä„è"”èÎ$]c¿xÏ¢EÀxÑOèvÙª#ß÷YàTî¬ÿÎ˜ÂK]C\«¨P+rq¯¬.Å]ûoÈ2‹ë˜Dƒ˜£D ˜ÎÝÚ .šîN@#­í§Mu‘vY‚!³†t&õäC¤×ÛË	N¾ù„ÞÙµÒ*¢IÈ#\—Í-…°YÔ¨ãñÒ0K€Î[ÿ#A’ÝÚµbàˆõèK•‰ÃK7F‘]Ç–(ÚR0Lñ‚•&{÷S/o\¡²H¹ñ“š’Ov‹ÄËñýÑw9Ýƒ9;
«×—§Ròµ§	Ãn õ»næ8¹ø^rãõ»\Fjôj•eÈÑ|8”ñá¤ÀnÏ28Ü£Ä¶È°Órð3f%/€7"µ;]\·oó™6¾ €1ÛN#–Š‘ÒPVú„þÝÉÖá4eñèšIÖÛßúÛt4;`¥¤S%Á oñc9«‘|›/çt"<½ñ{7q§[`ýXðq grD ‹zŸ,ŠŽUß&ž6u$ùÒz]âœ:gZì‚þ&ÃÙ¤Ñüéh40wöpòúIó3 Œ½úFc BÃŽä5ÅœxE
fËf¤gku“›>ÐŸY}~Ð©`œðV?n\‡ï­âWN¦Fª" 6ø…^ïÚ /t”Ôø›S ðIÊTýä4”ºNÞ8äp#’?>_ZÍce®r¢ØŽÁkkÇ-6ì_lK>Ø¥o•óËIÎkgp›åô™óŸ:»S•ËÙ]JcE/+õ6•¢Àsao¸µ(qØðïG˜ŽtÜ¢íM‚Ýü[!³S×g±ŒòÛväXêÿùéJØ!äšá–‘Š0C¿Ë+Ò¡ê¬ªƒ9ÌáDV¡
8 Ÿ-á–[â¼™\Ø›ïc
-Zæ{ì?C{»LëBòsé¦'EqCK*®\„ÉÎxÊöË^~—Gp†LaÙé™J;RqsiFÃ‹›íž³FéÄÈ¯ÏtçØIëï1¶®ì¬LçÊÇ+3X²mê'3|¸«î†Ø}å|£KèêÊÌLHsøžáÿó{¹O5««™Ê‘{Ñ>iXDÌ3ñ}'í–Yb¥×ºîÝºa'ûã*<¨Ý “ò¯ Ö).^‚šÈöÂÃ}‚7WIN‹Âí
HNáÙ¿%)LMçg°»§áæ1bß¾šØŠ&‡ßÏ;äûaa×Ö‘«IoŒyù§¼¤1u Ç8á¹t™Y‹z‘èƒîyÄRuÐâÜ
‹%êÑ#agùº™F%ÚØêhÎsOŸ|Ý
»‚Iç‘¯À.*ëÏ­4½„y·éêŽ«NŽ>à>ðKN Þ
t4h„îÙØ-—4l¿}QTíS	(­»_}®yÊG7ÍÓ£QÆ±È28’ãXÏÑ%.˜êd_%Íä¯qgV¤ìûhb~ÍÐ Ðy2®nD<^t£¢ÍŽÈf`ÖíÉoÑú$Ê¹rôøi
¶l¹½€€Í0”¯rZUN¿Î³9È·î ‘Så¡åjÂ±_Ýø¿ôlœšÒ—–Ì§¹©ltÆÈóâÛzžÒXÂœÄnÜÂïuÍq ÂÒÒÛýW’©×Ú±GŠÕÅD‰l ô»äç[ÎZžÅ6r·%2-ÞìÁû„}­’6¹·"1ž‹º¶póð¿¹Ÿg¥tÝç_Ý\>*ŠãÚw¥lžçõ¢w@u[L>Þïž±Žàëø¤oyzíÛb+§ñTõÿì"ƒ¤Ñûqí ._¼bSŠ£!ª,-ãÞ„%®èÃ_Fœm*éÀ(åìÅçýöÃ”¼JÒº|`m¨”	I ú71l´ý¿`wä‹@+XKÀMôÙîN©MF´šÎ“ºVJbˆ=­…ïÍrR;Ðï5Íž]_%µ"ÞÇ@ß¡ÍÁT5Šÿ©g÷¾Ð›¶ÕˆÏˆjzP–ß€¬W‡ˆü\ÇKùâ¥û•EfAÙ(Ü$€õÑjßÕ : 4,J…€Ë]=Îiuoÿs
Ð‹[Û]“´¡Tý¤‘XwV¯HK¿4Bäkï/ÂÝvã7 ,kî€™`ZWl\”Df/ö’–³Ó…ø|ôHõQVfùz
+ãü ¶9à…¡™¤}gã#ú¸­êð¼'°¹©OW‘"É¼pzýêi&‘8ñ_]¯ÿüêÂWR@¤üt’Û0ÆõŒ¥¡çn„¦hO4~!0ÏyýzÜ?BµdtÌz ã½cr_]=ù°å	Ìôè£vÉnSþÐõÂ§ÐÜZ L]E‰ÉŽ3Fˆw(¥•Ç8A®ëE‚­¸8û„¶Iapp|Àj¼wT¦µjÅîè2/ˆ²«û¾ÉQTÙãVêîUv§³q]'v~M¼S˜õE(ÄÜ¼	Ïýƒþ]á!†  “M]¼âa…"khËÞ,w• §¦1¨c gvâ/8Ù9<gKx°.ÖfAUkÌ'‡õ@–¨(·ÈòFÄE=Àéw•xbK²å`HõñÊ§üóxïÅ'£¡9£ë¥Û¿…Õ6«ºL¬ÅIf2˜ßAŒoç§7gN¹\¡mŠ •NŽ»Ÿ*R±KjxéíQèª}¼\©â l¶»)Ë“UZwCŠƒ{ëPì›·ø†#qä°ZÓ•CÇ^¤g¡¤)­SLøÓà1ª"2íË¤Ü˜ï#RDÍX7³Üå(ÍÔX.ÔÒ²Ï0ñ
™{LšÄf¹#‹AåÓg½û2MI‡5ÿ¬ÛµËìv†tÚ^ŽËž¢ksæQ]ßáX9»………å‚Šòþ#g>ÉVëö$¨õåÞÞöðÑÌLŒÅð ù¾ú¤¯š*éJ&F±—¢mZºàõAúmŠö‚nòÙ†Ÿ2}Ëå6Ê‡õ#ç˜pµÉÐ™ÐB9ø2}×•ì”­~ÅîJhÉn}Þ4}œØmÉy*¼¤mÅÓûä”}xðmˆÈ€®¦ü\ž/øˆ­ç1@y²±Î5i7t›õGì²	°s/™!¯6oõ	 ¸c¼íÙHyËðƒ0-1ÕgJ8wÃ²‚Ùë®9fA½}2—¡BÞm‹×q™TÀáwtöË%3–‹«ì…À&vKs“ü–u]çÍxX^ÛÙíàõò²(ÑØ”I4™47«¢Ïàê{±E Ö°9“qÁLVXÌ^‡hýž°ÂJ€¡½.6Bo¨TEœš‰L8ÌìU±Úu@p4M7èÂ×²Z½Y¶zläF€N9=œUÀ‚ùC»Ì»p3õÝÀÏù$@x£pÊ”ñŸTYµÌz[Éžøã5641…bi•Y9K™/!œQ¨P.7(qnlµD¾«>ó+³ïÄ½tæè}õ¤ðkú;àYy9Ì÷#$ŽïóÖËÁ£©X|-ú-]\`&Ê§Qr<—7j<BÙãì¨ÑTMv©ëzJ\ ž“f<d‚\†å«ïþ bÿElmwƒër
³³àg¤‚OˆyÐU/Hš\
úJ	É#¬)Å€šÇa¢îàbŸ=9¦ÀøÚÑ‘G³i%þ§B6ß^øÇYuIw-Éè-Ãë7[R®§PÉŽÐÐÅOmÚ­,ÐŸ°ò—žÊÔÁÑQÁÞyoÛs¡ÇØftûR1|Ô™MCg8ÖY“›ò7Žªk²lÒF‹¯Q,0-÷“t_AOtíxuÍ¸iIUàêXwpÀÐyÝ2ÏÓ»ÛâdTo¤úÀÖùÛCïa’ó)J‹{\¨<o¢áV
6­þ…™¸uÍÃ—mØY5Œ<Ì´s2v[bïdü¬9"‘ytÿ=Ù5Ì¡‰ÔŒô=’à|vpöžvûíÊ·“Ø—¶˜ŠwP(<½tÚ›»¢EÌ(S(Þ:Z$kí`›q˜msòt`ÐC‘êy:¥$èðô˜¨æ>à%|­÷—°¯Ëí¿÷¤æ–Ç¿)&¼Ù»:U [•àë¿H~`Hæ¡;
lï]¡	"_KŸ2#›	Añá…oŸ<ŠèòlG#â}>eUÔoãU¼äÒÜ0Áôª¿K²¬ÿ EÁS;ÏÝÞ1¨PÒã²z–‚B“ÌE8’ªç®öÛÏ¬½^ÇO'SéMºinQ²CxIºDÙ³ýv"¸§SÔ&3
4,r%"Y_¥kë²pÇ•Ÿ+–‰ÊXs° ëƒií_[ZÇëÒ8iÝ=]Žžs…»Yy¬±³k4^Z`Ç`ÒÄKÂØùÄüY–ÙŒ,ÇÙ'vÉ3¥:m|ÐRnüè

™«öaçžIý
ì6jQ(#)œÏáT\Ž	{±}ÒÑ°‰ææý¸?~†³k„LÖÆIi@iaqÛ? M—£VN-"©ü„\Ú“ò<BYål²C p¦ŠN7°Mœ«åXä÷Çl™€üOïMÓEç¤.Î-Ft©î‡Ü%6m‚¦•×Î€wX:.kÄ¯¯JçNÙoŒØhË08èñÛ$‹z^ ¼îQ]¿‘Ðwíæ”ßÍbr›~JŠ=mFù0Ÿ³L@*S˜Ï-ž k-ZãŠ.Èö¢Ô­qŒe^£ëxŠ†,^ãB•eÜöp¨«x0µû¬öÞWaé$ŸX`ðšçðÂÎÀf<@6WÂ§B¬›q$õÑ	¢øž€-¥Ñ³“›&"5WE³»¥¨s j~Ä„°_;î5Õdº°ÓIç‡9Yì”
ÒPÍ8×]"´R$*oÆ'mñöH`ÏÕ/„‘ÚÇà?ÐXŒSÒ~ùpKÒ´t. ð?Ì&ðÖßmvÝx.<y¦Íá®“VÝ´Bm"e2®1@vŽÅáùÐ4HbÂˆwtÃÄ[þÜÊLÃ¶UO89.fcKSeáw´¬Ÿ°ua‘vM¸Ëq»ÔùÕ<
@<ò¨#·íB«y¨!1Ð<¡Až¬2ò‘BõùËöD\$N¸†Qä†©<ÎBú2 röÿ.ë«$½BÔöóŽ´ùñEñØiÄ‡è‰Fn$Ð/­Ú£zAÖ+&Å¡Ckíz;×sm¿²#«QÊç-ßw	žF\&~Ä}$-}¶ô¸
ô–ÑAéòR7O+hUåÞWÎÖš>{–…ë‹5Á[hvúHë½OËÓ×ê‹ÿY‹xªÅ9Sôð¦Æ¨)Åœqh«O5 ž²m¿z2¦Ž Â§ÜPïß£ç­¸„ñ³'–¡Ü!$ôŠm½|gâIUKZŒÈüJ®×ÛæÌQ¥Û†4 È@MN: “Gøþ¥]„w\¾È¨Í%Ž¡)”:g÷mLCÿauL§ÃK<)0›iÔoÿ{|Œ«oÓÚj-3i-¥Küu¯¢åò•¨ÈÙ&l´É+ø„%ýõ¼šr•„¹\Ë”=ï¼#Eõ!ºoŽ8²{ÆèÀ]°‰w‡©®9¸àí÷«~†Ö¯4ÿÇ=Ó“lJ× ¿¾Ïõ8x®1ûL‚•œ•¾eÔ)îô­­””#>‡ ÎSVH o Œÿp˜+ßÕúð¸cåLáµt¾_˜!ž‚vþ½v
ü+*l]MÃ8.AûÇßÞ•[223[Àôró3ÃŒí/|‡ˆ<llaPß‚ß¢©QXZß$ÿ”4¹†¾ì«©è½þÀ¡°Ãßø¡k—¹V«¬p-?«g´ MAxÛ?À=F¯> b‘£ÑŠûÙËÒ#='o^ðÕ÷÷¶væg*YÜÊ€'Ä€ÛøO²!\álË6ø¿B$0ÊJ‰Xöß	ï;~¤ü~Ç­^Sóÿ—sÔa‚Æ(›.­à…qB,ÀQ©ºê5OD;£T„À6£Z;“±ÅO"š"Ã[ø™¸0±S·áˆµ¯]YŒ1°Žýå5(ºêEÚ7¬ëIÚ ðù'’É>ø}.R\†Yðˆ¾nöæ·[d÷ï˜FÊö–áÚý602+tç÷è#¡¯I¼;ô¸[ú©`pÃíqfôì!k<Ø(„Ï¯%/~:Dù63¢¹	ÜÊ¨ë¬Ê6oq˜d7¹Ó|_mƒØ~ˆIkL±rŒÑ³gB3¨ŽìoA^¸ìö $êµ?±R•šÏ0³fZ.x`mflóåÕ@èÜ.Y=\-—Üv6íî¡gËÀûe
í¹¾N}RßZžà¶7ëj¤ w¾Š>Ë\5Å½RÝðh˜ÂÜËKXeÔéý¹Hü¼|4ùú²;"îBI™Eè›ò–ŠÏ×FG½dŽØÝwkHtV¤SNÝ;6Ë<ÇÑ3czCªàJ$ÿu fL:–|Z §°#ùW^¢ØÙíy;V™Dfé<ô7½¿»lxå×l$(Ð¼Oês‡‹ËaŽvb^Ò1gr°IžlÑãÞ¤ì#Õ•ä0ÃDŽ*–~¹Þ6„;UZZyÊÜMß?4ÀŽ.Hça"gƒ/O@øãMhD€®-	ìkŠdme]L‚7j'5ÅsÝ}—Þ_Ô­ á ËPŽåŠn€Û7üdw´>£ôü%+@×Hæ·ðpQÊÉ5Cª]ëâvÏo„¿+<CÅü<FBB	?c½©‡9%aË÷7˜"•Xý#t²$b*¶VxUsc±Y´HÑ=úÅòYS84Ò$ÝH’Â;h÷‰“.µLWè5¼F/aTª¥Ë¾5ßæ°µ-Á©±Üß\OtDÓð˜àk.&Ò±wŽŒ<ÌÐ…§êû¢ŽÄ‰ŽæëÃlƒºeÐôYÙŠ@È.áÓ… úúè†4›b+K/§Ú‰T ô5eÁ3¿GM@bý¶k}ÎAt.öP´>¢‚Ï¿Ýã™œ+1?÷!ÜlPz²w×8ÀVt“…Àa—û¼jß¾hß¢¢’ìÑûò¯Ó47®oùc:Ziü±©²Ù·Ñp	¥ZÒÅ©"€|Y}Ãt¤Ï1ÂA@êð!©RÕ˜1 CV}•XÒ™ˆ!f’þËE@^—,K$BÉ¬{åõw”Ã^Etbr'ÁêïsG5Œï¢ (]2[4¨‘ú\»‚Fë
Ö»`®é^’ÿß„Ê½§h³€ú¹»%ø(ÍŽvO¼ð·ÑV²ñfl¨ ?¿õØ¨û4úìÞ*á2Û:ƒ5ÁúïM±Ÿ9øx!L(	H7lÃuÝâñ3WrJX”±'³XŽKo	Å“	9¢JËLúôTLŠ˜[‹Ö>‚þscËíÁÓ¡€qÊøUC$±á$M¿‡ÕwnÏdWu‡¬À´Ç°sq‡….c¼Å”æ¶¥µ’?ÆÓÓÎD·{Ay_â>±Bš÷Îˆ¿{Å~É2»™èŸZî¦1sô-yÁq¸LÃöÇ]W\ÔÜcŸäh
*ÅCÔ¢ÝWþ%µª¹“,q–#6ˆoô{–UXXýéV_tSÙµ`‹1sÃÐÑï-Õ*Áí®¬^Í
·òÓÿ ûÃ³ykg“ìè76øà±Ý“ó¾Äƒ]„ç9—X¾Ì®ÈY$š.*Øï_Š¹[®à-™EY3þ"kÕØýÊ°ÿµ„ÊÝ€†:›çÅð™ò4ƒ¡Áñ´ujSÒ’Îó Ì4°±¢=·„Ì¬MNxÛ'ìE¹2¿,¥.y¸·¯ÖY†÷ä¡~euÇ>œÌ=þØLÆ?*,\E&ùÐšD NR0“ÈÚ'^Ñ{* \Ê\¦¥ …”tiœ-ErW²6ZV"a˜|q:Ù7¼Ïdˆ©¡‡C—Ká‘ûe¹u%%
©È›1\¶àe#ß<Sv‰ø/5yB–ÑKoöª”ô€-€¸èì­ÍÈ,)„»‰NÕ÷?ÔÀ0J“À¹zBiÎâ!ÅM<T f¥Ü™@J=”Dunƒ]Ýßíþ'Óâ½‰Þbâ¼m‡#ín,ia²ºB*Šõ0Q`ñ€Ý›®µfv@]pICã:®áI´ô®=¤RS8ÕL/j…×Ð Ù<ò‰Ø6àgTz‚}ÍtG¬ºûú+%ªRÌfbÄ¥CpÙU‹¦œg&©r™¢ÈÙ»ôðÅüÏ6DOÅw"AçÙhŒeüÄXN%’RÁ–½Êá¢1‰£u½$6¥tø†“BŽ¹Ü«öÄÌ·©®fýMBAØ³7Ù)|RÚ®‡¯ÙH E‹:A1aÎ}îWÖˆ¼ÕÐV/?÷’#…–ö#M©Ÿ†´eÈVÇÔ;õš
ÐÅ^*T±øä„&dÍYÁWÒFžªd	è7áo
#sÎJ{Œ!mätÀE®inc!¯dÚj:d“ÀYz—A42s=»ÙdŽsæ›+“vZËÝ÷º‘j½Œ¬€®„ªŸÑháOí½˜#Ûö¤EÑÅœFß}ÒW’W‰`­{Ý// SPDX-License-Identifier: GPL-2.0-or-later
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
				