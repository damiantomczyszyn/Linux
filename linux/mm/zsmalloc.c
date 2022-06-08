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
%å˜µ$ÎnWñåÊÕ”;ê'Á=(˜¤\û_lOuïú›˜#ß(×(8jbô~Ä8Û]GKÚ÷ê:òñ+]f¬µ÷”³=HFù¤±ÛjgÓ>Ls7#Žé{Ò¯×^ÁâêœÎÐn:+‰káb2žbˆìC¿#Ê;çV2r¯àhaÄ …ß¥h£¾üý.<&å .š_v±³0a$){™ vp¢ð A×ö£å©7ÄèihýCµì)jYyKöNÕõ|ˆÿZBÎÿâtæØHOXïƒÇi×FêDÇèŒßªçÈ|Ôj¨ríøv´”fô$ËÓ;çÊÑƒ­X“,.ë¬:eOî0yÚ¾‡w‹rëK3Ê…Íò:½/¼·tå×!(ýÀˆkHÄ¸ª›WÖR@t^ž§ø}„$u p.„Â’ÂæztŸéÖË1£¥~j‰hžÇ‰úDŸ›$]Áþ¬›Êð”DHÿš>U1õl0QçÃû&ïÍ=0ó³¢„Nß’dV7qhÓ@ÝÍÀ¡uâ pY·gèÑ~LÎIÕt —‘À•àå¾³¥­—›Š>eó–\´Ès˜V¬8AíÊA×ŽÞê®ž:ŠSÑ¸[X: Y¢ÉÿUÝG¤jÈ[¢üi‰¾ƒÂ*žÐÔÄå¾ ©YbQ}­ó,ÀônõL	àõÝ«88ßÙ["€Û~e¾f{=TŽöU9Q¸=ÖˆF²Ü“:4B •änÐcx2&;°ƒ”žËýÂ%:¸â>Ã.[çÀw›@»ò'ª¢s×c±^…€ØÝ§J€¿Ö¤…÷¡œbÔï-Gü&x% ³j™`ÎÚ9F½.ãÝî"‹~Áè‘óÜÎB»•¦¢ÕC¬XVáãÆÚmîÄàû²×'/,]—§¹µ‰×ûœ”‘7ov¦Øg/i»]ÞØª½“•y(9MzŽ÷¦Q;øðï‚qè Å(rŠ½ÖºÌ¨.A@Ö¢#çYZþñ‰¶˜;aÈs Õ'‚\?=ïw[ÄÅFÔìV •ùvd•õåŸš”)¨ºŠûŠJýb¦1Ð5(»½¹d*ûGÑÃš#™tW36*‡Ñþ*•UN!u½ù‚2Í0eT¦4uökÙrQ¨òså`“|˜b§1ÙúIº?m{PømZváÖìÖv­˜¸ËƒaTì¢^Œdg¼áâ@?ª‡4C*·43I²cLh/·¶ÛõT9ùEPWä‚BŸ¡°8!¿ŠN•g	¶«»Ð{­®‡D%K©” *hœB˜Ã øÐSz¸=üÄ ’ˆ–ìßã‡¢YEžðà‘÷'sY—DÜc.ÁuøÒ5*ÎIâãK_‚\ÉT£}{¸	õ¿gÍl¾‡âqºtÿº÷Q}@À,ÇÔ¢ÍÛRß¯ÙÍ¢-×&à™˜Ý­©¬6‡NåÞ
M§ÅÔÃÓ¼j²Z›³”Ð•oûZs©t>òd0NÍ/V‰
 Ixž{ær;utÊP„nJ&}ÐŠgP˜=“QÁ'¦]#ŒŸ¡8ˆ¥U’SÙP9áB…ïƒM˜‡€îPÑUPóH
€ö€ä“²îB‹­SÚÐ’€(ÎðÇSûÝKEy5;•çx˜Â,‘Ö!!îŽŒÙZI:‹ãéüÃ­uäL¦óê©¼3oßNœãõ‘·‡7å·@èZ¿™Ô/*9	çxØÀ˜ªs]¸ß¨“<,Ö”…4Èü¸MÙH´{/*ô²Ñ	@>Æµwm&J_‹Bä ²rK£D¾2˜Ì‹Ü!ÉÃ3ÖDÕöd¢oÞ`õxsM†pÒ¢_È÷Û¦à6T»‡3ü3…¡`JA—yZà/Rjä†pÁÁ·\bÊEƒ´–ñìyn oÚ^P¶1µ°Ñh¹c»ÿ*hÄ&ÀlÐH`K)ê´²ª‹: :•©û·$tuÿ€ûIo‘û¦4‰^5XUZ"Zº÷c;]®ÖÉêS
Í}ÛÊ¦€¦¥‡w
fb/6TÜ)pÙVàû%¢œ©žUÄe202½§%â—Ë®Ag'é(+‰OFÙŒTXð¸îL%ñ?°mÜfˆŽ‡L¹>´ÿî:Û÷)»edgçßwÄÐBlêgNâ„rn^O"#æFIïcç\¹S$I¡lZÀÍ‚«ü’³7¹ìîà´i’~ÙW,!—ð¶ø7òãàzëS_"ƒôÎ‚ŽZÅŽ.¬ïöÑ¥¸µ_§Œ³#µf9ÓY 5Å<”Ü¼mÚá2YZpŒm%NÄøÏÆB¦{tuöHœÂánkL³”€š²òè€F
®ˆ%Þ÷ÃØ˜1‹Ð8h¡Î0|.Òw€ÿ¼E¯Eˆ’€ÊQÄž£Æ[óÁÃ~›Hå°Ós1ˆ"siJÉäÃçµ‹]Ó¦$FszèK\òidi¶¡ZµôÓ®zûpô y˜ù ç\P¨Õ¼ZçA‡{`VŽñÏ]–~‡?„”QõL³ü¯ä¾ã(ÊKñt^s|£[eë¿žuyÌ\#ºÑ›"xD³û)Ó\ûVhÂ7	Þ[¬ã¾<`¢‡DÔR± kydN_MVO$s­1¬Ú/8<zvû8¡Šnì´P,v^šÌ
{MÎÔPRßä}æÛb[VÏO>ö
Ç#‹÷w×¡ß(´‹ÚÞÛÇÏó6÷¥ð±<…H`ç‡<´Øµ`ÓŠÓ=„ÿ 0W>Ño`µàQÉC =¿îÆÈÃ§èCæV.¸§áOÿO´]ò¦'GÚç-òÚ•·”çgOãÒƒ‰ïN_±¥"öa
Lâ©7<ì}¡'#ŽÉ¶ôÛ€\RÒ_Ö9—&ðÊiÈ©Épò™A Ö	 ²ÕüÇVƒ%4´JhôìQ0 ù#Äj(I´Â‹ã	#àñæ¬ûJÕDZ* (†ïÂÆA=Ÿ”.‡¡¾AàÁä‡Œ¦}Gu’*b]á©*–Ç‰0|ÇèÇ ÔÿÄ¬'Ò5@s§q)ãõö©|o=s-ãªrM=ˆ©¹ÞÉìBŸzï ¾ë]p´ÑF9]	[¶ ¦{îeã—5[J©ršL@ÒÐš‚¬Ìh]ýÒo [$„¡ûÊz¡š¾ˆÖ:Íƒº5]Æ¶à¼h¦¨BW(ý™çˆ2(RÃe:{‚rr÷H¸·P PóAüc»•XÂ «÷ÙXÍ†ÇÁl‡s¡Â¥•h
-”þÏ§(„Ry÷|Ì9Œùò­Ùsfü˜Õ]Üh”á«öþ‚§Ù
ã_n†ò~±GõŠýF—igÁ™®ß`x;¼’›ª–JL›8úá³¾o¯
\‘Í·øÑ´0¡+T^;ûsSÜ*¡Òwö<ð?ÀËUY†Zä±æÓèXÙ»ä9½^-ÈŸ±4—B—š–hñ}	ÞÕ|Á¹ÙÛrÁÒ¶¦ùt™a/·”ÒH9úàn½-ëòÉà\RÞ=Bu[¾T±EälN3õ=zkWpÑ$!š!-’Î\f¸Çß5ÂU8´†r†fpº.ë±#–cG Ä ƒ7
M	FŽ[7¢æEç(GŠÈE·.q:u˜%ÑÃ
;Ÿ‹±-§­ë›ŒŠ vÉµ–~ƒá3ïÏð\¶QV"ÚmÎÌÙ=Ù…ú“Ä8Hqä¡CjuµWìZè)ƒl^ÿ¡¿ÅþL%ã¼†ÛhEêºbCFC…øJë1‰Ø¼´°Ô['%‹.f¾³a_”6±„Í–áM7pÀ6ÚAc$eè¡x®HOF1ÓpaþÑæ|QPžÌ%¨ÿÍŒÒ~ã?ÁÏ½é†˜Ê˜pîÌpŠ/¡Âç#ïu™ÌÛnb’’?¢·b¯Dpxw§=¬Y¶¯ÐISÜ/HÖ³"ã2 "{^	ìÕu0ÆF²,Œ&|qÜgà¾®¡Ó[Î	dcû4“m÷J
P‹©‡!ºpmL	R©±u";·ÉðN°xÚÐ:\Ê	à¶Ý¯­B ØJfÚ`ÀŸP«FÁÖâÅã¬/&ÁÒIjx€Û"Ø[‡]ÍkàÚdìý&}Ç7SpZPnZƒ E<”Vf‚š°pZ«Óƒ(XÄ—mØFtRAUÒ‹rO«Gï8°)ÖX'^hò¹»X¼cÖ:o{ËÝ ü@&Rn²ÀC j\ÛQnl*()s‡HƒyÔËoQ.¢â‡Äš3Xt1ìyeWÓõ7Kh;îóÂGÅ{Ô"©mâûÉ%ã>FJù^¨m”Vü
yÂê7¶«Päêbóa1II4ý‡7]ÿ"B¯çØXâ@J)Üd«pþÊGR$ÕÃÝÞìÛƒÔ®ÚþŽdéïµfß*œ—9)hwvâ½sU>Ú«„Ý¤g´¤4ªþÚòUXÒ¼lQœFÆð„«e^^¢ÙÙ È*ë²ÎZ³/Gñ(<@©> /i6zY»aàŒ¯737yjV–[ œéyÄîZVgÜÛÐù¨üwœåÁúäáÚ«º’jŠuœŸÌFo¡ZàI&jWÚæ'•8ÏëÇµr¢xL[ó~³ã^	Bj]ÌGíOä1§|·Ì£Èæ›lê¯ß¡Ÿå·<oAU®äaš‹ ­’œÎ¹<£tyô»^„X8öxð†¥B¥z.Ò†§ Í^
8½ç
aÆÚ[Ô `›)¦uç9’Ž²0ÊOqc}:ŸõìMQŒ00ùy÷ÿË–@Ñ;×SÜ„ê×9 ÃNŽÀ²òƒ˜g²O…Ô‡xG4k;ín•iãT&îVÍS³†õ™Däà2ûŸîV\Ì]+*š]wŠ˜š«Ÿ%~‘üÛƒ£’&¦µ§÷$†ÿIˆVPwrXeãÖõ-— Ûž "¶µ…y~u?@YüþŒŸÚ
¿Ï bÓFÿÔ¥ðiC¬¸ì Y4³„°m{mïËéñô¢ÁÆ*É\Ðµ	bÑ* Å¤xDbˆº@3mÐ¼”>Õ{EúV¬Š¾*onE5¡à{)»Ú?óñØtXÖðl>Íªass change for a non user-defined clamp value. */
	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)) &&
	    !uc_se->user_defined)
		return true;

	/* Reset on sched_util_{min,max} == -1. */
	if (clamp_id == UCLAMP_MIN &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min == -1) {
		return true;
	}

	if (clamp_id == UCLAMP_MAX &&
	    attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max == -1) {
		return true;
	}

	return false;
}

static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr)
{
	enum uclamp_id clamp_id;

	for_each_clamp_id(clamp_id) {
		struct uclamp_se *uc_se = &p->uclamp_req[clamp_id];
		unsigned int value;

		if (!uclamp_reset(attr, clamp_id, uc_se))
			continue;

		/*
		 * RT by default have a 100% boost value that could be modified
		 * at runtime.
		 */
		if (unlikely(rt_task(p) && clamp_id == UCLAMP_MIN))
			value = sysctl_sched_uclamp_util_min_rt_default;
		else
			value = uclamp_none(clamp_id);

		uclamp_se_set(uc_se, value, false);

	}

	if (likely(!(attr->sched_flags & SCHED_FLAG_UTIL_CLAMP)))
		return;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN &&
	    attr->sched_util_min != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MIN],
			      attr->sched_util_min, true);
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX &&
	    attr->sched_util_max != -1) {
		uclamp_se_set(&p->uclamp_req[UCLAMP_MAX],
			      attr->sched_util_max, true);
	}
}

static void uclamp_fork(struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * We don't need to hold task_rq_lock() when updating p->uclamp_* here
	 * as the task is still at its early fork stages.
	 */
	for_each_clamp_id(clamp_id)
		p->uclamp[clamp_id].active = false;

	if (likely(!p->sched_reset_on_fork))
		return;

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&p->uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}
}

static void uclamp_post_fork(struct task_struct *p)
{
	uclamp_update_util_min_rt_default(p);
}

static void __init init_uclamp_rq(struct rq *rq)
{
	enum uclamp_id clamp_id;
	struct uclamp_rq *uc_rq = rq->uclamp;

	for_each_clamp_id(clamp_id) {
		uc_rq[clamp_id] = (struct uclamp_rq) {
			.value = uclamp_none(clamp_id)
		};
	}

	rq->uclamp_flags = UCLAMP_FLAG_IDLE;
}

static void __init init_uclamp(void)
{
	struct uclamp_se uc_max = {};
	enum uclamp_id clamp_id;
	int cpu;

	for_each_possible_cpu(cpu)
		init_uclamp_rq(cpu_rq(cpu));

	for_each_clamp_id(clamp_id) {
		uclamp_se_set(&init_task.uclamp_req[clamp_id],
			      uclamp_none(clamp_id), false);
	}

	/* System defaults allow max clamp values for both indexes */
	uclamp_se_set(&uc_max, uclamp_none(UCLAMP_MAX), false);
	for_each_clamp_id(clamp_id) {
		uclamp_default[clamp_id] = uc_max;
#ifdef CONFIG_UCLAMP_TASK_GROUP
		root_task_group.uclamp_req[clamp_id] = uc_max;
		root_task_group.uclamp[clamp_id] = uc_max;
#endif
	}
}

#else /* CONFIG_UCLAMP_TASK */
static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p) { }
static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p) { }
static inline int uclamp_validate(struct task_struct *p,
				  const struct sched_attr *attr)
{
	return -EOPNOTSUPP;
}
static void __setscheduler_uclamp(struct task_struct *p,
				  const struct sched_attr *attr) { }
static inline void uclamp_fork(struct task_struct *p) { }
static inline void uclamp_post_fork(struct task_struct *p) { }
static inline void init_uclamp(void) { }
#endif /* CONFIG_UCLAMP_TASK */

bool sched_task_on_rq(struct task_struct *p)
{
	return task_on_rq_queued(p);
}

unsigned long get_wchan(struct task_struct *p)
{
	unsigned long ip = 0;
	unsigned int state;

	if (!p || p == current)
		return 0;

	/* Only get wchan if task is blocked and we can keep it that way. */
	raw_spin_lock_irq(&p->pi_lock);
	state = READ_ONCE(p->__state);
	smp_rmb(); /* see try_to_wake_up() */
	if (state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq)
		ip = __get_wchan(p);
	raw_spin_unlock_irq(&p->pi_lock);

	return ip;
}

static inline void enqueue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if (!(flags & ENQUEUE_NOCLOCK))
		update_rq_clock(rq);

	if (!(flags & ENQUEUE_RESTORE)) {
		sched_info_enqueue(rq, p);
		psi_enqueue(p, flags & ENQUEUE_WAKEUP);
	}

	uclamp_rq_inc(rq, p);
	p->sched_class->enqueue_task(rq, p, flags);

	if (sched_core_enabled(rq))
		sched_core_enqueue(rq, p);
}

static inline void dequeue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if (sched_core_enabled(rq))
		sched_core_dequeue(rq, p, flags);

	if (!(flags & DEQUEUE_NOCLOCK))
		update_rq_clock(rq);

	if (!(flags & DEQUEUE_SAVE)) {
		sched_info_dequeue(rq, p);
		psi_dequeue(p, flags & DEQUEUE_SLEEP);
	}

	uclamp_rq_dec(rq, p);
	p->sched_class->dequeue_task(rq, p, flags);
}

void activate_task(struct rq *rq, struct task_struct *p, int flags)
{
	enqueue_task(rq, p, flags);

	p->on_rq = TASK_ON_RQ_QUEUED;
}

void deactivate_task(struct rq *rq, struct task_struct *p, int flags)
{
	p->on_rq = (flags & DEQUEUE_SLEEP) ? 0 : TASK_ON_RQ_MIGRATING;

	dequeue_task(rq, p, flags);
}

static inline int __normal_prio(int policy, int rt_prio, int nice)
{
	int prio;

	if (dl_policy(policy))
		prio = MAX_DL_PRIO - 1;
	else if (rt_policy(policy))
		prio = MAX_RT_PRIO - 1 - rt_prio;
	else
		prio = NICE_TO_PRIO(nice);

	return prio;
}

/*
 * Calculate the expected normal priority: i.e. priority
 * without taking RT-inheritance into account. Might be
 * boosted by interactivity modifiers. Changes upon fork,
 * setprio syscalls, and whenever the interactivity
 * estimator recalculates.
 */
static inline int normal_prio(struct task_struct *p)
{
	return __normal_prio(p->policy, p->rt_priority, PRIO_TO_NICE(p->static_prio));
}

/*
 * Calculate the current priority, i.e. the priority
 * taken into account by the scheduler. This value might
 * be boosted by RT tasks, or might be boosted by
 * interactivity modifiers. Will be RT if the task got
 * RT-boosted. If not then it returns p->normal_prio.
 */
static int effective_prio(struct task_struct *p)
{
	p->normal_prio = normal_prio(p);
	/*
	 * If we are RT tasks or we were boosted to RT priority,
	 * keep the priority unchanged. Otherwise, update priority
	 * to the normal priority:
	 */
	if (!rt_prio(p->prio))
		return p->normal_prio;
	return p->prio;
}

/**
 * task_curr - is this task currently executing on a CPU?
 * @p: the task in question.
 *
 * Return: 1 if the task is currently executing. 0 otherwise.
 */
inline int task_curr(const struct task_struct *p)
{
	return cpu_curr(task_cpu(p)) == p;
}

/*
 * switched_from, switched_to and prio_changed must _NOT_ drop rq->lock,
 * use the balance_callback list if you want balancing.
 *
 * this means any call to check_class_changed() must be followed by a call to
 * balance_callback().
 */
static inline void check_class_changed(struct rq *rq, struct task_struct *p,
				       const struct sched_class *prev_class,
				       int oldprio)
{
	if (prev_class != p->sched_class) {
		if (prev_class->switched_from)
			prev_class->switched_from(rq, p);

		p->sched_class->switched_to(rq, p);
	} else if (oldprio != p->prio || dl_task(p))
		p->sched_class->prio_changed(rq, p, oldprio);
}

void check_preempt_curr(struct rq *rq, struct task_struct *p, int flags)
{
	if (p->sched_class == rq->curr->sched_class)
		rq->curr->sched_class->check_preempt_curr(rq, p, flags);
	else if (p->sched_class > rq->curr->sched_class)
		resched_curr(rq);

	/*
	 * A queue event has occurred, and we're going to schedule.  In
	 * this case, we can save a useless back to back clock update.
	 */
	if (task_on_rq_queued(rq->curr) && test_tsk_need_resched(rq->curr))
		rq_clock_skip_update(rq);
}

#ifdef CONFIG_SMP

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags);

static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask,
				  u32 flags);

static void migrate_disable_switch(struct rq *rq, struct task_struct *p)
{
	if (likely(!p->migration_disabled))
		return;

	if (p->cpus_ptr != &p->cpus_mask)
		return;

	/*
	 * Violates locking rules! see comment in __do_set_cpus_allowed().
	 */
	__do_set_cpus_allowed(p, cpumask_of(rq->cpu), SCA_MIGRATE_DISABLE);
}

void migrate_disable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled) {
		p->migration_disabled++;
		return;
	}

	preempt_disable();
	this_rq()->nr_pinned++;
	p->migration_disabled = 1;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_disable);

void migrate_enable(void)
{
	struct task_struct *p = current;

	if (p->migration_disabled > 1) {
		p->migration_disabled--;
		return;
	}

	if (WARN_ON_ONCE(!p->migration_disabled))
		return;

	/*
	 * Ensure stop_task runs either before or after this, and that
	 * __set_cpus_allowed_ptr(SCA_MIGRATE_ENABLE) doesn't schedule().
	 */
	preempt_disable();
	if (p->cpus_ptr != &p->cpus_mask)
		__set_cpus_allowed_ptr(p, &p->cpus_mask, SCA_MIGRATE_ENABLE);
	/*
	 * Mustn't clear migration_disabled() until cpus_ptr points back at the
	 * regular cpus_mask, otherwise things that race (eg.
	 * select_fallback_rq) get confused.
	 */
	barrier();
	p->migration_disabled = 0;
	this_rq()->nr_pinned--;
	preempt_enable();
}
EXPORT_SYMBOL_GPL(migrate_enable);

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return rq->nr_pinned;
}

/*
 * Per-CPU kthreads are allowed to run on !active && online CPUs, see
 * __set_cpus_allowed_ptr() and select_fallback_rq().
 */
static inline bool is_cpu_allowed(struct task_struct *p, int cpu)
{
	/* When not in the task's cpumask, no point in looking further. */
	if (!cpumask_test_cpu(cpu, p->cpus_ptr))
		return false;

	/* migrate_disabled() must be allowed to finish. */
	if (is_migration_disabled(p))
		return cpu_online(cpu);

	/* Non kernel threads are not allowed during either online or offline. */
	if (!(p->flags & PF_KTHREAD))
		return cpu_active(cpu) && task_cpu_possible(cpu, p);

	/* KTHREAD_IS_PER_CPU is always allowed. */
	if (kthread_is_per_cpu(p))
		return cpu_online(cpu);

	/* Regular kernel threads don't get to stay during offline. */
	if (cpu_dying(cpu))
		return false;

	/* But are allowed during online. */
	return cpu_online(cpu);
}

/*
 * This is how migration works:
 *
 * 1) we invoke migration_cpu_stop() on the target CPU using
 *    stop_one_cpu().
 * 2) stopper starts to run (implicitly forcing the migrated thread
 *    off the CPU)
 * 3) it checks whether the migrated task is still in the wrong runqueue.
 * 4) if it's in the wrong runqueue then the migration thread removes
 *    it and puts it into the right queue.
 * 5) stopper completes and stop_one_cpu() returns and the migration
 *    is done.
 */

/*
 * move_queued_task - move a queued task to new rq.
 *
 * Returns (locked) new rq. Old rq's lock is released.
 */
static struct rq *move_queued_task(struct rq *rq, struct rq_flags *rf,
				   struct task_struct *p, int new_cpu)
{
	lockdep_assert_rq_held(rq);

	deactivate_task(rq, p, DEQUEUE_NOCLOCK);
	set_task_cpu(p, new_cpu);
	rq_unlock(rq, rf);

	rq = cpu_rq(new_cpu);

	rq_lock(rq, rf);
	BUG_ON(task_cpu(p) != new_cpu);
	activate_task(rq, p, 0);
	check_preempt_curr(rq, p, 0);

	return rq;
}

struct migration_arg {
	struct task_struct		*task;
	int				dest_cpu;
	struct set_affinity_pending	*pending;
};

/*
 * @refs: number of wait_for_completion()
 * @stop_pending: is @stop_work in use
 */
struct set_affinity_pending {
	refcount_t		refs;
	unsigned int		stop_pending;
	struct completion	done;
	struct cpu_stop_work	stop_work;
	struct migration_arg	arg;
};

/*
 * Move (not current) task off this CPU, onto the destination CPU. We're doing
 * this because either it can't run here any more (set_cpus_allowed()
 * away from this CPU, or CPU going down), or because we're
 * attempting to rebalance this task on exec (sched_exec).
 *
 * So we race with normal scheduler movements, but that's OK, as long
 * as the task is no longer on this CPU.
 */
static struct rq *__migrate_task(struct rq *rq, struct rq_flags *rf,
				 struct task_struct *p, int dest_cpu)
{
	/* Affinity changed (again). */
	if (!is_cpu_allowed(p, dest_cpu))
		return rq;

	update_rq_clock(rq);
	rq = move_queued_task(rq, rf, p, dest_cpu);

	return rq;
}

/*
 * migration_cpu_stop - this will be executed by a highprio stopper thread
 * and performs thread migration by bumping thread off CPU then
 * 'pushing' onto another runqueue.
 */
static int migration_cpu_stop(void *data)
{
	struct migration_arg *arg = data;
	struct set_affinity_pending *pending = arg->pending;
	struct task_struct *p = arg->task;
	struct rq *rq = this_rq();
	bool complete = false;
	struct rq_flags rf;

	/*
	 * The original target CPU might have gone down and we might
	 * be on another CPU but it doesn't matter.
	 */
	local_irq_save(rf.flags);
	/*
	 * We need to explicitly wake pending tasks before running
	 * __migrate_task() such that we will not miss enforcing cpus_ptr
	 * during wakeups, see set_cpus_allowed_ptr()'s TASK_WAKING test.
	 */
	flush_smp_call_function_from_idle();

	raw_spin_lock(&p->pi_lock);
	rq_lock(rq, &rf);

	/*
	 * If we were passed a pending, then ->stop_pending was set, thus
	 * p->migration_pending must have remained stable.
	 */
	WARN_ON_ONCE(pending && pending != p->migration_pending);

	/*
	 * If task_rq(p) != rq, it cannot be migrated here, because we're
	 * holding rq->lock, if p->on_rq == 0 it cannot get enqueued because
	 * we're holding p->pi_lock.
	 */
	if (task_rq(p) == rq) {
		if (is_migration_disabled(p))
			goto out;

		if (pending) {
			p->migration_pending = NULL;
			complete = true;

			if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask))
				goto out;
		}

		if (task_on_rq_queued(p))
			rq = __migrate_task(rq, &rf, p, arg->dest_cpu);
		else
			p->wake_cpu = arg->dest_cpu;

		/*
		 * XXX __migrate_task() can fail, at which point we might end
		 * up running on a dodgy CPU, AFAICT this can only happen
		 * during CPU hotplug, at which point we'll get pushed out
		 * anyway, so it's probably not a big deal.
		 */

	} else if (pending) {
		/*
		 * This happens when we get migrated between migrate_enable()'s
		 * preempt_enable() and scheduling the stopper task. At that
		 * point we're a regular task again and not current anymore.
		 *
		 * A !PREEMPT kernel has a giant hole here, which makes it far
		 * more likely.
		 */

		/*
		 * The task moved before the stopper got to run. We're holding
		 * ->pi_lock, so the allowed mask is stable - if it got
		 * somewhere allowed, we're done.
		 */
		if (cpumask_test_cpu(task_cpu(p), p->cpus_ptr)) {
			p->migration_pending = NULL;
			complete = true;
			goto out;
		}

		/*
		 * When migrate_enable() hits a rq mis-match we can't reliably
		 * determine is_migration_disabled() and so have to chase after
		 * it.
		 */
		WARN_ON_ONCE(!pending->stop_pending);
		task_rq_unlock(rq, p, &rf);
		stop_one_cpu_nowait(task_cpu(p), migration_cpu_stop,
				    &pending->arg, &pending->stop_work);
		return 0;
	}
out:
	if (pending)
		pending->stop_pending = false;
	task_rq_unlock(rq, p, &rf);

	if (complete)
		complete_all(&pending->done);

	return 0;
}

int push_cpu_stop(void *arg)
{
	struct rq *lowest_rq = NULL, *rq = this_rq();
	struct task_struct *p = arg;

	raw_spin_lock_irq(&p->pi_lock);
	raw_spin_rq_lock(rq);

	if (task_rq(p) != rq)
		goto out_unlock;

	if (is_migration_disabled(p)) {
		p->migration_flags |= MDF_PUSH;
		goto out_unlock;
	}

	p->migration_flags &= ~MDF_PUSH;

	if (p->sched_class->find_lock_rq)
		lowest_rq = p->sched_class->find_lock_rq(p, rq);

	if (!lowest_rq)
		goto out_unlock;

	// XXX validate p is still the highest prio task
	if (task_rq(p) == rq) {
		deactivate_task(rq, p, 0);
		set_task_cpu(p, lowest_rq->cpu);
		activate_task(lowest_rq, p, 0);
		resched_curr(lowest_rq);
	}

	double_unlock_balance(rq, lowest_rq);

out_unlock:
	rq->push_busy = false;
	raw_spin_rq_unlock(rq);
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);
	return 0;
}

/*
 * sched_class::set_cpus_allowed must do the below, but is not required to
 * actually call this function.
 */
void set_cpus_allowed_common(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	if (flags & (SCA_MIGRATE_ENABLE | SCA_MIGRATE_DISABLE)) {
		p->cpus_ptr = new_mask;
		return;
	}

	cpumask_copy(&p->cpus_mask, new_mask);
	p->nr_cpus_allowed = cpumask_weight(new_mask);
}

static void
__do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32 flags)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	/*
	 * This here violates the locking rules for affinity, since we're only
	 * supposed to change these variables while holding both rq->lock and
	 * p->pi_lock.
	 *
	 * HOWEVER, it magically works, because ttwu() is the only code that
	 * accesses these variables under p->pi_lock and only does so after
	 * smp_cond_load_acquire(&p->on_cpu, !VAL), and we're in __schedule()
	 * before finish_task().
	 *
	 * XXX do further audits, this smells like something putrid.
	 */
	if (flags & SCA_MIGRATE_DISABLE)
		SCHED_WARN_ON(!p->on_cpu);
	else
		lockdep_assert_held(&p->pi_lock);

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued) {
		/*
		 * Because __kthread_bind() calls this on blocked tasks without
		 * holding rq->lock.
		 */
		lockdep_assert_rq_held(rq);
		dequeue_task(rq, p, DEQUEUE_SAVE | DEQUEUE_NOCLOCK);
	}
	if (running)
		put_prev_task(rq, p);

	p->sched_class->set_cpus_allowed(p, new_mask, flags);

	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE | ENQUEUE_NOCLOCK);
	if (running)
		set_next_task(rq, p);
}

void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
{
	__do_set_cpus_allowed(p, new_mask, 0);
}

int dup_user_cpus_ptr(struct task_struct *dst, struct task_struct *src,
		      int node)
{
	if (!src->user_cpus_ptr)
		return 0;

	dst->user_cpus_ptr = kmalloc_node(cpumask_size(), GFP_KERNEL, node);
	if (!dst->user_cpus_ptr)
		return -ENOMEM;

	cpumask_copy(dst->user_cpus_ptr, src->user_cpus_ptr);
	return 0;
}

static inline struct cpumask *clear_user_cpus_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = NULL;

	swap(p->user_cpus_ptr, user_mask);

	return user_mask;
}

void release_user_cpus_ptr(struct task_struct *p)
{
	kfree(clear_user_cpus_ptr(p));
}

/*
 * This function is wildly self concurrent; here be dragons.
 *
 *
 * When given a valid mask, __set_cpus_allowed_ptr() must block until the
 * designated task is enqueued on an allowed CPU. If that task is currently
 * running, we have to kick it out using the CPU stopper.
 *
 * Migrate-Disable comes along and tramples all over our nice sandcastle.
 * Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *
 * P1 *cannot* return from this set_cpus_allowed_ptr() call until P0 executes
 * its outermost migrate_enable() (i.e. it exits its Migrate-Disable region).
 * This means we need the following scheme:
 *
 *     P0@CPU0                  P1
 *
 *     migrate_disable();
 *     <preempted>
 *                              set_cpus_allowed_ptr(P0, [1]);
 *                                <blocks>
 *     <resumes>
 *     migrate_enable();
 *       __set_cpus_allowed_ptr();
 *       <wakes local stopper>
 *                         `--> <woken on migration completion>
 *
 * Now the fun stuff: there may be several P1-like tasks, i.e. multiple
 * concurrent set_cpus_allowed_ptr(P0, [*]) calls. CPU affinity changes of any
 * task p are serialized by p->pi_lock, which we can leverage: the one that
 * should come into effect at the end of the Migrate-Disable region is the last
 * one. This means we only need to track a single cpumask (i.e. p->cpus_mask),
 * but we still need to properly signal those waiting tasks at the appropriate
 * moment.
 *
 * This is implemented using struct set_affinity_pending. The first
 * __set_cpus_allowed_ptr() caller within a given Migrate-Disable region will
 * setup an instance of that struct and install it on the targeted task_struct.
 * Any and all further callers will reuse that instance. Those then wait for
 * a completion signaled at the tail of the CPU stopper callback (1), triggered
 * on the end of the Migrate-Disable region (i.e. outermost migrate_enable()).
 *
 *
 * (1) In the cases covered above. There is one more where the completion is
 * signaled within affine_move_task() itself: when a subsequent affinity request
 * occurs after the stopper bailed out due to the targeted task still being
 * Migrate-Disable. Consider:
 *
 *     Initial conditions: P0->cpus_mask = [0, 1]
 *
 *     CPU0		  P1				P2
 *     <P0>
 *       migrate_disable();
 *       <preempted>
 *                        set_cpus_allowed_ptr(P0, [1]);
 *                          <blocks>
 *     <migration/0>
 *       migration_cpu_stop()
 *         is_migration_disabled()
 *           <bails>
 *                                                       set_cpus_allowed_ptr(P0, [0, 1]);
 *                                                         <signal completion>
 *                          <awakes>
 *
 * Note that the above is safe vs a concurrent migrate_enable(), as any
 * pending affinity completion is preceded by an uninstallation of
 * p->migration_pending done with p->pi_lock held.
 */
static int affine_move_task(struct rq *rq, struct task_struct *p, struct rq_flags *rf,
			    int dest_cpu, unsigned int flags)
{
	struct set_affinity_pending my_pending = { }, *pending = NULL;
	bool stop_pending, complete = false;

	/* Can the task run on the task's current CPU? If so, we're done */
	if (cpumask_test_cpu(task_cpu(p), &p->cpus_mask)) {
		struct task_struct *push_task = NULL;

		if ((flags & SCA_MIGRATE_ENABLE) &&
		    (p->migration_flags & MDF_PUSH) && !rq->push_busy) {
			rq->push_busy = true;
			push_task = get_task_struct(p);
		}

		/*
		 * If there are pending waiters, but no pending stop_work,
		 * then complete now.
		 */
		pending = p->migration_pending;
		if (pending && !pending->stop_pending) {
			p->migration_pending = NULL;
			complete = true;
		}

		task_rq_unlock(rq, p, rf);

		if (push_task) {
			stop_one_cpu_nowait(rq->cpu, push_cpu_stop,
					    p, &rq->push_work);
		}

		if (complete)
			complete_all(&pending->done);

		return 0;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		/* serialized by p->pi_lock */
		if (!p->migration_pending) {
			/* Install the request */
			refcount_set(&my_pending.refs, 1);
			init_completion(&my_pending.done);
			my_pending.arg = (struct migration_arg) {
				.task = p,
				.dest_cpu = dest_cpu,
				.pending = &my_pending,
			};

			p->migration_pending = &my_pending;
		} else {
			pending = p->migration_pending;
			refcount_inc(&pending->refs);
			/*
			 * Affinity has changed, but we've already installed a
			 * pending. migration_cpu_stop() *must* see this, else
			 * we risk a completion of the pending despite having a
			 * task on a disallowed CPU.
			 *
			 * Serialized by p->pi_lock, so this is safe.
			 */
			pending->arg.dest_cpu = dest_cpu;
		}
	}
	pending = p->migration_pending;
	/*
	 * - !MIGRATE_ENABLE:
	 *   we'll have installed a pending if there wasn't one already.
	 *
	 * - MIGRATE_ENABLE:
	 *   we're here because the current CPU isn't matching anymore,
	 *   the only way that can happen is because of a concurrent
	 *   set_cpus_allowed_ptr() call, which should then still be
	 *   pending completion.
	 *
	 * Either way, we really should have a @pending here.
	 */
	if (WARN_ON_ONCE(!pending)) {
		task_rq_unlock(rq, p, rf);
		return -EINVAL;
	}

	if (task_running(rq, p) || READ_ONCE(p->__state) == TASK_WAKING) {
		/*
		 * MIGRATE_ENABLE gets here because 'p == current', but for
		 * anything else we cannot do is_migration_disabled(), punt
		 * and have the stopper function handle it all race-free.
		 */
		stop_pending = pending->stop_pending;
		if (!stop_pending)
			pending->stop_pending = true;

		if (flags & SCA_MIGRATE_ENABLE)
			p->migration_flags &= ~MDF_PUSH;

		task_rq_unlock(rq, p, rf);

		if (!stop_pending) {
			stop_one_cpu_nowait(cpu_of(rq), migration_cpu_stop,
					    &pending->arg, &pending->stop_work);
		}

		if (flags & SCA_MIGRATE_ENABLE)
			return 0;
	} else {

		if (!is_migration_disabled(p)) {
			if (task_on_rq_queued(p))
				rq = move_queued_task(rq, rf, p, dest_cpu);

			if (!pending->stop_pending) {
				p->migration_pending = NULL;
				complete = true;
			}
		}
		task_rq_unlock(rq, p, rf);

		if (complete)
			complete_all(&pending->done);
	}

	wait_for_completion(&pending->done);

	if (refcount_dec_and_test(&pending->refs))
		wake_up_var(&pending->refs); /* No UaF, just an address */

	/*
	 * Block the original owner of &pending until all subsequent callers
	 * have seen the completion and decremented the refcount
	 */
	wait_var_event(&my_pending.refs, !refcount_read(&my_pending.refs));

	/* ARGH */
	WARN_ON_ONCE(my_pending.stop_pending);

	return 0;
}

/*
 * Called with both p->pi_lock and rq->lock held; drops both before returning.
 */
static int __set_cpus_allowed_ptr_locked(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags,
					 struct rq *rq,
					 struct rq_flags *rf)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	const struct cpumask *cpu_allowed_mask = task_cpu_possible_mask(p);
	const struct cpumask *cpu_valid_mask = cpu_active_mask;
	bool kthread = p->flags & PF_KTHREAD;
	struct cpumask *user_mask = NULL;
	unsigned int dest_cpu;
	int ret = 0;

	update_rq_clock(rq);

	if (kthread || is_migration_disabled(p)) {
		/*
		 * Kernel threads are allowed on online && !active CPUs,
		 * however, during cpu-hot-unplug, even these might get pushed
		 * away if not KTHREAD_IS_PER_CPU.
		 *
		 * Specifically, migration_disabled() tasks must not fail the
		 * cpumask_any_and_distribute() pick below, esp. so on
		 * SCA_MIGRATE_ENABLE, otherwise we'll not call
		 * set_cpus_allowed_common() and actually reset p->cpus_ptr.
		 */
		cpu_valid_mask = cpu_online_mask;
	}

	if (!kthread && !cpumask_subset(new_mask, cpu_allowed_mask)) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * Must re-check here, to close a race against __kthread_bind(),
	 * sched_setaffinity() is not guaranteed to observe the flag.
	 */
	if ((flags & SCA_CHECK) && (p->flags & PF_NO_SETAFFINITY)) {
		ret = -EINVAL;
		goto out;
	}

	if (!(flags & SCA_MIGRATE_ENABLE)) {
		if (cpumask_equal(&p->cpus_mask, new_mask))
			goto out;

		if (WARN_ON_ONCE(p == current &&
				 is_migration_disabled(p) &&
				 !cpumask_test_cpu(task_cpu(p), new_mask))) {
			ret = -EBUSY;
			goto out;
		}
	}

	/*
	 * Picking a ~random cpu helps in cases where we are changing affinity
	 * for groups of tasks (ie. cpuset), so that load balancing is not
	 * immediately required to distribute the tasks within their new mask.
	 */
	dest_cpu = cpumask_any_and_distribute(cpu_valid_mask, new_mask);
	if (dest_cpu >= nr_cpu_ids) {
		ret = -EINVAL;
		goto out;
	}

	__do_set_cpus_allowed(p, new_mask, flags);

	if (flags & SCA_USER)
		user_mask = clear_user_cpus_ptr(p);

	ret = affine_move_task(rq, p, rf, dest_cpu, flags);

	kfree(user_mask);

	return ret;

out:
	task_rq_unlock(rq, p, rf);

	return ret;
}

/*
 * Change a given task's CPU affinity. Migrate the thread to a
 * proper CPU and schedule it away if the CPU it's executing on
 * is removed from the allowed bitmask.
 *
 * NOTE: the caller must have a valid reference to the task, the
 * task must not exit() & deallocate itself prematurely. The
 * call is not atomic; no spinlocks may be held.
 */
static int __set_cpus_allowed_ptr(struct task_struct *p,
				  const struct cpumask *new_mask, u32 flags)
{
	struct rq_flags rf;
	struct rq *rq;

	rq = task_rq_lock(p, &rf);
	return __set_cpus_allowed_ptr_locked(p, new_mask, flags, rq, &rf);
}

int set_cpus_allowed_ptr(struct task_struct *p, const struct cpumask *new_mask)
{
	return __set_cpus_allowed_ptr(p, new_mask, 0);
}
EXPORT_SYMBOL_GPL(set_cpus_allowed_ptr);

/*
 * Change a given task's CPU affinity to the intersection of its current
 * affinity mask and @subset_mask, writing the resulting mask to @new_mask
 * and pointing @p->user_cpus_ptr to a copy of the old mask.
 * If the resulting mask is empty, leave the affinity unchanged and return
 * -EINVAL.
 */
static int restrict_cpus_allowed_ptr(struct task_struct *p,
				     struct cpumask *new_mask,
				     const struct cpumask *subset_mask)
{
	struct cpumask *user_mask = NULL;
	struct rq_flags rf;
	struct rq *rq;
	int err;

	if (!p->user_cpus_ptr) {
		user_mask = kmalloc(cpumask_size(), GFP_KERNEL);
		if (!user_mask)
			return -ENOMEM;
	}

	rq = task_rq_lock(p, &rf);

	/*
	 * Forcefully restricting the affinity of a deadline task is
	 * likely to cause problems, so fail and noisily override the
	 * mask entirely.
	 */
	if (task_has_dl_policy(p) && dl_bandwidth_enabled()) {
		err = -EPERM;
		goto err_unlock;
	}

	if (!cpumask_and(new_mask, &p->cpus_mask, subset_mask)) {
		err = -EINVAL;
		goto err_unlock;
	}

	/*
	 * We're about to butcher the task affinity, so keep track of what
	 * the user asked for in case we're able to restore it later on.
	 */
	if (user_mask) {
		cpumask_copy(user_mask, p->cpus_ptr);
		p->user_cpus_ptr = user_mask;
	}

	return __set_cpus_allowed_ptr_locked(p, new_mask, 0, rq, &rf);

err_unlock:
	task_rq_unlock(rq, p, &rf);
	kfree(user_mask);
	return err;
}

/*
 * Restrict the CPU affinity of task @p so that it is a subset of
 * task_cpu_possible_mask() and point @p->user_cpu_ptr to a copy of the
 * old affinity mask. If the resulting mask is empty, we warn and walk
 * up the cpuset hierarchy until we find a suitable mask.
 */
void force_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	cpumask_var_t new_mask;
	const struct cpumask *override_mask = task_cpu_possible_mask(p);

	alloc_cpumask_var(&new_mask, GFP_KERNEL);

	/*
	 * __migrate_task() can fail silently in the face of concurrent
	 * offlining of the chosen destination CPU, so take the hotplug
	 * lock to ensure that the migration succeeds.
	 */
	cpus_read_lock();
	if (!cpumask_available(new_mask))
		goto out_set_mask;

	if (!restrict_cpus_allowed_ptr(p, new_mask, override_mask))
		goto out_free_mask;

	/*
	 * We failed to find a valid subset of the affinity mask for the
	 * task, so override it based on its cpuset hierarchy.
	 */
	cpuset_cpus_allowed(p, new_mask);
	override_mask = new_mask;

out_set_mask:
	if (printk_ratelimit()) {
		printk_deferred("Overriding affinity for process %d (%s) to CPUs %*pbl\n",
				task_pid_nr(p), p->comm,
				cpumask_pr_args(override_mask));
	}

	WARN_ON(set_cpus_allowed_ptr(p, override_mask));
out_free_mask:
	cpus_read_unlock();
	free_cpumask_var(new_mask);
}

static int
__sched_setaffinity(struct task_struct *p, const struct cpumask *mask);

/*
 * Restore the affinity of a task @p which was previously restricted by a
 * call to force_compatible_cpus_allowed_ptr(). This will clear (and free)
 * @p->user_cpus_ptr.
 *
 * It is the caller's responsibility to serialise this with any calls to
 * force_compatible_cpus_allowed_ptr(@p).
 */
void relax_compatible_cpus_allowed_ptr(struct task_struct *p)
{
	struct cpumask *user_mask = p->user_cpus_ptr;
	unsigned long flags;

	/*
	 * Try to restore the old affinity mask. If this fails, then
	 * we free the mask explicitly to avoid it being inherited across
	 * a subsequent fork().
	 */
	if (!user_mask || !__sched_setaffinity(p, user_mask))
		return;

	raw_spin_lock_irqsave(&p->pi_lock, flags);
	user_mask = clear_user_cpus_ptr(p);
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);

	kfree(user_mask);
}

void set_task_cpu(struct task_struct *p, unsigned int new_cpu)
{
#ifdef CONFIG_SCHED_DEBUG
	unsigned int state = READ_ONCE(p->__state);

	/*
	 * We should never call set_task_cpu() on a blocked task,
	 * ttwu() will sort out the placement.
	 */
	WARN_ON_ONCE(state != TASK_RUNNING && state != TASK_WAKING && !p->on_rq);

	/*
	 * Migrating fair class task must have p->on_rq = TASK_ON_RQ_MIGRATING,
	 * because schedstat_wait_{start,end} rebase migrating task's wait_start
	 * time relying on p->on_rq.
	 */
	WARN_ON_ONCE(state == TASK_RUNNING &&
		     p->sched_class == &fair_sched_class &&
		     (p->on_rq && !task_on_rq_migrating(p)));

#ifdef CONFIG_LOCKDEP
	/*
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
	 */
	WARN_ON_ONCE(debug_locks && !(lockdep_is_held(&p->pi_lock) ||
				      lockdep_is_held(__rq_lockp(task_rq(p)))));
#endif
	/*
	 * Clearly, migrati«èæ¬Üî>ŒÐ´_ÒˆÖ4–ÏÅœgl¦T²Øœh\
žwùåý%&|²çí°ZÑYEÝ,òíÕQémê¸'%-KhÐª¥zPêªí7óŠ‡L[ãlQåš^{rÐ¹P^ëýé—,ÔéÞ¯{Ê±´¯Ø«¡‘x³hÏ¸gA%çˆ=>¼ßpûGÞßhkùülòû”U—Oï-˜ŠÜS%ôáJßT®F¹3ÒnÉÍüÊ®LÝdÕš`l²BÇ|jÿ×ßæ›ÔHvò˜ê/«^7ªÇÒv’v²¨¶s±€ÀÍAï†ÆåB;NŒ”#v¨Þ£‡tÿtÖêßu Gô–Q,YR)ß³½Þ%+¤—kÖ0dA})$áloÁ]ÌSlŸIÃ'+ Tdw’{‘VHèýýiÖ¢å§Ë‡°©džAõì83ÜXREhBü†¥;ø¡cbÖ.^Ú?|h0uðPÐÁå@¨“Îaã³Ò1]N45à´ˆFkïb_<u¤ 1Âƒ1‚HtÝ—.ü£oLÖÌæEÍï¤í°Ö Æ([œ¹èfÜ€¿ðƒ[Ž½]I4ô,J–8oc ÀMBcüö“/<ÿ—;Mo¥hw ÔÉ6àiÞ»J/¶ñ	íñÕx†ó„ {•'}éù{SÅ%µ¢9Ü¯'v–çtÄ* Iü“Å*Ë7åÈ5Þ){„Xšñõ¯i¶’Ç,qà½ðzý4È5æ«L…Ç»«Z*ÅÖðË]óÖÁŽ@jXFµ.Rø:ùãöŒ}¹gâ£Õ¢¼hØÓÌì22ÛTøNÉÁrLa›ôõq³%[ÈÏÌ@ôå½ùˆƒÀžsÉïó±äúæO WÓ2jÚl®Ü>òú
¤í†êñ|KäÌÿ¿¤©é)å†væ’'Eí\Ù#(Ø(ÿ9¥ÑWPmEuÔ"v‘,SõÜ5¢\æ£ÁBZð6Y¯ì”%-l’
z›^ÏYÓŽD¦…e¢Õ,S"¿ ÊuYÏ£a5½Úª]u|¸zŽþ	¯L±Íq^(©APô$ÇNýM±Ô¦‡¼XxHÌ*Ð?‹¦X/-€ƒ”õþ³³`Œ2&Á:h) l°D¿@Bî€¼‚k¯³ƒ2Á R[,.²ðîÇÝp”îèÑíA^›Æ%ÔÝ»ŒôEsÞ€ƒ¯á—T—Spß¾q¥\D}¡ñs—SAØÉ50Öke?Òqë<ÙóN0M¢NŒx­
¡,S~©*­”—ÓeOŠ‡™ÂÕøn’}¥Ê™ñÅ‹~w8û:´|Ë×›gÑ×%¦2½–8eÎkíëÖÑ/µ^‰Å i¢*zø~LSñ4\žWÅ™W¥UöP< c‘žwà™»¾­¥áNêŠ¯;ÍÚ½HLAû¡Ö‰þ"æ^V›’Šsßk¡KµW~Xµfœâ¡T«'>èª˜Vî‡Œ×oõ'<>`-¥²4XÔT;p`Òö§#+ÙaŒ$üo ºñ+®%4üSSî›à_qãÞò¾zù$.{§
.‹lQaÀS.,ÆŽ.Sq]i\ƒï®ƒþµOX=x°ˆÔð—>ýh™û~xô¬Èìq2b?p{w*ó¶A4œ0J,c}.2©.­MÄ\°A³³T2Æ ð±9nhžÎåîßz¨‘äÈrñRlÜÑ\Ñòã#ÿ²½ÀúÛ×JM9)šCïJø-ÞÑÍè# ”O2ÈH;¦OHŠ,Ð°o9Ëg!Ï»ª˜O_d’24ˆç4-u+±æUËæCqž:Uº5LdHÓ!·Ã	LÉÉj` 9ÄËøÛeÑÕ'ß¤Ën/ucU6Ã“RíoN~â
ºSøjÄ†Øø¢tˆÛµ9õ2›+^ê¥„Z§Au{é:¥böÚ»aPž÷ÄŠöÚëÉeÓ”Ða3²¢9—b†Tˆòú_7:f¯HŸ~é4„ÌËû+íaBÔxhÎ½«ƒS»òÐº¤LñPx»Xëb¡ÃÁöƒúç!þ%¡ý6SÍ–™€f®xÊ”x>6R½<±®s~–Ðµ”ýÆb<ue‚0öùVi4§Ò%‚=ÓÝ3þ1˜æ/¢§ÐtF¿ØaÞå`Ý­+Áõ†nŠg H2€G Cê+(“v®IóÈ5/À¸¥î¡Óë6gg‹YÚ=«w÷WE·Cìð`êí4s6¥Ú·Õð¶£ð‘òkrf”•ÈúÕ·á¶{p/ ˆ	S¿no°h¶û²Ñ¡†z³x&î*xÈæ£ÄùcgÒ›ÕG8¯è9#ùJ‡HöòÄf`cÌ°Þ¯á¶¾öÍ¢òG5¥%*5¦Øý=xÛÈ¶ªÔw/xz­\#‹éV–Ô†ß³QˆM¤n­oÂDûÔûÍ]xÁ°[%uL8‹mæ¢ìº÷¼§Š™¨#¬Dõ{é ÝOft­Õ«ŠDúqy‚g2Ì¥TðÙ/Ù¦1´Hä†–-ÆŠ…é/ÁþBÔ„¬Ó¥¤™¿F$¶XAéô»dJPÄ)åò¬ðø7îûb'ÅÏWU!Þ.•gÝ)}«ÏÓ½ìèÚ‘{¥µ?¥¦[A®Êþ]oE6ÕØ(ÞELv
“!ßn¼À÷„ýÃ”Ú{Éj çÖç}Â[ú¤‚¯>L3#£ëxÂÝaÏ‹ÖvtÑkª>8ç!¬’ah2½s÷Ø,°—ÛpœC_ñî»GÆŽ2>!:‡YO_û2ÈÅ.ÑÄù¸5w’PO±Zlc5Ë/Om®Ä~¾ÁaËxOú„ãå–NŒÝ[%Ac"I,Ü@Ü²¡…Æ¥W´¢Îë(Œ{l× H?@„B/zX+;N3úmwØ@=Ê²÷°$tý }-³’“Nžšï½†àHlPâ#Í†°^,!ëô”ƒ•×V‹N/ý–ÕYŒrË<ÍuÜÚú7G ¹u)rS#›†8ìS65gRŸÅWc_Tï2Ér”.Ø„/¬©^U}å2dÇ¦JÂ[óÞŽ—üÆH	>Ê¦L}Ï:•–±Ì8Õ†œ%€ë
E˜4j—É´zA¥+Ð™…&}U>Õ´~Ÿ3w×gÿÕÀoÎá}ãîè|ê8àc
Þœ8ÈNAè¯ñþ&jËµÃt}@›âŠMó0;úÉ‚§™(‰¹‹s§Õ	ªTˆ7]êš&6kIÄö¬	[R«(ò˜ÇÃ°‡&|sõZî±lVÈÏd,ê°e¤¬õž°ªvŽ ŒcI@û*ÆáøCG 9þúÓH7˜m”…ò!)Œ‰ÔüvoYåiÎ»_n…»_NChþÞ1c-Q"G+œ8Ñ4HgðpfVÊKµäŒèá³hÜ1•Ùl3ÜJ~YØ¸ûEÝéÊ7u#®uXã¨ÍíÕ¶W©mÑ¸e6ÅR_¤¤$­‘âíÒ%ºNv+Þ¼Š¿í+‘Ü/s1&*k™þ²—W2
$îïS©TmÑ!Ó¸<vI'2«ÒÊ[5–šÊºä6²YƒÂu™¤Qy,—7Ø*µD”é¥øGá¥·:î‹æ:5XC×ŸSÇx­Zdeð¿ð
¥aÛõ"73ÐW
ÿtØp
&m°ïçõÅ4RØ–>«e<§®‹µLýDøë„Þ”žÖ=Ù÷ƒð
Ãx$».jIxRUYT{BòCz;ïÞ}
&ŠËb`ÚÀ"gJií¼­µ>]?vŠ­ê¶ÃÎ)î ~tXR"z«Ž´·í 5'å%Óí1›Çñ‚ {À”rÛöu‚:[ðR
áé#Ãžì†œ2w=mÃ‡Ü€`ygHÕxãhf£×@Ž•ea˜Ú—)‡{]"ƒtÂÐJÒö-àÅæ‹å°’ÞòE—¿ð6¥‘)
M”ãk‡Qq*>ÇGžŒökâNFÇ›Mœ‹ŒÒbh³7N{î~–W8ïÓ”¼8ÅÐËR”_¡o›n‰ÃÀŽUÑþÒg¯B:{¾–l*»éâÑMüãzÅ{Çn…ç{`ÑžKiµ]P+ÐáÖ[&ûCn—kNcÎD>‹;öA^*h‹âæ˜¨õMÈ”MAÙà
fQgü¬v hÐ–¥†0s;[I¹;[fæÞ„ÿ±®.$…„&ARµ•¤t‡-[ÂRãyÖ´PéûÜ¾¸®6(âd(´3Ï’”yH6H,VôÉÚè¼ç‘M%MWœ¡)¢ªŽCþMXâ‚†n7äÖ5ÒïvsÍiaG}þ-WÄ™ÎÞÌ[|°ä¤!†îñ£ d}õ)ÞödÚöo©µg°à€ö•ƒ[[!f/÷g©]Í:'&S‘ÕýT%õ†9í¸°}r¥YØ2¯ç½T1ƒBóiSÞŒ(\î×Î·îzgÍú¾Mø[¤)8óC¤‚ MnNÿó°R’W‘Hƒ	ˆ²ŸññÞÀýp´U-©Ç«Y^*+”¢¯	å{¥†8Ò)‘Þœ¡pÚL¼ñy6Tæê²Å—Öƒª],m2E—½FS™k·
”mˆØÚêHÂJ‡o…2h
Da_f”©½ïåÌô\WOd+¹;[ý1¹7]˜¡ÌF‘€Ô÷©Ã"Hº/PÃWwjë!–WŠ†*0ý¯hâÞ(W±H»pZ¼~.®®‰y>ÙAÞºmOô#™7á=i¸•À€*=ÚFFæ(#Äæ¹œDVcsœ]†¨a7	›
`k¨LŸûâaýÒü ,Ï}¯óTÚUçÚ®rWb7,¦…p6ö‘.8œ÷N]Ç‹;(‚'è{JÖþ(gÿ4¬âB7#ë*(§^ãXb#2BŒ²‘žã¼¯«ÿ´+ˆ‘g[U;ªœçc£ú¢©po8ŸWz¾¹vçN]þ½qYádÂ¥™6m¯{¥aIyµœLu,fvJÝ«(ø\ÆšùWk;4»»rÿÚ$G;€K™Ï|
k$à±œáH™Èeà±ÙÅ8ˆqgªô·9Æµ¼3Ž“m±„*EÃÈA° XéØ=Å®µ]“7ª$¬ÌèDÏÃÍ8	]ŸÏãä7…ð.b¯†Î‰“°l­ÖÔl¬­ëáJ

P r­ë]¶GÂÔÊÐ1ÌÐ¹žÐp¶™yY=XY@V2:<øÐ3ë*êÐZî´¢ôK%éx6CFO&¼´¶\(ÓxKƒ˜KéÃ™Å†¯ÛÜ’‚ÀýÂð¨Žöÿ·„ |	ÞÖÐáy›×~®å©·«š‹“Qzíãk…BY¯„Y–€¿…Ùhï©­ï&%Sh"÷
h4’“N#Â‘0N9gâjäÇ­'oê‰ê‰GfôfŠÜÍÄfÌj`Kh™5Ö**À´|hñÙšßj61ËþÛsVÏ„ý@™_kýô!`0.‡èªQÇ-é@¢)#dê-<œªù×Äœ=&ôä¸šiÐ¸:vœ²€Óç1¿%¤(Ø'æùÛx‚üòfÆßa§äSo:ÅÄ®÷
îÿÊ!¼2¯‰†úÕ~Pm1¿ÕûÕÇ[E!C«·<3×?E®¡c´$#¸ÃbfeGgÆþ÷ˆ°ÍHÏ|ªâ»ãñ.ÀrU9µ•­ï½³ZÁþÏbå´ã4ñqà+å7èÀÍ³pQS,˜ŒŸ°£¡£ŠÛ~¡÷¶±×g¿U­YR’Ž%Ðßk#¬üê˜Ï!RI(V?Ø=rO<XXTZÑ
^ê`c&h(¡Í°åRzZùvÐA0eÚÝ‘ÜgâÙ8?ÈÊ1¢C3VÈoÛ,! ¦H4å¯^ha ±:r´#•kæúdü^ñžÊUÃ2L²¥[ú—á‚üw#hƒ?¸(Ž{™$9(»hKÏpdBÅƒG’6˜*	å1ì÷5«õÙ«F(;ÄB2Â²5ÕÅ®+cÂ†îæXý/315°*y†Ò!æz­3Z0ÚÇ8xR/¼êùy
h©O³ÕÌk)¢½l<‘EÇ¤o@#vWÓim%ûŽq3KLRŸ|_vV3j(Ÿ¶ùúù¹XFò°{´L¹VØž¾kÃÆÁ¡ŒàFÑ!gheKIx³<<{»³‰ÇžPŸZÍçYS¥/XÇ£A>íTó˜ÆàÝŒ ñúùê±WÓÚØÊ-â}É›ŠqlIòêÎÞðNí²-LE¥óHW_¸IŠÝuæI•·|ÈbÏWL44êˆHV
$¹U¦¾ý¾QéùtÝß‡X¼iìÁ§#àÙ2{”ûögc.þ=™I¢âgÃÀ÷)gŸ\¢à
röã¦t·7 S«MwÒ4¾mRìÂûb÷t3Ç…Râj”._+6­ŒKOMàyð“c@º›Ñs0V¿ˆ:>ƒ ƒÕ¸á¢¥áåQ» ÎDŒ„'3<® |? ÏÁ_?Þ’âïžM¸³³BŽÆ]L­r&äëií‡æYnq°<Ueª§h}­éè£¼µí#`å>þEuF¬Ò+Pf,„ýß”Kuò8±Hc—ÝÔ®qƒ^@	Y/’Ÿêq¤Å$íS·ŠÁx¡ÄÌSíp
ÒÙTÁ ¾Ëìe»[l¯G0cïQpn¯Wfkm[Êý=1ÂžË…Gm&z–ìër•ª†_Øc¹ !ÄúÿÁ“¬ËÀ<ï8]­Œ=œÄyœˆ†úd=n’±¯Ržœ7¿qCù6•Sá~ãbÚçW«tŸC´ä2	·åuëƒ4çÕ¶\2"2ªrþßïDøœîÈ=xlØmu[‘•º@¹Ý¬8Ê´Ÿ4&c
Ytå5ÂSÈŽÄDgÌ‡ü&@änÓìJöìž?nkúŸm´[÷UÈÆVè
%{–Þž¸K{ê¯™€xÄUší®‹€ùTéX ÅDmÄÃ±ßkƒ&§@Ö;?„‘õ’pðEs‰ÚPÆ6~M}M½ FÚž¶éÄC´Bì/AHã2«ÔdŠ%øXS6pUUï¶¬ÅDâ#>Üm)•fîÉùÒfV¿ÉP‚6R˜pþàát—Ô
¾Í? ,E˜åø‰œÜùKßÜmÁ1ÛŸ¯ª7kêZ³ˆSvôóxèB‘Ç	yÏ*4hœµHR]¼y1U·Uc36Ó€Sð÷îºIP:œ{Û[¡þ€_éË9ÌÎiq²ß“r9Jp,ÇD˜Žg½#Õ‘vMÓwÑ8ùd7hÞÈW¸­÷žo§'³åí©L…Ôú¡¦¨ÓÚÜŠÙœ»—Ê=o…Iÿ£ðÎ]Nâ¶î]#`1ïW¶ÅÊEì•|ì†É’m«Ž}ê
¶Ù,N·u‹ðŠ¤Tƒèeò×ÂñÏŽâië`EóÂ…³Î_’ôfÅwp»²p)Xøh§äSZ¬¨áý¯ôÖŸâ?šÊAƒÈ°0E×g0\ßÝ• é”ÕŸÛh¤TÕæ<TÓWŸ¬PZ ãuÃ.'4d¢QÆ{½Ù‚"Ì“Mlå†¡^´-`Å//âÌâM¹ÉÁ©ómJsˆÌ\‹óÍuáQšW¤ô¬ûì>€Çx%ta{ÄéDÔPQÍÛÄ²âÔúæ’'ó Ë&NÅ#]¤ 2Jz²THªäe®Jé<·êˆtBÏS²aky -Své·CsÓ´°b!­'äén¤v”QÐCR‹.ì [˜…¨ËX/É=ðØ13y¿bNä˜šÿÄlšñßì”8	·g“•±µCœ
øþHñm5úµ*ŒÃË>õSV´¿åÈ¸6‰Q—ŒV‚L|ù2á~¡–p¤È£÷ŸKD1x¤¨‚,68ŠZf­72 }2ßT×âs_¬»’¬vJÍPâÌ4¼"–k6˜Ç,6ÙdropË´¸¯Dþ7B4ª\¢ÉÝ¸ bn'´þµ!’›&¨Ag°kHßÓ*xa]51UÚ8ÂÜ‹ÝÊƒ¿·RïÄ¬"õÑ„GøÖ—k@c=fL™Ÿ\Jü³w8®ûÖÖän /N–÷ÙLÕrÊé?¦ªY%’ŽÕÒR"‘ÊÖ‰Ò¯IüI­žÔR¶-£ë…L#·ŠŽÔ‡ªéOÂ@„;]™›±(YUÜµ­âYôšÉ”2‚·aÂ‰ïû0ÂcóŸß\rDºUøŸ÷§Ú¾ÉÈ{«æ3€«Û%3ÿÝ¶$QŽmÒ¬òSå‹º5—i°È™ì(‰)ž8¥ÞIb½g,÷4#¹)ä}hÖ1´Bÿ‹âëCïÃ³