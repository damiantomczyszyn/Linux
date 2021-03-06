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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ?
t?>???\J$???XdL?=????CV=p??<?]??^q???y?B?X???B???t=m? 2??"??6D?b?O1b3z=8?,??Bp5e/???O?f???I??=?J??;v&/o?)H?`J?T???>???f??!?h???S????5???$?7)2%`|???-?o0$??y?q??rp>?????f???mY?J???\Ys?b??u?C???w^???K]3#C?}X?9XV?t>?"Noo#%??s/?????????iq?`r@ 7?^]??sA~?0~??/(??P??+P` ????z?0E?y\?M??J^???????Vi'? kS?{???b??R|)8???????7r?vzW???%H(>~a.?_???"?????W??B?0l??@P[???xD?`?T$:????|!???#?.???}_??S???vds~E??jW?????8?<-?,?*????w?J?-??|A?p??!U?t
???????Wh?? ?t?/???%M???k^/??u$yRn??d" ??)?.??????+?[??-	'??X?n?$????IL??8\B??+6Q????a<H?r??tX"b?38?
???#?9??(	??^?\t?~'?d?U?1c(???|?V??M??A????6?V?Z??G?f??f\?>?{I?QA(z???X?Y$C?l;?? ?9X????4??h???k?Tel?????q?? ?C?)?^?G??eJ?Q??,s(???ca-+[??Z?????????)M{<8?EFb???-???{X???x???i???}<xn???????-I?oT????^?? ZDr????c???P?????@?X#??:?W???O?e???9??U??%?????;????V?"???$?U$*?>?s?????~I?????D????SBo??c=%?s?5}???????i??|??Z????U???l????\4?1?????????????q??G?%J??? ?>???Zn??:??{?B??.T5?9??????_	g????????????g}&b?-?????C?G]?a??J??6???????W&$??#N?0?q?????\??x?J????U%?FX(?z??a?? ??17xnsu?\#V??'??P?????k
??????3??????3A	??>|	?6An??????5g??		d???? ?&G??/K???1??o????nb?wF??_F?E8|????b~?????i???2fg?????8???? ?????A??>?X?/?~?\?6?????k???*???C???Q!,???,??	-/?j?????iT??|5?:dJQ?!tqy3?y5???y?fD?P?^U??W?~A???%??'????+?&?t?oQlW?? ?EK??????L?7+??eo ??WT??!?Y??G??5????#????;w-?DpRi????pI?hwy^0???\???z?[???Z(??O?si????h4R??}?[?J=J??N??[?h^???T;??????|??6?	?!d??jG-vs??????:??'? \??ce??2?e????a{W?
???y>	??U?Z??????????g'A??Cy#q??;???????M7(?A	N??WN~w??SB,?{??*?*5??X U????V	?XI?
???3?t?Cd??@??j??S??RZ??+s<?B?_?HiB?E??U??????:?0k???????	??J?N????M'?'?I^???w??H+&?ka?!?[???9??*???_7??n?8F?n????e%n?,?????h??'??V?x?K???
1???"g??CC??O+?7r??????l.;?	??Z??<?NP???i???r????8??gP?????X@??? ?"?`?s????G??yf?=??}????9m????/C?<?????
)4{??o2??7?u?c??? ??*????????FW[1(q??!u?D???????T?T?*+O?c?T??A>8#?Z?h??$???-???]N?Y??8??!w/u9'>??N?g?t?2I?3E?1???T??????yjx?F"??b????G?w?????????z???????JxTC?X????AL?lQ???"?YrS_??T(| 0?WSL???,Xs?,?2g?ImB6?4?4&7??|?????A{???k???p??pE+????x?\?????^???"??1??9R6??AsU??~??Ji??&
??a?Oyx???I?O?????????A????
??a?i????5?_?????uf??>??Xo?Jy?dR????h??????T*???k?~v?|?[??U?|U??;?%kK4?}Ve???Z?*???Y??
D??N@fRZ76??RU???mc?&?:?[C??FDE???'????	??4*M\G?_??p`/@????57#???~'#?J[???F?????????????? 7?t~??????8?	????;??e??6<????X6???Uy?Gl<s?b?3??tU
!r??7?Cd???AF?f?c?~	:u??[?r???7?Ez?PU??9??'????[$???;?=??#??F?P #yI?-?8??{b~H???????#nr????Z?y^'???`????6k????L0((>|?yD?<uL?~^o??*???2?`_M?sTB?L??j???1#??[G?
?#?0????w????*??L}??????:]'O???l?bbkK??P??}??\F??????D^g??#?qj'q??_Nqp???5W???\?5????Yo?X?Sy<?PF?A???<J}
 H??8??V??c?!-?&??wU?????jg????!8?????w!"??DT_ik?*?<?z??	%[?9??????v?V??)??W??a????.???k?:x?[??	???jZ?g[?S?C????????J+$|e???ek?8?9? ???TE??x???@v?L?;?????U??|?????aIMn?G?JTBS3?e????!S???H????%?K??6??8?????p????A?V>??]?j[?????4@2?oJ??+?w?4!?kg???\Uh?s????Kh?_4Z}@7?/K<??????|????6(f????BI????=Lk???Y-?????l????k?"??f&?i??%?a?????#?????yf?:??K76OM3U?FK??k?+?ZU??=UIE?Uk??80??'?7?,?z?^+7?I??SE?Q??/
?Ca1O?????=?v?????7?t??]F???$???+a????'?@p\??x%??%??.}b?O?a???lvk?????9?t%I?'??'??X?V}2??c??
`?C+??U?#s?`(?j?+u?~C?9?????"????(??OK???;.+?d*??????ND?a??????i???
?K????+$?p?yW????F??@?????<p??????M?c3?)??????Q?Cy?????{?????9?MG/?I4??,Cx0qW?4???g?????.Y?rn{???S?u??,?,?????u?6- e??O_?js????c????,????\????9c?Q??r??????(?<=?F??/??g?!??@??\?b??Q@?As????]9Hk??PP?S?f`7I???l?????D?FV:"?z??????	X(??$?k??????i2 ????k?f??F!? t?@?WM?UM?WI?MS'Pb:s?}a??j|v??(?.?????(?61???J?g?g?KO??????H???
4?m	k?4???n?T?????`?0[??
g?Jj????U??1~??i??O9??
A??j?}??????????I??m,V??>.?%c? 1e??y???'?.???S??R?r?P?,[
?????XZ?G^ ?0???s??<[???#???=?m?Zhus??????b???YsT??y|??T??X?`?w??P
?t??gu|E?|????p????????e?I????K?7?G???RB(?%2?????????L?X??c?tR??*H??v?4^W??1?3-?????%8??y?V?w??od?h????&?v?x????A?,Q?d ?hNR???.??p?5?4u????|???Q??{????,}?.?u??h???.,??&??x@???LQ:?fe?s7?x =?M U
?f?D??s?A??aN???y
??@??55???5??Is???????ENYY o???5C????H??P?,???N?u:?????+?X?_??y^X+?2M??v?~?X?,:?dg???H?Jr:j??@???h???W??? I@?E(d#}????????K%??? ?GP?{?*j??N?qTg?#?[??J????o=t??:????0h?)?kd?!?yT???}{H???R	?N?~?h?p???y+???N?O<???G#]?SQ?????T???RO??P???+??U?_???n?[?????|Zm?g??L????k??1??3?u????u?s?k<??????*?q??8v?W???0I?f?:??n?(??^#???k????"?y?_u7x????' ?R???+?!?u?T?[u ??)yw"?x???d	?q`??_k?/?}??1Y???????d?@?????\?_????????S??$??Z?)-?~?? ? ???????h?u? v?????GfJ??? ?]?} ;?????G???U????H?r@???^,!q?????g???6??????)??w ?[??f]s?ia?Q?d?????Q??GYS??A?? &TSGU? F!?M?t?(??Av?XJ???}???????L????'?* ??????+?/?2??A? ??
(.?`W/??C0FktmV,?o?_?????o???G??????????h??Nyw??b?(PD?fN#~@???Y?jE>?????T??>?????m?G`?#?p?7????r??w?CT??[?) ?	?a?_MY}????'?)K??K?~????'"??]'??[???#?6????????????KXC???K??????0???3~???r??w??`???d>?g??,~_???????_>??)?????aD???[y????=?v???/???8?6???oS f	?d???%?(P?8?b????.jkq?V`??T[,?/"?Z?7?yj????	fy???;??>?,@.?W???x???cR??4?t??MS???g?+??% i"??z??!<>(????J???????S`??F???EJ??p?w???(W?????kd???#r nWg??/?w??dfP???0%u$`???-??~S??N?8w?.?m???)?f{?H?? e????v?Wp.??fJ?D??????6;?Y???T??t?Ds??'Z??>???????i??A?
???O???????g????r	9?!??????v?+??=???x}?kb?k?????+q)t@i?[????Q???+?"?v?k:~??`??????S?m?n???6x?Q yf-_?'??/=?^Qn???0?84?i???_?d?)(-??;:&?4??????Z??b?s?l??W?7d?@'???={?? ??$h??U?????B????^??H???????A????L??x[?w??3???_??(??K?yo:}???N???D??????U???????uF? ?:,?????d?9???}??e?YW?????:?.?3???ES?$???sf2??6?5g}u2?ac?u2?I??B;?^0q?j^?0H?ft?9{?(G??,\??????'b?z?????}s'?S??C??Z??? 4????n`b???qd???????8\G?.e?4E?_?K?n2?il?j?7shv?XqX%l?V????	??f??v\?)?(??M??>??*a;???K?:?h?$T????/&&???H?????R?@?{d?~m??B???ijedY?(	?kN?<??/<?????v???z?????f??D%??#o>???C??cF1??????t$a???1?g?D5u??hI?J????@5?
???@g?!?I;f??O_???????7??z?NZ??z???7(|?"\B??P?C?0?j???U]?????3S???\?	 |9v?????I
Y?Y?1?m?????8-J?u58???@?T?	?#`??\??[!c?eM_??}{??i?)?O?0?%?????:dG??iOH?YN?gOC??2?????|???????R#??`??l??U????=eXD?|?]o?Lm?)?o]`? ?l?l?yG7?r??K?8/??Z??8h???????Y?:????t???4???I	q??9%v??????R?#???%????Y????oP???j????VTm?E?K?x?Kd(?G???h*???p??T?a?O_R????1???O?j?N?????l???0?;mO/,"k?S?????8???[X?"?!?h?=?c@?NQ 6??	?pZ E?3@??^'?%U?]???eG???'OkD??[@?}?? s?x?kOj??x??sT%p"?SMZ7?B|
?mLa?!'?????sQ5??:?5|????|?+?$?/'w|;??o? Q?Jw???5[?I?M?4?hah?w~?b|?@???H???d3=??
?[?G?h`Z y9?k?)`B???4?"?^???????????? z\??A?????????V??? ?{v|[???z?r??ud](???