truct cx23885_audio_dev *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	if (!chip) {
		pr_err("BUG: cx23885 can't 