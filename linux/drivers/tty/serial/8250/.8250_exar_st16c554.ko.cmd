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
