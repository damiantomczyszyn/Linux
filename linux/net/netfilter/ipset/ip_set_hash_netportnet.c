e        *video_dev;
	struct video_device        *vbi_dev;

	/* video capture */
	struct cx23885_fmt         *fmt;
	unsigned int               width, height;
	unsigned		   field;

	struct cx23885_dmaqueue    vidq;
	struct vb2_queue           vb2_vidq;
	struct cx23885_dmaqueue    vbiq;
	struct vb2_queue           vb2_vbiq;

	spinlock_t                 slock;

	/* MPEG Encoder ONLY settings */
	u32                        cx23417_mailbox;
	struct cx2341x_handler     cxhdl;
	struct video_device        *v4l_device;
	struct vb2_queue           vb2_mpegq;
	struct cx23885_tvnorm      encodernorm;

	/* Analog raw audio */
	struct cx23885_audio_dev   *audio_dev;

	/* Does the system require periodic DMA resets? */
	unsigned int		need_dma_reset:1;
};

static inline struct cx23885_dev *to_cx23885(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct cx23885_dev, v4l2_dev);
}

#define call_all(dev, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, 0, o, f, ##args)

#define CX23885_HW_888_IR  (1 << 0)
#define CX23885_HW_AV_CORE (1 << 1)

#define call_hw(dev, grpid, o, f, args...) \
	v4l2_device_call_all(&dev->v4l2_dev, grpid, o, f, ##args)

extern struct v4l2_subdev *cx23885_find_hw(struct cx23885_dev *dev, u32 hw);

#define SRAM_CH01  0 /* Video A */
#define SRAM_CH02  1 /* VBI A */
#define SRAM_CH03  2 /* Video B */
#define SRAM_CH04  3 /* Transport via B */
#define SRAM_CH05  4 /* VBI B */
#define SRAM_CH06  5 /* Video C */
#define SRAM_CH07  6 /* Transport via C */
#define SRAM_CH08  7 /* Audio Internal A */
#define SRAM_CH09  8 /* Audio Internal B */
#define SRAM_CH10  9 /* Audio External */
#define SRAM_CH11 10 /* COMB_3D_N */
#define SRAM_CH12 11 /* Comb 3D N1 */
#define SRAM_CH13 12 /* Comb 3D N2 */
#define SRAM_CH14 13 /* MOE Vid */
#define SRAM_CH15 14 /* MOE RSLT */

struct sram_channel {
	char *name;
	u32  cmds_start;
	u32  ctrl_start;
	u32  cdt;
	u32  fifo_start;
	u32  fifo_size;
	u32  ptr1_reg;
	u32  ptr2_reg;
	u32  cnt1_reg;
	u32  cnt2_reg;
	u32  jumponly;
};

/* ----------------------------------------------------------- */

#define cx_read(reg)             readl(dev->lmmio + ((reg)>>2))
#define cx_write(reg, value)     writel((value), dev->lmmio + ((reg)>>2))

#define cx_andor(reg, mask, value) \
  writel((readl(dev->lmmio+((reg)>>2)) & ~(mask)) |\
  ((value) & (mask)), dev->lmmio+((reg)>>2))

#define cx_set(reg, bit)          cx_andor((reg), (bit), (bit))
#define cx_clear(reg, bit)        cx_andor((reg), (bit), 0)

/* ----------------------------------------------------------- */
/* cx23885-core.c                                              */

extern int cx23885_sram_channel_setup(struct cx23885_dev *dev,
	struct sram_channel *ch,
	unsigned int bpl, u32 risc);

extern void cx23885_sram_channel_dump(struct cx23885_dev *dev,
	struct sram_channel *ch);

extern int cx23885_risc_buffer(struct pci_dev *pci, struct cx23885_riscmem *risc,
	struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

extern int cx23885_risc_vbibuffer(struct pci_dev *pci,
	struct cx23885_riscmem *risc, struct scatterlist *sglist,
	unsigned int top_offset, unsigned int bottom_offset,
	unsigned int bpl, unsigned int padding, unsigned int lines);

int cx23885_start_dma(struct cx23885_tsport *port,
			     struct cx23885_dmaqueue *q,
			     struct cx23885_buffer   *buf);
void cx23885_cancel_buffers(struct cx23885_tsport *port);


extern void cx23885_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern u32 cx23885_gpio_get(struct cx23885_dev *dev, u32 mask);
extern void cx23885_gpio_enable(struct cx23885_dev *dev, u32 mask,
	int asoutput);

extern void cx23885_irq_add_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_enable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_disable(struct cx23885_dev *dev, u32 mask);
extern void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask);

/* ----------------------------------------------------------- */
/* cx23885-cards.c                                             */
extern struct cx23885_board cx23885_boards[];
extern const unsigned int cx23885_bcount;

extern struct cx23885_subid cx23885_subids[];
extern const unsigned int cx23885_idcount;

extern int cx23885_tuner_callback(void *priv, int component,
	int command, int arg);
extern void cx23885_card_list(struct cx23885_dev *dev);
extern int  cx23885_ir_init(struct cx23885_dev *dev);
extern void cx23885_ir_pci_int_enable(struct cx23885_dev *dev);
extern void cx23885_ir_fini(struct cx23885_dev *dev);
extern void cx23885_gpio_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup(struct cx23885_dev *dev);
extern void cx23885_card_setup_pre_i2c(struct cx23885_dev *dev);

extern int cx23885_dvb_register(struct cx23885_tsport *port);
extern int cx23885_dvb_unregister(struct cx23885_tsport *port);

extern int cx23885_buf_prepare(struct cx23885_buffer *buf,
			       struct cx23885_tsport *port);
extern void cx23885_buf_queue(struct cx23885_tsport *port,
			      struct cx23885_buffer *buf);
extern void cx23885_free_buffer(struct cx23885_dev *dev,
				struct cx23885_buffer *buf);

/* ----------------------------------------------------------- */
/* cx23885-video.c                                             */
/* Video */
extern int cx23885_video_register(struct cx23885_dev *dev);
extern void cx23885_video_unregister(struct cx23885_dev *dev);
extern int cx23885_video_irq(struct cx23885_dev *dev, u32 status);
extern void cx23885_video_wakeup(struct cx23885_dev *dev,
	struct cx23885_dmaqueue *q, u32 count);
int cx23885_enum_input(struct cx23885_dev *dev, struct v4l2_input *i);
int cx23885_set_input(struct file *file, void *priv, unsigned int i);
int cx23885_get_input(struct file *file, void *priv, unsigned int *i);
int cx23885_set_frequency(struct file *file, void *priv, const struct v4l2_frequency *f);
int cx23885_set_tvnorm(struct cx23885_dev *dev, v4l2_std_id norm);

/* ----------------------------------------------------------- */
/* cx23885-vbi.c                                               */
extern int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f);
extern void cx23885_vbi_timeout(unsigned long data);
extern const struct vb2_ops cx23885_vbi_qops;
extern int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status);

/* cx23885-i2c.c                                                */
extern int cx23885_i2c_register(struct cx23885_i2c *bus);
extern int cx23885_i2c_unregister(struct cx23885_i2c *bus);
extern void cx23885_av_clk(struct cx23885_dev *dev, int enable);

/* ----------------------------------------------------------- */
/* cx23885-417.c                                               */
extern int cx23885_417_register(struct cx23885_dev *dev);
extern void cx23885_417_unregister(struct cx23885_dev *dev);
extern int cx23885_irq_417(struct cx23885_dev *dev, u32 status);
extern void cx23885_417_check_encoder(struct cx23885_dev *dev);
extern void cx23885_mc417_init(struct cx23885_dev *dev);
extern int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value);
extern int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value);
extern int mc417_register_read(struct cx23885_dev *dev,
				u16 address, u32 *value);
extern int mc417_register_write(struct cx23885_dev *dev,
				u16 address, u32 value);
extern void mc417_gpio_set(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask);
extern void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput);

/* ----------------------------------------------------------- */
/* cx23885-alsa.c                                             */
extern struct cx23885_audio_dev *cx23885_audio_register(
					struct cx23885_dev *dev);
extern void cx23885_audio_unregister(struct cx23885_dev *dev);
extern int cx23885_audio_irq(struct cx23885_dev *dev, u32 status, u32 mask);
extern int cx23885_risc_databuffer(struct pci_dev *pci,
				   struct cx23885_riscmem *risc,
				   struct scatterlist *sglist,
				   unsigned int bpl,
				   unsigned int lines,
				   unsigned int lpi);

/* ----------------------------------------------------------- */
/* tv norms                                                    */

static inline unsigned int norm_maxh(v4l2_std_id norm)
{
	return (norm & V4L2_STD_525_60) ? 480 : 576;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   E������������5wDf���>��{i����Ԩ_`��x��X�k��
?X�a-@Ώ���oO��5j�a��&D@,0�:-�M���Y#�"TV�����oYYfJ�-����@�,�cr�~�T�&��m�C���&#j�P�x��9M��C��p���VEC�{z�k2h��[wK��Pa�?�r��4*׵�>4���HGX߫7\����\�ͪ��mE�:�����TQ�tt�n�r���E���ś�ok#�k���b�EY�
ٰT����o�|����Ԓ̵7��ȋ����d�_��Ț��6��&h^��n�I� ����k��羑t�{��2Ӿ�@�\ HW{��+����Z�$��r�"ƒ{��(s��5<Ǌ���/4�U�`���^���L�f�w��ٶ$��Ѽ0��&2�\ �h(d��H��l�vM�oT�g����V�2�x�4��{��U�֌�|4p{��J`��N�tT�����G Jx�=@ c��@rh{�j	-�v{�ڐ����OX��e��e��0ѹl��5!����I��ڌ�� �a�qz1%��%�y��Cw�L�6©��Μ�L�ʻ��[��c����~.���*�M�z+ձ���U��:>������*tPhi��Q�(ᖛ z�/�D�@޷/���}���I~rmW��/��D@S@:Qp����<CJu�[��F�]D��R@y�&��&
�f���T �gJ�j@3E�v^�)C�X��/��lʨT�KN3jf�ͪ���a�?\i�g��6,�=|Y����S�g�����pI��Wnoݬ�e]1��,k)��]%��p�:]H����ĪbM� ��Z��З��yPZ���+zg@t�K�٘1���B���`�
 �/��P-��l��I�H<�ے1��\��jE���k������8�ޅ����쭢U��c�E�ٲ?b,񗊱��,n�Y��J��$>M�2rW"��3��_`����8Y+�T6-w� ���B@Z�zq��)�����0�����}�'���ׂ�z�h��a�1"�W7m.���h�q�S9���A�t��_�N�R�2����S�7��p����:���i�ZCH�a����yz0�^m��p�u#X$g4�"U�Ї��Cu�!xcٔC�^��>UH�����R݀�/�/��i�/���LT�DہK��ؔ��U5UC���j��p��-�l��`����6�?�r#�����chQ[�jL��"j2~����>H1)qJ$�U��Q~Q��.�Be��?�7�J��G�y����.�r�U�0�)���l��)S���;�@��$���������'�cR1o#_��9^��z22�*��Pk`l���*���м#|'�YJ�SV����L8+�B�	Ӟ�+���;0����m��w���M��L�8u��K��>S%(S7k�.� "�w�gY�7��'�0!y�å�L���"�/���zϡ����MU�D���$���"�Q����+59���-� ������F�����y y�l�cjg�Թy�v�"�&�D��3�T�2�P��L7�p��5��q)m��yS��ʬl��!�/O�,N��Zp���Pҋc�^Q~��M�Zͬm5��n�ڏ⒛�Ω�ˡ�Z��P�\�*��tɣ�X�D��(i�G^�ѕ"�֧��N�����J���K�6}�R#:�u�騒q��Qm�̩���@FMg���pM��7P*e�2*a�mAk~Oj;y\q���;��I�N]Ă��[o^�F��\b3"���W����r	����ӑK9��kgt�0ԐJx���$�խ���&�CqP�%��7��i���J�ox��1�����*�sA0�����<	vw��t �S�|�����A㇘u�2s�{���zJ�⽈ɹ6f?sg�6���wQ�R�ك�� ���
�~���@((%Om�F�8�5�c�`�2=�K��hMT��l�5l92A��L�-��f�҃v����(�4����C>_x�AB��j� ���~O�@���|�b�B���fvqx%#��T��W��N���e��������}�Qs��g�����tٳ�������Q�+�_�'�i;o�Μ�}�J�=�*�a-��(K�	��"�6��g�ë��_��#,�9'mJ����1���dC��=JfI�t�bV'�������hh��(S��o�
�]��HD��9&��lx��Gr1R�$����y��>3�\��(� �B/6����Λ� �'a�L5��(��r�jG��M�Dj�1���e���6B/��F ���_���1����%%iU
D�15$en����׋�ꁶ����ٿ�f�B�O����a[�!5őh܄�vI�M�eo����<VY`�5��w �*Vm}%�V�\7sL�xf߽�^LfhB�51n�(2���8u�����ۨ��P�.y���VVM��[�A�M�V@��c��sӸ�wj6�����J���X��w"�;�z�~�"��(�������@����I�n��ʘ�B�Q��kT�gz3�t�nP!)�|�8��4]A`j:�(�����d��
����V��H�mZ0򩪋��0CZ�H�i��B�z�u;�R��6���R^�$Cv�?C{�"���|�6bQt\v>���]!�e��@�N���$�����w�>t��P�L������9
ԡ�L|�S[��bZ�����v��э��l�:����m�)�( ���S�� �<c9�����������q3��:+�!��P)io��ѫ�"t1)Հ��.	^�%����ۃG�b+���m�d?�s�;�;,�S��N�{qD�T�J}WXuڄ!�eŘmm1��AMh9�7=�~�2�gu����G�P4�'� S���9YҀ�����??&6B�� p��=�:����f�B�v ��m<e�1^[��V�ݡ~��O3�����gyv��K��J�NYfHyG��⩥��e����jg�n����ܐ���h�1���l�x������S�;�Uρ?�B��%�{�י���˻� 9[N��>u��T� �\y ��pttf�W@���7�t�7�4��$�	1�c��r���>Fm��A�^����cP9Lޣ�r�l���/�h4�+)3`�FX�yU�d��*Z���H���0K�.j�D�TW��z7T8ap����Q�H��Th}�AO&���Ry�s'X�%-	��|j	"}�F�y���=����&v��<'�#���K}�oٙZ7��UJڷ,Yiu߱���Y��e�d���'��[%c��k�z�L��0�uw��r�����e�������7�|����'ĭ`P���ݜq*h�f�$���?�B'L�O׺~���iUK�k3��YF�e+�sN�e��7j?)8�l90��C�v���㺃c>��%ϻ����޴ζ�?���"��G#���U�@��!Y�2���ayE�A.n��0�٪�7bL��"�ah]�����DM�we���"*�G��U�#����� �v�����b����Iݭ�8��7$���J)��(��tM-v*:G�|�Wx�"���K�j�`��[�ַ�n"U�9]��Ȯv0��6�\E�X�R�(\Gŋ~������`7%�أ�D��Ι�U1vK�]GU�y�X�Ɇ++/A5QS�y)
��n:C����f��5=�N�`�?�~��
=/�b�L����mv�Y��k L`�0��/��3H[ѹݹ0��b��M5V0������j��|�'t��;��@���>˫q
N�I4 �}����aAD�FpǪ�6^�^��rg,m���ɜ�G��Mm����䅵��pf�B��c�k�6��X�m��6y\�O飡Q�������>����>,n��Z���c�_�/�5��!6.��vO�m�?Ip�9=��K��b2ؖ*�⯯��$�� /W����f�~LC| �Y�h��L�33�7G9�7�9/�鳎��H��u������*{<A��cYL@��m�S�e�&?����������'S����W�6��[�k����3��&��}�0���1���V'6�}��-؈��}Xi�=[�{�������e{-/�