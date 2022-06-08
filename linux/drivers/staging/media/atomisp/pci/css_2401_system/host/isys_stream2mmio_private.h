truct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
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

	err = s