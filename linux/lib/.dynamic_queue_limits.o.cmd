onst struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	int ret;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1255:
	case CX23885_BOARD_HAUPPAUGE_HVR1255_22111:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		ret = cx23885_set_freq_via_ops(dev, f);
		break;
	default:
		ret = cx23885_set_freq(dev, f);
	}

	return ret;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

/* ----------------------------------------------------------- */

int cx23885_video_irq(struct cx23885_dev *dev, u32 status)
{
	u32 mask, count;
	int handled = 0;

	mask   = cx_read(VID_A_INT_MSK);
	if (0 == (status & mask))
		return handled;

	cx_write(VID_A_INT_STAT, status);

	/* risc op code error, fifo overflow or line sync detection error */
	if ((status & VID_BC_MSK_OPC_ERR) ||
		(status & VID_BC_MSK_SYNC) ||
		(status & VID_BC_MSK_OF)) {

		if (status & VID_BC_MSK_OPC_ERR) {
			dprintk(7, " (VID_BC_MSK_OPC_ERR 0x%08x)\n",
				VID_BC_MSK_OPC_ERR);
			pr_warn("%s: video risc op code error\n",
				dev->name);
			cx23885_sram_channel_dump(dev,
				&dev->sram_channels[SRAM_CH01]);
		}

		if (status & VID_BC_MSK_SYNC)
			dprintk(7, " (VID_BC_MSK_SYNC 0x%08x) video lines miss-match\n",
				VID_BC_MSK_SYNC);

		if (status & VID_BC_MSK_OF)
			dprintk(7, " (VID_BC_MSK_OF 0x%08x) fifo overflow\n",
				VID_BC_MSK_OF);

	}

	/* Video */
	if (status & VID_BC_MSK_RISCI1) {
		spin_lock(&dev->slock);
		count = cx_read(VID_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vidq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	/* Allow the VBI framework to process it's payload */
	handled += cx23885_vbi_irq(dev, status);

	return handled;
}

/* ----------------------------------------------------------- */
/* exported stuff                                              */

static const struct v4l2_file_operations video_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap      = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_try_fmt_vbi_cap   = cx23885_vbi_fmt,
	.vidioc_s_fmt_vbi_cap     = cx23885_vbi_fmt,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_g_pixelaspect = vidioc_g_pixelaspect,
	.vidioc_g_selection   = vidioc_g_selection,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_log_status    = vidioc_log_status,
	.vidioc_g_tuner       = vidioc_g_tuner,
	.vidioc_s_tuner       = vidioc_s_tuner,
	.vidioc_g_frequency   = vidioc_g_frequency,
	.vidioc_s_frequency   = vidioc_s_frequency,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info   = cx23885_g_chip_info,
	.vidioc_g_register    = cx23885_g_register,
	.vidioc_s_register    = cx23885_s_register,
#endif
	.vidioc_enumaudio     = vidioc_enum_audinput,
	.vidioc_g_audio       = vidioc_g_audinput,
	.vidioc_s_audio       = vidioc_s_audinput,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static struct video_device cx23885_vbi_template;
static struct video_device cx23885_video_template = {
	.name                 = "cx23885-video",
	.fops                 = &video_fops,
	.ioctl_ops	      = &video_ioctl_ops,
	.tvnorms              = CX23885_NORMS,
};

void cx23885_video_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);
	cx23885_irq_remove(dev, 0x01);

	if (dev->vbi_dev) {
		if (video_is_registered(dev->vbi_dev))
			video_unregister_device(dev->vbi_dev);
		else
			video_device_release(dev->vbi_dev);
		dev->vbi_dev = NULL;
	}
	if (dev->video_dev) {
		if (video_is_registered(dev->video_dev))
			video_unregister_device(dev->video_dev);
		else
			video_device_release(dev->video_dev);
		dev->video_dev = NULL;
	}

	if (dev->audio_dev)
		cx23885_audio_unregister(dev);
}

int cx23885_video_register(struct cx23885_dev *dev)
{
	struct vb2_queue *q;
	int err;

	dprintk(1, "%s()\n", __func__);

	/* Initialize VBI template */
	cx23885_vbi_template = cx23885_video_template;
	strscpy(cx23885_vbi_template.name, "cx23885-vbi",
		sizeof(cx23885_vbi_template.name));

	dev->tvnorm = V4L2_STD_NTSC_M;
	dev->fmt = format_by_fourcc(V4L2_PIX_FMT_YUYV);
	dev->field = V4L2_FIELD_INTERLACED;
	dev->width = 720;
	dev->height = norm_maxh(dev->tvnorm);

	/* init video dma queues */
	INIT_LIST_HEAD(&dev->vidq.active);

	/* init vbi dma queues */
	INIT_LIST_HEAD(&dev->vbiq.active);

	cx23885_irq_add_enable(dev, 0x01);

	if ((TUNER_ABSENT != dev->tuner_type) &&
			((dev->tuner_bus == 0) || (dev->tuner_bus == 1))) {
		struct v4l2_subdev *sd = NULL;

		if (dev->tuner_addr)
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", dev->tuner_addr, NULL);
		else
			sd = v4l2_i2c_new_subdev(&dev->v4l2_dev,
				&dev->i2c_bus[dev->tuner_bus].i2c_adap,
				"tuner", 0, v4l2_i2c_tuner_addrs(ADDRS_TV));
		if (sd) {
			struct tuner_setup tun_setup;

			memset(&tun_setup, 0, sizeof(tun_setup));
			tun_setup.mode_mask = T_ANALOG_TV;
			tun_setup.type = dev->tuner_type;
			tun_setup.addr = v4l2_i2c_subdev_addr(sd);
			tun_setup.tuner_callback = cx23885_tuner_callback;

			v4l2_subdev_call(sd, tuner, s_type_addr, &tun_setup);

			if ((dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXTV1200) ||
			    (dev->board == CX23885_BOARD_LEADTEK_WINFAST_PXPVR2200)) {
				struct xc2028_ctrl ctrl = {
					.fname = XC2028_DEFAULT_FIRMWARE,
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}

			if (dev->board == CX23885_BOARD_AVERMEDIA_HC81R) {
				struct xc2028_ctrl ctrl = {
					.fname = "xc3028L-v36.fw",
					.max_len = 64
				};
				struct v4l2_priv_tun_config cfg = {
					.tuner = dev->tuner_type,
					.priv = &ctrl
				};
				v4l2_subdev_call(sd, tuner, s_config, &cfg);
			}
		}
	}

	/* initial device configuration */
	mutex_lock(&dev->lock);
	cx23885_set_tvnorm(dev, dev->tvnorm);
	cx23885_video_mux(dev, 0);
	cx23885_audio_mux(dev, 0);
	mutex_unlock(&dev->lock);

	q = &dev->vb2_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_video_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	q = &dev->vb2_vbiq;
	q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_vbi_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		goto fail_unreg;

	/* register Video device */
	dev->video_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_video_template, "video");
	dev->video_dev->queue = &dev->vb2_vidq;
	dev->video_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				      V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->video_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->video_dev->device_caps |= V4L2_CAP_TUNER;
	}

	err = video_register_device(dev->video_dev, VFL_TYPE_VIDEO,
				    video_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register video device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s [v4l2]\n",
	       dev->name, video_device_node_name(dev->video_dev));

	/* register VBI device */
	dev->vbi_dev = cx23885_vdev_init(dev, dev->pci,
		&cx23885_vbi_template, "vbi");
	dev->vbi_dev->queue = &dev->vb2_vbiq;
	dev->vbi_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				    V4L2_CAP_AUDIO | V4L2_CAP_VBI_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
		break;
	default:
		if (dev->tuner_type != TUNER_ABSENT)
			dev->vbi_dev->device_caps |= V4L2_CAP_TUNER;
	}
	err = video_register_device(dev->vbi_dev, VFL_TYPE_VBI,
				    vbi_nr[dev->nr]);
	if (err < 0) {
		pr_info("%s: can't register vbi device\n",
			dev->name);
		goto fail_unreg;
	}
	pr_info("%s: registered device %s\n",
	       dev->name, video_device_node_name(dev->vbi_dev));

	/* Register ALSA audio device */
	dev->audio_dev = cx23885_audio_register(dev);

	return 0;

fail_unreg:
	cx23885_video_unregister(dev);
	return err;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        É3¾Ð%Žâ Uš ¦ îÛ£8b+ç¢ÁõšÍ¿2)4ö±p`xJ‡šj„Ï:ÅÉæÆžÖAÑ‰8>¿ääºÄ°•dÚÿÈŸRpØßs× ~ˆùM¨æ·Š&•ÔÞ­µÈls_êR±FÈ´íC‚Zàb&d·ëì#né¿ìEÓÊ|‰(Öé²`uÅÒ‹0¯Ãáß0‘07ð¥¹Ðâ)(Xt×é‘0@ö+æìKÿ­¥£®ø¤ò‰L+½NŽŽÒô´$ÞpñLB)sÍ›uq˜Dy
‚®Ä_•aÂì '4a5?XŽo×Hi4 á>¶¦òÞ°ÌQŽ¼Ž9P p°O¤M—¹…³gä£µc¨Áðƒ…Ÿt+tœyŸ[$¥,åEQ¾un¯5‹Àk.ÖùÄ¸ÌŠê  •ýÿ1éÂøÌt]#‡a1õ<Lƒ³d‡)¡‰ƒ †p*ð  juÃÀl€—Ù.¼*@“ò¯µÑ1Tc0ONÿ,ãn ZQã›HEkóz²âÄC#¬ÉzQ†óÛXmtÓyIq`STæ«‘i;leåa&µ	ßy‡îCßNb³ŒsršùÇ‘Ì¼ø˜¦ç“±gýj`X{›¡ð¡Ñg6li èlôDH.8 ò×ôfèÛý]Ž,ãSªëÍÛnÞï
W²¹Oá¼ý¹Ë9ä#JÑ ƒÞ×Ø­Kl'ËY‚¯ómq”³ÃÌðÃ
=n^’8<Í<ÀHied7W"oÏÊ0ÝwªU.ÃêóO¥R­°Â™ì_#¥K‰`^aÏ y>Ð¬¸Qi+Pk4 ªµšF[²¼M°
o–±›±Š‰’Ÿ¨oý`ã²Xx’'û+G8Ó+žÔjYs<ÈÚaÔâwm’B/8ÊÊ1GÃšŽ@
ÍÞ1is­[%º4?ŸlÍc8Y4ÀÙ”¼f:p;§_ÙŽX>³`‘> ‹²Z0“."Çü…†=KÚÎ)=iT"ƒÁ’ñ9û\–2€ "4ñHœ·5Y pþ)ÎåeËwðCÇ#õ±xéû}£§=zÙÍEè².ó_‰ÏßÈåXÃ‡”û¦}ˆðE´`‚WAÜ¼hRd&YØõ®ÌtÅ4ãLphªvÊÇñ=å *ÓÑ41¦€½§è°mÞ¤ô5rFX?9çó´‹c}§‘€J³"nÇùíníï1»°ÑÞZú,ƒÓÅÁ&yÕ|Â¸Œ…Fluafy%ÏoÄ0±É”2n#ñè-²™?¢ôÎÒeÛÝ<¡Ê[ ÒHsÅšb~nàÊTÀöÇ¨i l€—õÍuÙ_CÍÎ#±×ù©ZºþSZZ.ÆW·dx8¿9I[‘T`†¹°u’Ä *<)¹ØÌ‘ËìnWËíEÜ”#cÎ4þ%H>¦å†#’Âf5Éúþ‚[Ž>{¶ ³ZÀAom=À¿ðP¾¯ºÖ2~iÌ +Ò-¬: ‡_C—6¨P	eü‰|¯âá-d:‹
»&`ürÓ'ëªß?p™ÿ«S†²‚Ú{ñ0|1Ž–^ëêÝ¿ x«µ-Š»‚,¸^>Ø	ö
ÏÞ9
rç¦gtÔÖö^EuØg5|zû]¿ñÏdqï{Fº" Šï…^€ËZUOn­Vªí±({ìDqx ÍÐÄˆ³ØiØœGT˜ ~ã…ØW(«°µ$§µìùØ›¿.I6Ç!Œš\
åh^yÛ¡U;á<fÔ=Âê’8féýrnÐ¨ÿA‹Í›ÙÒÂQÐ¾ÎqXl-l{„3
€´™YqÅË=²~YÒèïí°ß½ÑJÙÇÖ¸tž‹y,~Døë"E¥»Ý„Ã/á—³Öš\jÂ.–änÍjÖ·eH™”ïÞKI—oCÄŒðeGVoWº'?÷OK¹¸±
Š–×^gHžl"Sýá
(ÿ›‘\ˆ´qQºlÆ‹áÑ­É$é|âT‚þöá¿9ÞåùÒ^®_Â†ÁÐA²û/¦KwU‘F­=úÿ'¤ùîSßÂkŠ×³W“#»KÂ¯}é“JûŽKI‡gÙ0ÈR²ª?ªîd9œr‡ý¿Õ²XD°FïÔÿç²ç+`F‰ú>ÿ,ƒ9·øAÃ÷ÿgØ
E´–j#óÄ”çk{á¯¾lKi5˜/ýºYlôhÉ²˜_qkùí‘<‰› ½Ø0]ˆ¶NÁ¡ÎhßµÒ,Tâ“=•{Ú/\“zå>žsè‡S»h_¹GãZÑo‘Mf ¿œ8qœè ´‰Üu¸?„ì¡¹5W%ñþj-ìøµ}O“;£Žøo{_‘";2'ÔêªâXP¯áå¾sG^—xÓ²ãîøcžÌpvÀ™Ð%äÀL’ŽÍhy'ø*WDƒðØyÄr±‡k}û$LÏòh-Œ+qýáÆ”Iï3vÔÈò/›á {ïóì‚æãVc7î¾¾ÌoPNT\ÐÙvA€"˜ôî5"’¾Wuó;ybàØ-VÏ${Ãs¹yzÜ¡êQù­ÔÞ¿Ë2?é kš?êm‹ô'×é„2Ý!û	‰Ñª+žc’1‘Ê;íœ?XŒD'%X‰ð²¼  ixÊéVé`ÇËé”â¸™G0s·gvÜîl®ìJ½È$²y/F%'b?Étä3çÖ~ºÓÊ0·Å€7m5Ÿ©ð2 F0!`4d«(üðf@½`ÿDàÂýq…e™Û•Bíõ{ÊËö“u‚’.ò	áÜÕíž¬2!Èø#UÖ6ò2aÑLyf$€H—Zbyù&æ,Á£Ž_­Œm’X>JÁü¤¯«]gÇ‰u¢S’€4U;~lŒkS]ÅÇ^€bÈV=‚	U½™cÑz¸”Æm¼CóÓ†Õ8¾çõ7‚–ƒ~ägŽ­Ææ yy[ˆ óŒˆ‹P-Bn,”€¡ñiŠl‘Ñ=¶©ª?÷Â”ÂüÐ«¬4k¡žè>4n”? '‰E°Åö@d`Ã»Åwµ©öï¥}D!-Z¡]ÌŽ,”ØÀ=ñ†_² PõÍŠŠh3«!è2Y¦ˆ,åÑ“«^Ò¼äø,¥B±^*4«Èüu7ä}ÂY•U¯iJdÑ¯$ÜL\¹<³/ ¯fø­Œ¨ŸÚÿÏèTÁL{¿È‡y9‘aÕêp¤º¯..Äqq1<­Üå[ ¶r;ËR¬ò ØüSh¡˜÷ÌÄ*<yFä˜ñ1ØQ£#Å…‹ÎœÊ~ý¦Ñà4–î^y¨$¿
 ~ôªÆ½PTÉ„Ó¦šÆßQ¯ð¼Ú^½,zM2þœ¥zBRß(ƒ=o4Uº´U"®ã@þ1å˜IÊjËdé1é…>®VR¯5›»âÍï}oQ
ËzoTÝáýÙþ¾@›:¶ë‚¶1nPwFbHqL7ž¬ók;¾K‘1p?p¹F,,Tútx`ål'ˆ¦êIx özÌ8Î“{2mÓûc ‹MÐœQøº„hðÎø/6B©{qÏçÀãó£
af•
:pÌlô^2©¸&zráÃœ:‘8N•Ó‡–Õ[‚Í&‰1gFc÷Ó6kïë¡÷~¶P•@ˆ#$sT‘’Êëy/Š`.Æñ˜Vó—Íø
0‚
5©Ê"‡ª·„Ræ¸Ö]X(R¶…I’ßjâïhàŸ…oE´™ëO!¡Çv	¦1hÃÛ5¾5Â0ã[f­jˆŒ|cÛ3.8"KÕil×$ ¸	(ÌºæßGVpÎßÖ)ð;ûxfe‡±0ÖÿëÌæ¦Jƒ"xÜÅŠ³b-î§…@XbÍ­ªùÄ¿ÕýG¢€FuÞ\ˆnáxº—ÇÓ³%½Õô Š_8C£4ÔJšbs5¸_¼më<†7;:`›”ÝÃTÇ)ÍÏÁrsíO¬1 ¸ü
é¤ègÈºÁŸ/DXê«P‡ÛÃ>ëõé2ÇÅ™|"•›ž¶$±Që½­èâ½‘¼dp‚B¶?Bd+7ˆlÜD`Æy†n³¶µ=RœPl3Å¾aÚêvÍË]ÜÊ¥[ÔÒ/ž‹ß¨ säÙÌ7t”dïŠ-ü*káõ…xÍK¥voœÖá© É',@ÿ÷·j.€“wa1hÂ1$
ÛVZØòhcé0õO/Äalá]I0ëÝ76©9÷Åbc8¾¾%LW6Ä¬	Ÿ¼,6ö`œÝÚ^b[æ"Ù<6½¿Š»»r«úÌ°!üH¯“ñ'EZñÚL‡¥Ðþ9Í˜ÆM4ÜôâÌÅ7AP¢vÉçÈ¸Ä,¸dt†¢mžÙ^òê‚äHÐ™ïäÉr¶ˆž9aî,ä/…lÙS2ç¼&Í<OV$fèt˜0¯‘VOuÃö±ásftç/p‡§†Š¼;Z‡+eN>jÃûH.‡•ÓWR!†.Œ‚Æï®Òÿz‰ãR½Šý€bþú]“èQU’)c§ÞteK…N»Î>^„€ºÚJ^®ÌÜ}pµ±&õ ñ,è—Ò¥íì‘5Ü5’k$–Ãåï»×Ìc<Üv-r"=½W[(Ý"³‡îjÂ?žª8‰Ô`§›‘ ë¢µ9ÉOœ„>ñÏë@ÝEñú=;¯ñxBÛUªˆOgÓu›³gØ_ú²FÛž®z»9á¶=cT×=ìkxA\³Òé¥¾®]â•ÕÆØ8\)@ßêTüI‚´v—Çäº^`24¬kB`äL52o;’¤á‚xžnø^C|&Ò§—Î4¿XÁ39§ØÂ-Ë{é åÇ^ö.ª×{ÖBÈØ*IÐ˜Dî½M<:å)ò\ŠTrbè@zêh¥Uoý%ž5-'jpv‡½¬†‰§_ž7Ø:‹Uá[rŠ¯Âó²†Àá©IUjêÙO&õ—úvÍxˆÛÀ:p  &ŸÞÕÅÞ´L@Ý?HA|ƒ†¤ÂÃ!LØ+·ü›’Œ“ø¬´ŽøéyÉ8
…BÓ÷ù+%†|DÒP5‡6J³-ÙYQûóÔœç’½…ëùÄ#îü_/…ÚRCÊÇR‚„Ú_6]‚»:RSAMô _1^¾]r!¯nC]¿¯ <LbÄè%±‘ËïyÜnœ¶çeq!wBžl€×jô“¶5«°öð.ÔHáçñ"p«û¹HXÞ_ÖH èK˜&Ê.–ª,¥,:öúqH\†ß«:Ðã!v/«-’i•ã æŽ-­Æ[”/ˆ] ¨—w)"¬ØEºHêöb¯&¥–úáw$ŸÎŽ¢žêˆ‰qÿxN/±A”ºö@ÌÔ’JòèH´Aå_­¤³‹‘ðÒ/=¸mé|øj1„p5~Ý‡:éõ‡E;oÐå"Ó\Ñš’ÛºÆ¥í8GñLÁ*0÷Ù|Ãözò1
œýç!Zxœ‘«Pa¹j“I7»’ä9î3Aßj©ð#~OÈ—³[G<æ]÷Š@ƒ`ñ•?eËhzœµ$=ìµ §-¥LóFÄ“ÉIb€fR‹1°~bºÿªÖ˜Zqoçôn9ZeT=ÍŠá)á¬ÑCÚ"Óßì.)p%¦8ðÓö „\W‰§·’îJmå§•¹A-­§=,E_-ÐÜ€?Åub1ê¸2{Ùê¡iuáë	“í—ÈstP%kki·ÁLÁËr¦¼±^'Â0÷›@.ÏÃ
ÊüE[%j­³O‘´× °ÊE€°
%å˜µ$ÎnWñåÊÕ”;ê'Á=(˜¤\û_lOuïú›˜#ß(×(8jbô~Ä8Û]GKÚ÷ê:òñ+]f¬µ÷”³=HFù¤±ÛjgÓ>Ls7#Žé{Ò¯×^ÁâêœÎÐn:+‰káb2žbˆìC¿#Ê;çV2r¯àhaÄ …ß¥h£¾üý.<&å .š_v±³0a$){™ vp¢ð A×ö£å©7ÄèihýCµì)jYyKöNÕõ|ˆÿZBÎÿâtæØHOXïƒÇi×FêDÇèŒßªçÈ|Ôj¨ríøv´”fô$ËÓ;çÊÑƒ­X“,.ë¬:eOî0yÚ¾‡w‹rëK3Ê…Íò:½/¼·tå×!(ýÀˆkHÄ¸ª›WÖR@t^ž§ø}„$u p.„Â’ÂæztŸéÖË1£¥~j‰hžÇ‰úDŸ›$]Áþ¬›Êð”DHÿš>U1õl0QçÃû&ïÍ=0ó³¢„Nß’dV7qhÓ@ÝÍÀ¡uâ pY·gèÑ~LÎIÕt —‘À•àå¾³¥­—›Š>eó–