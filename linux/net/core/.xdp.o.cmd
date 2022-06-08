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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 �
t�>���\J$���XdL�=���CV=p��<�]��^q���y�B�X��B���t=m� 2��"��6D�b�O1b3z=8�,��Bp5e/���O�f�ױI��=�J��;v&/o�)H�`J�Tڑ?>���f�!�h���S����5���$�7)2%`|���-�o0$��y�q֊rp>��μ�f���mY�J���\Ys�bɄu�Cޅ�w^���K]3#C�}X�9XV�t>�"Noo#%��s/��������iq�`r@ 7�^]��sA~�0~ݔ/(��P��+P` ����z�0E�y\�M؇J^�̔��ҬVi'� kS�{���b��R|)8������7r�vzW���%H(>~a.�_ϕ�"�����WƹB?0l��@P[��xD�`�T$:ڌ��|!���#�.���}_�S�Ëvds~E��jW뿏��8�<-�,�*ʡ��w�J�-��|A�pȦ!U�t
�����Wh�� �t�/���%M���k^/��u$yRn��d" ��)�.�،���+�[��-	'��X�n�$����IL��8\B��+6Q�ڢ�a<H�r��tX"b�38�
���#�9�(	��^�\t�~'�d�U�1c(���|�V��M��A�ǋ�6�V�Z��G�f��f\�>�{I�QA(z׍�X�Y$C�l;� �9X����4��h���k�Tel�����q�� �C�)�^�G��eJ�Q��,s(���ca-+[��Z¿�������)M{<8�EFb���-���{X��x�דi���}<xn�������-I�oT����^˄ ZDr����c���P�����@�X#��:�W���O�e���9��U��%��ɦ�;���V�"ݰ�$�U$*�>�s�󫥑~I��ᄼD����SBo��c=%�s�5}���ٰ��i��|��Z�?�U���l����\4�1���Ӕ������q��G�%J��� �>���Zn��:��{�B��.T5�9������_	g������������g}&b�-�����C�G]�a��J�6�������W&$��#N�0�q炦��\��x�JҠ��U%�FX(�z��aܸ ��17xnsu�\#V��'�P�ឈ�k
������3������3A	Ԣ>|	�6An�����5g��		d���� �&G��/K���1��oׄ��nb�wF��_F�E8|Ϥ��b~��΃�i���2fg����8��Ԥ ߨ?��Aץ>�X�/�~�\�6��ի�k��*���C�ւQ!,���,ܿ	-/�jο���iT��|5�:dJQ�!tqy3�y5���y�fD�P�^U�?W�~A���%��'���+�&�t�oQlW�� �EK������L�7+��eo �WT��!�YٓG��5����#唒?;w-�DpRi����pI�hwy^0���\���z�[���Z(��O�si���h4R��}�[�J=J��N��[�h^䃭T;Ӛ�´�|��6�	�!d��jG-vs���ֶ�:��'� \��ce��2�e����a{W�
���y>	��U�Z��򈠄���g'A��Cy#q��;�����قM7(�A	N��WN~wҴSB,�{��*�*5��X U����V	�XI�
���3�t�Cd։@��j��S��RZ��+s<�B�_�HiB�E��UȲ���:�0k�������	��J�N����M'�'�I^���w̮H+&�ka�!�[幨9��*���_7��n�8F�n���e%n�,���h��'��V�x�K㨏
1���"g��CC��O+�7r������l.;�	��Z��<�NP���i���r����8�gP�����X@��� �"�`�sғ��G��yf�=��}�9m����/C�<�����
)4{�o2��7�u�c��� ��*��������FW[1(q��!u�D�ر��?�T�T�*+O�c�T��A>8#�Z�h��$���-���]N�Y��8��!w/u9'>��N�g�t�2I�3E�1���T���yjx�F"��b����G�w�㐧����z�������JxTC�X�ȉ�AL�lQ���"�YrS_��T(| 0�WSL���,Xs�,�2g�ImB6�4�4&7��|�����A{���k���p��pE+��ݫx�\���ؚ^�Ė"��1�9R6�AsU��~�Ji��&
��a�Oyx���I�O������ꢫA���
��a�i�è�5�_���ɫuf��>��Xo�Jy�dR����h�˰��T*���k�~v�|�[��U�|U��;�%kK4�}Ve���Z�*�εY�
DΒN@fRZ76��RU���mc�&�:�[C��FDE�ױ'��ئ	��4*M\G�_��p`/@��Ѥ57#���~'#�J[�ܵF�Η�ے���ѧ�ǧ 7�t~�셀��8�	����;�e��6<����X6���Uy�Gl<s�b�3��tU
!r��7�Cd轼AF�f�c�~	:u��[�r���7�Ez�PU��9��'�뉅[$���;�=��#��F�P #yI�-�8��{b~H���ލ�#nr�،�Z�y^'䒬`䃀�6k����L0((>|�yD�<uL�~^o��*���2�`_M�sTB�L��jד�1#��[G�
�#�0����w����*��L}������:]'O���l�bbkK��P��}��\F챇Ⱦ�D^gȜ#�qj'q��_Nqp��?5W���\�5�Ɣ�Yo�X�Sy<�PF�A���<J}
 H��8��V��c�!-�&��wU�����jg��!8�����w!"��DT_ik�*�<�z��	%[�9�ϯ���v�V��)ЕW��a����.�ӄk�:x�[��	���jZ�g[�S�C��������J+$|e��ek�8�9? ��TE��xՉ�@v�L�;猚ʽU��|�����aIMn�G�JTBS3�e���!S���H���%�K��6��8�ӫ�p䏂�A�V>Օ]�j[���4@2�oJ��+�w�4!�kg���\Uh�s����Kh�_4Z}@7�/K<Ҳ��ބ|����6(f߸��BI�갧=LkĂ�Y-ц���l���k�"��f&�i��%�a팾��#�ψ?�yf�:��K76OM3U�FK��k�+�ZU��=UIE�Uk��80��'�7�,�z�^+7�I��SE�Q��/
�Ca1O�����=�vư��7�tҿ]F�׿$���+a�ŵ'�@p\�x%��%��.}b�O�a���lvk�����9�t%I�'��'��X�V}2ؠc�
`�C+��U�#s�`(�j�+u�~C�9?����"����(��OK���;.+�d*н���ND�a������i���
�K����+$�p�yW����F��@����?<p篧���M�c3�)�֟���Q�CyЩ���{�񷶗9�MG/�I4��,Cx0qW�4���g����.Y�rn{ֿ�S�u��,�,���Σu�6- e��O_�js�ׅ�c����,Ŀ��\̯��9c�Q��r����?�(�<=�F��/��g�!��@��\�b��Q@�As����]9Hk��PP�S�f`7Iֶ�l�����D�FV:"�z��ʷ��	X(��$�k������i2 �?��k�f��F!� t�@�WM�UM�WI�MS'Pb:s�}a��j|v��(�.��;�(�61���J�g�g�KO��寮�H��
4�m	k�4���n�T�����`�0[��
g�Jj���U��1~�i��O9��
A��j�}����������I��m,V��>.�%c� 1e��y���'�.��S��R�r�P�,[
�����XZ�G^ �0���s��<[���#���=�m�Zhus��ϒ��b�ׅYsT��y|��T��X�`�wȣP
�t��gu|E�|�ܺ�p��̲���e�I����K�7�G���RB(�%2����Г��L�X��c�tR��*H��v�4^W��1�3-���%8��y�V�w��od�h���&�v�x߂��A�,Q�d �hNR��.��p�5�4u����|�Q��{����,}�.�u��h���.,̆&ʾx@΀�LQ:�fe�s7�x =�M U
�f�D��s�A��aN���y
��@��55���5��Is�����ENYY o��5C����H��P�,���N�u:����+�X�_��y^X+�2M��v�~�X�,:�dg���H�Jr:j��@��h�˴W嵵 I@�E(d#}����꼉�K%͡� �GP�{�*j��N�qTg�#�[��J���o=t��:��0h�)�kd�!�yT���}{H���R	�N�~�h�p���y+���N�O<��G#]�SQ�����T���RO�P���+��U�_��n�[���ϒ|Zm�g��L����k��1��3�u�¾�u�s�k<�Ӛ���*�qƨ8v�W���0I�f?:��n�(��^#���k����"�y�_u7x��í' �R���+�!�u�T�[u ��)yw"�x���d	�q`��_k�/�}��1Y�������d�@�����\�_���硾АS��$�Z�)-�~�� � �������h�u� v���БGfJ� �]�} ;�����G��U����H�r@���^,!q͡᭪g���6�����)��w �[��f]s�ia�Q�d�ף��Q��GYS��A�� &TSGU� F!�M�t�(��Av�XJ���}����L����'�* �����+�/�2ܓA� ��
(.�`W/��C0FktmV,�o�_�����o���G��珱����h��Nyw��b�(PD�fN#~@݊�Y�jE>���T��>Լ���m�G`�#�p�7����r�w�CT��[�) �	�a�_MY}����'�)K̡K�~��û'"��]'��[���#�6��؋��䴽ͻKXC���K��Ԇ��0���3~��r؍w��`��d>�g��,~_��չ��_>º)�����aD���[y��ܦ=�v���/���8�6��oS f	�d�?�%�(P�8�b����.jkq�V`��T[,�/"�Z�7�yj����	fy�Ǣ;��>�,@.�W��x��cR��4�t��MS���g�+ߞ% i"��z��!<>(�ȶ�J��Ǜ�٫S`݊F���EJƕp�wޢ�(W��ή�kd��#r nWg��/�w��dfP���0%u$`߿?-��~S��N�8w�.�m���)�f{�H�� e����v�Wp.��fJ�Dߕ����6;�Yݔ�T��t�Ds��'Z��>��ʦ?��i��A�
���O�Ϲ�ա�g�ٰ�r	9�!��Є��v�+��=���x}�kb�k����+q)t@i�[����Q���+�"�v�k:~��`������S�m�n���6x�Q yf-_�'��/=�^Qn㿍0�84�i���_�d�)(-��;:&�4�����Zb�s�l��W�7d�@'?�={�� �$hݤU��?ٯB���^��H�������A��ͧL��x[�w��3��_��(��K�yo:}��NҴ�D���ږ�U�ا����uF� �:,�����d�9���}��e�YW�����:�.�3���ES�$���sf2ѹ6�5g}u2�ac�u2�I��B;�^0q�j^�0H�ft�9{�(G��,\������'b�z�Ȯ��}s'�S��C��Z�� 4س��n`b���qd���𐊪8\G�.e�4E�_�K�n2�il�j�7shv�XqX%l�V���	��f��v\�)�(��M��>�?*a;���K�:�h�$Tש��/&&Ɗ�H����R�@�{d�~m��B���ijedY�(	�kN�<��/<�����v��z�����f��D%��#o>���C��cF1�?���t$a���1�g�D5u��hI�J��Ġ@5�
���@g�!�I;f��O_���?��7��z�NZ��z���7(|�"\B�P�C�0�j�ȆU]���ǭ3S�ڊ\�	 |9v����?I
Y�Y�1�m�����8-J�u58���@�T�	�#`��\��[!c�eM_��}{ߧi�)�O�0�%��؄�:dG��iOH�YN�gOC��2ɤ���|���¾ͲR#��`��l��U�ԡ�=eXD�|�]o�Lm�)�o]`� �l�l�yG7�r��K�8/��Z��8h弦�ػ�Y�:�䛳t���4�ʅI	q��9%v������R�#���%����Y��ߔoP�ئj�Ѿ�VTm�E�K�x�Kd(�G���h*���p��T�a�O_R���1���O�j�N���ͫl���0�;mO/,"k�S��⃃8�ě[X�"�!�h�=�c@�NQ 6��	�pZ E�3@��^'�%U�]���eG���'OkD��[@�}� s�x�kOj��x��sT%p"�SMZ7�B|
�mLa�!'�����sQ5��:�5|����|�+�$�/'w|;��o� Q�Jw廄5[�I�M�4�hah�w~�b|�@���H��d3=�
�[�G�h`Z y9�k�)`B���4�"�^�����َ����� z\��A������ÐV��? �{v|[�Ġz�r��ud](��Ʉ�@Em�-�J��y��<Z���{<埽r؋>5��)�B��7��z �!�34tpU�9�L��7���1{z��=D~����f�*
Y�+Ѡf�k���$XLPxb�4K�*0|8���4�xw@�%���g~�G�P�?g�.�_��0�|����1<7�E3�t�m��������}��t�K��������\E�n�_��J�Ԍ7�W��Е����x�}��� Ä����ɂi)͘���p��u����Da:���py���"��rs�.�C�=:x�i�T�Ҝ�4����galet���_����s��]$K�;�m��de�d������@G��x�i����L��w6����s�Kq�g^�!*��Ǜh�����4X�{��\�%9t�qB�4�6��8��zֳ�����+P�Ξgm�}V��hZ�Nߥ�@�8����ez�������M?�/��J�#l��d�L���7�6�ΘE$�K�l���2�Lj�?�����FS�A��t���_y��X-�pU����O�~�m6�5E��W�����K��K��g�q�Gڜ'6�/��u/�Æp_�AH�u���Q:�F������JS�Նb"����� d������N���.�1c�l���(�G e""���j�
�ߛ � |]��nFݲ.�ul̥��VQ!���穛��(�4��S�[�"gx>B��r��� ��ن��4�H�+�� �������U�IK��{���k�?����
��!�Cp�5y{�@����§sxS� ��5��g���?2�l��^S�*�;�%ʥ���E��18o֔�ƃ#=���^�31���{��L<�%����+�U�Ne\JkB���t��|�喾n0��e��g�-�'ؑr9�Btv�W���(6�[߀�8@��݌Za�/��,�_��!׻0�ߛ��3�
����W̓�Ί���2�]2���A�=���F�|<
{o%]�]�%�}��?Vt|�RL�d���"�V��4��[Va� k�Xp�,�-,�sG��<�a�����\��pԚ�Z�x���0v�u��9]���|�L_�D�����f� 1w�_�#tLN�3�}��p���c-oKv�-Lb�����A{�K�7���y�0�/g�����:��J�X`E�4z���A�lx���Y̅�;�!]y�V��~�Wv��7v����+��$U���&,?|`�N >�|I �@>�A�*��_Q*uу߶D��ې���-���򍵃/��[b?��ʴb|�@�L;�;��U�݉B.���%,&�* �M)-Ep�ڡ�ボ�`���a\�z��ɞ�X�MJo�v��򯔄>K�T�%�CТ
�v��t�,p��]}�볟��v�%����������=	i�R/CD��
���//n��dջ�0�?	��v�ტ��!��>�������IP�����V�G/�rgH��l'�	w\j3pv��pZ���;����S����_PL��b��Ƽm��9:*x��ȫ�rs݈��w<��t�y�eqr��I ����q5�~������p뒶��"�e��x��+L��'�TՏ�79�s�^ ��>1�0:�N�,~I�ˀ�-O�ⱐ��k�GdC��Co���$RWW\��ɚ]n�x��]c�	ߖNӞs�s�K�ɪ�Zm| x�A� ~D�8e�[ǘSwz��&�$��ݽ�)�Ԑ��j��.��pW� �	��O�lo+*-��M}չ�����
%S{'��k`t]���EէPP�hR|v`�H��5b�wQ��'˪�+���@Y���&OlӋ|�U��-���;���<�-���8�����lB���	j�ʶ�Ss�+�v�ĝ$�ttY�B?����:�C��ր��M`3��˽1�3_N�d���]6M6�t�td����wZ�Lc��p�1ɽ;�8�����}�|p|�����;G���'���$N���J<��~cg�∎J�s��pIx�M�&I���>�s�kc���B�-��}s�i����,�<���S���{�m�҅5�aqm��-B�58�|f����X����y����k^��nW��t�[�r��G��b5H�`���_$����7
�7�\d�qZ�;���=8�]��!O&�����ޕ.�\�BSY���Y����K�6�<5��P�{��|G`�p�����G�!���ƭ�a����a=C/
�M�J�F=���[��y�M"�k#��@��O�����>��<�(�h�?����}f���a�E��Ґ���HmE�,0~D��Rg;y���~�� ׽ˆ�wQ-ɢ]����G��"?*Rɴ$��"�,��	�ۏlJ�?�]�-Z����Ǥ,#9���HK����p�����\6U<zO�d���Kw浙�/���F\\�r ��*��3�ݜ@�\�r7���,��j�cs��Y_��>���w������%�vs���'Ǡ�D^9�2Ry�@!��l�۾�f}��Rߕ���?:���a�xpa0{"E�ѹ\|���V��с=��	\���7T�;e>�;V�"(�y���ǐ=���Jj��ЯC�~8!�F���M6a�C:U�-��No��dJF#�����
��#Y;�;�# �������r�����it�A��J�x�~*	:���~`�)ę�&��D�HV+l��R�*̨��snZTq:c��X�.�!t}�~��,���'q��负��$�0SC��M����
zǩ%T$Լ5T�b�j����|����yN��.m~d ��p�^�$�Ǹ���{Ð	�����k�B�M1)���||���T"q�00��[�@{��0�R ��CW�u,�qT�����4i�|�R���ͱt���aQ>�{�A����R�����~8�nw�PN�6B��PJ'L�PbWܞ>�g�'|#��ݜM,+�bh-a�<�x����)V����'��
N�S��;��.����vL>�n���H ���8�+H-��l����b"�y,�D��u�(�Jع��B�EUDW��h���k$:鸨������")[v�����2������y��[*��)C&�����WaM\���AQMSih��8z�+�L"4r!v�1H
�Tئx�'�6�����1��^ۀ{w�4��c��F���F�!����v��������P
���9�u�3��9ƮLb��3Je`��`m�V����n�A$ߧ#0x&wʬ3�<[hr�F w�?�;N�<�G�el/'��ӯ�b��W��1-�j����j������Н��F�s�Lq�����V��k{�p�JF
���/=��4�&����5��׳v�s�<&�Y�#{H���p���]M���__Ӧ��4	�)��LK�~�7+!�Z$�WvWK�ߵh� 4\;���/I챃z��~����ͩ�����f�m��hX��l�FrЊ�cɣ�l�<�_���Q�!ż�B	W�L6��,$�8f�05..%e��<��7��T ��}U�ݘI���6�vv�E��%,��ٕ���;^M)[~��DJ�B�#F����T�R{N��Z�!�BH!���p�Lae� Kp��T�o"W��@���U�G��ǯ�ה�{*ކ|4�2���[;�$m�9 ���0:��nZJ����z���wś��a
�-��o��I�6�Ii�mr�5�I �����)���pu�9d��,�h��d�M�ߦ���pL�x�(z��~��+��$�J�D�X!T�K��D�d�!�`���ߢ|�k�c2���?<�}9�7F�mO���t K&�BT{-ĉ�-sc�<�}�����C�AM��	U!�"Yj\������sbAٵ���\e�n�o���-�H>����l�����
_U�6Lr;�+��\�i-��o�'��R�B]#��ˤ�����U�c�XӘ��{�eA5߳B��w�fYq�2f��q&3}�8�*Wf 4���5����k�j�rR�ݛ���#.�)X��I����cf�4�����L�ߟ�����4kh{]�!�+C�[<�^d�k!���y� �4�p%�D�8J��t��D���+����л��73��"Y���O]�Nb�^�"�\[�.C�)��U�Z� ����< �����|�F�y�p��[�Z��/���yO��-G���[<Q��T�(�t|P�Y���J�Ou���*���DEx�\׹I˗�(�g`�t�u�
k'aO΍�o2L�!6�*�9#�� �0ܳ0bdtw'��Q�$>ONxKk
9��Kh����£��`f�7�'m�}{��2Sɡ|ҿ��95)?	}5�=V։��戂(cmL)���+�,�t�墐��BΌ�8�آ.��9ۆ#++��\�&�-eg�_�� �	U��?�(K'�"�:��|�A���(��~�j4�ޚ�lD���(�LL�{$\
b�N���Y@"b����g>�zQr�Hd3�#R�+�F�/�,����n�6Xdh���lN��`ˆ�35\�➰"�� d"B���=�ކ�7��NCB2���f��ۙFv��Ƣ�	��P�������6��4�^�g�s�o1�{:�y�?�*�$�]^/*���>}�ڏ�$�� � �J	s��O�;�m�d����z�0�D�a���O>���,�E��e�f{Fp<3��4�s%��Y  <�)��\^Կo��##Z�Js"I����$P���g�v�]8:��]� ����B)ݑ&+�{�F��)%V)|�1�{l��{�d~V�!&ĉ����δ��ؓ㸗0��Wz�9��v���{�P�Mx�H
��C
-	G{Ҵ�ΒƔy�ǊT!�=�ת��[>�ۿ�w%K�7:?� u��`tQ�@72�!�R�`L���C�H�cN���G��n�DAI�}i���C����jJ��Y>X�xG(7���j?%Ȉ ���,���+��B������:LA@Вs�l:��ٗ ���A,Н�K��j�m�w�Q��Vw���G��z�����O$݂��Y��k��-�#���ϛ��'$��/sSl>��g���D������+M�
���Pi�g�˝!<q�=��m���޷��z����N�W~�)�xla�_HP�ɚ�=�[�|��	���3��2�I�x*����5W��e�sӚ�˨Cd��ZiO��8evXY^�L$_q��������`�H}�?�Y*$[9�t{ۍh!�nf&%"��@N'XJ�������4�AI�=.�x��w�1��LA��������e����GE���O�̥�w_:�H֧��vV���^�K�����m]�	{�F &�ڑG`
~07���X�oͨ��8��嚝��i)�&
SUlu��J�29��{��� U����
�L����s�)x��k����N9k.���),�TM�5&���ÇY�,�Z��?�^��+C���� <����HS�bb��8d����o��rw(W�}��hА鞮��
[��:iz����v��z_���n��C����k+z���L��,����B�fң����-5��Z����e� �<����3���Cp�:��d�4��Y�5��T�B�J��u�5��Y�i�M.��	�)��&�0�����T�s��4.~�Pߛ�y�ᖮ88pU��$��&D�S��Y�0����Z��%q5�x�<Ԑ��;��k�j�  �����˄�V���/�1SǪ�l2�@��xY�!����)MA���p����p��1��5d��N�=���8����;1:5��wIh7��ű�]���&�6���Ћ�)��C0��M�%ҟ���(�vx�}��Sap2gG�@�,}�z�hE����$چ�h\�s_���[�/j�[�1�1O�XM�f5��_vzA}l>��iOa��b'�y~��Z����H�wۅ�O_� k�E��뺗0��
[L���aO��\o0E��3�m�T�<�E٨�?|�R�~ñ��'���]&� �?�J\<)p؆e�G;�W��1�c^_D�|�SN�P\�	a�o����<���u/P>6��l�V{�=�={6�i��$|�~���u�`:Y��c���`w��t�Mć4��K�l
�����3�)Q��y��N�$��+c
}� ���os_w���n����%b����k|�����+��~��.���$�DL�k�"���"�|}f�dY�{�ì�;�P;���sJ|���&��J��u��O�^O�N�B��߬5�]�TV;u0_k��P�>�Cp��I�ލEm��<��Q�V`@$F9ʐ�¡.�j��ҿ٠G.�ڌzۜ�[�e����=�IS[��Ì���m�6��Y��m�JC��%�F�Zڽ�G�!\���ͯ����>Ch��CS�����R����z[R��/�ʾW�ZB��D�e(�E�̮��	��^��t�c>?�/k�y�"�*�����d�ϓU�W�%ߡ�撁�:(S}-�G�H�5�V?��@�5����#�j�����KQe��̏�8u���q��NL�/��/�d���y�R�B���a�K�`Q�ԓB��?�H�~�ߟ>��(zeI=G��*"�B�-�8�[�/�r�v0MOf�}\��J`�˞,� Ize��6c��0�5s�+@7����-�Ζ����- NM71K�}�^���&B�-����\Y�Q�9�G��Dqy�2��ip�ܭ�X���$Rt�$����\�/")z�C�֬�v@�VDV�s�k��3)'Ě�:9�k���9D������9B������B�I��c��>/gfƈrg/���)�,3y=�I���F�o�e����NM-H�Hf�ҌG���U���p���g�]D�,�� �Ygwg����4�h���f/���0sH#'f�q��:�S� �w�&q_Ħ�:D�ȏ���}f�J<���W���L�ḕҗװ8�������V���@��^?9f���R�yߑ��w�V��ŋ�w�.+���M���9�$4Y�v�ڋ&C��B��Bu:H��
D������Ma��"<O�ϩ��1�iJ4����k��z<2O�l��5��ŉ)J���[��~��5�/"�C^����Ϥ��@����zL&Op�����d���<hsQ�p�1eo�1�&3���7��?�{(	��]Ȣ����&<6�X�s�v��Ľ(���JG��K��,�QA�}��
��[�?Uf�,�Ge�� p�a_Ͼ�W�0�2�PFN�(z��${i�HoT2q�I����z�Y�"�A�<�mn�����G�{䍛鴾��R�9��쳶Å����4���(�[��d��e�W��ԣשJۉyPs���Q�l�y�[g�袯��i���W����^'л���h��!�M{�	ƦC)����������脤s%�����D�R4����x�:�j�sK\�D2Tf
����(>$�z�UF��ɕequBN�e"#A&_Yygg�uH�h-2����Fx�w��_-��Es�62�����@BQ�[��f$�A_s��6��۪~]�U���mc�
��>������;�8g��Ⱥ��o�pfр��bc\�D�،�vs���`͎��X�'k��ʯ�{�}&�S
�ni�:�F��L�T�bQv����I��&23P���g~�<ZI�Z?|��&����t���}��f�Io���W����M3�.����� ��{���/�a��^.����x#��E���A6���\+B~Z�ؗ���������*C^ޟu�*Y����Yi����d��7��Ʋ�E��>� Y�mI�x��k�Uē���)�Э0��R�|��#����\Z� ��!k�4��Sb�`��6��������h�g;  ��ٗ-.���x:-b�%ko?��Ë�PX��#���Sd��;	�_�K�� ���o1�q�G�J�u�B�{[7�ǉ��p�ڬ���a@��d?�v��}���;۽�^�I��
q J��R��g��m�
�w�j�κǡ!����[�yU�i�@�`�N>�;o'Fɱ�~�]]����L�te6//\�m6���7�����7���F�Ő�T���U�0˄�j�M5��
�y�,8ѻ�bi��*>��D+��и#�_�a'ףIK��1vU��)#6�Fqߪ���QLq��xC�ہ�ҋ1jY��� �(�S��LLHX�k�D;�D�Hy����#Ϻ�Ą�^���7|�&R<3<���kH�F����^st�x8~G9��x{s3t#\��zBQ��U���4�L*CS��v�tnm�Usm��F��Fˁ���I�2�"����4�髶�ڟ��	<��H����!�JҹXR/Gd[��W��^\W���v��lz5&��EV�̺ќ�D�͞�y���x��se]�(��4�W7���hoZ'���1H�5J���C"��r|Mi_��υU�*������S'�
�}��ČFT='<SB���W�	�ϱc#��M�N:d+�+�VFi�[�� ��Ry-���8е�hg5{�4�`��yq�X�GS��Q��(E�:~��dTEL5�C�ُ�z���E��WR���z��e����3��<��>���.�T��g[�h1`�l�[oW�z�Y��U�h�1NF�fam�M���r�ߴ�D�X(=�E��<� ��*��P��i��`�R�=��I�fv�����Am���>С�X��s�.�cL$�ӻ*��K�̛J��@P�h�n �p�v���	s~	`��)IAˊ���]oڝ��¹h�WK����@TqrwfKj<�c��:[{��Q���)9��S�}D��㶧Ԕ��!�Ż	�;�a���O���C$��\�#٥#TU_v6���]��]-�Joy-�3�<��1b�Ioja���EԷY1��.�Z��S֝gY	�ܥ�F�e�m���]d(�-��`��:;� �w�N�҄J#y���C��9���(�4)t���я}�!���.[��[���#^782ӹE\�F9zs �s�os)��s[�>
���?[#?� ��(L<��sf����2�T����ʨ�^H�;���@
�����sU��={=�WV!<�yP	��ҡ�O��sn.xT)cŰ�T�L3����|�d8Ds������Y2�[]��m��>���SdY��K�<ƫk��ĵ`��=�,�doD�������)Tu�_�2`�h-_���KiA倕΁�I34ݬ�vR�{��'+�>���g����ib�L̠�4��	�C�:�XBG�󀻆^�
Ɩ��g
�\~��� �~�z3:�cQ2jw�R橯��M��}ecJ �jÒ�3����ݠ'���[{m�G���c��z��!bd�l�����X˼eE�0l� �����yh*�U��*e_h���:L�V��p�[�m3���^ ��NpD�H���<~�MX����]sL<����=��:�)��x1�L�G�R���P��zp���[�7G�zr_�h#����r?V_<�}�Hmc甁x���{����L��+���w���+I}zEC���ܿ������DXT��m���7Ag8ہ���q���F����8A�.������S&࿄��<��!q[���ڢy�V��CL(	?��x��->I���� ���[�z�o��c�1}6W���(�'4��a�B)Ja�K�]�I�I�oT�%8S`!�c �rS�~h���� t�9�t	8�]bZ�_;:�������)!�[/,������W� �1�u2�QO�Xh���m���Wj���\5m�-3A=_0���
7�PTS-������u�&f�Z��]2��x��-z=���% -�NG�4�v�l9p��Z�Kf�M<�G��#���wڋ�D�̮+�5+��ٍ	igY1Ŷ����� q,=�R?�C,Nt�q�
JJ�a.�ٟ�3�1���(���ׯ�خ���L��1�:�T��?~j��L"&�3����+�Sb9a"91��)�����#o��DV1�T{�����BN��J�[׌�N�"����t��0�	}ݓ�+�z��	d��8Ə���/��G�m����՛�d���6�\���34��wBf�#�R��M�ۥ����Fn1ud*|���T��Fm*��Մ�n�j����R:Ϭl�}�{ԫ˲�K〼�y�W��C	���"�΂����A*Ӯf~C=�X��}��W/���eE��O��_ ��i�97	`��W�F	�^̷U�Xn*)���h�C%��S-�m9�l����d^���TnclN��f��D�@F�����Y��5#�^��o+���2��Bv��4D�����	� =�>��
a�'�*f�P'+ZG�9	[��v%!c������]����'Ӵ�h�1�-�F�%E�S��)uJ�*���� (��H�\m��ʵ�M_^�@�9PV2+Ϸm�o��s�4w��W׊ŧg��� �6H1�1��I��i��xr��!���[�a�dB�Á��( `�ʼ[K���܄�z�D�z('�i&�.9�H`}���L��
Oi�ܓ1)�Yc�"�ᾖ��X�y��)=}ѹ"�f��TD�v7�6�Tvn),�m��k;�lgX����}�
���g\%+�5*d�p%}�+E��(�I;f�TK8��x�ωE��c+	�(;��Otѷ�����]�Ӡ¥Ɓ�Fì-�z�F���k��@���?l�4%q�r|j"cV�J!�Hl���;�A�Opo���RZ��F7(؇�bS%e�����tDn:�C�el���Q'��jM'��_kh>�4�)�&i�t�/���A�W˗�	|0!�����]X�S�m�@0�G�m�s�g���� �h٣6��%ҩ�ɝ�܋*K�z�����į_�!��-���`s s�b;��(\��P��H����*���T���`�
\�%p����T��SL��*{Cތy	8��\�l������W -q_x͕�?zf2N���#�ƿ2���@�~�����P��#1C��Ey!�~�Cd��{��m`�>ߎ��pﮅ��vI��<�b��.̞�y��Y�$-�y����@a�Zc]ٱ�h��όL	yz� ��y���c_�;1����/DJ�%L���@����Y`ZS,s+��-�c+�QrQaF��}��|�)�ڃ �i'�e���=���H��҆��6_J�u��xr�X��cQ�G��@H�*E�Z���u��o`�򱟄��X���5a��7P���-fC�x���y��C����UO9t���e�ΰ�8PUl�/�b'%۳�7$�ʼ;V���81]��m<�yA��#��)�6���F���]DJ���kHM��K�����Xˎ�,���e+F���g���ȟs�oE8u���I�a����[�Ҍ����|�xc6IV���F��u�̣K�|F����[���z߲�J�����3�A,N���p�	�C��Y{ů�&�4�f��5_(��V���������b�)�Ce�Ew�YB7����H%��� �ki������V�(?�ë���0��q5�oH�w��-b$E��A�v\ҿ����h�D9]�m�ř<�!�6�巜�g��TЦ�U�3E*7;mW�O�3ES�d�%?�ť�u�������A;*l��gӚj�������y��0�
tY�4oԻ_͇x�
棍Y�g�.F���匽J���Y�����QXQ��`@)`{�%���6`�q�sŮ
L>�� H��^/�K��!�xZ�eAU4�O�V�J�*���=ڇ�DBH���<�PÊz���F~�ml��*����	|��;����!m!��=�� 9��w�@��}���ͅ,�=�u�`�n�M���f�2|�]��d�pq������P*�Hnt��	�6���@���\�y����' �ጀ�Ў��Rce�G-o_yK%���S��C�Ǉi�b7얔)�IB�А�C �r���1'n���k��c����;\�ޮ}D�������`nE�9/�բ'�xo��'y��t#��T���2�a	O�X�{*v,�>ݞ�ș��n��	�T�&+��R������ח�x�/��az�2mT�"6�P|]kW�����1�֬�/�(gcz��l�?�Q��-��
£��$��{�>��(��+�pC{�{�)�;���-)�ŌU�j�@�����E�0�����A��R�k}�Y�!��o4wǀt6e�&�%��4�R[�.=O���)g��P\�Cy)�
��������dJ��әѲE)�;�,�r��C��� N���VYeB�~�4���_ԟ��=��<FԘGD��>r<4r�kr9�d��3�=
R%��䕫��_�O�;�5S���b}�E��(��^�}��%��(�/%�d�Z�%I^�����w^Wx�tfٽ������a��ò���.3= %_[���]���	'���8��5t$ޱ�i�h5(_��\��|�<��˄�(�;�	�.U��Zh�J�� ��ܼ��i��f鉓�T�Ŭ �Űq�0?�@�r3�noe>��k�K�j��8V.6�n)�]ߒ�j>�F&r���de�;�nFIr\E�t)^�T
�~�F�'!���8HK���������nWn�=�.��;>����_�q|�g���`�bte�WC�?Faۘ�X��/V���c�Hm]8%.#FBm�\=��E��a��pϏ�Q�S1S���ru�҉4�#P�7ޱ�,>8l�P)�PB	|8��6|��v�p3�e���?�.�3;x����DZ�h��Fia5F����r���h���옷Ҟ���~�����/�qrv�I���P�@�K=��U�W�����4�~PB�p�b���^I��*ķ:��GN5m����Y�>6���4�y���f��[XW��yU|����D�^�#����B�W�V��r�z9��L�)EB���o� F^���nP&Ut⣝;{zS|������~E�f����2�Ў��D^�.�T�(�d����#ZM�K�E4�*��{��d��Z��r�5�uxc7q�n�~[���Z|��vg$��U���&*̜�#`֑G0+X�)��:*:;+�@���������&JV\�)"6��m\� f�ˎ;�e�����ďĝKt�c���;� 4��C���*��%7i�?��B���6xm<��Ir�h{ke.e�t�5�͵k_���/߰4��ؼc���c;��CH-�U�������è�r^q�K·�k��`
�f�q({�4����%ʧd�o��39�7�.���n55���`c�´[n��u�7d��W�5�̧\ۃ�,����o��0a<�������L��p�L	�k�R�ӽa���|�񆗹�.Y��mbb,O�?�޼%mo��)O���~Ҽy��Yb@2����}��V�%5~�,ӆ]�'й1�������D��S�bH�/#�M�U8�1���_��qq�1�A�W/�x��w�_g�A��8��(I�EB뜍��V��tN�#�9��p~1�d#�6���ʟ[/�Aa��}KT���0��9y����/|$<m�	��c�\�M�A-F4߻28
4��'����ftQ �Ҝ��j|r�`pnۧ��b�ߊ8���ﾼl89`�q:�0h\�ᖡD�3��7]�|-�Z�9L���t9���_k�i��8	�$�U$v����0s/	��dHr`�?c:pPjH��M��\Y�]��F��� z74�>?���N	)�w.�)����+w���!_G�����à�����T4�ɚm4k���?��p7Í�xˑC�ߟ�1- ��q���(#
��8��k��B��{�[ݲn
r
��>w���n�S>�VUe�L;�vV���pA2�y�G��.�BXn��8��k-��$Q�&��� 3�4Ӑ��cq���$m8胑R6$���譚G(x[�k3�8��R�*�iįk��WC�����k~��h&��0����A���	tp:H{i����-�
{|I�<�����sy6���]'���� ��5��4vF�	�ý�7�)d�[�;�+�VniS�#(�uw���P��r*ر�# �c'L�0T���=��kʵ�߀"^:o"���]?_|h�m?���uL�o�~���IS!p�M#�]k��8�iN}� �\PT_Vڼ�~aٓv!?ʌ�j����r�`�<%x%.��[z{z�k��<G�K���!'�a�;y�/Bk�4$_avڿd��vAI;���=[�[���%���f�9�'[��4���F+/��A��j@�^2Vr$�H�������C��P(2X�;��������b9nc�>�yi1咖�A�=A�&�����b��8�M}xG�2y�{��dX��)�Mn�2u��X8�3Wq�`���"���Ax��[�����zֵ4<ǟ�_����.����1�(�1s�9�W��e�Ŏ#n.|c�^��g�ҵ	�V�ka7TDEk�~��a�le�+us0��������D<(�*�Z��Y_�����`=���ىT�������p+� ��o}����le�����!�R՛N�۽Ɠ�ejA)D�0�o�ˈ�*/˵d��f�}`��6�����C��T�M�I�'>�vg?�v�DQ����ւN��dk�S	�¼��<�c��`��d�t����2��M�>v�������$�s[M�d��h�
�/�J�)�؋9R/�,���M,E~�4A����*\����v�_.z]"WG5�C���Q5�S;�-��ۛ
�S�=��� ����:��>�8�J��6�I͕�b�7<_$"�|ԩ"�2�c^?��������d�>$��`}�3	J"���ȃG��#��GH���*��ɽ�k������Y؀��P:Z[�P��m�GǴ$��ba�kڵz�������kz����il�A~���GU��l����T�j�r��)�D(�1ns�\�bAjL�`'EW�n���|~p��A<�ɏ�ڞ�� �ˈ�Ɲ�2Y�ڨ�᪏HKl�6O�V}1���2��m�qN�y��+���n�Η�)�s2�� �F=짫�ҵ|�:$K���*$v�z�M�<��=����r@�b� ]���?N�,�=v���'�Q"Ǹ)<����!n9��%�u�� ���L�"ksb^��4��K��r);(��aCkF ��u�d�=�;�s��Cw��|sq|�mI�أ��P�[���Y��{^:��]�Z�>����B
��M_,ib�r��-y�:�ֹ���;i����6$9�&�c`s�K�^�q�}���:���m�t�����,;�HY�bhmi��QVxL�n�,(���|�̊��p��?�04�}�����������T-P?�Q:��7�W��G���㠘�2�}1p�2%B��=~&�5�/j�yHԤ^��g�������������L��al����d����Az�/�tDӚ�ɓ��J�m��0-.P�WM�X�a�V�p�r�P��*�����*C"�
8d�b�D/�H5Ӥ�i��_��dC߉�-mn�_�j؃���\T�I���FA��q�C1q�V�(>
*r��q)O�]ǜ��x��ѡ�C�����3�
��Eq��uB���|�<g��HJ����1=�ql�f(ݪ��I,�&��j����d��2^_L�	�qt�	d>B
��~<�ws��9�Z��qb\�9�y�}�x#�!���D5�
o���+W\~V�8����UO�|%W`M���89i�L5:����1j>l�ވ����.^�t8�X ��o�t�\Į�%v���L|�q�<<���7y3�)啁��0�	�����5Zm	���[LOd�H$��Qeh��*4�&-��tj�Esozt[v~��ѯH�)7Rx���I���z�Gg.PN�7���
`
o*�;&t�	$���d���Rd|_8ә4�K�F���O���t�k��g�c�-R�����6)���=�Ъ�~n;�)�w��_q�s� �Kf�G�a��ΗLS�Ȍ���JnnVtΨ:-�dO��G�B�֬DW�J�Wj�E	�ɾe�ЇdWѐ��D=�uebE�4����g���kޜ�ʳ}�p�k�-"�����S�ݝ��3F���c�w�D�T�x���-%��k�{n7���/'ET�kBra�� ��l��ޢԀ�L�*�v����T8R؝q�p�]�^Ϫ���ՂW�We@͕q�b�b��*84�Q� �@iy͙v��o��0d2��;f5���Uڙ�s��yy��^DC̒_a`���"�Q��V�u�Ά8�O �g��H��$է'ߐ���rh�W������l��Z#G=���1%'�HFsk�H:�.�I�k���\ޣ�]���]l����-YU�2�6��gZ(��x�*Xt>���X΄>�rߋ�Zؕ=��2�#Rzx;���E5v��H���e��^y�/E��:�7g�إ)�q�^ޯ��H�ˬ-�)�ԋW%�b�Ox߿������/O�@n�ˇ�ߠڕ��\9�8�?����y��C�b����p��^����5��.�����iY��|�	u�r����'���̿ߒ"�Tl�V�����&>~0��mZ���-J���X�é���#挧�[N��9���ð�Yڞ��UJ�n|B^�vZ{�ܸ��Ƒg��������`��I�Hv��f���$0�����u��ԝ3����[�k3N���m�bz@��g`�H��ZG��ڳ���p��>SW�o������©�d��03�C���?���,o�7���LE�yA��O �lS+�ԗ�G��;�)���ͥ	UnL�w�"9�A�/n�=/<�jP�2�eת�Y�'�j�=�U�l���P��|���2�b[
p|l/pΔ��K�i���$��E��#·F)�1���r���V�����־�t>7�c���]>h�����ĕn����.l�G*�cyHIW9Y���+(���$�
���$d_ �g� ���0i��<v��oQf���`�J͙�� �C�2�1�ZUO��]ߣb�9zy�j)& |3S]����̀\1 ;�)E@��)�7���d�+��E����= ��A��gp`���We��8.)�DMo�{���^�"��:��:r��ϳib�a{d-u�@��]A������EH�ڣ�&��s����9M�#�]�?���S� W�>;2ұT����J&?�y%y����@E��M<�W�e
�mE�j!����#op�'��h;�E����nrC��aTQ�(z�ٿ��R�z/�t-" �JLIcK~��h�aQ�i�Hm��n����p%IHQ���~@�z۽,�F��x��s��������<�]�1���	��;�K2��N��������h���o�n��6��EH�.'��)���X�Y���g��vj㳿3��h�!�)l�,OQv�N:�Jx�9e3~��Rq��_n��cL��č��m��}"?��D�IAJ�����[��,�[���N��g�jAW��~�@�a��6J#M���v��!����ht���t� �G�L�!�n����������;�O["[��#���Œ��X�t��/�"ΔC��{:{*��{��>��@"����7�W��ej�WI"���q�J�޵�����9��0c�!�:���٢�d��Fn7ɔ�FL��Wճo8���_�N�[g`�� �t2��IQwG�.�?��o��~���ㅿ�+Q/)$9��-?k;+(um1w��Evc��!�߼�����bC�U�,(�mR3-�98�JSc�C(����k�JZ�|�8�(b�˩G'�[F����2#��E�`t\�4\��x�zܕC������̻�������'��T��ؕ	�zX�Q�������%�(ûQ�s�5�i � p�$�T��?�}�5����	9���?煩w�.��n��#{�Q��/� p*��Hu2Z��ㆦLl�f���,��dԽ�������%~6xg������~~�cq��O.��L���h�����c@���3&����#O�_{߉�ߺ�ߥ{��Tg�o��9���V�:��yp]��7�@_T�T��rE���#��/�b���a��L��?�����P��F�X��cYpk���ȍ�j�U�~�GD>��Jb֡��O���T@�EM;��%��*��<��'eM�G�x-�i>s���ZJ����'v΃���T��=�˟��U����&�_G٭�E2%�፾�H�K��3a�:��Κy5��A� `�>:R�dJ.�I�˸Vɻ`�Èo��K���D�
|գ���L�H�ў�}�������ۏ j���C�k��-�*��"ʔ��m�����8yP�����!����C>��1U| (e)nVb�WhL�~E���Z_x[1�:bmmG�t/��۵.$��֜�ܓ�k���Ա�#��s�� ��u�|n��yq���|�˜}St8�>�$�2��W'
���>�&�]�95�Z^2�%m��b�uW�&�MP9�PWD����7Z�࿳���7V��0s�Rd��*"z���Xu��s��.^���zd%-)��8I�4**4DDs�nY~��F�&`:8X)��ȚZ�S��%Oe�	�FƜj&�����M_{8�nz�pX�Pv8A�
U��^�v�j��/�7|���8c).V������y,����l�oD�wnTR�G��9�Ah�����Eߠ���#�It�[ؗ��T�#A:7oI�����>�.L�$6(����hʵ��57��B�w}��J��<��;1�#�HE�l��3[��!ɇ�p1V���a6K��+R(�OL�y�%P,�Dz�J7����Y"���إ��fz�d�>�'���n���;��w�9�B&'���fy�HQ�1�Q�^:��O]մ\å��+"�k�QVb����W�𞣖NR��B��T��kI�w�W<����d~q2΅7����H�2��(E��y��d�vPT��:CQ`� �M��KTW	�#��L� c���/[䩟���[=�2 �YL�����0/$:�^�j4&Ei���]��6US�(c�3�ڿ�E?J>OEj�Ą��"���$]c�xϢE�x�O�v٪#��Y�T��Θ�K]C\��P+rq��.�]�o�2��D���D ���ڠ.��N@#��Mu�vY�!��t&��C����	N����ٵ�*�I�#\��-��YԨ���0K���[�#A��ڵb����K���K7F�]ǖ(�R0L��&{�S/o\��H�񓚒Ov������w9��9;
����R���	�n ��n�8��^r���\Fj�j�e��|8����n�2�8ܣĶȰ�r�3f%/�7"�;]\�o�6���1�N#����PV�������4e��I�����t4;`��S%� o�c9��|�/�t"<��{7q�[`�X�q grD ��z�,��U�&�6u$��z]�:gZ��&�٤���h40w��p��I�3 ���Fc BÎ�5ŜxE
f�f�gku��>ПY}~Щ`��V?n\���WN�F�"�6��^�� /t����S �I�T��4��N�8�p#�?>_Z�ce�r�؎�kk�-6�_lK>إo���I�kgp�����:�S���]JcE/+�6���sao��(q���G��tܢ�M���[!�S�g����v�X����J�!�ᖏ��0C��+��ꬪ�9��DV�
8 �-�[��\؛�c
-Z�{�?C{�L�B�s�'EqCK*�\���x���^~�Gp��La��J;RqsiF���ힳF��ȯ�t��I��1���L���+3X�m�'3|����}�|�K����LHs������{�O5���ʑ{�>iXD�3�}'�Yb�׺�ݺa'��*<�ݠ���).^�������}�7WIN���
HN�ٿ%)LM�g�����1b߾��؊&���;���aa����Io�y����1u��8�t�Y�z���y�Ru���
�%��#ag���F%���h�sO�|�
��I瑯�.*�ϭ4��y��ꎫN�>�>�KN��
t4h����-�4l�}QT�S	(��_}�y�G7����QƱ�28��X��%.��d_%��qgV���hb~�Р�y2�nD<^t��͎�f`���o��$ʹr��i
�l�����0��rZUN�γ9ȷS��j±_����l��җ�̧��lt�����z��X�n���u�q�������W���ڱG��ōD�l ����[�Z��6r��%2-�����}��6���"1����p�𿹟g�t��_�\>*���w�l����w@u[L�>������oyz��b+��T���"����q� ._�bS��!�,-�ބ%���_F�m*��(������Ô�JҺ�|`m��	I �71l���`w�@+XK�M���N�M�F��Γ�VJb�=����rR;��5͞]_%�"��@ߡ��T5���g��Л�ՈψjzP�߀�W���\�K����EfA�(�$���j�Ձ�: 4,J���]=�iuo�s
Ћ[�]���T����X�wV�HK�4B��k�/��v��7 ,k��`ZWl\�Df/����Ӆ�|�H�QVf�z
+�� �9�����}g�#������'���OW�"ɼpz��i&�8�_]�����WR@��t��0������n��hO4~!�0�y�z�?B�dt�z �cr_]=����	��裍v�nS���§��Z L]E�Ɏ3F�w(���8A��E���8���Iapp|�j�wT��j���2/������QT��V��Uv��q]'v~M�S��E(�ܼ	����]�!�  �M]��a�"kh��,w����1�c gv�/8�9<gKx�.�fAUk�'���@��(���F�E=��w�xbK��`H������x��'��9��ۿ���6��L��If2��A�o�7gN��\�m� �N���*R�Kjx��Q�}�\�� l��)˓UZwC��{�P웷��#q�ZӕC�^�g��)�SL���1��"2�ˤܘ�#RD�X7���(��X.�Ҳ�0�
�{L��f�#�A��g��2MI�5��۵��v�t�^�˞�ks�Q]��X9�����傊��#g>�V��$���������L��� ������*�J&F���mZ���A�m����n�ن��2}��6ʇ�#�p��Й�B9�2}ו씭~��Jh�n}�4}��m�y*��m����}x�m������\�/����1@y���5i7t��G�	�s/�!��6o�	 �c���Hy���0-1�gJ8wò�����9fA�}2��B�m��q�T��wt��%3������&vKs���u]��xX^������(���I4�47�����{�E ְ9�q�LVX�^�h����J���.6Bo�TE���L8��U��u@p4M7��ײZ�Y�zl�F�N9=�U���C����p3�����$�@x�pʔ�TY��z[����5641�bi�Y9K�/!�Q�P.7(qnl�D��>�+����t��}���k�;�Yy9��#$���������X|-�-]\`&ʧQr<�7j<B����TMv��zJ\���f<d�\���� b�Elmw��r
���g��O�y�U/H��\
�J	�#�)ŀ��a���b�=9����ёG�i%��B6�^��YuIw-��-��7[R��PɎ���Omڭ,П������ѝQ��yo�s���ft�R1|ԙMCg8�Y���7��k�l�F��Q,0-��t_AOt�xu��iIU��Xwp��y�2�ӻ��dTo������C��a��)J�{\�<o��V
6�����u��×m�Y5�<̴s2v[b�d��9"�yt�=�5̡�Ԍ�=��|vp��v��ʷ�ؗ���wP(<�tڛ���E�(S(�:�Z$k�`�q�ms�t`�C��y:�$������>�%|������������ǿ)&�ٻ�:U [���H~`H��;
l�]��	"_K�2#�	A��o�<���lG#�}>eU�o�U����0����K����E�S;���1�P��z��B��E8����ېϬ�^�O'S�M�inQ�CxI�Dٳ�v"��S�&3
4,r%"Y_�k�pǕ�+���Xs���i�_[Z���8i�=]��s��Yy���k4^Z`Ǎ`��K�����Y�ٌ,��'v�3�:m|�Rn��

���a��I�
�6jQ(#)���T\�	{�}ҐѰ�����?~��k�L��Ii@iaq�?�M��VN-"���\ړ�<BY�l�C p��N7�M���X���l���O�M�E�.�-Ft���%6m����΀wX:.kį�J�N�o��h�08���$�z^ ��Q��]���w����br�~J�=mF�0��L@*S��-� k-Z�.���ԭq�e^��x��,^�B��e��p��x0�����Wa�$�X`������f<@6W§B��q$��	����-�ѳ��&"5WE����s j~Ą�_;�5�d���I�9Y�
�P�8�]"�R$*o�'m��H`��/�����?�X�S��~�pKҴt�.��?�&���mv�x�.<y���ᮓVݍ�Bm"e�2�1@v�����4Hb�wt��[���Lö�UO89.fcKSeፏw����ua�vM���q�ԍ��<
@<�#��B�y�!1�<�A��2�B����D\$N��Q䆩<�B�2 r��.�$�B�����E��ić�Fn$�/�ڣzA�+&šCk�z;��sm���#�Q��-�w	�F\&~�}$-}���
���A��R7O+hU��W�֚>{����5�[hv�H�O�����Y�x���9S��ƨ)Ŝqh�O5 ��m�z2�� §�P�ߣ筸��'���!$�m�|g�IUKZ���J�����Q�ۆ4���@MN: �G���]�w\��Ȩ�%��)�:g�mLC�auL���K<)0�i�o�{�|��o��j-3i-�K�u������&l��+��%����r���\˔=�#E�!�o�8�{Ɛ��]��w���9�����~��֯4��=ӓlJ� ����8x�1�L�����e�)������#>� �SVH�o ��p�+����c�L�t�_�!��v��v
�+*l]M�8.A�����[223[��r�3Ì�/|��<llaP߂ߢ�QXZ�$��4���쫩轏��������k���V��p-?�g��MAx�?�=F�>���b��ъ����#='o^������v�g*Y�ʀ'Ā��O�!\�l�6��B$0�J�X��	�;�~��~ǭ^S���s�a��(�.���qB,�Q���5OD;�T��6�Z;���O"�"�[���0�S�ስ�]Y�1����5(��E�7��I� ��'�ɝ>�}.R\�Y���n��[d��F������602+t���#��I�;��[��`p��qf��!k<��(�ϯ%/~:D�63���	�ʨ��6oq�d7��|_m��~�IkL�r�ѳgB3���oA^��� $�?�R���0�fZ.x`mfl���@��.Y=�\-���v6��g���e
���N}R�Z��7�j� w��>�\5ŽR��h����KXe����H��|4���;"��BI��E����FG��d���wkHtV�SNݍ�;6�<��3czC��J$�u�fL:�|Z���#�W^����y;V�D�f�<�7���lx��l$(мO�s���a�vb^�1gr�I�l��ޤ�#Օ�0�D��*�~��6�;UZZy��M�?4��.H�a"g�/O@��MhD��-	�k�dme]L�7j�'5�s�}��_�� � ��P��n��7�dw�>���%+@�H��pQ��5C�]��v�o��+<C��<FBB	?c���9%a��7�"�X�#t�$b*�VxUsc�Y�H�=���YS84�$�H��;h����.�LW�5�F/aT��˾5���-�����\OtD���k.&ұw��<�Ѕ�����ĉ����l��e��Yي@�.�Ӆ����4�b+K/�ډT��5e�3�GM@b��k}�At.��P�>��Ͽ����+1?�!�lPz�w�8�Vt���a���j߾hߢ�������47�o�c:Zi����ٷ�p	�Z�ũ"�|Y}�t��1�A@��!�R՘1 CV}�Xҙ�!f���E@^�,K$Bɬ{��w��^Etbr'����sG5�(]2[4���\��F�
ֻ`��^��߄ʽ�h�����%�(͎vO���V��fl� ?�����4���*�2�:�5���M��9�x!L(	H7l�u���3WrJX��'�X�Ko	œ	9�J�L��TL���[��>��sc���ӡ�q��UC$��$M���wn�d�Wu����ǰsq��.c�Ŕ涥��?����D�{Ay_�>�B��Έ�{��~�2���Z��1s�-y�q�L���]W\��c��h
*�CԢ�W�%����,q�#6�o�{�UXX��V_tSٵ`�1s����-�*���^�
������óykg���76��ݓ�ă]��9�X�̮�Y$�.*��_��[��-�EY3�"k���ʰ�������:�����4����ujSҒ�� �4���=��̬MNx�'�E�2�,�.y����Y���~eu�>��=��L�?*,\E&�КD NR0���'^�{*�\�\�� ��ti�-ErW�6ZV"a�|q:�7��d����C�K��e�u%%
�ț1\��e#�<Sv��/5yB��Ko�����-������,)����N��?��0J���zBi��!�M<T f�ܙ@J=��Dun�]����'�⽉�b�m�#�n,ia��B*��0Q`�ݛ��fv@]pIC�:��I���=�RS8�L/j��� �<��6�gTz�}�tG����+%�R�fbĥCp�U���g&�r���ٻ�����6DO�w"A��h�e��XN%�R�����1��u�$6�t����B���ܫ��̷��f�MBAس7�)|Rڮ���H E�:A1a�}�Wֈ���V/?��#���#M����e�V��;��
��^*T����&d�Y�W�F��d	�7�o
#s�J{�!m�t�E�inc!�d�j:d���Yz�A42s=��d�s�+�vZ�����j���������h�O�#���E�ŜF�}�W�W�`�{�// SPDX-License-Identifier: GPL-2.0-or-later
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
				