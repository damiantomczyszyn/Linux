E310B         62

#define GPIO_0 0x00000001
#define GPIO_1 0x00000002
#define GPIO_2 0x00000004
#define GPIO_3 0x00000008
#define GPIO_4 0x00000010
#define GPIO_5 0x00000020
#define GPIO_6 0x00000040
#define GPIO_7 0x00000080
#define GPIO_8 0x00000100
#define GPIO_9 0x00000200
#define GPIO_10 0x00000400
#define GPIO_11 0x00000800
#define GPIO_12 0x00001000
#define GPIO_13 0x00002000
#define GPIO_14 0x00004000
#define GPIO_15 0x00008000

/* Currently unsupported by the driver: PAL/H, NTSC/Kr, SECAM B/G/H/LC */
#define CX23885_NORMS (\
	V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_443 | \
	V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK    |  V4L2_STD_PAL_I    | \
	V4L2_STD_PAL_M  |  V4L2_STD_PAL_N     |  V4L2_STD_PAL_Nc   | \
	V4L2_STD_PAL_60 |  V4L2_STD_SECAM_L   |  V4L2_STD_SECAM_DK)

struct cx23885_fmt {
	u32   fourcc;          /* v4l2 format id */
	int   depth;
	int   flags;
	u32   cxformat;
};

struct cx23885_tvnorm {
	char		*name;
	v4l2_std_id	id;
	u32		cxiformat;
	u32		cxoformat;
};

enum cx23885_itype {
	CX23885_VMUX_COMPOSITE1 = 1,
	CX23885_VMUX_COMPOSITE2,
	CX23885_VMUX_COMPOSITE3,
	CX23885_VMUX_COMPOSITE4,
	CX23885_VMUX_SVIDEO,
	CX23885_VMUX_COMPONENT,
	CX23885_VMUX_TELEVISION,
	CX23885_VMUX_CABLE,
	CX23885_VMUX_DVB,
	CX23885_VMUX_DEBUG,
	CX23885_RADIO,
};

enum cx23885_src_sel_type {
	CX23885_SRC_SEL_EXT_656_VIDEO = 0,
	CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO
};

struct cx23885_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

/* buffer for one video frame */
struct cx23885_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head queue;

	/* cx23885 specific */
	unsigned int           bpl;
	struct cx23885_riscmem risc;
	struct cx23885_fmt     *fmt;
	u32                    count;
};

struct cx23885_input {
	enum cx23885_itype type;
	unsigned int    vmux;
	unsigned int    amux;
	u32             gpio0, gpio1, gpio2, gpio3;
};

typedef enum {
	CX23885_MPEG_UNDEFINED = 0,
	CX23885_MPEG_DVB,
	CX23885_ANALOG_VIDEO,
	CX23885_MPEG_ENCODER,
} port_t;

struct cx23885_board {
	char                    *name;
	port_t			porta, portb, portc;
	int		num_fds_portb, num_fds_portc;
	unsigned int		tuner_type;
	unsigned int		radio_type;
	unsigned char		tuner_addr;
	unsigned char		radio_addr;
	unsigned int		tuner_bus;

	/* Vendors can and do run the PCIe bridge at different
	 * clock rates, driven physically by crystals on the PCBs.
	 * The core has to accommodate this. This allows the user
	 * to add new boards with new frequencys. The value is
	 * expressed in Hz.
	 *
	 * The core framework will default this value based on
	 * current designs, but it can vary.
	 */
	u32			clk_freq;
	struct cx23885_input    input[MAX_CX23885_INPUT];
	int			ci_type; /* for NetUP */
	/* Force bottom field first during DMA (888 workaround) */
	u32                     force_bff;
};

struct cx23885_subid {
	u16     subvendor;
	u16     subdevice;
	u32     card;
};

struct cx23885_i2c {
	struct cx23885_dev *dev;

	int                        nr;

	/* i2c i/o */
	struct i2c_adapter         i2c_adap;
	struct i2c_client          i2c_client;
	u32                        i2c_rc;

	/* 885 registers used for raw address */
	u32                        i2c_period;
	u32                        reg_ctrl;
	u32                        reg_stat;
	u32                        reg_addr;
	u32                        reg_rdata;
	u32                        reg_wdata;
};

struct cx23885_dmaqueue {
	struct list_head       active;
	u32                    count;
};

struct cx23885_tsport {
	struct cx23885_dev *dev;

	unsigned                   nr;
	int                        sram_chno;

	struct vb2_dvb_frontends   frontends;

	/* dma queues */
	struct cx23885_dmaqueue    mpegq;
	u32                        ts_packet_size;
	u32                        ts_packet_count;

	int                        width;
	int                        height;

	spinlock_t                 slock;

	/* registers */
	u32                        reg_gpcnt;
	u32                        reg_gpcnt_ctl;
	u32                        reg_dma_ctl;
	u32                        reg_lngth;
	u32                        reg_hw_sop_ctrl;
	u32                        reg_gen_ctrl;
	u32                        reg_bd_pkt_status;
	u32                        reg_sop_status;
	u32                        reg_fifo_ovfl_stat;
	u32                        reg_vld_misc;
	u32                        reg_ts_clk_en;
	u32                        reg_ts_int_msk;
	u32                        reg_ts_int_stat;
	u32                        reg_src_sel;

	/* Default register vals */
	int                        pci_irqmask;
	u32                        dma_ctl_val;
	u32                        ts_int_msk_val;
	u32                        gen_ctrl_val;
	u32                        ts_clk_en_val;
	u32                        src_sel_val;
	u32                        vld_misc_val;
	u32                        hw_sop_ctrl_val;

	/* Allow a single tsport to have multiple frontends */
	u32                        num_frontends;
	void                (*gate_ctrl)(struct cx23885_tsport *port, int open);
	void                       *port_priv;

	/* Workaround for a temp dvb_frontend that the tuner can attached to */
	struct dvb_frontend analog_fe;

	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	struct i2c_client *i2c_client_sec;
	struct i2c_client *i2c_client_ci;

	int (*set_frontend)(struct dvb_frontend *fe);
	int (*fe_set_voltage)(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage);
};

struct cx23885_kernel_ir {
	struct cx23885_dev	*cx;
	char			*name;
	char			*phys;

	struct rc_dev		*rc;
};

struct cx23885_audio_buffer {
	unsigned int		bpl;
	struct cx23885_riscmem	risc;
	void			*vaddr;
	struct scatterlist	*sglist;
	int			sglen;
	unsigned long		nr_pages;
};

struct cx23885_audio_dev {
	struct cx23885_dev	*dev;

	struct pci_dev		*pci;

	struct snd_card		*card;

	spinlock_t		lock;

	atomic_t		count;

	unsigned int		dma_size;
	unsigned int		period_size;
	unsigned int		num_periods;

	struct cx23885_audio_buffer   *buf;

	struct snd_pcm_substream *substream;
};

struct cx23885_dev {
	atomic_t                   refcount;
	struct v4l2_device	   v4l2_dev;
	struct v4l2_ctrl_handler   ctrl_handler;

	/* pci stuff */
	struct pci_dev             *pci;
	unsigned char              pci_rev, pci_lat;
	int                        pci_bus, pci_slot;
	u32                        __iomem *lmmio;
	u8                         __iomem *bmmio;
	int                        pci_irqmask;
	spinlock_t		   pci_irqmask_lock; /* protects mask reg too */
	int                        hwrevision;

	/* This valud is board specific and is used to configure the
	 * AV core so we see nice clean and stable video and audio. */
	u32                        clk_freq;

	/* I2C adapters: Master 1 & 2 (External) & Master 3 (Internal only) */
	struct cx23885_i2c         i2c_bus[3];

	int                        nr;
	struct mutex               lock;
	struct mutex               gpio_lock;

	/* board details */
	unsigned int               board;
	char                       name[32];

	struct cx23885_tsport      ts1, ts2;

	/* sram configuration */
	struct sram_channel        *sram_channels;

	enum {
		CX23885_BRIDGE_UNDEFINED = 0,
		CX23885_BRIDGE_885 = 885,
		CX23885_BRIDGE_887 = 887,
		CX23885_BRIDGE_888 = 888,
	} bridge;

	/* Analog video */
	unsigned int               input;
	unsigned int               audinput; /* Selectable audio input */
	u32                        tvaudio;
	v4l2_std_id                tvnorm;
	unsigned int               tuner_type;
	unsigned char              tuner_addr;
	unsigned int               tuner_bus;
	unsigned int               radio_type;
	unsigned char              radio_addr;
	struct v4l2_subdev	   *sd_cx25840;
	struct work_struct	   cx25840_work;

	/* Infrared */
	struct v4l2_subdev         *sd_ir;
	struct work_struct	   ir_rx_work;
	unsigned long		   ir_rx_notifications;
	struct work_struct	   ir_tx_work;
	unsigned long		   ir_tx_notifications;

	struct cx23885_kernel_ir   *kernel_ir;
	atomic_t		   ir_input_stopping;

	/* V4l */
	u32                        freq;
	struct video_device        *video_dev;
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Eş˜‹Åœ²¹áõ¶¥ä5wDf¸à÷>ÌÌ{i—ûÁ§Ô¨_`¢ê¯x³ÔXôk¯¿
?XÍa-@Îµ‹oOË÷5j‡aÃ&D@,0¡:-¹Mîú±Y#•"TV…ïÓÛùoYYfJ½-£°ó–æ›@ , cr°~T¬&£ÔmğC—ïÌ&#j³PÎxƒ¯9MõµC´ùpš·ÁVEC¥{zÙk2hÒË[wKò‘Paí?Èröğ4*×µ§>4§°„HGXß«7\‚¬ëÁ\Íª­âmE•: åí¾¹ˆÈTQêtt¤n™rµóäEü¹ßÅ›¯ok#ùkëÛÊbòEYˆ
Ù°T…³°oô|©ƒ†æÔ’Ìµ7²È‹öÙˆ¡dş_ÕÅÈš¯¿6ÓË&h^šÑn×IÍ ®÷ª…kœˆç¾‘tÎ{¦²2Ó¾ƒ@Å\ HW{ùå+ÌÙÒúZŠ$°r¸"Æ’{¶å“(sÈ5<ÇŠÕÎò/4÷U`ô™ı^¿ËÒLÁfêwø…Ù¶$ûÊÑ¼0¦§&2®\ “h(d‹ÜHÆŞlĞvMáoTègßÓÃğV2Áxê4°–{ÛÔU”ÖŒÑ|4p{üéJ`û×N½tTŸûœ®G JxÂ=@ cú@rh{Âj	-Şv{±Ú«ş«ßOX‹‹e“e¼½0Ñ¹lå›ê5!”èËÖI ÎÚŒØé æaõqz1%Œò­%y»ÛCwïL±6Â©­ÉÎœ÷LÔÊ»¿Õ[ğÈc£óé~.Ôş”*ØMï‹z+Õ±°¥•U¬:>ù©ËÎû*tPhi›½Q™(á–› z¸/³DÉ@Ş·/¿¥}ÎÓäI~rmW é/ù†D@S@:Qp…•Í<CJuÄ[§¬Fä]DÿşR@yé&ºß&
çf‡Üå¤T ´gJƒj@3EÕv^)CŞXöÛ/ñËlÊ¨TŸKN3jfáÍª”û”aÕ?\iñgÃë6,ù=|YüˆĞÅS£g½æâ³ÛÍpIÂÚWnoİ¬«e]1òá,k)»°]%‰å›pé´:]Høµ•¢ÄªbMœ ç©×ZÖµĞ—ÿÊyPZ°¹İ+zg@tKïÙ˜1“®³Bü¢ã`„
 Ä/ççP-ÌlŠçIüH<ÙÛ’1³ì\æójEØÍk‚Òå÷‚8©Ş…ôùŠ¯ì­¢U¤İcÙE’Ù²?b,ñ—Š±Â˜Œ®,n¹YìJ°¾$>Mî“‘«2rW"áÇ3ÿô_`Ÿ¡¨È8Y+åT6-wÑ œìØB@ZäzqÑù)€¥­°û0 ¹…³Î}Ñ'‡¶­×‚Åz¿hµÔa±1"¯W7m.Øù›hÁqS9­’AçtÜÇ_èNÛR£2‹û£ïS·7ëŸp“ŸèÒ:Õú™iÜZCHía¡†ƒ¹yz0ø^m Çpëu#X$g4î"UèĞ‡ˆïCuä!xcÙ”CÍ^ïË>UH€©øü†Rİ€–/Ã/öiã/öá„ÀLTõDÛKÑìØ”ÔÿU5UCâäj‰¾p‹±-ªl«Á`ÿßÕæ6Í?³r#úµˆóöchQ[æjLõø"j2~ÿÃûç>H1)qJ$­Uğ¶ËQ~Q’Ù.àBeÉË?Ÿ7‚JÙûGÕyÏÂëé.r€U»0Ã)±èÙl«’)S†ÿŞ;Ú@±Í$İàıù¯¾íç'ÓcR1o#_®9^Çz22¨*êÓPk`l‹˜Ü*’‚­Ğ¼#|'ÅYJ«SV‚ûí˜L8+íB	Ó¹+ğõ;0‚š©®m†áwÁÌÀMöâLª8u•İK´ê>S%(S7kÓ.‡ "ÔwîgY‚7ƒİ'0!yÄÃ¥÷LëŠş™"Š/ÕÓæzÏ¡š¤áÊMUÔDÒòç¢$ù€Ş"åQ©á³íÃ+59²é£Ç-’ ¥‹ŸéêéF«³óäy yêlšcjgµÔ¹y¨vÏ"Ó&„DêË3¯TÃ2èPº€L7ÿpøå5ªÍq)mÿ©yS¿ïÊ¬l•¡!°/OÖ,NÆÁZp—ığPÒ‹c•^Q~ËÓMšZÍ¬m5À›nãÚâ’›äÎ©äË¡«Z€‘P‹\¿*²tÉ£ûXÏD¸ƒ(iµG^êÑ•"“Ö§Ê›N¥ÔŠøŠJş¯Kş6}ÓR#:‹uèé¨’q–ÒQmã¨Ì©¾¥¡@FMg›‹ğpM„Ò7P*e‘2*aîmAk~Oj;y\q‰¨¹;µ’IÚN]Ä‚‰©[o^—FÎı\b3"œ½¶W•ƒ°Ğr	ö‘¼ÒÓ‘K9“Çkgtå±0ÔJx‘´õ$ßÕ­‡™Â& CqPÌ%öØ7¨Üi”€ãJ·ox­ë1‘¢¨¼*ÛsA0çú©î<	vwÈ¥t ·Sş|Šı¬­‚Aã‡˜u‚2sÑ{¬ÂzJÈâ½ˆÉ¹6f?sgò6³õàwQ¦RÆÙƒ…‚ µ¦Ã
Ø~ÿÌÅ@((%OmÇF®8°5Ğc¶`À2=ùKîíhMT¤l5l92AŸèLÕ-‹©fòÒƒv¨ßıÙ(ß4ò°íŒÙC>_xÏABŒÙjå áö~O@äÂä|ç¿bÈB”²¼fvqx%#òËT÷¼WÕÁN¶øºeˆäÔØô–´Œ}¥Qsöıg í§ø†ÓtÙ³Å¦ª£˜şQ+å_±'«i;oåÎœå}âJò=Ö*­a-±ö(K‡	ñò"Ş6ÿƒgİÃ«•Õ_¨ #,í»9'mJ“ùÉÙ1û¹ãdC©›=JfIàtöbV'°¢ÿã›ëáhhµ¡(Sö¤oª
ó]¹¦HD¡â9&çlx”Gr1R$¡š«“yı¶>3è­\Ÿ¸(î ì­B/6Ÿ³ºÈÎ›• Œ'aØL5‹(†¨rÆjG‰­MõDjîœ1¬Æÿe¼’í6B/ùªF ‰™˜_‚–Ã1ÓÄ°ï%%iU
Dß15$en´àŸ˜×‹êê¶ïêï¢ùÙ¿ƒfĞBôOÌóÎa[É!5oÌ‹hÜ„ÏvIòMÏeoÃûö÷<VY`5“ïw —*Vm}%æVƒ\7sLÏxfß½«^LfhBÁ51ní(2ø´8u¯‡¿äÛ¨åÍP’.y‰Æ™VVMáğ¶[øA²M¹V@ıËc«³sÓ¸áwj6ˆÑê¤ç¢úJíŞøX‹ºw"û;Åz”~ "”»(ËÂˆÌíªû­ü@›£€ˆIún‡šÊ˜ÑBâQ‰ÙkTgz3štç•nP!)–|Ñ8œª4]A`j:Ö(‹¸ü×d´×
¬šŸÌVêµHò¼mZ0ò©ª‹€¸0CZÌHğiûÄBŠzÍu;ŸR——6éò×R^ê$CvÛ?C{¤"ü¢Ç|6bQt\v>‹¼Î]!õeşÕ@ŠN»™‡$³©½Êwñ>t‹»PÎLÎîÄÇ¡Ğ9
Ô¡éL|S[¢¶bZ¾ŠšŠ‚v†ïÑ‰ˆl¬:¼†º²m„)ü( İ°ıSŒ à<c9·û±ìşö‹ı¢·¦q3ÿ”:+ò!¼ÂP)ioºÑ«‹"t1)Õ€èş.	^Ô%¹¼œÑÛƒGÊb+ÌãÚm”d?Çsó;Ô;,³S¸ÏNµ{qD÷T‚J}WXuÚ„!ŒeÅ˜mm1èâAMh9ô7=…~“2›gu•ÈòG€P4‡'é SÏïÖ9YÒ€…›ò÷??&6B†Ó p›í=·:ßáµüƒfƒBv ç­Úm<eê€1^[ÚçVßİ¡~ôO3íÀòøÊgyvÁ³KïŞJëŠNYfHyGÿÌâ©¥šè¤e†§ûÓjgnò×î‰ÕÜæë¾òhˆ1¶öøl©xÿ¢²’õ¯SË;UÏ?©B£û%{ï×™ØµóË»© 9[NŠ>uÏóT½ Œ\y ïípttfÂW@Ø±‹7‘tÛ7™4³·$½	1Èc»Ìr¶¦™>FmşåAğ¥^Îš±cP9LŞ£rÃl¹ı/üh4˜+)3`æFXÏyUÅdœ¯*Z¦ÎêHœ¾ó€0KÓ.jÌD“TWÿÚz7T8ap©ÿ¹†QH‹àTh}¬AO&‹õRyæŠs'Xó%-	Š¤|j	"}ÀFİy‘Òı=ÃòÏ &vùÿ<'ÿ#ÚÄç–K}¢oÙ™Z7°“UJÚ·,Yiuß±şİÇY¦ÄeİdãÖÁ'Óç[%c“èk¦z¾L„º0ıuw½Ór—ì÷½Åe™ßúŠˆ7ò|¡‚¿Ó'Ä­`Pü¡ñİœq*h¥f $¹½?åB'LàO×º~¥ÉiUKÁk3óÙYF‚e+âsNŸeİô 7j?)8¤l90€†C™v†Æ×ãºƒc>ò¡%Ï»ı¥œ±Ş´Î¶Ğ?Ñûæ"ÆÚG#¬ÈöUõ@ûû!Yõ2ı¹ˆayE¿A.nŸã0î®Ùª²7bLá£"’ah]éõÌÀ±DMñwe÷šû"*şG³U¤#İÁ‡ÔÂ vÜíŒÀîÁb¨ÚùËIİ­•8ñ÷7$ˆ®ÎJ)ˆÇ(•‹tM-v*:Gï|úWxÃ"ŸÖÓKÃjï`¶Æ[‰Ö·Øn"U’9]³ÏÈ®v0¨æ6Â\EàXıRâ(\GÅ‹~èŞöËõ†`7%÷Ø£ŞDŠ§Î™…U1vKÿ]GUñyXÎÉ†++/A5QSÊy)
¢Ïn:C¨¢°ñfîĞ5=§Nú`ë?â~²ú
=/ÿbŠLÇëÊÿmv¿Y§ñk L`ò0—Æ/•¢3H[Ñ¹İ¹0­³b´çM5V0¦ìÇÈÌÏjÑç|²'tÒò;©›@å÷Ò>Ë«q
NÏI4 ñ}º…£áaAD½FpÇªµ6^·^rg,m„í“ĞÉœëGª®Mmé®ÖØåä…µ°pfÊBäöcùk‚6·Xé¡mƒÒ6y\Oé£¡Q‰¢ôúª¶ˆ>‹ŒÁä>,n“Zˆªîc¦_¾/íˆ5¶!6.¦vO¸mÎ?Ipí¿9=«†K®Ÿb2Ø–*êâ¯¯²´$áÑ /Wœ››çf»~LC| ÚY¶h³L¥337G979/Óé³¾†Hˆ“uşÆãêÉ*{<A˜cYL@¶«mÉS‚e‰&?¾ìÒ¥Òı‰·ş'SªáËŠWÛ6´[€kõÀåªÇ3Ñ÷&Ûâ}®0““1¢ê…ÖV'6Ï}·ƒ-Øˆ™}Xiü=[´{ùá÷·øöe{-/—ößLûŒ™‘µàĞÿÌcéş³ÔU¤œ	&u¹4K8ˆ.Ón.ˆ‰Ïçe6F-0,íyP9y~ÿš<R4¼\”"C-€á]Åt@é¾)9óŠÆú,ß×­¾É´:.fM„cë˜1GˆÅ<²%Ø/ U"gÀÕQ?ğfà¨;õ¡“#¼Ÿ5VWC}ı††É^ï+†N;ÕÔ¶Ü]b î¡Õ±èiSˆ„g=Ï=°ß+6eóÉxÌ\òwp0#“¹ÉÒEãÅÆ8éNŸÂJ[V§(çùyyØ0Gj2Çn^\Ú¢GÄ ó.ñ×ºW†šTÎ4ò-·•¿jÆX²Âj;¨€êŸ#ŒLÏJÈ—|ŒîNd5#Ï—©BÜãZêyo³ökÒÉÄK/«öá
	3™3fAgwëÙÆèáo†ıÒÖıqTJb¾Ôã|èR9tR˜0Ôìpæµ¯×—İŞ-Õ®—Ğ9V"vç´ ã¯›õãØ4×ûrÎ—Íçwü‰°,ªQ6Ö¸Ç¯Õ6Q£fÊÎB#LÕŒİÒ7<¯½ÂL¾ıôá»	°èo©éÏ¤Ÿb»"Wü¦{rø,TãëI9ÜÎ?™AQ¦wÒVedŸÂo=×ˆ,¾iâÃ*Ï«â³”üõ
6~»^Š~NkIk×iÅ£™Xká%À%)ò+³Êëô¸¯Jr¬6Ò}áQ'Şˆ²H„ÿ¾Û‚\ÓH,)¦îSïgPCÖÒe÷LËæ–oE¯ßsV eÎGùHÄT¼äÏ»õY½G”&C³Å6@Ûeï23ÒQ¸é/IæíëhöGçC ½QaøñI (zPªŞ¼òJ´ÍkÒÉ›¡Â´ô¹m?°Sƒ!«[è£™Ï-M§EÊ¯rõcàö‹eª_şäXÑ	G æš'ZIÚ²z«]ÿ^›ñ÷z"AÁEa¶Lçg7"Æy Á¶ËvÄ'12–
Ú”ÿj[/=V“•Œ,ù©Ò1l[²gÙaCğA§µØ)i°³vÀV¶¬—yêg³†öQˆÙ(‘øØ6î¯<ái²1M¦CQÎ–=áP³+µKÌá=]Rê¿|Ä(TÏ:*Ù¤$mn,ÆgZ0Õ±Ãüï,ãP53.¾'~¸ï5„şŞğMşÜây¶}™¬ä¸¡0ZÜ~Ê¯g*øĞ'xÚ£På	åÚÈÙÀ"ÇÂ~UÆ ‰fâs½””qqæîu×š³è:~%®Á˜ÿ}q'ü¤¡ º*Ù5=º<”YşÁ`…h“‰3¼à	ôï=¼‘^t
1ë]dysSÑ ú"ÂøW?×÷Y˜CÎx‰L4ÿëŸ6“Ä —e…ñ
ˆ­X¸:¡Kš¢AäÌKó(i {.zSC&(›ÚlûMU #Á‘-1+áØáëÇâ³EÊ9F?Æi+YEBELüíÉô*v,Æ§:äÛã…qXJ§<ˆÎ€ZNdªHHcpù^ïîÙÿQ|Á«mì@LxãË¾h {L)ÊÉÑë½-ñÔ>1¥(6ìô|(_§ê!íeIHz±¿›`…ş“Hìû¾µ‹¹|nëj\¾‹m9è´Ô×öĞÌ|;˜È¥ƒ¾¨­Ãí'MY´#ÏGb¡ı*:ßœ'ós$uk‘xM;€–Ü·°ïÖNÁê1ÈdŞ½œ¸åx†®Òh1ú‹*à½©¢»ŞÄ-k¦Ä)AÅi‰Ü¿DÍ :¶q¶»ƒµ}w‘ÆxbÆÑS]»ír$ğáZxÆÈÇÜşéK¶O-Ô“Ø …({Æ£;(’È¾z|;Y¼ıvôFŞ"øõò Ÿ¤@8ÏSÀ’ ñê`Õ›¬É»!ŞA‡­4]¿_à™È
ÆçËÅ÷‘5?úá ìÊWsãê˜qX4Cw›ö+é˜Àå´F˜¹oaÍÙŞ+Ö#S²K•ÈÀzçÕeÖî¨A›!—*‰¹É¤®V'ÌJ=ÌÍ¬ˆ6VF½h[¯
s¦¡¯÷F> 7ğu…ûştaüµ,M”ø—½Á‰Øµ
ıyá±÷md³¦†ÙÆ¬ÓÛß–7MV9%–¸ÑÑv«Fa4Ÿ¦&û‹„¦¥Î³ŞŞ8¾²˜’3bk™ë-·Óõywıæ£é¢iÅálşA“§ĞŞ™.fzgEö?K©	Ñ}ˆÜÅ¹¾°¢W¢šo-@³ÎñªG»ƒ¼OåU0h~Ôzî„-ÖEæu¦*²=ÍNA ßZx~§zÌmè+ûY0	oèW;óáÀØªÀ˜–'¨•q×÷ŒÅéÉ=¢NÔ&­EüÃª(£±~G»®¬?­¤ò5“œL´åu³
§3a€ ¾ÈC}A¥ïN'BUq°ÂMâ"¬ á?ˆ£Ó°©’pã×Ü?jMR»+al–Çæ$PÄg]t¶RÜ¨‚ °Xœ!¤ß¥l`X¥¿pı‘J…¤šT[-æ¯„‘ï®	Àºº‚Âò5ëì”¢}"À5î§³¦Ü0
Tš³Ô÷óµZÍ‹y–€q¸\=y_MÄ|´†hgı([mlÒB+Ûä¸àüÜ”>¿,q®ÕßÎH€/°ç…ó¾•H‹E”uÕøİ¨Ú+°Ã˜|ç+8ZJz±ü´<éTfëhYüMaùê8¹³¥rL.2ëãiçlS$šÑU¨¢i¤÷›ŸêE{·wÑ	ÜÁşƒ€KİV¢4JqsÆ“;êÉ0@QVšÕ¡¥ï˜G ÛB-©~qêXò=>Õ2%HEÆËR}Ï­›Ó=.Ó¢öİ“7ÛzŠÎ ÿ=
½˜R|—ğ¥‘mXw¸…²7©Eı»oÆ],\¾µÎ­ç˜ Ô&÷[˜j¶	.R¢
Lº^‡ ¯Ü–Kù-åË
ÍÇ²iĞO	S,´%FM¨Cf¬ bg|¯úgL™L¹­bÉ½7oñCÅx¼eÂÇÉ«TH¥jô·¦Ÿ^§Š÷O‡¬óÃfn·€fÓSP¦ëpºl`w)¾`Îd½ECÏEfÚFdÀõ™…Â§ónÑ××XãÓì“{T$-æ÷Ÿ^ÔùJSİ+å™==”Æ? 4`W½ÚİZú`[´İÄë¿”53ÈsR‘î{VW_C•…-ğzK°åÓßã¬ÂQ_áXƒŒüÕö$ãÉR ;>‚`Ù§ç¸®@ ù#Ç-…ÊWfÃ oÒfj $ÄY‘Ô\µBúé”bYöPË«\ÍÙ˜×îUi0¸>k¥ì§ãÂÚH¹œı‚¨"U^ñıËˆ?0>‚?Ãô>¿§ıá"5¨D„ƒk~°Úz–ëĞJ§±Î` tôÂåĞÇOI|¨0‡’ei-Z·æÁ‰Ä%;$g}U¡5q)‘ñC"ÑäŠãf(’ğÛ5­µÔll^ÙÏ^<±÷iË%ıd°O`,5HúœÆXÿG¿|DRQ€æ;óMÒİD„:¿Æ vB“uV|–¯ò™¡2„ä*š¹­¥^Rí–[t|òèd7Gù)©™õûÄ9xåÊ‹¦Ëk¹†dì&;G¨'õ!ÒŸıR½3ã9*\´Ğ~oğÛ¾ÙÁ5–M¬|º´…Qõe&Y{ş<^Ëãg 'à»>ãq[„ÆeH\aØ,yÒïŞÆVĞ|BCÃÉ?ˆír¡·] ­uK_˜8ğ»x… X°!ƒLü6¢QS”«ÿÒ9¨N‰ød·ÇZEŒ‹¯\9¸×ë9¨4KHXI([?ÆPş‡jeâú PæÃşŠ#Ì!Ñ36Ô·çºò´íñmÊ”x#0P­W‡š%Ÿä.'ñ/K>=L˜¦eHL$$‚R;ËR 5ÃlKI¨$|ª8ÿéCwD7uz»ix6·Q[d‚¡ß½¬Û9«<ğº¿­n:m›£¹&U× 'EÍnşŞ°8v/=)U3¡ÄÛş’
“ÆX÷(½´p¯Ê¢¯5guÌ`bB.Í7İ™],9Ë†€¢£”Z%¦[‹k}¼]ßî”0Á›nrÄ¼–SLd]û ˆ¨÷bñ¥á±‚oÇLíuª¦A¢,[œv³=ûz]Ù':ç°å
áîUETğm ğ0e`› °"GRıŒ&Cş~Çş±ëæÙNÖ›Ğü¢y9\r)ƒ@ÀM[ŠÑÙ@±¥™ÙP¦w³\8@Ğ[İ®\7 á>~Šä•	¦·ÚV¦kš¾Ù%Ç»ªÓ^¬W|dÖ¯Ê—“
ß-ı$ûçBR5sEp1İ¶Ó¦ÏøË·@h°ùŸRÁècshä1O´¢Ğ>ÛÙGÎÎäáø$<ÙÀ‡ 'Aúø.ºÆ­G©gˆêÓÆì"q\ã—‡ØZ{K ‡Üğ9‘KìUD°.²[eØİ}4Wšãô((‚û¹[]_Íe,Ğ_ÙŠèjÃ\øaÜé4”;ÃÂÚ!Eò ó­Ø¦Sò„×x‡<§mpŠä@~AÆ½<uĞ¾=(‡0Iƒ3aµßss£±Y0ğªûô)†´¹~^6[ûû%_‰@É¢%³Z€M5Ø§HùöaÓ=³äÒSpÌ3Å…#4"…
~ıwzkEis¬Ò©r´ìWÂİÓ
ª;ü´(V«‰1	'í#gN»t,5Ùq¿^Ùàİ~evh%kQ¾zİo‡œÎ¤t‡ŒZ	ej%[âå(k„¨³õÌ¯åÀ
B…Œk/ g„ÈïÁ_šwj…Ñ±3øÃæe•ÒzíjFıYŸ"Š¨j9Ûg<C÷^GF?Z¸›ğ¢ÛÍ›•ÓtL#âí²7;öÄàçv!!ŒÓj$ãôù­©Šçm~8§ru‚óş‰yq'O ÆP ¢’D$k–&ìxXZd.ì´}½¨'w/í+…±ã~vÜ2¯Û‹³‹M,_ó›äƒ-›oç‘ı:B»SSã2¬2ŞU_`S‚¸ç,EX\tı«À£±”3Bµ	L*lâËÓN$æÏÇ
£™½ÏŠ2<y)?"fÍ·½6vè®­	a/ûßÄÔ`‡°õ3}¥&à8!,h-DÙGÄ«÷–Bòy~»çO˜âs
äJbÚ„ÔŞ°NÁã»Udı:ø®£Ú8(o“•—O+Å“™@û‡—wáÍ×ÖjaÌ^mÅšI¥SÔ¼L|}’/÷4îÚôlqtcßOd’¨ßÂj“ıR!!Ÿ}Šz«¸Y|a‰™míWÇûLÍl*ß2ÿÙ+4åø0ÿüi­_ü­}‰óa÷ê7\˜+Šˆï<'u«’ë„çÎ+.¥;4_>ÏLUc?Q ÕíL;ÊÖƒÉ½áŠ![+û5úø ê”q –İëğÿ+Äš¯¬1s”jÜl¤‰ÒS ´VFR*"m‘¸LÍ`Œ˜ıÃzíÚ¾ñæÒÁ™Ã•(u?¨‘ºOİÍ~G±3Üúª“1gÄâTc;P¶ÑÒãEQÜĞÌş¶Ë±¢ñq\v\=†ßM _sƒ—	÷I[è 5õ­D¿_Òv¨Uki„ùzl÷ı3ˆâûÃB+~ÒêU¾0p¾ëŸÖàñßı`¢÷vÉEòÂ*á›†×!Òvãq°aMŒ>eÓLh	«#&£ÏlOiÆß}YÆ œ|(Ş’‘N-]œğz¯œ‚;ÉÂ6´á’”	(S)i8Ag­@Ì:ÿ§Õ.x²ÕB„>!À‹İœÿt§øßªkÁğ/ŸçğóÖeñ|tS!Ê/‚ÊÙÉmMùºóm×½¾Äq*è§fíÛ[i%ßbjL*}†“zòƒªGŒ¼ÊŞ'	·Îä—lŒ°¥=4Ñ¢ùR3rwF·pJ³ËÁn¤åTÎ¢/pÀ“[x€¹Ï
$9nò“0š‚ËKµ.^º tùĞ=ÖıŒÉL Í	¤£o¶æ}ŸEK!Jšr½%ì=…5ê^ÏİgPDatÛMŞíµ{ÿŒ ” ÆÌ±Úm»é|ƒh&šÍá¯UJĞû¶V6®d‰Ò¡›Em_wšøõ‹tD«,¬È¿®¡4ÊĞÁ_:–áv I«'Ï¯ïÈÒw$äñkmÔø÷&bÌ'ùNü2U÷€öïU;Ht	o‰D[v åÓş÷ßÃgZ-¼“@N¼´s¸ƒÀ¤À$.}ç}§°³¨å×€CØöCäã:h0*27YŞN³®¦ÌØŞXö¨áøÁ|óÛ[35#»_¶zÑ˜fÂšÄ¨³$°J.xzfY‡	—]‘>ùå¤ó¿ûö°‚²˜²gŠ9¢4±‘AÓ‹¸(:~èæ>2’|Eìg¢)ì«ª¥ÓÙœ-ôKn†PQE®Ÿ‡+pw4~ìÊ÷¶í’_ôËq!~›ßûqØ‚óøvŸoé¶Š€´ÎÕGDïzseÌxâêKCøW†y›¹¸¾«¸jzîò÷¿‹dzØ‹‰X
ıoÀ³(2Ë¥ˆ˜{AùvŒãéĞ…¤/¹ñs+H(iìók)ïBhdaH¦eq˜P]â·u/ÍjİŠş·|@AÛc)zhh…SõW©]ìäš˜›$ÂÛãoİ¥t7SO%¸¡ -`PŸë½N’HCšlŞ’# å!sCñ	İäL‘œl0,ûD‰lÒ–Ú$¶€{ÿ7¤zEÛ­˜ÀÔÎrÉqjkÏ{÷^È‡ ªtGÌ(Ö=v$[iVé>5*…’–UãX`õÊ<W¯¦Pdìšùyå°(ÜM^ê¹ñ!®'€¶¼ WùMºpX&Šêœ2åøñ´äÃ#ÆÏAÒ•»¸DzùÁ0·)J½(}ïªÎò-“ğc¨ùEËüÄêxNì'şu÷^UÃ4°¾šRÃ¬œæ„{•¼•ÌyO°Í&¥I1ƒj[Àÿc;/=Ä¶>$kÓ¦íY¸[şœ:!3ƒóTTÁ) 4+–ï •İQŒËÔgÀw(öyUã·îj.hj*Vœöúõ¦ôß>àı–M#·³ã.jÖpÚw[~/JÆ–¥	Oç54B»§ó=ütA˜kû¢æ`ŠÃ'(¤VÙenÏ.ìú†öDB˜(îgV•l#—Æ¢]+aGiíà^g˜¤hŞlB©Ùã´	UãŸ“·WœÅkóì0ß«ËĞ+á®†ÛÌvüıcß	huÛ É!ÿÖKJºzoÛ£iiÈQ¢Äœ'mhı²ŒÜKB$dİÑêGŸªAáo™‚¥~qú€öL*n8”Âù.šü÷¥a|Ğ¾\¨*´ö2ÊOÄa+5Àñ'10êùÆ'¶Ë>€×tò¦rã­öçßŒ|®rˆVxYONIF^$! Q‹[ûÁñş¸Ê^9¼ıGÌ0äO ÎıGÚ†Â?ê$5óe"Bd©Ğ†¼m¬ûnÆušÌ¿¦Sd%@tLïEÊ‡rà.‘”Ä)¬§!1¥MäD~<šMÑ%(î`<Ä)Ëå[â¬½£Ôç:}„!ã9Î)+†{ş„©ùÅ¸ë·q5»·õ•“÷ßÿuÒe§ş!^FêkZIROİ'0q$	;Ó,Ï¼üc27’@*1¼†öYÄ#åü³	3ëÀ‘­¶Œ(KÑÛdP¸ ´êğà0¬‰n¹nYœy¶­Ÿ³vI‹sòKÊ1É‰2É°Öòÿ±,¢ÀYŞ—’åá0E1…¢ŸØ@Îg›¨-Zlò§.ÿÑ'å¨şXÖMõ¬}3•€¾+¸dŞ‹F†dÈ‡¾öÌÆ^xG£û-ÙÉ?åU¨Ø´‹Î¹€6íRqyÅšëméLŸ‚ ùš^íêÏ¬?¾îB=)ªŸ"1u£è—R•-‚%ƒ'“+€q0s¦‰Q$1ŸŠ_ğ¿éİ­¨€æY(¹âkv$½2ó;Œ'>œ›èP üvHtĞ	LâÅWDxÉ?È‘âºóŞV+¦k‚@mè°R<Ì#ÃkdÜ{´ ÒK¯Ü¿tpZPr7¦J±5|ÌÙ|)èÙmTdÃN³Ä¹eCó“£Ç*M¤õ&¾¸¤ŸhX‹%îÍÛ’Ûô8Ã÷)!L~ıÍ.›ùŒá%‹Áe6HáHX‘Rm'…„Y†k¨ğ"­$Â¶›«Ûƒ0/ã–F“LÜßN†é~ PL]è-v¦÷†íöòïÏ¾ÿ[É+q(Pç´ñ|7ëGß1Ì[tgxhîS”6ê‹şÃœ­Ç+£¿Eÿ<‡S¡äÿµš³ïÙÑİˆZ7Œ†À9Ôp&>¿*9ÌÖÌ½tª~ãH‡GŸ`ÎpóË·Ãá³4úi*H‡¼_8‡mZuÔ6ô ³)ˆ~´ÅKØròW´zìv}nOz:{));ëßÎÌş¶ó:5^¤X×™¦ÏD4İÿê=l˜¾Ù@<åaHèĞØö­©ÈPø1Ğ `¼+¬ü/Ç÷ÛÆLò êt®îÆ-ä7RæğCtæ½§—aí•¨é«A‚_PBc1{I÷8C9‰¿"TF}ƒåqgfŒ”Š‡˜xİµƒSøR¢{QãyÕæXÄ0Óéæ.Ÿ³ı!ã—/ô¦+¾WKe"¨»¼!Ü|åW,;«¨ƒ™1-+¦;^ÃÜÓÈÛSÌˆ>X•£\Ô}ßšÜßuòÄƒQò„	·ğ­’œ¿gÃEI¿Œ/¡;aR äÛŞëiNyÂQñ@§}uWCUË¨íe†g®*(™C¦<8"ø¨&(j?+GÎÆ’„ÔS~esÍQ ²t»SºYğ§ã5	â¾a‰:î½ÌÅ`™uÆÖîşSü¸âI?›	EĞ^â”hów*‚ö8ß"èÆÅri#ÀêÉ÷æŞí‡íßL‚€‰Y	àÓùàÔ!Ö¨CÕî Î¼G[ÇŒc¬öo-ÛÒÕ¯ügtç)ı·ıçë ²ŒìöS-8ÓÜøù¯¥…Ùøos>âb	híâl’o˜ğ¶¦lò¬±Œy=×šÅ°ÛÑûé"RUÓ±——{‰@Ÿ¬lÕ$_W;NB¿ü7[}¯¢d2{º	ôÊ~yÌ«Ğ°ÚH¹€1à4	² 
HóÅÀ¶±#>O—…>ÏÿÖS¯=Óµó‰İ¨újÍã–Ù3€Ü¨áv°"‹«øô­Â„q1xiÂº™øÇã¶µ³İmk>„T³õòß'„Ai4°hç£¯J^/Æ‘X j€~'Ù¸¸¶ü¥@¥ªP\×_gO_Š‹6ü5epH_o‰ƒ1°híüösœ7Á ]Vvê±&Æ;xRa¥ìJâãèŸuØú^GO ›^Öw½t-w‡Zÿr—äåğÇ–±3;g›Ò¡şøÇb÷gˆuïö?;¹R¿fsz¨ÿ·¡Æí¢•œï™:ÙÌ˜<¸É`bû'rÔúë‘“´¡0ÁLñûj
s^İ “‡‡3ú‹[§Á‚ÀŸ¨³Ô@]£ºã´CÚ]•XËÓ¬ÿ¬.ª¸¦·"‰5T`4Jİï–µñÑ û+9ÎåˆÑ`“6deæõ3QMø: ^¥Ìç Sï[çpõºi¦!|PPí:çø”ÂÄbêœ6Ö]Ç½­Ì‡ÌA†ï/[G«ÚAô+ò|³Ä ™¸¹ÆIn<ÍÄûøÆºÛ”*•2`l.‘_\[wöàJ\0L‘€ƒ/]	+—Q.ê<gK¿,Î²dÆCùK“zm¤¸è³²÷èœJÊµ»ßˆ¹Ş)À¬éä`?ò™¬oQ}³Ş²ä© áx<rghê±è‰¥E]*ß«ïx¼È%iûÿ¶Ü=	ğæ4.)Ì²¦B¾ip¹”‚ë}Ÿ0·ûX4j©€%9êí¡{S#ë93
%ëzmş7ŠÄ£^Ó9,ïØ.È™1y5Ü¬4]Qòdæ•w/\[³¸ğè$ *µÍ¢×·Öô¬–Laİõò.î;UŞ!‚˜”qè²õß‡DªÌ«ä8ëÜÍSËCNñBÆ¹ö‡Â’ãô0¡_Ô!š«µÙLOI‰uëŞ÷O½ĞéY<Õ›³Ôb8áğQµ¯êªM'®tPcÍ^ü
ÜıI«á ö¨ØiÀ™©°løşmGéÇ1ÌXªipÕØƒŞpAe±—aŸØ£Qƒ/—¾$ú]ú	eî7®xòî<d/|w#ãÕ:Á[ÕQ„·ë‰SëÓîÏ‹ÒéØRkœöQ×—@ÜP„Éğnê†Œõ]™b„Ÿ‚££LP"±²`Ò-[p¢1õ!Jü½×o(V“Ú§Rï‹¹¿¦=£,Ê çK<ëpĞjÊÄk5%:Út¼jâ&®E«Ãf§/æåy¼»İ‚Ùø»= ç0£9İéV°*&£	TO2c'²,ğ¯b:d‡•ô£İí61ô}4­1¿F†§nßö"^,(s\*7Û%^Î_ÁAù÷ó ^AtR¢<_Ú ‘Ñ¿‡âHÅmºŞA5FæóB±ˆw ]Ÿ jõÍ?AÖ¼ÎpPg[(`ÿwuÎL-ZÜdÀ@Ö015:?°ò,¯ÎfMöqJ€Ù—5èü£(†É	*§”ÜçûÅ<É=ıqVË¡i®†rµRoÇ‡ş8Í”¿%­z¸çyØ³´Z6¾§Yá/_à .¦¢PãQñ·ğŞø¬\±GÌ÷öŞ}µ¥sÖ”´azT:m©QEÔ(ùÆÙ²rê…?%ùH¸o¿cÌ«{p×9OL Ğ4“ƒQüFZ×OúÌ‹x
íls"5öûşì„¼“üP´×t%ÆÉ˜jô_½–-.ºÿ6ÚœÙÛRˆX5Íea›²ôSÛÏ`Ú.&Ğ£NX5æôø…÷wtç€›ñ%‡û×Áï×¸Ò‚qvŞ·Pd!hGÆŒr†6]+Õ´”ÕmË­n	ãäøš^ù;(­s€şƒñ"ôùOÉ¶•ÉMW>mYõì”rT`váZ¤ÄuVÿ~¢NHúÑÚ¡”½¨š!>7v[n%¡¹Tû³€üÑêsöø´ã‡¶:Íp¯ƒá Sa<Mó(/>‰şb_%Ìş—¤íéÎæ†@¦†<°”_©À«ì$ƒ¨uR…¤éi™/8úKV5_Àdf%×1î¯Áv†ÍQ¼>Â+c6òpSé¬S²•zÜˆ¤ôI>¢E¬Øx-ÌÂÿ{2vhµãqÅ	Õ=&rÑEzjòòY`Î£.á‰dŸmûÃª§ÅÀçÙ+¶l­¸ò©Á˜si‡pu ¤…ïnsCQ#¤Æ_ëOØ¿e”nqŒMÄcïßìLUK ¶AQpn‚”4»Zï°düb4w`”È§YËˆ=å”ô=õEbH^İk_ù^¼	:È2‰3á?ğ(¦‡]v¾Ğà¸cÓÑÍãû!‚è¶oøë¿ªnäñÈÓ¶iÜ fĞ:[¬z]ìï„s¯qšÍĞÃøb”_`Î°,pIy?¯zS›ÿAe*×b« [†#Mé=@Û5p²JëPM0\¿¡»³7Öhé(ks«E×ĞÑİÄ%ñ¥}3J¡rµ‡+­y‹Czk(É{ä÷#tjúŞ–>Ğ1ğZlóO¾¯ml8–4‡Ø³HØF#\E®áD*‘°z,Û”š#k•Ô!*^;E™äñò…A7pY¶Ö¼Ì^ıE(Úûø^7‹İó`øöR¹ˆ8CÀm¶[wÑç”$“ÉÂ€¡‡:ü¢ŠºÍã«	Q–ft»–@1L¶ªWZ%¡guâaç`r«KNÎª÷$Ÿ(ƒN®;Hi†©Ñ<”]ùìe†œŸÏ†Ù8o4“„ªe–cÚñ0õöôÉGvœjFå•9GÒ[Òqï¤6XÿñŸEÍjœB¹¶CëŞ—±==TAÍ_Y;§ÎVü5–iL­PiÏ¤_‹Ê˜å±#E,–Ëq}¢ıÜWµÓê«¦œæW–ÂcsÑ¿³ñØ#O}ÍÛ‚DåP»Š'•gÿ[°ÔºÎåè˜ï·Z™ûaï¹@"á?wÃy•¤¦÷87Ôšw ×ÚñâKÒQb„‡é½èyø…c£!£Å`ÈŠÉ .¹Mk6TÂq†0Ÿ¶²&ÏæTpZéİ’+Båùûkò=Pµş‰ë…m64¹À?¹º™)/ËXıô§ X†u)Écì]ïâ|·b qH“Í(êÒÇ]ÖF=PÚÛ°b”›bD4ËÈ÷‰:#…ÊëğÁ2bÿ’t2p6Ôb+Æsü	Ów¢*$C‘Á:¶±*î·ÉrÍ×ôğÎmñ-Nú<å7–·¢§_»+maÊjy¹`ù%Ô_·hÂ+/‡ÚmªZÜĞ`Jëƒ^L$‹7;XSêbŒïn ;N3{•:a) ¿uÇçíĞäB€Á`T¤œE!ÓšR^¼z…‹µ×kêB.Rì„¹kRQ|ÛlJ$Ú•$“&ò*äÀÓ,ĞáìJÌ8aÏŸÈ,}O[ıg3Xæ£w M¹S²&!\¨™´¿‰1ÙyIªïeªs¤y7\ıGüå›é*ShOÅ&G©·³óøA3ÀEûÑ‹ÃSeY¯÷ÒL>ÀxI@ø·…»XÒ³ŞGb MhÓk†ÿ)wî¸éñŒ°Â™›^ƒ`¯é¹™ç€ÂyYÿv¯ã#¹Ôcu á´Â°¾»	÷ÈiÁ·‹êo±=Ân+ââ²ä˜}»‡0”ÑüJö©EP‹\³µò”ØÜn(N<îµí¸~ÈáyI4-ûº„í•üú
$%İ
€F…ÙpÛB~hj´†NöÁd“Â)çä}¼gLŞ õ«¦Ì"º¬L),ñFŒçğv~„)!éÈ`®ÙÑ—:ûz??Sÿ¯&ˆj`3ĞÓ%òÀYå\şs¾±Şıvü"7Ô¿@Êğ™x¶ŒJ5.äğÌğ§ä6œ-²!0ÃÃÔe5ˆıøÌì¬åÕHğàoL»İM”³p5b@ÊÓ½'pàŠOÄÓ'dş°´]qW&œÒQUæİ~P…0]MÄŠ$JM™_8…1Î¦56ÔfCÖ|;Ç¸¯ğ¼¶óî¹k
Ö#iÈ”_‘,¬w»ĞQx©ºA…†ï/xWè¥Xm6íÚ€e9¤‹$OïO+¨ìÑvã]B©oÓ³€÷ÍÔ®¦ «:“K3çÛíW`kÒ&NòÉâTaØÆâéõ"_O`dø9ô´Rn7ÖƒùÀ£¿‹›’ò¶2çU.G{‹¨õì+äxl¦õÊüEät±B§ .3ä–Q"å@*ëĞµŞF¬œƒ“\—–•àšAf“èÓµrIh×OÚ¶TO¼?ë¡&êº¬æÆÔïÌ„>IúTyùù,%Ã/¨w‘§oÑÀG ¦\¹büĞ~§Ü©ãbßì1µ ¨óÄ½(ôÁtÊx)¬µ-KE2HÁ[úfŠ°°±Ô·\ÀjB¿ëÏúiø‘’ÛÚ,Í4…Ğ†€41¿=vl÷4¯|ĞÕ³»µ)^¯“]¶£ºY°v×Ğ5qZÆÅnÇÔÜï·ú¹‰ĞéÑMYJ¢šk<oŞßçIBİe_¹a.åÀPÙ‰Î:W÷?¿ÂKá»èâ¤Ş®Dáª`Ã4}"sI­è/vGÅEÔ:l"4	¨=!Ó˜BP]ì¥¬kv;¬8së€:è]¥¨´c½ê;)-æWğux{;5±E|z˜âo”ıëÃ?øNÆºòæ ¿ñkÕJwãÔ]x"¯{¦œµR…§Aäl,CÔ½Ú İìÂŞhSèèoºÏ+Yˆ!WfGÕs!¨ïmã ‰8~ã©ÅÊ ÖÈ¢¦cíÍ“Lú˜ Šjgk}JYOÑ¹^~mÄ_ª>LÀHkßnEˆydúÚÚÙ¥² 3nÃ§)äƒ6ªğ”7lÙVØıYÖ!OEÏvZCÊr•æåv²á)Å–O3Y¼ó[z6L?= ~:+'"Fc$£jÿ³]?ö¨ ZáIÚãA82^¸¹¦¡Ál`dƒÙJÛÕíJæ§Pbğyï`É¶Qœ5°AĞ©`J£-¨Ê8O#Æa_Gƒuˆ¿¸½ÙR‡°RoBÒ¨·xİxîÔø…Ø½s¥­
…@ÑmĞú•íjû}Î`6¬î¿?Šçõ­AÚ.ŸÁxGbØÃ óâ}.Ø,Á"[„×öƒøl%ÉVé~Q¿e‚æZ—> Á‰²TaÙEC"áùÇšpÚ¼äèäÒú®°×Ø—eoÒNqQâ\°Ù¤dN´üUĞ7›çÓwOèõO”ÔÕ~<u÷óÕ¢íÅ<L¬ôiRšÈ…´€ —¦bJÁ2'f³ªpQï®;ä£”õœ×yt•E]KÉzæÛ¿ÆÎ£"ü»DUª2"IU?Îv!qÌ°…öIûĞ)ë€µpº ìF DÈ¿œVß6Nw ã~B½6•ÃKı¢ê¤¤"%öë”EÒ49ì 4´ É¼‹Q‡Ãî(K_hy#ŸÄ¬ÈÎæá¨¨Ãî>ÿB,´DD}†t¥ªæ¯Y,ò€¬‰g˜–Ê*!( ï®£Í–ÒHä9>€?k‹Á­më…ÇÖïÇXBšÆ8ø´ŸG7”a	‚CHšÛèBëkøÜö‹l“¨ÓOa•ş¾!ä¤yQCı•%÷Û^Ô‡ë~Â¤ËğàKÄSCÎ¥÷úoQ¶Œ<NJ9tÂŒ…Î<66-~
lå…té–Í­€R¤a…j„xÃ‘t?¹b'l.ËŸ#Ş°Ú¦‚=vt†1ì-*÷í!rQ@±´ÁÉÎŠCÈ©/—Ï4öAk"'&dˆ›	Ù¼3½Îİ*g1·#ßœ|µç)XšI ²iÿ‹…ŸÎå/ùòdÊë<Ì7çm çĞõ°Ç[C–N´ø=	ÍO@Iª|r­±8ØL'`²si£tK¼³W'zõ8,%ÁÍÈ’¹Æ|Mçi
éüŠkø‹	ÕÅÛgŠyÛŒa_^I¸YkvX0ˆ5P6ÌãŸş£6¼ƒ„5<0&•Î¥¶R_]Ó¸öä¶Lå‡-Ò,Î`öãøn‹‰'Oœä¾óO,~·Â3’T¢ı){!¶yDŞeH9Â/9ÒÆ'4oÑ3è;œİŒäÂ‹Â¹ğ}x¾Ñã‚’»6O9d H7-Å™•[]îà®Œ=šÕÎÌı­ï\ºÚ–×(»vP¿†¨4ÈÇ?Lîn\Ku"qnømÏ|.}2Ÿè¶$í1î°haíü.qÍ½kl=''Ø
]'WÆlÄ¿zY?€tr%g0Â¸”å-~dÇöÃgb+·t‹–„ô8´MQCÄy­«Xd‘2J—Öø–ô·Q°ëï5·Ól6‘Ü„Üùo(SQè­á²ºšo,fã®gŞãáŸ¶OÜÙCõhª]o^Ô2Ÿ^8ıÔO®7Æ—¿+qBq”ÈŠƒ½S¤w&4Ä_kÆH­4±ŞZ5¯–:Kõ×9
ëÔ¥Î7cÉi9Ğ^€‰Ñığ÷-VkQÚ;IHxëüÏÁ+Ûµè…jSo…N¡Êa:ŠÆŞpË›*yY–Ñæ\z¡¡Æ2ŠùSÕ¥hIS.pú3UÖ¥»ş3¤@·£ÍC*(A$cEƒ›>ÏD²ıƒç‚k·¯d.Ñœ¦S÷öB6L¡5	ë#dØ?$ØÕ!næ3Şˆİ´uk_÷oq¯úT­*ÄÍ·@’¢ÖË€/¼‘Ò§yv·uÂıÅ§òÀ1­`bÁdÙ¿şşöÒ2C³¶Ë]Õ ü]Ê"›2ÔŸ1Ùş^™	Øæ~KeOîwIËL1mÃNÉ&œZş&„:%ùÌ|€ß"°E¡x	lÄégq¸àÊ\¥ˆ_	 ÿÆ²åÑµdr‚w`ü±úrÏS( Ñâ_ÜËZ	²ãvF™ªãÎ
JîTvxObºµÊĞ¬ÉQC³:UÛJ0Ö?ÆqSí¯2ORRUÅíş¦*g›‘.AÂ•X+éVW'×xyø	a˜hŸy¬1Nó ¦Ìg_»¥ö¯yˆğf²Ò˜€ºKwRs°ùë ÒÍ©a£‰ıØ¢`^íôìÇÖæ¿ ¶?@˜®z*+Ck8\.qšÜê7L"Szò¾ñQŞd¸¬ÜX Î[c{™OL8j±ÒİBÒõ'q.C·G‡ø¨â‰2X•³"lZ]¨ˆ†¯Shn$ñ‚¬+‚}hÁh,	öäü-‡ƒ¤rè½ËûPN{t¼„_ˆå½4¿XÔC
›5*½!¼ì;AV:Æ µrA‘w¹ßÔÅèº—g½U23™w«H+ÊêO™@À`Ã7³wÒ1 ¶Çû$yß^¤½öÂ”û&”eÔH»"4Í2m„!îå©§âİ›¢¡ÔñãOB$—ı eŞW•]	æ~C‹bwX[5ƒ2˜˜™¤ôÙ‹×Şœ‹pàÂ€˜U±væÆ÷É·@·QjáàÆÎ+¨	²Ìm{`ršÂ«Ùš	¿* À‡'¢„„ˆÌğöaÓòdy èk—Î!‡±©{;Ú—À;¬;=W+ÑpN5]U¹k£ÛeÂüh?ºıÀ'(±Ú$±¨ŒÙo$æNÚ¨»÷ ÷×AO›Í¼¬^ ü®OhÂ³KÖÍn
:¥êJ­Ş{]!(
HmİİQù‘Ş‚µ‰uKºÙ*ê.îee}ùñ(©úPC×háİlŸ~1gwé›‰<¢`‰…5!Fiõ`î9W²ÈË¯Œ®b5•æüÑ’Ö¯ëit‚ »KÍyë·İù>•*Éc–hN¦@µ¿ñBï÷`Kô´Ÿ©…ö"3ö8’GàæÏ¾ıï3q13RaºãÈºÂñm’8¥?Bw†şz*cüæI”µ¤Kİ“Ü&ÿ©ë°=<j2«8_œHÏ«Y–­ø"˜ı·±‡ë¥×§E[†7´bşŒ(V<{ÚÚ€ølÿ‘InÌÛ×Å–1(ğI÷ò¯dxÃÄ“¥’££ü°¢.hŸ;ÇiU}3ã;Ê:¦«2tŞA@¬Bx™±> ¦ª6™áãƒÒn@Î ĞHEûS“¸~¢ ğ®M·¿*½ñåõıšpÄàÔT¯S,Ÿr0kíÒ±|ƒ›¬íõÏ!L,6ıGbğË™9Ycß7&İ¾ŠIoİ0†á)1ã¢/UÔÚËŸÌ2CÁiê3CàhjdÑ*F¡‘Œ„ëèO,}G'eÊs˜ÂP'%,ÂcûeJ0´61WÇNöVõ€¥÷ÕÀFXAN?Ş¬UÙëoœ$Úß¼uK§¸'’	¦)¯ˆÓ€Aô–^x´»ò`’e÷‹?ê±nü\¾y
Û`I	h*¼P&ï’+¸í›ûı_ˆ~÷ê”}ÿ “ïßô=£GˆdÂ8æËe(‡±"V®Nƒv¾É¬šúòóÎ,ñF»—Tp6½²Åa©ŠqîÆ‡QŠX¶‘öÑ¨óî™—êãâËEGöâçÒÔ®“`~ÁoŠšû“bS‡ÀÄ3Å¾÷c÷”q“%¥Pİ{Ù£yê;GOÒEˆ¸×Ì­ù²o]po|~§C#ğµ‡&¹k×Zæ‡P8ÿa!—½ß(pÊéˆæŸ$bvøèèd°¸°VÉ[d›c$‘B²ëeÅ*‡KÂÿõ‡ëïú¹ä"Ø°IÄq¦(D@Ù7ßqV`õ×
ÚÍ0t€á|	ã‹8‚z~İÃÌC¹Š[ğˆŸ._10³`ºä!‘ˆÂù¾ì[d¾¯:‡/o
ˆ,7“ÙEpïÅB“vlò/ÕğW#ˆÿpáÜËmNp\kş VèÑ¸ì¸ hEg÷'»wã*²¨%òNÏ	ÂI–9dñZ§¼¢µƒL€Ä¿ƒû’—ÍRq¹loîªÈ6{ÌKaşp}Ç¬X¼« {5áæƒÔ#`Ù™°oüÉ€¥zPÔäÌV_íŒ
WušÍÇ£Í¹¼ššgQíì¥»Š9ïŠ©fÓøq²yÇ{°/–u+"Âß’É îÎº‚c8ÉSú	kqêM6£ÜÎ÷P:ëqµÀ9:0dşÚÛì%i4U’³:œÈa³À®n»‹ºsåÆì,¡.H$6MÏº'mº&?Ğ¯èœİĞî[ŸÉÆ»+™µ:ªfÿ}&EÔ‘á¾œÜhÇ€†íONïó}J†á:Dgµ/¹’ LA¡=ã{­è)şÓ¿wòüæôphÎÄy¥!D Æ†$^‚uVO !<Nw€5öè¨20Š“S¶¡Õ•™7ĞÔ¹Cè!šd×já<#dÀ[SO¡–ZOæZ±lÑÌé;"’{ë„ÇQdÌBz%ŠyŞ§P„¤Ú†|É?n¢qÈ¢'®ñöD&	sMû{CöjıÏÎÊ³[FûÇ.&Î¯š›‚Y¼+«Q§²9,wí¥Y÷ÆY@*C±[â¬üßw«¦ŸQV/ªÌ•ëv–[(îïht-æm
>ày˜xï!º~Ør9ïb‘Ñ0;Eé&ÕO('ßooŒïW‰á¼D€cô<»lwÊi8­@·ÁMI„,¯ä˜ï‚IğşÂªŒ0şw
êI	:€—9Š«!-­1¬†ÓhOæÅŸÌøÔ©~iØ=b-V™OdWKg,œ°ÌOÕ¢ã"{dPº€†1DÒ½œQ_1@ä^,°¶JhIJ-u€
%ÍÁ‘Xº\Æ¹¾Ú‰ß+k¡ƒ6<7ÉÕI&s ZSùâ=vYmuèµ©ÍD sï•u*ä¢Ù†
7Ú´2Ã³|sÅh!¾ÃU¿£u]¦% üğ^éDGãà ÅÈa1Áµ L^¥‰_ˆ›Í\ìğS{p@Hk:xoâî|É+«;âùxŠİ‚½ä„Œè#®¡ãñî0Óƒs@d¥¿ ôÈaÆóàx­[ïÁÌÉ £?dM¸ÊÁ¹ny-<Cd&ÁİM<}¼†ÇªmXq3Wëù´æ^jˆg„„à±~¹–ºz˜0ÏÁø1Ê‡ª@G|<ëL€¼Vƒc49/D"ö‹7Â(ç]Dò¶Áš9H•­ZîRÁáÅ5·Ÿº™	­%^ êâ¿LŒÒLJ›fCïæt)Ã.ğ)´ÔA‘=¾İiwÿÀ£ê!J9rr9X±óêÿ73Ãqy½ÛÇÜı¨K!¼}šŸ|\6¦˜kYãñ`¶j0|caÿ¿qŠ“J2ávÕPÛ4äı(èç´äFšˆ³Áj4bˆ¿-Órøå;?ŒÄ o"Ëö×à÷åq‘¾ÜŠt”È1dğc—¬²šz‘°ò¢ÌÇz`[$›ô‚(ÕfAUòÀÕí““È]ğ¯Ü8k„"20ıÿ[ä@ ³‚`–¸ÚoxÅîMÂ¸n‘	8]V›Òƒxé	ÉzR†îo¯cÇ^áÜÃÛ5yñÛ©È¨Š¨ ‡'Ç©µ­mËSx§'<¢\_#ËIRá¨äPàŞZªêö[Õ0½„#Ê¸;Í÷6¼Jü~ÅspßØ&lë@"g×7Á+jŒLE:•+Ädt-ÄÚòÄôÌb'ö0±YCÙ4ÔA¶FÅ‚6DĞêe5Ih&İŠÉÓt<¶Tœ¹š9’6„TÍÙ}úCÉG‘ÄudÔk²ƒËµ…°[*åfQÉÉä^˜ERì§e{ò=oÑzš•ˆ+{gN"^é“]İnê¹¤a·Äò‚sÖ¿x²»õ‰›H.ØCÄª.ÑÀ6‹"‚ÌÁ!ªOFŠÅÂß·ã[ÛsHÂk1Á4¥'[nª×cíù[ç“²­iK	±úrü†à,q;‘ÂKƒã]Ğ%¾üV^8è…†«-îP³î*Í—eé¨×ùÿyWäÍ¾GJÇ’Ø\8Å¼­š”‚ÉÂâGÁßÁLÛ"©\ôÒ[ÑM©ñ‰)›1|
ÌMtkí/ÚE9÷|}#‘FÁÿ÷·X:Šr€ÂÆÇVÌ»G öÖ$+ œíÚÒ2Àw$¿Ø¾!–Ûqò7#"`'”h£†!ïQ¯Sh¨®rhÅ¥°³³µM×ÄX!ÉSUwC¬'î–ßExùƒ&§6QMı0„ÏÖÚD˜/s\aÓv@;¯}‰÷«ŒË™6zº}	¢·@`¯ˆ`UdwZı\ ŠTE&Ãã³Ê³ÇşÄ™ñÅ¾(J„¦¯N…RÉÇ&7ÂŠ^­¥¡§÷qbÒ×lNZÈ$,»”'ÿaÙ8¿&±ãy>^[®Ê8ì„´tîÖ†µí³Âu;ã0|ŞÈ2"¯4à²d¶WdŒ2õ*Nş	ÜÅÏºzØÎ>®MO"TıŒ‡ «ö·M"›Ñ;5@›â¦T&y$hêú— `ª÷¼>Ÿëq1Zâlƒ×‘^V,Ãêf$JwGÖK2~.XÇŒÙx«G÷íNÅÂ%UÍJS6Œ¸5¥ØÀ°OİÈğáˆgUŞ—}=ç˜¥gİ,è2IÈ•ÕwüëXĞo²ùK<ğTÌºìÅdóü¾e SSñ¨™Ô•	(Ç¾=‘»ã–xbÉ¶k.U£ ”Ş˜J?Ğ%™W@·Ş»iÊ™³Çª·QŠ‚±ƒ)6Z$±§Z‹àû£cğ
'×E¹÷@ñc¦éË$ Gå‚ğ'ç™u>!úş5-©Şlğ”Ó~Àš¸7¯sµŸDUpä‰o¯kk¨ùí¾†Ç#F9¿±íÿ¤$ÈæuğŒ6&İ£aSã;³p”\Gï|g ¶'¿-UT™?‰™E¶tÊ¢i‚ÕÛbq£I@§’EÀ›áz™”OÁ¯›—ÉSÍ÷ûZ‡x ‰@Z5R>MÎÃê?!~´»ÜaôXMºÙG,fñs³<}ó!¾À½.•jï÷ŒV’ÖÅFóÃ[Iœ(•in”·q,*´@á¹øKZ%K*Ìÿ›$ØZš¸¨M}.ÀRkšÎÂ=3;Ev¼@QFOuÜ®PG–2yÜ<qş×E5‹IÔ‘Ï[oìcÁ¾JŒ\72Œ’Æ¹üÜl±±±Úõ^³Ö	‡È·;Sì“F+úæM‘aIrBÑl¾äËÒCÀ[\£™
\µ$³ÂºË3'¡	-½tG·*kÆŒé„pFÜ•°c2\M§“yŞ¬¸"Ü“ŸX®f–›êÒšwò+ç÷¡7Ê:Ô÷ŸwDè1¾†$¢`ª~‰g0bEšŞùíÜÿnİñ ƒ|@>+roŒ0îŒ§=ä6Â»î E:XY~wÜZÁê‰-V7×e-ûz‰™_œ­B$+‡˜§çgd›äCvı¬UÀçwKø’ãÜo
9dd'j›†®º¼Êæ"ò1àŒŞœïâSCJ.Õr¶‘84·.¨‰>;q³+á{ÿWº‰³„Ñ=ë¤XøyHü*:XŞJ˜/ÑËuWÂIç‘+uÂÕF+Éù?ÓÒ‰€Ëï£õáä„·¨
…øÄÃ¶ZO?ÃV¿¶_€I3ÜµŸ1×~ŠÁH†Å`e§ùÃÌzäá$$t«ÖæoâÛë‚œ±/ğÌ®şT:Ùœ\™Ç%@—g£áföçâÿóšÛ)I¼!(%NøŞµ´*f]Ò7DÊ*"È‹pÒ.RÚ„“_QJ\b®”€o‚*´(j~ÑÀ‡5¹íïÎùfvn0%ˆ„Å¡˜XL¦jß[$ßhL³]=Œ­¿4h´nïma µ¤ï.û€ÑIAƒ¬t:k˜ )…EZîÌT›Ü¶¹ÁÄCKœ¬‘"IbH2í
Æ¼<ÇÄ{Ã  ×—>Ë,%ÿ3çôõkà#nú8—sñrIwš+ª¤|¯Åì™eÍZTÏE4œàÁôF5<w$¨éa)ş£õŞ­ïÉéË½ç×LU(WÇÆºP>Cû^áJğÚQıB2âkâ¶$êkaeËlšÛ«^`ƒ÷‘¡P››rfAØø§oÜ‰ BÈ¢¶Ï·dá~‡:ÍÃ	§·" Ü„B§´Õ?¯@ó]—ş}‚@Óå€g*Ù¡”Ş¾?./«S^Õ)ÿY-Vp|Êq`´‹;¶ƒ ]ÙèWÃ+™êİş)~8±²Í§İ¢Àè£'$j¦®ë'*áfÉ°šû
_ï4O™ÑSÀ°ï± ûV‘éİoX4åpåjÉ¾¡¬ßõçQÎâó8D'æHeŞ¸áêí0m;ÿJÛ£E­ˆr`Œ£ğÃ­%pÒ$ÎTâêíõo6=Š9¡jVËdñêÌ„„º½Dtï…DóúágúºÚıö¡ŞGh»v&ÌNÚÂWºŞ)^¼iæ»Ï'¦håÑH‘ÎîF—ÉÃü”¦ˆ­C<¢£à\Òî›†%²Ht¹?µ^FìE,š÷~qùÆÖ¶Â÷v¾ëˆœŒ sá;
ëÿkTù=ÇCí<J^‹iUµ(ºnjY[[şË_íZO‰jğ;ôUëÌ­Zß¿%ÏÀOY¤½Û[C3a†Tƒ5›À<!Ø¦ 5ıH•ˆàü°bñH–+¦­PÙ Cr÷ıÑToˆõ:–DèB…ı‡e»©±Ú¥r•ÊZZVêÆÈdNFnÆ:m%FcMÜwá“
s¾Nõ“Æÿ®æÊ	ÕÙ¬ ^“~”şfºƒù0Èà@áÎjÊÇ»Û‰C×)I¢JLô¯Y5;T³:SÍ‘¹û1	„ôÀqd1‚™İ$ØĞ,pû»u	qAÕùÙ¶Q”B…ìß¹›´A†#¤!ôëqÀe€pÓ‚@ÇQÍšÕòˆ±è¢“/)h8âXÀ¦1¸
EFƒÛeKÄ|ZÕE=¶CB8õ¨Ÿì4š½Ñˆ*äuÏ3‹˜ò¾Zª`¸›1Œ¼±<.„˜Ş|SiÙëœª¸ÄÍc>H,ÆìiÔ‘È¸ÂÄ¢F«>¶"Uå»,–#,ó„"c€ˆ;•Î1–’WùÈÍ±Àø%®÷4Ç:°;ô[n¢V
EK@ìà¢q>“¨ÑÿËZ-7üÏBõG/ßö&4âöKcØ®ë^–ùˆøE­Âó£ìè[š·*"=ú‹nÕ4Œ)T©ãŒ]úgd¨Rà(F¤şÄQŠ³À9¯PJ]X¨Tí?N*»rf-»â+_-E7T5Mï22„QHM
5œÕ ûV?+v÷
5CoĞÇŞ:g‚Bøá’T#§Æú£De³KEâow‡c‚v[§şòD0Ù÷šv‚•åï¸5xu­³.şĞï­3|ÍbĞyÉgT.·S~Cø¨Ømì^¬)«;Vkô¹»Q¦ÜÎÒ9÷İänA?oQ„EüMi»îá?Ü$ÈÔyÙ{jFšÍÙĞë2»Á¼t&ÊD³~ÈÃšõÇÖ	·ëƒ—Og”6Fi9Wã}"rĞEŸCÇ:‘™c  ¨v`H„[Qÿj°u$¥=¾Ó.-V…î[bÆğj]•N[Hşoê&`H]qÙÜfÀğç¯ø*£UÚæÍ~‡ ışm¼Â°×¨,ÛGRX{påMƒ#ä„5{Á¢Ãy¿m®óR#]tÌªØ³mª.îsOTYg­:dæ'ë<|¹|e‡?+¦r~“ô6şI@'Kgà:äĞcC#¬>VZA)€Ø7?ğrdÕ‡Üq#Wa;!gB“ßûÓ3K:3zu)†"VûšŠ¨‹ÚÑRÉØq/²]ÒIÏ11b'|¯¯ºX¸ô.dã{ûjâ"›+øƒFfşr~ÊUë)?0!ú0%«³×ë˜—•ˆ@ºhµ·|ë@Drhƒ‰yy›‡¡°ğrê&ÿEÒ( q‘zqlÂpêZ¥FÍã/5‹Ïíïx›¬õw^çÛÜİŸìgĞ<VOJk¸€î6İ³õšfGÿ’5ö6_!G8iTP·‘@wQI£ÎÿŠ},l³(¥X1GF]jŒaA¼G³7XKëÁSŸ ]O5? ÍøúÁ–Hp .—R*QS*²:„»ÕœÎ_Ú¿?a2wQˆ?.)â r…ô‰$oŸW²Ò‡ÒâWÎa"æ7„#õ-S-Êl–„VäDÂ"Lµî™­¥`‡uzÏcYds3¾Bê¦¾G×Ï-½P™W*z $¥iŒf¡BRÛRi¯îã;|©Á)-/©7jb…,¥NËc2Vj¹`½zùmè}'õvÀlÒ­•ªŸì¢€«`©aĞÎCRå’%ú#˜i×…qÅş”6›‘Pû bhR3yU“Û}t¾v‡ÿ¶ş*)Ûı£%oøub7ü–?~’oçwùeóE@DÆK|uohøÀ;’ë‘–Øp]ÈÒæã‰0ÄãØ;Y{€ğmßiùßœ;¾.îCèi£x=d¤6Úêg¶cu9˜k ÅF$&tÊ%ì:x–ø†3Ó¨«‡ú
e
æ„'«ú*sOÂ@c¾£ÃûŒnTÕšüÛŸš’/‡S?«Èá»!ËÃ)Î£ä—2¦}¾İ&cÖl-T·%¢ÆğÌëQsDª	ÁVYhÎ¡+ËÌ¬V0'’eè1i÷n„Ëú:`Ô»3Şò qx®QTh/ÂÕ2J s´¢Õ½{5ğJ9ıÙ#î–1j²Ü?Œß/C6¸PwŞ#Äguı•¿B:~oLï •¿$÷YxÍšK•œ–5¨Ş%?…åaxÇÒKXÿ7Êy¥ØÅ>\oû[Ã·;8ßXxLzæO'Ê\îï7¸õ°›Œ„¸îš©’ÙVy[qP@òË¡êË…uhøíté…|‘zĞÏöô€ãN—DÁ"¤4¨|Ö¯%dÆìÖ
 I½ ½*,|Ø—¢aÏØ³’òûÙ6³Í³:Dvlıäêåøğ”àËæƒú“|Æ¸¢¹ü&æbşÄI$·õ_!™ÿ¡V>…6ˆ¶~Ú“’œòÿ€£©Ğ{‡¿Ion/ªm8şµº´á11˜Ñè-DËwˆU/ØäYšK &áK	ËÑz\5åèDšô	¾A™=ÙOĞL®…X²o¥S?¹O°öÕÂ0"×åöA±†çgSm)Ú©®0s›Î¯´X!Íïıà:d†8D>k·K²(Qª­ÃÕ&z%Ã³U9„0>?ƒàü¬xh‚ovzê–'_Êšœxl;ù%×Taç8º:øF&b0æe9BG‘7rh-­¹§_ à@YæÈÑÊ¶@à€Sƒy‚^¹v¼”Ù2‘Lä(E”P0¯j¯.ôaşæşï(™Iö•â¦ú(©JeG4£&&©™éáÑĞÙìÏC¿ytbò³ÌëÓÂgpˆ_¶Õ4Ãhµåÿz¸uTE§œ:e=juµ£#b$ví]´oÆİOH–S)ìyŠwtFOtÔyŒ¾é„3#K¢ºÇÉ«·eñ™JR™¶\ö©ÌÇ«¥ißHõ>I0bY-õ(…å¹Z5Úñ½{Øù%Ïá0®Ô’¼ÑNÔåfJĞFˆğÓÀ¾ğÖI §«íç€Ï“/ jªñş	@b¿ÃcoLàe€ä™:@Ş#ãÃÕîÀã?úTÉ®v µ®Q¦‘•ËõeŒŠş^!èKåöu€ÊZPbç=t@İ$b¿ÌY‡UWÁåàs¾ìñ;Ğdà÷rãğ.WüCWóY K
áĞê²}fev0Y+Ú å«)‘1D‚©`Ë„æv±¶ı„WÙ,¡s3ğ´ék9e¤QBÎè± y5Óû¤öøªÓĞ_ Qø™yÚI¬r,½ø÷G›k.S1?„VˆúoÃF•øÅ6S]²G-MÕ
 °ÊoõxdbvvôË[€ñ@`DğÊ?üiÍ“?ıÇÑz¥]áv¸¿¹W0Õ-‚9ã™ÌÏRè`l>1}¿ÓBK§’šDã’‘6MR>°¬óàgiÄ„²¤3S8Á|–W&z«ÙmW<ÚP8p½>°Ä@›Bô¸_Ñé!»J‹éµŸ±±—z[°o¯]fT~#¸] 0b¸ïÛ2¤GF)DU>ŞĞ"Ôí ßñæU`'©ù‚éˆFfu²5Iö"™~Ë¢]$šråê}œIE<ia‘ ¿#ş±ò¸vsøHÕSpk0m°„†á@¸ï¿^¿’wh6‹•ÁÈÉm<öÈbìzÃhæå^î/î$€½¸!Í†ªCŞˆÃ(†¸EsxP‚¦´f'ª°˜ÑåOšç×cÃêBTréê­Ó'bûû˜Íà°n’¶BË!{E±%µ¶ˆr!²şæRUPê‡§ÀªZ*,zŒAÙqò“qáŠG¼.ÂI9bHĞ¯„~5İ‚vØF„TÆ3-¼UB*eKéß4xù>ıê,¾ÔF1µ5ÍõÁÄÍÔòqÅ=o[ôU0ÀsF²áz?²Çû±`ÊˆĞrkz(6Â™ö³Ü@àÊ// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  CX23888 Integrated Consumer Infrared Controller
 *
 *  Copyright (C) 2009  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23888-ir.h"

#include <linux/kfifo.h>
#include <linux/slab.h>

#include <media/v4l2-device.h>
#include <media/rc-core.h>

static unsigned int ir_888_debug;
module_param(ir_888_debug, int, 0644);
MODULE_PARM_DESC(ir_888_debug, "enable debug messages [CX23888 IR controller]");

#define CX23888_IR_REG_BASE	0x170000
/*
 * These CX23888 register offsets have a straightforward one to one mapping
 * to the CX23885 register offsets of 0x200 through 0x218
 */
#define CX23888_IR_CNTRL_REG	0x170000
#define CNTRL_WIN_3_3	0x00000000
#define CNTRL_WIN_4_3	0x00000001
#define CNTRL_WIN_3_4	0x00000002
#define CNTRL_WIN_4_4	0x00000003
#define CNTRL_WIN	0x00000003
#define CNTRL_EDG_NONE	0x00000000
#define CNTRL_EDG_FALL	0x00000004
#define CNTRL_EDG_RISE	0x00000008
#define CNTRL_EDG_BOTH	0x0000000C
#define CNTRL_EDG	0x0000000C
#define CNTRL_DMD	0x00000010
#define CNTRL_MOD	0x00000020
#define CNTRL_RFE	0x00000040
#define CNTRL_TFE	0x00000080
#define CNTRL_RXE	0x00000100
#define CNTRL_TXE	0x00000200
#define CNTRL_RIC	0x00000400
#define CNTRL_TIC	0x00000800
#define CNTRL_CPL	0x00001000
#define CNTRL_LBM	0x00002000
#define CNTRL_R		0x00004000
/* CX23888 specific control flag */
#define CNTRL_IVO	0x00008000

#define CX23888_IR_TXCLK_REG	0x170004
#define TXCLK_TCD	0x0000FFFF

#define CX23888_IR_RXCLK_REG	0x170008
#define RXCLK_RCD	0x0000FFFF

#define CX23888_IR_CDUTY_REG	0x17000C
#define CDUTY_CDC	0x0000000F

#define CX23888_IR_STATS_REG	0x170010
#define STATS_RTO	0x00000001
#define STATS_ROR	0x00000002
#define STATS_RBY	0x00000004
#define STATS_TBY	0x00000008
#define STATS_RSR	0x00000010
#define STATS_TSR	0x00000020

#define CX23888_IR_IRQEN_REG	0x170014
#define IRQEN_RTE	0x00000001
#define IRQEN_ROE	0x00000002
#define IRQEN_RSE	0x00000010
#define IRQEN_TSE	0x00000020

#define CX23888_IR_FILTR_REG	0x170018
#define FILTR_LPF	0x0000FFFF

/* This register doesn't follow the pattern; it's 0x23C on a CX23885 */
#define CX23888_IR_FIFO_REG	0x170040
#define FIFO_RXTX	0x0000FFFF
#define FIFO_RXTX_LVL	0x00010000
#define FIFO_RXTX_RTO	0x0001FFFF
#define FIFO_RX_NDV	0x00020000
#define FIFO_RX_DEPTH	8
#define FIFO_TX_DEPTH	8

/* CX23888 unique registers */
#define CX23888_IR_SEEDP_REG	0x17001C
#define CX23888_IR_TIMOL_REG	0x170020
#define CX23888_IR_WAKE0_REG	0x170024
#define CX23888_IR_WAKE1_REG	0x170028
#define CX23888_IR_WAKE2_REG	0x17002C
#define CX23888_IR_MASK0_REG	0x170030
#define CX23888_IR_MASK1_REG	0x170034
#define CX23888_IR_MAKS2_REG	0x170038
#define CX23888_IR_DPIPG_REG	0x17003C
#define CX23888_IR_LEARN_REG	0x170044

#define CX23888_VIDCLK_FREQ	108000000 /* 108 MHz, BT.656 */
#define CX23888_IR_REFCLK_FREQ	(CX23888_VIDCLK_FREQ / 2)

/*
 * We use this union internally for convenience, but callers to tx_write
 * and rx_read will be expecting records of type struct ir_raw_event.
 * Always ensure the size of this union is dictated by struct ir_raw_event.
 */
union cx23888_ir_fifo_rec {
	u32 hw_fifo_data;
	struct ir_raw_event ir_core_data;
};

#define CX23888_IR_RX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))
#define CX23888_IR_TX_KFIFO_SIZE    (256 * sizeof(union cx23888_ir_fifo_rec))

struct cx23888_ir_state {
	struct v4l2_subdev sd;
	struct cx23885_dev *dev;

	struct v4l2_subdev_ir_parameters rx_params;
	struct mutex rx_params_lock;
	atomic_t rxclk_divider;
	atomic_t rx_invert;

	struct kfifo rx_kfifo;
	spinlock_t rx_kfifo_lock;

	struct v4l2_subdev_ir_parameters tx_params;
	struct mutex tx_params_lock;
	atomic_t txclk_divider;
};

static inline struct cx23888_ir_state *to_state(struct v4l2_subdev *sd)
{
	return v4l2_get_subdevdata(sd);
}

/*
 * IR register block read and write functions
 */
static
inline int cx23888_ir_write4(struct cx23885_dev *dev, u32 addr, u32 value)
{
	cx_write(addr, value);
	return 0;
}

static inline u32 cx23888_ir_read4(struct cx23885_dev *dev, u32 addr)
{
	return cx_read(addr);
}

static inline int cx23888_ir_and_or4(struct cx23885_dev *dev, u32 addr,
				     u32 and_mask, u32 or_value)
{
	cx_andor(addr, ~and_mask, or_value);
	return 0;
}

/*
 * Rx and Tx Clock Divider register computations
 *
 * Note the largest clock divider value of 0xffff corresponds to:
 *	(0xffff + 1) * 1000 / 108/2 MHz = 1,213,629.629... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_clock_divider(unsigned int d)
{
	if (d > RXCLK_RCD + 1)
		d = RXCLK_RCD;
	else if (d < 2)
		d = 1;
	else
		d--;
	return (u16) d;
}

static inline u16 carrier_freq_to_clock_divider(unsigned int freq)
{
	return count_to_clock_divider(
			  DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * 16));
}

static inline unsigned int clock_divider_to_carrier_freq(unsigned int divider)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, (divider + 1) * 16);
}

static inline unsigned int clock_divider_to_freq(unsigned int divider,
						 unsigned int rollovers)
{
	return DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ,
				 (divider + 1) * rollovers);
}

/*
 * Low Pass Filter register calculations
 *
 * Note the largest count value of 0xffff corresponds to:
 *	0xffff * 1000 / 108/2 MHz = 1,213,611.11... ns
 * which fits in 21 bits, so we'll use unsigned int for time arguments.
 */
static inline u16 count_to_lpf_count(unsigned int d)
{
	if (d > FILTR_LPF)
		d = FILTR_LPF;
	else if (d < 4)
		d = 0;
	return (u16) d;
}

static inline u16 ns_to_lpf_count(unsigned int ns)
{
	return count_to_lpf_count(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}

static inline unsigned int lpf_count_to_ns(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in ns */
	return DIV_ROUND_CLOSEST(count * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static inline unsigned int lpf_count_to_us(unsigned int count)
{
	/* Duration of the Low Pass Filter rejection window in us */
	return DIV_ROUND_CLOSEST(count, CX23888_IR_REFCLK_FREQ / 1000000);
}

/*
 * FIFO register pulse width count computations
 */
static u32 clock_divider_to_resolution(u16 divider)
{
	/*
	 * Resolution is the duration of 1 tick of the readable portion of
	 * of the pulse width counter as read from the FIFO.  The two lsb's are
	 * not readable, hence the << 2.  This function returns ns.
	 */
	return DIV_ROUND_CLOSEST((1 << 2)  * ((u32) divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}

static u64 pulse_width_count_to_ns(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1) * 1000; /* millicycles */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000);     /* / MHz => ns */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return n;
}

static unsigned int pulse_width_count_to_us(u16 count, u16 divider)
{
	u64 n;
	u32 rem;

	/*
	 * The 2 lsb's of the pulse width timer count are not readable, hence
	 * the (count << 2) | 0x3
	 */
	n = (((u64) count << 2) | 0x3) * (divider + 1);    /* cycles      */
	rem = do_div(n, CX23888_IR_REFCLK_FREQ / 1000000); /* / MHz => us */
	if (rem >= CX23888_IR_REFCLK_FREQ / 1000000 / 2)
		n++;
	return (unsigned int) n;
}

/*
 * Pulse Clocks computations: Combined Pulse Width Count & Rx Clock Counts
 *
 * The total pulse clock count is an 18 bit pulse width timer count as the most
 * significant part and (up to) 16 bit clock divider count as a modulus.
 * When the Rx clock divider ticks down to 0, it increments the 18 bit pulse
 * width timer count's least significant bit.
 */
static u64 ns_to_pulse_clocks(u32 ns)
{
	u64 clocks;
	u32 rem;
	clocks = CX23888_IR_REFCLK_FREQ / 1000000 * (u64) ns; /* millicycles  */
	rem = do_div(clocks, 1000);                         /* /1000 = cycles */
	if (rem >= 1000 / 2)
		clocks++;
	return clocks;
}

static u16 pulse_clocks_to_clock_divider(u64 count)
{
	do_div(count, (FIFO_RXTX << 2) | 0x3);

	/* net result needs to be rounded down and decremented by 1 */
	if (count > RXCLK_RCD + 1)
		count = RXCLK_RCD;
	else if (count < 2)
		count = 1;
	else
		count--;
	return (u16) count;
}

/*
 * IR Control Register helpers
 */
enum tx_fifo_watermark {
	TX_FIFO_HALF_EMPTY = 0,
	TX_FIFO_EMPTY      = CNTRL_TIC,
};

enum rx_fifo_watermark {
	RX_FIFO_HALF_FULL = 0,
	RX_FIFO_NOT_EMPTY = CNTRL_RIC,
};

static inline void control_tx_irq_watermark(struct cx23885_dev *dev,
					    enum tx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_TIC, level);
}

static inline void control_rx_irq_watermark(struct cx23885_dev *dev,
					    enum rx_fifo_watermark level)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_RIC, level);
}

static inline void control_tx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_TXE | CNTRL_TFE),
			   enable ? (CNTRL_TXE | CNTRL_TFE) : 0);
}

static inline void control_rx_enable(struct cx23885_dev *dev, bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~(CNTRL_RXE | CNTRL_RFE),
			   enable ? (CNTRL_RXE | CNTRL_RFE) : 0);
}

static inline void control_tx_modulation_enable(struct cx23885_dev *dev,
						bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_MOD,
			   enable ? CNTRL_MOD : 0);
}

static inline void control_rx_demodulation_enable(struct cx23885_dev *dev,
						  bool enable)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_DMD,
			   enable ? CNTRL_DMD : 0);
}

static inline void control_rx_s_edge_detection(struct cx23885_dev *dev,
					       u32 edge_types)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_EDG_BOTH,
			   edge_types & CNTRL_EDG_BOTH);
}

static void control_rx_s_carrier_window(struct cx23885_dev *dev,
					unsigned int carrier,
					unsigned int *carrier_range_low,
					unsigned int *carrier_range_high)
{
	u32 v;
	unsigned int c16 = carrier * 16;

	if (*carrier_range_low < DIV_ROUND_CLOSEST(c16, 16 + 3)) {
		v = CNTRL_WIN_3_4;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 4);
	} else {
		v = CNTRL_WIN_3_3;
		*carrier_range_low = DIV_ROUND_CLOSEST(c16, 16 + 3);
	}

	if (*carrier_range_high > DIV_ROUND_CLOSEST(c16, 16 - 3)) {
		v |= CNTRL_WIN_4_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 4);
	} else {
		v |= CNTRL_WIN_3_3;
		*carrier_range_high = DIV_ROUND_CLOSEST(c16, 16 - 3);
	}
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_WIN, v);
}

static inline void control_tx_polarity_invert(struct cx23885_dev *dev,
					      bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_CPL,
			   invert ? CNTRL_CPL : 0);
}

static inline void control_tx_level_invert(struct cx23885_dev *dev,
					  bool invert)
{
	cx23888_ir_and_or4(dev, CX23888_IR_CNTRL_REG, ~CNTRL_IVO,
			   invert ? CNTRL_IVO : 0);
}

/*
 * IR Rx & Tx Clock Register helpers
 */
static unsigned int txclk_tx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static unsigned int rxclk_rx_s_carrier(struct cx23885_dev *dev,
				       unsigned int freq,
				       u16 *divider)
{
	*divider = carrier_freq_to_clock_divider(freq);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return clock_divider_to_carrier_freq(*divider);
}

static u32 txclk_tx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

static u32 rxclk_rx_s_max_pulse_width(struct cx23885_dev *dev, u32 ns,
				      u16 *divider)
{
	u64 pulse_clocks;

	if (ns > IR_MAX_DURATION)
		ns = IR_MAX_DURATION;
	pulse_clocks = ns_to_pulse_clocks(ns);
	*divider = pulse_clocks_to_clock_divider(pulse_clocks);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, *divider);
	return (u32) pulse_width_count_to_ns(FIFO_RXTX, *divider);
}

/*
 * IR Tx Carrier Duty Cycle register helpers
 */
static unsigned int cduty_tx_s_duty_cycle(struct cx23885_dev *dev,
					  unsigned int duty_cycle)
{
	u32 n;
	n = DIV_ROUND_CLOSEST(duty_cycle * 100, 625); /* 16ths of 100% */
	if (n != 0)
		n--;
	if (n > 15)
		n = 15;
	cx23888_ir_write4(dev, CX23888_IR_CDUTY_REG, n);
	return DIV_ROUND_CLOSEST((n + 1) * 100, 16);
}

/*
 * IR Filter Register helpers
 */
static u32 filter_rx_s_min_width(struct cx23885_dev *dev, u32 min_width_ns)
{
	u32 count = ns_to_lpf_count(min_width_ns);
	cx23888_ir_write4(dev, CX23888_IR_FILTR_REG, count);
	return lpf_count_to_ns(count);
}

/*
 * IR IRQ Enable Register helpers
 */
static inline void irqenable_rx(struct cx23885_dev *dev, u32 mask)
{
	mask &= (IRQEN_RTE | IRQEN_ROE | IRQEN_RSE);
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG,
			   ~(IRQEN_RTE | IRQEN_ROE | IRQEN_RSE), mask);
}

static inline void irqenable_tx(struct cx23885_dev *dev, u32 mask)
{
	mask &= IRQEN_TSE;
	cx23888_ir_and_or4(dev, CX23888_IR_IRQEN_REG, ~IRQEN_TSE, mask);
}

/*
 * V4L2 Subdevice IR Ops
 */
static int cx23888_ir_irq_handler(struct v4l2_subdev *sd, u32 status,
				  bool *handled)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	unsigned long flags;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);

	union cx23888_ir_fifo_rec rx_data[FIFO_RX_DEPTH];
	unsigned int i, j, k;
	u32 events, v;
	int tsr, rsr, rto, ror, tse, rse, rte, roe, kror;

	tsr = stats & STATS_TSR; /* Tx FIFO Service Request */
	rsr = stats & STATS_RSR; /* Rx FIFO Service Request */
	rto = stats & STATS_RTO; /* Rx Pulse Width Timer Time Out */
	ror = stats & STATS_ROR; /* Rx FIFO Over Run */

	tse = irqen & IRQEN_TSE; /* Tx FIFO Service Request IRQ Enable */
	rse = irqen & IRQEN_RSE; /* Rx FIFO Service Request IRQ Enable */
	rte = irqen & IRQEN_RTE; /* Rx Pulse Width Timer Time Out IRQ Enable */
	roe = irqen & IRQEN_ROE; /* Rx FIFO Over Run IRQ Enable */

	*handled = false;
	v4l2_dbg(2, ir_888_debug, sd, "IRQ Status:  %s %s %s %s %s %s\n",
		 tsr ? "tsr" : "   ", rsr ? "rsr" : "   ",
		 rto ? "rto" : "   ", ror ? "ror" : "   ",
		 stats & STATS_TBY ? "tby" : "   ",
		 stats & STATS_RBY ? "rby" : "   ");

	v4l2_dbg(2, ir_888_debug, sd, "IRQ Enables: %s %s %s %s\n",
		 tse ? "tse" : "   ", rse ? "rse" : "   ",
		 rte ? "rte" : "   ", roe ? "roe" : "   ");

	/*
	 * Transmitter interrupt service
	 */
	if (tse && tsr) {
		/*
		 * TODO:
		 * Check the watermark threshold setting
		 * Pull FIFO_TX_DEPTH or FIFO_TX_DEPTH/2 entries from tx_kfifo
		 * Push the data to the hardware FIFO.
		 * If there was nothing more to send in the tx_kfifo, disable
		 *	the TSR IRQ and notify the v4l2_device.
		 * If there was something in the tx_kfifo, check the tx_kfifo
		 *      level and notify the v4l2_device, if it is low.
		 */
		/* For now, inhibit TSR interrupt until Tx is implemented */
		irqenable_tx(dev, 0);
		events = V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ;
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_TX_NOTIFY, &events);
		*handled = true;
	}

	/*
	 * Receiver interrupt service
	 */
	kror = 0;
	if ((rse && rsr) || (rte && rto)) {
		/*
		 * Receive data on RSR to clear the STATS_RSR.
		 * Receive data on RTO, since we may not have yet hit the RSR
		 * watermark when we receive the RTO.
		 */
		for (i = 0, v = FIFO_RX_NDV;
		     (v & FIFO_RX_NDV) && !kror; i = 0) {
			for (j = 0;
			     (v & FIFO_RX_NDV) && j < FIFO_RX_DEPTH; j++) {
				v = cx23888_ir_read4(dev, CX23888_IR_FIFO_REG);
				rx_data[i].hw_fifo_data = v & ~FIFO_RX_NDV;
				i++;
			}
			if (i == 0)
				break;
			j = i * sizeof(union cx23888_ir_fifo_rec);
			k = kfifo_in_locked(&state->rx_kfifo,
				      (unsigned char *) rx_data, j,
				      &state->rx_kfifo_lock);
			if (k != j)
				kror++; /* rx_kfifo over run */
		}
		*handled = true;
	}

	events = 0;
	v = 0;
	if (kror) {
		events |= V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver software FIFO overrun\n");
	}
	if (roe && ror) {
		/*
		 * The RX FIFO Enable (CNTRL_RFE) must be toggled to clear
		 * the Rx FIFO Over Run status (STATS_ROR)
		 */
		v |= CNTRL_RFE;
		events |= V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN;
		v4l2_err(sd, "IR receiver hardware FIFO overrun\n");
	}
	if (rte && rto) {
		/*
		 * The IR Receiver Enable (CNTRL_RXE) must be toggled to clear
		 * the Rx Pulse Width Timer Time Out (STATS_RTO)
		 */
		v |= CNTRL_RXE;
		events |= V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED;
	}
	if (v) {
		/* Clear STATS_ROR & STATS_RTO as needed by resetting hardware */
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl & ~v);
		cx23888_ir_write4(dev, CX23888_IR_CNTRL_REG, cntrl);
		*handled = true;
	}

	spin_lock_irqsave(&state->rx_kfifo_lock, flags);
	if (kfifo_len(&state->rx_kfifo) >= CX23888_IR_RX_KFIFO_SIZE / 2)
		events |= V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ;
	spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);

	if (events)
		v4l2_subdev_notify(sd, V4L2_SUBDEV_IR_RX_NOTIFY, &events);
	return 0;
}

/* Receiver */
static int cx23888_ir_rx_read(struct v4l2_subdev *sd, u8 *buf, size_t count,
			      ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	bool invert = (bool) atomic_read(&state->rx_invert);
	u16 divider = (u16) atomic_read(&state->rxclk_divider);

	unsigned int i, n;
	union cx23888_ir_fifo_rec *p;
	unsigned u, v, w;

	n = count / sizeof(union cx23888_ir_fifo_rec)
		* sizeof(union cx23888_ir_fifo_rec);
	if (n == 0) {
		*num = 0;
		return 0;
	}

	n = kfifo_out_locked(&state->rx_kfifo, buf, n, &state->rx_kfifo_lock);

	n /= sizeof(union cx23888_ir_fifo_rec);
	*num = n * sizeof(union cx23888_ir_fifo_rec);

	for (p = (union cx23888_ir_fifo_rec *) buf, i = 0; i < n; p++, i++) {

		if ((p->hw_fifo_data & FIFO_RXTX_RTO) == FIFO_RXTX_RTO) {
			/* Assume RTO was because of no IR light input */
			u = 0;
			w = 1;
		} else {
			u = (p->hw_fifo_data & FIFO_RXTX_LVL) ? 1 : 0;
			if (invert)
				u = u ? 0 : 1;
			w = 0;
		}

		v = (unsigned) pulse_width_count_to_ns(
				  (u16)(p->hw_fifo_data & FIFO_RXTX), divider) / 1000;
		if (v > IR_MAX_DURATION)
			v = IR_MAX_DURATION;

		p->ir_core_data = (struct ir_raw_event)
			{ .pulse = u, .duration = v, .timeout = w };

		v4l2_dbg(2, ir_888_debug, sd, "rx read: %10u ns  %s  %s\n",
			 v, u ? "mark" : "space", w ? "(timed out)" : "");
		if (w)
			v4l2_dbg(2, ir_888_debug, sd, "rx read: end of rx\n");
	}
	return 0;
}

static int cx23888_ir_rx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->rx_params_lock);
	memcpy(p, &state->rx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->rx_params_lock);

	/* Disable or slow down all IR Rx circuits and counters */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);
	control_rx_demodulation_enable(dev, false);
	control_rx_s_edge_detection(dev, CNTRL_EDG_NONE);
	filter_rx_s_min_width(dev, 0);
	cx23888_ir_write4(dev, CX23888_IR_RXCLK_REG, RXCLK_RCD);

	state->rx_params.shutdown = true;

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

static int cx23888_ir_rx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->rx_params;
	u16 rxclk_divider;

	if (p->shutdown)
		return cx23888_ir_rx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->rx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the receiver */
	irqenable_rx(dev, 0);
	control_rx_enable(dev, false);

	control_rx_demodulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = rxclk_rx_s_carrier(dev, p->carrier_freq,
						     &rxclk_divider);

		o->carrier_freq = p->carrier_freq;

		o->duty_cycle = p->duty_cycle = 50;

		control_rx_s_carrier_window(dev, p->carrier_freq,
					    &p->carrier_range_lower,
					    &p->carrier_range_upper);
		o->carrier_range_lower = p->carrier_range_lower;
		o->carrier_range_upper = p->carrier_range_upper;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, rxclk_divider);
	} else {
		p->max_pulse_width =
			    rxclk_rx_s_max_pulse_width(dev, p->max_pulse_width,
						       &rxclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->rxclk_divider, rxclk_divider);

	p->noise_filter_min_width =
			  filter_rx_s_min_width(dev, p->noise_filter_min_width);
	o->noise_filter_min_width = p->noise_filter_min_width;

	p->resolution = clock_divider_to_resolution(rxclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_rx_irq_watermark(dev, RX_FIFO_HALF_FULL);

	control_rx_s_edge_detection(dev, CNTRL_EDG_BOTH);

	o->invert_level = p->invert_level;
	atomic_set(&state->rx_invert, p->invert_level);

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		unsigned long flags;

		spin_lock_irqsave(&state->rx_kfifo_lock, flags);
		kfifo_reset(&state->rx_kfifo);
		/* reset tx_fifo too if there is one... */
		spin_unlock_irqrestore(&state->rx_kfifo_lock, flags);
		if (p->interrupt_enable)
			irqenable_rx(dev, IRQEN_RSE | IRQEN_RTE | IRQEN_ROE);
		control_rx_enable(dev, p->enable);
	}

	mutex_unlock(&state->rx_params_lock);
	return 0;
}

/* Transmitter */
static int cx23888_ir_tx_write(struct v4l2_subdev *sd, u8 *buf, size_t count,
			       ssize_t *num)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	/* For now enable the Tx FIFO Service interrupt & pretend we did work */
	irqenable_tx(dev, IRQEN_TSE);
	*num = count;
	return 0;
}

static int cx23888_ir_tx_g_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	mutex_lock(&state->tx_params_lock);
	memcpy(p, &state->tx_params, sizeof(struct v4l2_subdev_ir_parameters));
	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_shutdown(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;

	mutex_lock(&state->tx_params_lock);

	/* Disable or slow down all IR Tx circuits and counters */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);
	control_tx_modulation_enable(dev, false);
	cx23888_ir_write4(dev, CX23888_IR_TXCLK_REG, TXCLK_TCD);

	state->tx_params.shutdown = true;

	mutex_unlock(&state->tx_params_lock);
	return 0;
}

static int cx23888_ir_tx_s_parameters(struct v4l2_subdev *sd,
				      struct v4l2_subdev_ir_parameters *p)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	struct v4l2_subdev_ir_parameters *o = &state->tx_params;
	u16 txclk_divider;

	if (p->shutdown)
		return cx23888_ir_tx_shutdown(sd);

	if (p->mode != V4L2_SUBDEV_IR_MODE_PULSE_WIDTH)
		return -ENOSYS;

	mutex_lock(&state->tx_params_lock);

	o->shutdown = p->shutdown;

	o->mode = p->mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;

	o->bytes_per_data_element = p->bytes_per_data_element
				  = sizeof(union cx23888_ir_fifo_rec);

	/* Before we tweak the hardware, we have to disable the transmitter */
	irqenable_tx(dev, 0);
	control_tx_enable(dev, false);

	control_tx_modulation_enable(dev, p->modulation);
	o->modulation = p->modulation;

	if (p->modulation) {
		p->carrier_freq = txclk_tx_s_carrier(dev, p->carrier_freq,
						     &txclk_divider);
		o->carrier_freq = p->carrier_freq;

		p->duty_cycle = cduty_tx_s_duty_cycle(dev, p->duty_cycle);
		o->duty_cycle = p->duty_cycle;

		p->max_pulse_width =
			(u32) pulse_width_count_to_ns(FIFO_RXTX, txclk_divider);
	} else {
		p->max_pulse_width =
			    txclk_tx_s_max_pulse_width(dev, p->max_pulse_width,
						       &txclk_divider);
	}
	o->max_pulse_width = p->max_pulse_width;
	atomic_set(&state->txclk_divider, txclk_divider);

	p->resolution = clock_divider_to_resolution(txclk_divider);
	o->resolution = p->resolution;

	/* FIXME - make this dependent on resolution for better performance */
	control_tx_irq_watermark(dev, TX_FIFO_HALF_EMPTY);

	control_tx_polarity_invert(dev, p->invert_carrier_sense);
	o->invert_carrier_sense = p->invert_carrier_sense;

	control_tx_level_invert(dev, p->invert_level);
	o->invert_level = p->invert_level;

	o->interrupt_enable = p->interrupt_enable;
	o->enable = p->enable;
	if (p->enable) {
		if (p->interrupt_enable)
			irqenable_tx(dev, IRQEN_TSE);
		control_tx_enable(dev, p->enable);
	}

	mutex_unlock(&state->tx_params_lock);
	return 0;
}


/*
 * V4L2 Subdevice Core Ops
 */
static int cx23888_ir_log_status(struct v4l2_subdev *sd)
{
	struct cx23888_ir_state *state = to_state(sd);
	struct cx23885_dev *dev = state->dev;
	char *s;
	int i, j;

	u32 cntrl = cx23888_ir_read4(dev, CX23888_IR_CNTRL_REG);
	u32 txclk = cx23888_ir_read4(dev, CX23888_IR_TXCLK_REG) & TXCLK_TCD;
	u32 rxclk = cx23888_ir_read4(dev, CX23888_IR_RXCLK_REG) & RXCLK_RCD;
	u32 cduty = cx23888_ir_read4(dev, CX23888_IR_CDUTY_REG) & CDUTY_CDC;
	u32 stats = cx23888_ir_read4(dev, CX23888_IR_STATS_REG);
	u32 irqen = cx23888_ir_read4(dev, CX23888_IR_IRQEN_REG);
	u32 filtr = cx23888_ir_read4(dev, CX23888_IR_FILTR_REG) & FILTR_LPF;

	v4l2_info(sd, "IR Receiver:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_RXE ? "yes" : "no");
	v4l2_info(sd, "\tDemodulation from a carrier:       %s\n",
		  cntrl & CNTRL_DMD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_RFE ? "enabled" : "disabled");
	switch (cntrl & CNTRL_EDG) {
	case CNTRL_EDG_NONE:
		s = "disabled";
		break;
	case CNTRL_EDG_FALL:
		s = "falling edge";
		break;
	case CNTRL_EDG_RISE:
		s = "rising edge";
		break;
	case CNTRL_EDG_BOTH:
		s = "rising & falling edges";
		break;
	default:
		s = "??? edge";
		break;
	}
	v4l2_info(sd, "\tPulse timers' start/stop trigger:  %s\n", s);
	v4l2_info(sd, "\tFIFO data on pulse timer overflow: %s\n",
		  cntrl & CNTRL_R ? "not loaded" : "overflow marker");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_RIC ? "not empty" : "half full or greater");
	v4l2_info(sd, "\tLoopback mode:                     %s\n",
		  cntrl & CNTRL_LBM ? "loopback active" : "normal receive");
	if (cntrl & CNTRL_DMD) {
		v4l2_info(sd, "\tExpected carrier (16 clocks):      %u Hz\n",
			  clock_divider_to_carrier_freq(rxclk));
		switch (cntrl & CNTRL_WIN) {
		case CNTRL_WIN_3_3:
			i = 3;
			j = 3;
			break;
		case CNTRL_WIN_4_3:
			i = 4;
			j = 3;
			break;
		case CNTRL_WIN_3_4:
			i = 3;
			j = 4;
			break;
		case CNTRL_WIN_4_4:
			i = 4;
			j = 4;
			break;
		default:
			i = 0;
			j = 0;
			break;
		}
		v4l2_info(sd, "\tNext carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz\n",
			  i, j,
			  clock_divider_to_freq(rxclk, 16 + j),
			  clock_divider_to_freq(rxclk, 16 - i));
	}
	v4l2_info(sd, "\tMax measurable pulse width:        %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, rxclk),
		  pulse_width_count_to_ns(FIFO_RXTX, rxclk));
	v4l2_info(sd, "\tLow pass filter:                   %s\n",
		  filtr ? "enabled" : "disabled");
	if (filtr)
		v4l2_info(sd, "\tMin acceptable pulse width (LPF):  %u us, %u ns\n",
			  lpf_count_to_us(filtr),
			  lpf_count_to_ns(filtr));
	v4l2_info(sd, "\tPulse width timer timed-out:       %s\n",
		  stats & STATS_RTO ? "yes" : "no");
	v4l2_info(sd, "\tPulse width timer time-out intr:   %s\n",
		  irqen & IRQEN_RTE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO overrun:                      %s\n",
		  stats & STATS_ROR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO overrun interrupt:            %s\n",
		  irqen & IRQEN_ROE ? "enabled" : "disabled");
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_RBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_RSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_RSE ? "enabled" : "disabled");

	v4l2_info(sd, "IR Transmitter:\n");
	v4l2_info(sd, "\tEnabled:                           %s\n",
		  cntrl & CNTRL_TXE ? "yes" : "no");
	v4l2_info(sd, "\tModulation onto a carrier:         %s\n",
		  cntrl & CNTRL_MOD ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO:                              %s\n",
		  cntrl & CNTRL_TFE ? "enabled" : "disabled");
	v4l2_info(sd, "\tFIFO interrupt watermark:          %s\n",
		  cntrl & CNTRL_TIC ? "not empty" : "half full or less");
	v4l2_info(sd, "\tOutput pin level inversion         %s\n",
		  cntrl & CNTRL_IVO ? "yes" : "no");
	v4l2_info(sd, "\tCarrier polarity:                  %s\n",
		  cntrl & CNTRL_CPL ? "space:burst mark:noburst"
				    : "space:noburst mark:burst");
	if (cntrl & CNTRL_MOD) {
		v4l2_info(sd, "\tCarrier (16 clocks):               %u Hz\n",
			  clock_divider_to_carrier_freq(txclk));
		v4l2_info(sd, "\tCarrier duty cycle:                %2u/16\n",
			  cduty + 1);
	}
	v4l2_info(sd, "\tMax pulse width:                   %u us, %llu ns\n",
		  pulse_width_count_to_us(FIFO_RXTX, txclk),
		  pulse_width_count_to_ns(FIFO_RXTX, txclk));
	v4l2_info(sd, "\tBusy:                              %s\n",
		  stats & STATS_TBY ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service requested:            %s\n",
		  stats & STATS_TSR ? "yes" : "no");
	v4l2_info(sd, "\tFIFO service request interrupt:    %s\n",
		  irqen & IRQEN_TSE ? "enabled" : "disabled");

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int cx23888_ir_g_register(struct v4l2_subdev *sd,
				 struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	reg->size = 4;
	reg->val = cx23888_ir_read4(state->dev, addr);
	return 0;
}

static int cx23888_ir_s_register(struct v4l2_subdev *sd,
				 const struct v4l2_dbg_register *reg)
{
	struct cx23888_ir_state *state = to_state(sd);
	u32 addr = CX23888_IR_REG_BASE + (u32) reg->reg;

	if ((addr & 0x3) != 0)
		return -EINVAL;
	if (addr < CX23888_IR_CNTRL_REG || addr > CX23888_IR_LEARN_REG)
		return -EINVAL;
	cx23888_ir_write4(state->dev, addr, reg->val);
	return 0;
}
#endif

static const struct v4l2_subdev_core_ops cx23888_ir_core_ops = {
	.log_status = cx23888_ir_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = cx23888_ir_g_register,
	.s_register = cx23888_ir_s_register,
#endif
	.interrupt_service_routine = cx23888_ir_irq_handler,
};

static const struct v4l2_subdev_ir_ops cx23888_ir_ir_ops = {
	.rx_read = cx23888_ir_rx_read,
	.rx_g_parameters = cx23888_ir_rx_g_parameters,
	.rx_s_parameters = cx23888_ir_rx_s_parameters,

	.tx_write = cx23888_ir_tx_write,
	.tx_g_parameters = cx23888_ir_tx_g_parameters,
	.tx_s_parameters = cx23888_ir_tx_s_parameters,
};

static const struct v4l2_subdev_ops cx23888_ir_controller_ops = {
	.core = &cx23888_ir_core_ops,
	.ir = &cx23888_ir_ir_ops,
};

static const struct v4l2_subdev_ir_parameters default_rx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5, RC-6, and RC-6A carrier */

	/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
	/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
	.noise_filter_min_width = 333333, /* ns */
	.carrier_range_lower = 35000,
	.carrier_range_upper = 37000,
	.invert_level = false,
};

static const struct v4l2_subdev_ir_parameters default_tx_params = {
	.bytes_per_data_element = sizeof(union cx23888_ir_fifo_rec),
	.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,

	.enable = false,
	.interrupt_enable = false,
	.shutdown = true,

	.modulation = true,
	.carrier_freq = 36000, /* 36 kHz - RC-5 carrier */
	.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters default_params;
	int ret;

	state = kzalloc(sizeof(struct cx23888_ir_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	spin_lock_init(&state->rx_kfifo_lock);
	if (kfifo_alloc(&state->rx_kfifo, CX23888_IR_RX_KFIFO_SIZE,
			GFP_KERNEL)) {
		kfree(state);
		return -ENOMEM;
	}

	state->dev = dev;
	sd = &state->sd;

	v4l2_subdev_init(sd, &cx23888_ir_controller_ops);
	v4l2_set_subdevdata(sd, state);
	/* FIXME - fix the formatting of dev->v4l2_dev.name and use it */
	snprintf(sd->name, sizeof(sd->name), "%s/888-ir", dev->name);
	sd->grp_id = CX23885_HW_888_IR;

	ret = v4l2_device_register_subdev(&dev->v4l2_dev, sd);
	if (ret == 0) {
		/*
		 * Ensure no interrupts arrive from '888 specific conditions,
		 * since we ignore them in this driver to have commonality with
		 * similar IR controller cores.
		 */
		cx23888_ir_write4(dev, CX23888_IR_IRQEN_REG, 0);

		mutex_init(&state->rx_params_lock);
		default_params = default_rx_params;
		v4l2_subdev_call(sd, ir, rx_s_parameters, &default_params);

		mutex_init(&state->tx_params_lock);
		default_params = default_tx_params;
		v4l2_subdev_call(sd, ir, tx_s_parameters, &default_params);
	} else {
		kfifo_free(&state->rx_kfifo);
	}
	return ret;
}

int cx23888_ir_remove(struct cx23885_dev *dev)
{
	struct v4l2_subdev *sd;
	struct cx23888_ir_state *state;

	sd = cx23885_find_hw(dev, CX23885_HW_888_IR);
	if (sd == NULL)
		return -ENODEV;

	cx23888_ir_rx_shutdown(sd);
	cx23888_ir_tx_shutdown(sd);

	state = to_state(sd);
	v4l2_device_unregister_subdev(sd);
	kfifo_free(&state->rx_kfifo);
	kfree(state);
	/* Nothing more to free() as state held the actual v4l2_subdev object */
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 x—ppFåâì²`V"†N,v0 ½vµf-ã© 0h|ˆï]ÇÒ”½{3§6Ç—=–bb)ûwYjv€ÌÎ•ú=ßıõjeñû²˜ğû›¶\È­½b>åĞÑm !LĞö‹¸ĞVĞ?÷Â­sÃJÚ®•®¼s	Û¡C‹ÿz1¶pS•Ë(¶üPå¸j@ßb›¢ç"ÖĞ-n³`´ó¢?Û5‚ÔFYÜªD @ş	Ó
û¶dö'©™–ÃTŸõQ“JÑuaº\ªÒæ~
¤xO]æ›8Q	Á›ãs?åJLQ¿·ÊP—ûŞAR6èbğÆ?!ŠH&H3Ô>5`$_¡{Íé•vİ4-Rª-²Å& G5àÉs´9IÁë†d.'ç»yw´¢§{B†$7‰<­ËBGËÉïòä³ºç£"BÕÂ™¹İ{„+ĞKğâÑ6*šüeHës „PNá89ùQ!¦…>I™æwÙ83FåsÙDòÎí·Ò¡áæµú ÚÊFß¸¶:*Ÿ²Ê<¾Uü„qÊw¦3"gx$ÈÄ 	ûİùè9Ëàêìig³Öî1¼u+‚º­ÉX©éÀÿ¶Äêè¹iİ*kñwX\y`Ä„m7kî)ÍvïúV\ùcá.I&×9;GW9	ìL{`^&ƒ¦6¹Í>VÎ:Çu™}.±Œ_è:Åª…$²{X+İ Â%şÃ|åíÏ'„¥fô â”à1ğßëêú-}¸»kÁnÌÀÖ¡©RYTÖsÜ¾•Ã-NÛ3¡™À¼ª…u‹rl)8ŸøÀÂ¹¹ÓèXÒHˆÖºC"CÀGA³s·&•¢Cù/^p]Ç±ì
e,ßRQÇZëdqí{ŠYSšÂ_à&z©åUâª{ú1÷‚„7G³Úô¿î¬[å™3>Öm•ŠÒş©ş `$Û·cÍn‘3ü™™õ7+@j?æğd{NùV,sÓáüü¨§ñÛÿ2 7'mİşE)']£‘ğLGÒ\–s¢GRÄ»+Èx£89FK}¦€1-ÄĞúp¯zÆ×›s}ü¦:¶5ÚÌp ê:Zôc™ôE} 'ú¼@ƒı'"hKƒ£’G!xqƒ9À]ù'*ıe¹ó˜3+ÂC+UÌá8×ì=
JóşêO3Z\~$"v'F8İ—›Â	)Oâ¡Q\P!šòõ›/’ª
o¹äwéQÍ7œ¬Í_¼İŒ|½´Ş¬Õ‹æÙ¡ÒÔª6t÷gP/¼iviÃ/­/…ŸÇúc5(¿ŒÉVUI0V]Zİcî(¥™øò"õÛèvÅE˜úÉ;ËßŞ/
{wÿn‹–rc-Ù:†µ	õ§]¯¾:ÿ‡µ7¶€:«‘|¹µæîIxßç&fşoœÖ)ŠŠ[ïS¹<![Œxx`N?û	Täæ¸YAlÃ Ãx‹äÏî&‡Ü\²ËÇvşxÄLÔNÍ´\HuV÷Qâzî)hÆ*´W}ÄbF~Â9›SÄ{be»rÂ†\¥{ø’5~­‰V8ÍiÆÊ¯B©•£Ú-R´Ò°â$°Åx0L4èŠîEº8¯Á’qc	Ø´Ï{ÿ³t³;^™Ğ(¦Múh¶KP™¸k­Z‡9ë`}~Ë4|\N¼	#yµ9WÎ`y·Pm|šV’	Œ¡w0QŠoü(	ze¯Éétª¯Mƒó<<"
Ï0fÄ·Íæğ?¦}Fó¶<t!»LêA“Ğ[ƒXMĞ¬ˆÊƒ±_Pİ¹ìeÉØ	ª4|1ÿM§z İyóÀ Òfù³ï F²šÏ!-oE'¨gämÍœ‘q­ Ñò–^ã©‰œ£„7æ]‘†ğ
l÷ÃnÆéy2…d×¹	>•Z„<Fı[W€¥SçX»€ÖîĞ‰RÚi¦¬³ZZ>?6bÛ³¿×)XFŒƒ¤CÚªÃ
²"×ÖV´é!¬§ö/Íj^Î3J5v¤$²Ãûãõ‡'Ğª-Ú*"¯ÛwL“Vl{bä¦Ê*uö¿‹¿l{jœE b¢ÜÇùŠ©^ÇÂ>…a¡&Y„Á¥?§‰+I«ŠFğ:[²Ùƒ’*o#°ciÌvè ¤_=é©Š#<`€®# ±+—4N‹æ¥ ¦¶u•§›šPQM5–c%Å§'ğŸ¨½4µ>+ñ8¡A_gãéŠ
š'ZŠGWÅøık“ëâuñæ"‡À7Òj+¹‡‰g÷‘l-\˜°xYyZxÁÓM«4S	Sİš/$ª¼¢‘µ´CexnF¥##ºNbÅ¯Øâ„‰¥™'dÆ£6])Û#áû[9M_5³oÎ?øõ¶›yùèÆ(¬~´½£·JQ#Dâ‚X©)3
«şÔ	¹ù|Gæ¼âˆ¥ .ézÂ6Ø¡•_¥Sö|)ø¦ª»ğeµ‹èÙ——(_{4„hø–ÿÔßeÛ¦ªèîtœuÇL¢µ»Šú4Íf£CL•`u8sÍ_ÛÆš].ø¿’°ÒÙ³ğúoz±Z	Á±Lu.¬ê+Û§«²™øgêŒê%äêæ¡3˜'©Íõ·0ˆJÛKı‘ Ö+E—'/dNóİÇ`ñ{UZKjØÃˆõóE°‚ça¹´¾TÇYÅ}ı¡¢ZØb;™XƒÀ7ôde2…föSESVe¡Bn:¨/€¼ÿC¼÷ˆŠ¹ 6Ç<l”ø?t$æ/ëc›ÂÁ§zÇàdcNWD…õ3{‘ûµ¬Ñ¡}æÃnl<š Ş%İ·!§\ø±*H¾H şü>ˆƒárá)p*õl™İ.-u IR”;rî—ó.—'0±¥¨é˜¨ç0Š#X„;lx®s\	‰Ä\·†5)¥ğ4½¦^ÒšdêéÔÍ$×W¬£õÅµ÷K™–]/šË6=iäòM;Ï‡˜½’ÃBş@ŒKCó% ¥lBvÿ)‡Î-´4§È•'ìí$æÊßÒP%-É[­7Ñó·Â¼BĞ.4·.ÆåvmÍ»ÍpŒ?¹|ÙÜ?XİÂ³)r—~‡Á.Ñ	³s(1ßÆtÕË´‡öuØÿ¼TS_3L«OL¶Í‹Õš#•BÑ6sº' à-uhº¦ZéÄl7vwˆ„Î–Ììyña¼¹›iÔª.wqâEö3.÷~ÏŸĞê/Î¿½9‰HŸ}v`Âvcñë\dü-kŒš­16m¹ık\š§8u§y¢;ÏQ”9á÷âá‰¤Ã{.İÃL]uªîÈ0æ›©yQ´+K†Ey—QAPTgJlÑwú9@£´¢­(ú¨óUŠÉ¶¾«±
Wş·’ñ¸êÍMz)Şd+…¢,õ‚’=ÿ‘“e¿×àöŠ¦Æ¾Ë
ÔğlŸÁÛHÜ%oı™6˜ÛâQ×‹*¤„Š]!ÕT£àÊóÀä:¬×Hë ÀÌÖ¡SbŞ„%
™oôÏúôŸãŒG±?b‚~¦çÊu*í¾?T™Ú†.âeZÏ|Ìæ:OZt¬´#(¼öâşmm}4db‘Ò×ªŞÎŠ„€54]RdâŒJKÛ†oß¯–ÖBƒ¯FÛ®¦Õ›OÖNGä¼üf®X‡YáÀ²¶ä;.­Jİ5£jl¼*¨~ï†·Òã±2Oõ;@FO¬B–=òº<qPÓÂ«‹†“İ¹ëfn„zÕÚ§3ĞÏí.öbskƒüÒTğÌìçÎ³8—#´Ê°üK	ÎÉØH{¤÷»ŠÏİèãx,
•ú~™L—{ü¼.	-m2s“{i»œƒgë	J”•ÃZÌ½ÔZ)VØ¢˜šŞ†òÄ²2y"šR±­ø¤·´M%°¹Ë¥£rÛzqå&†Ÿ•ŠïÏØµÄ¡W§>ÿ%?Ù'ÛÖ´Õ–4¹¡¸MÛïC*4—<8šúÖY"©TìÔ4Ò½h
BÄB;3²¢cæ&ïÇßÁ½.¼iºùœm .[³–…*]Mó¶œj8XÓoi°	ì¾VöA2 NYşèG®úê‹3ìaã°‘…*iÃ©¤ùo>uå7“(ÆCn]K¨˜±¾Å@;ù¹cî3º–ƒàG9ŸÃëÏº#‰R¼³ßˆ5s¨ÙhYXÅP^T`µI-·ó[œ€K2J—=”×O©?2úËÿCÑh=dÑõ›î]ŞËupÕñîËü¸cY†¢©¥±b°}H´Cq‰ošgÚğ#ãÌ¨æGöÀtXaÎ ZØŞıê¸yõb;ùïm@]oşºS¨˜ÆÛAG‰$ã/…êğĞGÿOñ©ušhQ>dÅx„pA(hç9&È65ïï–sÉ&š“Ñš):cv¯[¸ÿÔ›“Óú_[:`	±»á±_<L@Ù|-h<Ş˜;!½j³«ÒqiGŠø¸¾@SmZÙ¬G…ØÈšºµ²€áĞÜHF$+#f€ªü5ix(!ídq†ı%1lİóàß÷nYFÂñş¨( ã½Ã	àB¢äc¢`…Ë™Â.>}9Åw	Ã"s*)q@p–ñFšâ€o6õ„‡x.Š3û‹„dµV=5Ks˜ìáù£¨¥~Âh"øÆJ“QÉ},• Ïdœ=UÆUDª,?ë^ŠeM{#:Ñ^Í—¯ı:ğeFLgô}ô½´dûB¦—pçøĞŸÛŸß@¤‘œ#©è‰—œIîîÕ7ªî*İ:_w²V•’kó1c×™&öËU y±~ñ…0ıYï.u’µş°%,(K±•Ë‰]k^L}O´]úL¾¡»t37ö'–ŸÃá\ë“]ö›—à1ke»Â+\Ó„ĞëN3&VÄ|•uª®-)ØŸ>‹û'õé$›3Üô×ûY€y•Ñ •?*øª'²yÆK°H¸fedKÎ¡Æ²úÂn=ÄÃªw¦M-Ùâ‚û.d”±.ã¸Ódå Ù´#ÔÌW‚ÔX ›¸Ã/wo¯ÆU^ Î-Š´?ªb>oPßëwZK.š$X½ä¤HâGBü…:™h<rùK¿?EzîYò:[Åì2p5öÒ'7üšëYúÑ'/£&Za˜¢ ¯ãÖM#‹+®{~vîk]¸pî„Ø>¤vzÿÙ)ò-;-@à¬Ì)®²KRO'Áœ<®[øÏä@ã8-f 6–2vÚiı«î°¿L0âøå$²«P¶0dÏìu0ß#dşc` >Y#°ˆÅ¬K¾oX9Ïâô|\ng tasks to offline CPUs is a fairly daft thing.
	 */
	WARN_ON_ONCE(!cpu_online(new_cpu));

	WARN_ON_ONCE(is_migration_disabled(p));
#endif

	trace_sched_migrate_task(p, new_cpu);

	if (task_cpu(p) != new_cpu) {
		if (p->sched_class->migrate_task_rq)
			p->sched_class->migrate_task_rq(p, new_cpu);
		p->se.nr_migrations++;
		rseq_migrate(p);
		perf_event_task_migrate(p);
	}

	__set_task_cpu(p, new_cpu);
}

#ifdef CONFIG_NUMA_BALANCING
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}

struct migration_swap_arg {
	struct task_struct *src_task, *dst_task;
	int src_cpu, dst_cpu;
};

static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, arg->src_task->cpus_ptr))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, arg->dst_task->cpus_ptr))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}

/*
 * Cross migrate two tasks
 */
int migrate_swap(struct task_struct *cur, struct task_struct *p,
		int target_cpu, int curr_cpu)
{
	struct migration_swap_arg arg;
	int ret = -EINVAL;

	arg = (struct migration_swap_arg){
		.src_task = cur,
		.src_cpu = curr_cpu,
		.dst_task = p,
		.dst_cpu = target_cpu,
	};

	if (arg.src_cpu == arg.dst_cpu)
		goto out;

	/*
	 * These three tests are all lockless; this is OK since all of them
	 * will be re-checked with proper locks held further down the line.
	 */
	if (!cpu_active(arg.src_cpu) || !cpu_active(arg.dst_cpu))
		goto out;

	if (!cpumask_test_cpu(arg.dst_cpu, arg.src_task->cpus_ptr))
		goto out;

	if (!cpumask_test_cpu(arg.src_cpu, arg.dst_task->cpus_ptr))
		goto out;

	trace_sched_swap_numa(cur, arg.src_cpu, p, arg.dst_cpu);
	ret = stop_two_cpus(arg.dst_cpu, arg.src_cpu, migrate_swap_stop, &arg);

out:
	return ret;
}
#endif /* CONFIG_NUMA_BALANCING */

/*
 * wait_task_inactive - wait for a thread to unschedule.
 *
 * If @match_state is nonzero, it's the @p->state value just checked and
 * not expected to change.  If it changes, i.e. @p might have woken up,
 * then return zero.  When we succeed in waiting for @p to be off its CPU,
 * we return a positive number (its total switch count).  If a second call
 * a short while later returns the same number, the caller can be sure that
 * @p has remained unscheduled the whole time.
 *
 * The caller must ensure that the task *will* unschedule sometime soon,
 * else this function might spin for a *long* time. This function can't
 * be called with interrupts off, or it may introduce deadlock with
 * smp_call_function() if an IPI is sent by the same process we are
 * waiting to become inactive.
 */
unsigned long wait_task_inactive(struct task_struct *p, unsigned int match_state)
{
	int running, queued;
	struct rq_flags rf;
	unsigned long ncsw;
	struct rq *rq;

	for (;;) {
		/*
		 * We do the initial early heuristics without holding
		 * any task-queue locks at all. We'll only try to get
		 * the runqueue lock when things look like they will
		 * work out!
		 */
		rq = task_rq(p);

		/*
		 * If the task is actively running on another CPU
		 * still, just relax and busy-wait without holding
		 * any locks.
		 *
		 * NOTE! Since we don't hold any locks, it's not
		 * even sure that "rq" stays as the right runqueue!
		 * But we don't care, since "task_running()" will
		 * return false if the runqueue has changed and p
		 * is actually now running somewhere else!
		 */
		while (task_running(rq, p)) {
			if (match_state && unlikely(READ_ONCE(p->__state) != match_state))
				return 0;
			cpu_relax();
		}

		/*
		 * Ok, time to look more closely! We need the rq
		 * lock now, to be *sure*. If we're wrong, we'll
		 * just go back and repeat.
		 */
		rq = task_rq_lock(p, &rf);
		trace_sched_wait_task(p);
		running = task_running(rq, p);
		queued = task_on_rq_queued(p);
		ncsw = 0;
		if (!match_state || READ_ONCE(p->__state) == match_state)
			ncsw = p->nvcsw | LONG_MIN; /* sets MSB */
		task_rq_unlock(rq, p, &rf);

		/*
		 * If it changed from the expected state, bail out now.
		 */
		if (unlikely(!ncsw))
			break;

		/*
		 * Was it really running after all now that we
		 * checked with the proper locks actually held?
		 *
		 * Oops. Go back and try again..
		 */
		if (unlikely(running)) {
			cpu_relax();
			continue;
		}

		/*
		 * It's not enough that it's not actively running,
		 * it must be off the runqueue _entirely_, and not
		 * preempted!
		 *
		 * So if it was still runnable (but just not actively
		 * running right now), it's preempted, and we should
		 * yield - it could be a while.
		 */
		if (unlikely(queued)) {
			ktime_t to = NSEC_PER_SEC / HZ;

			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&to, HRTIMER_MODE_REL_HARD);
			continue;
		}

		/*
		 * Ahh, all good. It wasn't running, and it wasn't
		 * runnable, which means that it will never become
		 * running in the future either. We're all done!
		 */
		break;
	}

	return ncsw;
}

/***
 * kick_process - kick a running thread to enter/exit the kernel
 * @p: the to-be-kicked thread
 *
 * Cause a process which is running on another CPU to enter
 * kernel-mode, without any delay. (to get signals handled.)
 *
 * NOTE: this function doesn't have to take the runqueue lock,
 * because all it wants to ensure is that the remote task enters
 * the kernel. If the IPI races and the task has been migrated
 * to another CPU then no harm is done and the purpose has been
 * achieved as well.
 */
void kick_process(struct task_struct *p)
{
	int cpu;

	preempt_disable();
	cpu = task_cpu(p);
	if ((cpu != smp_processor_id()) && task_curr(p))
		smp_send_reschedule(cpu);
	preempt_enable();
}
EXPORT_SYMBOL_GPL(kick_process);

/*
 * ->cpus_ptr is protected by both rq->lock and p->pi_lock
 *
 * A few notes on cpu_active vs cpu_online:
 *
 *  - cpu_active must be a subset of cpu_online
 *
 *  - on CPU-up we allow per-CPU kthreads on the online && !active CPU,
 *    see __set_cpus_allowed_ptr(). At this point the newly online
 *    CPU isn't yet part of the sched domains, and balancing will not
 *    see it.
 *
 *  - on CPU-down we clear cpu_active() to mask the sched domains and
 *    avoid the load balancer to place new tasks on the to be removed
 *    CPU. Existing tasks will remain running there and will be taken
 *    off.
 *
 * This means that fallback selection must not select !active CPUs.
 * And can assume that any active CPU must be online. Conversely
 * select_task_rq() below may allow selection of !active CPUs in order
 * to satisfy the above rules.
 */
static int select_fallback_rq(int cpu, struct task_struct *p)
{
	int nid = cpu_to_node(cpu);
	const struct cpumask *nodemask = NULL;
	enum { cpuset, possible, fail } state = cpuset;
	int dest_cpu;

	/*
	 * If the node that the CPU is on has been offlined, cpu_to_node()
	 * will return -1. There is no CPU on the node, and we should
	 * select the CPU on the other node.
	 */
	if (nid != -1) {
		nodemask = cpumask_of_node(nid);

		/* Look for allowed, online CPU in same node. */
		for_each_cpu(dest_cpu, nodemask) {
			if (is_cpu_allowed(p, dest_cpu))
				return dest_cpu;
		}
	}

	for (;;) {
		/* Any allowed, online CPU? */
		for_each_cpu(dest_cpu, p->cpus_ptr) {
			if (!is_cpu_allowed(p, dest_cpu))
				continue;

			goto out;
		}

		/* No more Mr. Nice Guy. */
		switch (state) {
		case cpuset:
			if (cpuset_cpus_allowed_fallback(p)) {
				state = possible;
				break;
			}
			fallthrough;
		case possible:
			/*
			 * XXX When called from select_task_rq() we only
			 * hold p->pi_lock and again violate locking order.
			 *
			 * More yuck to audit.
			 */
			do_set_cpus_allowed(p, task_cpu_possible_mask(p));
			state = fail;
			break;
		case fail:
			BUG();
			break;
		}
	}

out:
	if (state != cpuset) {
		/*
		 * Don't tell them about moving exiting tasks or
		 * kernel threads (both mm NULL), since they never
		 * leave kernel.
		 */
		if (p->mm && printk_ratelimit()) {
			printk_deferred("process %d (%s) no longer affine to cpu%d\n",
					task_pid_nr(p), p->comm, cpu);
		}
	}

	return dest_cpu;
}

/*
 * The caller (fork, wakeup) owns p->pi_lock, ->cpus_ptr is stable.
 */
static inline
int select_task_rq(struct task_struct *p, int cpu, int wake_flags)
{
	lockdep_assert_held(&p->pi_lock);

	if (p->nr_cpus_allowed > 1 && !is_migration_disabled(p))
		cpu = p->sched_class->select_task_rq(p, cpu, wake_flags);
	else
		cpu = cpumask_any(p->cpus_ptr);

	/*
	 * In order not to call set_task_cpu() on a blocking task we need
	 * to rely on ttwu() to place the task on a valid ->cpus_ptr
	 * CPU.
	 *
	 * Since this is common to all placement strategies, this lives here.
	 *
	 * [ this allows ->select_task() to simply return task_cpu(p) and
	 *   not worry about this generic constraint ]
	 */
	if (unlikely(!is_cpu_allowed(p, cpu)))
		cpu = select_fallback_rq(task_cpu(p), p);

	return cpu;
}

void sched_set_stop_task(int cpu, struct task_struct *stop)
{
	static struct lock_class_key stop_pi_lock;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 1 };
	struct task_struct *old_stop = cpu_rq(cpu)->stop;

	if (stop) {
		/*
		 * Make it appear like a SCHED_FIFO task, its something
		 * userspace knows about and won't get confused about.
		 *
		 * Also, it will make PI more or less work without too
		 * much confusion -- but then, stop work should not
		 * rely on PI working anyway.
		 */
		sched_setscheduler_nocheck(stop, SCHED_FIFO, &param);

		stop->sched_class = &stop_sched_class;

		/*
		 * The PI code calls rt_mutex_setprio() with ->pi_lock held to
		 * adjust the effective priority of a task. As a result,
		 * rt_mutex_setprio() can trigger (RT) balancing operations,
		 * which can then trigger wakeups of the stop thread to push
		 * around the current task.
		 *
		 * The stop task itself will never be part of the PI-chain, it
		 * never blocks, therefore that ->pi_lock recursion is safe.
		 * Tell lockdep about this by placing the stop->pi_lock in its
		 * own class.
		 */
		lockdep_set_class(&stop->pi_lock, &stop_pi_lock);
	}

	cpu_rq(cpu)->stop = stop;

	if (old_stop) {
		/*
		 * Reset it back to a normal scheduling class so that
		 * it can die in pieces.
		 */
		old_stop->sched_class = &rt_sched_class;
	}
}

#else /* CONFIG_SMP */

static inline int __set_cpus_allowed_ptr(struct task_struct *p,
					 const struct cpumask *new_mask,
					 u32 flags)
{
	return set_cpus_allowed_ptr(p, new_mask);
}

static inline void migrate_disable_switch(struct rq *rq, struct task_struct *p) { }

static inline bool rq_has_pinned_tasks(struct rq *rq)
{
	return false;
}

#endif /* !CONFIG_SMP */

static void
ttwu_stat(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq;

	if (!schedstat_enabled())
		return;

	rq = this_rq();

#ifdef CONFIG_SMP
	if (cpu == rq->cpu) {
		__schedstat_inc(rq->ttwu_local);
		__schedstat_inc(p->stats.nr_wakeups_local);
	} else {
		struct sched_domain *sd;

		__schedstat_inc(p->stats.nr_wakeups_remote);
		rcu_read_lock();
		for_each_domain(rq->cpu, sd) {
			if (cpumask_test_cpu(cpu, sched_domain_span(sd))) {
				__schedstat_inc(sd->ttwu_wake_remote);
				break;
			}
		}
		rcu_read_unlock();
	}

	if (wake_flags & WF_MIGRATED)
		__schedstat_inc(p->stats.nr_wakeups_migrate);
#endif /* CONFIG_SMP */

	__schedstat_inc(rq->ttwu_count);
	__schedstat_inc(p->stats.nr_wakeups);

	if (wake_flags & WF_SYNC)
		__schedstat_inc(p->stats.nr_wakeups_sync);
}

/*
 * Mark the task runnable and perform wakeup-preemption.
 */
static void ttwu_do_wakeup(struct rq *rq, struct task_struct *p, int wake_flags,
			   struct rq_flags *rf)
{
	check_preempt_curr(rq, p, wake_flags);
	WRITE_ONCE(p->__state, TASK_RUNNING);
	trace_sched_wakeup(p);

#ifdef CONFIG_SMP
	if (p->sched_class->task_woken) {
		/*
		 * Our task @p is fully woken up and running; so it's safe to
		 * drop the rq->lock, hereafter rq is only used for statistics.
		 */
		rq_unpin_lock(rq, rf);
		p->sched_class->task_woken(rq, p);
		rq_repin_lock(rq, rf);
	}

	if (rq->idle_stamp) {
		u64 delta = rq_clock(rq) - rq->idle_stamp;
		u64 max = 2*rq->max_idle_balance_cost;

		update_avg(&rq->avg_idle, delta);

		if (rq->avg_idle > max)
			rq->avg_idle = max;

		rq->wake_stamp = jiffies;
		rq->wake_avg_idle = rq->avg_idle / 2;

		rq->idle_stamp = 0;
	}
#endif
}

static void
ttwu_do_activate(struct rq *rq, struct task_struct *p, int wake_flags,
		 struct rq_flags *rf)
{
	int en_flags = ENQUEUE_WAKEUP | ENQUEUE_NOCLOCK;

	lockdep_assert_rq_held(rq);

	if (p->sched_contributes_to_load)
		rq->nr_uninterruptible--;

#ifdef CONFIG_SMP
	if (wake_flags & WF_MIGRATED)
		en_flags |= ENQUEUE_MIGRATED;
	else
#endif
	if (p->in_iowait) {
		delayacct_blkio_end(p);
		atomic_dec(&task_rq(p)->nr_iowait);
	}

	activate_task(rq, p, en_flags);
	ttwu_do_wakeup(rq, p, wake_flags, rf);
}

/*
 * Consider @p being inside a wait loop:
 *
 *   for (;;) {
 *      set_current_state(TASK_UNINTERRUPTIBLE);
 *
 *      if (CONDITION)
 *         break;
 *
 *      schedule();
 *   }
 *   __set_current_state(TASK_RUNNING);
 *
 * between set_current_state() and schedule(). In this case @p is still
 * runnable, so all that needs doing is change p->state back to TASK_RUNNING in
 * an atomic manner.
 *
 * By taking task_rq(p)->lock we serialize against schedule(), if @p->on_rq
 * then schedule() must still happen and p->state can be changed to
 * TASK_RUNNING. Otherwise we lost the race, schedule() has happened, and we
 * need to do a full wakeup with enqueue.
 *
 * Returns: %true when the wakeup is done,
 *          %false otherwise.
 */
static int ttwu_runnable(struct task_struct *p, int wake_flags)
{
	struct rq_flags rf;
	struct rq *rq;
	int ret = 0;

	rq = __task_rq_lock(p, &rf);
	if (task_on_rq_queued(p)) {
		/* check_preempt_curr() may use rq clock */
		update_rq_clock(rq);
		ttwu_do_wakeup(rq, p, wake_flags, &rf);
		ret = 1;
	}
	__task_rq_unlock(rq, &rf);

	return ret;
}

#ifdef CONFIG_SMP
void sched_ttwu_pending(void *arg)
{
	struct llist_node *llist = arg;
	struct rq *rq = this_rq();
	struct task_struct *p, *t;
	struct rq_flags rf;

	if (!llist)
		return;

	/*
	 * rq::ttwu_pending racy indication of out-standing wakeups.
	 * Races such that false-negatives are possible, since they
	 * are shorter lived that false-positives would be.
	 */
	WRITE_ONCE(rq->ttwu_pending, 0);

	rq_lock_irqsave(rq, &rf);
	update_rq_clock(rq);

	llist_for_each_entry_safe(p, t, llist, wake_entry.llist) {
		if (WARN_ON_ONCE(p->on_cpu))
			smp_cond_load_acquire(&p->on_cpu, !VAL);

		if (WARN_ON_ONCE(task_cpu(p) != cpu_of(rq)))
			set_task_cpu(p, cpu_of(rq));

		ttwu_do_activate(rq, p, p->sched_remote_wakeup ? WF_MIGRATED : 0, &rf);
	}

	rq_unlock_irqrestore(rq, &rf);
}

void send_call_function_single_ipi(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (!set_nr_if_polling(rq->idle))
		arch_send_call_function_single_ipi(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

/*
 * Queue a task on the target CPUs wake_list and wake the CPU via IPI if
 * necessary. The wakee CPU on receipt of the IPI will queue the task
 * via sched_ttwu_wakeup() for activation so the wakee incurs the cost
 * of the wakeup instead of the waker.
 */
static void __ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);

	p->sched_remote_wakeup = !!(wake_flags & WF_MIGRATED);

	WRITE_ONCE(rq->ttwu_pending, 1);
	__smp_call_single_queue(cpu, &p->wake_entry.llist);
}

void wake_up_if_idle(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	rcu_read_lock();

	if (!is_idle_task(rcu_dereference(rq->curr)))
		goto out;

	rq_lock_irqsave(rq, &rf);
	if (is_idle_task(rq->curr))
		resched_curr(rq);
	/* Else CPU is not idle, do nothing here: */
	rq_unlock_irqrestore(rq, &rf);

out:
	rcu_read_unlock();
}

bool cpus_share_cache(int this_cpu, int that_cpu)
{
	if (this_cpu == that_cpu)
		return true;

	return per_cpu(sd_llc_id, this_cpu) == per_cpu(sd_llc_id, that_cpu);
}

static inline bool ttwu_queue_cond(int cpu, int wake_flags)
{
	/*
	 * Do not complicate things with the async wake_list while the CPU is
	 * in hotplug state.
	 */
	if (!cpu_active(cpu))
		return false;

	/*
	 * If the CPU does not share cache, then queue the task on the
	 * remote rqs wakelist to avoid accessing remote data.
	 */
	if (!cpus_share_cache(smp_processor_id(), cpu))
		return true;

	/*
	 * If the task is descheduling and the only running task on the
	 * CPU then use the wakelist to offload the task activation to
	 * the soon-to-be-idle CPU as the current CPU is likely busy.
	 * nr_running is checked to avoid unnecessary task stacking.
	 */
	if ((wake_flags & WF_ON_CPU) && cpu_rq(cpu)->nr_running <= 1)
		return true;

	return false;
}

static bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	if (sched_feat(TTWU_QUEUE) && ttwu_queue_cond(cpu, wake_flags)) {
		if (WARN_ON_ONCE(cpu == smp_processor_id()))
			return false;

		sched_clock_cpu(cpu); /* Sync clocks across CPUs */
		__ttwu_queue_wakelist(p, cpu, wake_flags);
		return true;
	}

	return false;
}

#else /* !CONFIG_SMP */

static inline bool ttwu_queue_wakelist(struct task_struct *p, int cpu, int wake_flags)
{
	return false;
}

#endif /* CONFIG_SMP */

static void ttwu_queue(struct task_struct *p, int cpu, int wake_flags)
{
	struct rq *rq = cpu_rq(cpu);
	struct rq_flags rf;

	if (ttwu_queue_wakelist(p, cpu, wake_flags))
		return;

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	ttwu_do_activate(rq, p, wake_flags, &rf);
	rq_unlock(rq, &rf);
}

/*
 * Invoked from try_to_wake_up() to check whether the task can be woken up.
 *
 * The caller holds p::pi_lock if p != current or has preemption
 * disabled when p == current.
 *
 * The rules of PREEMPT_RT saved_state:
 *
 *   The related locking code always holds p::pi_lock when updating
 *   p::saved_state, which means the code is fully serialized in both cases.
 *
 *   The lock wait and lock wakeups happen via TASK_RTLOCK_WAIT. No other
 *   bits set. This allows to distinguish all wakeup scenarios.
 */
static __always_inline
bool ttwu_state_match(struct task_struct *p, unsigned int state, int *success)
{
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT)) {
		WARN_ON_ONCE((state & TASK_RTLOCK_WAIT) &&
			     state != TASK_RTLOCK_WAIT);
	}

	if (READ_ONCE(p->__state) & state) {
		*success = 1;
		return true;
	}

#ifdef CONFIG_PREEMPT_RT
	/*
	 * Saved state preserves the task state across blocking on
	 * an RT lock.  If the state matches, set p::saved_state to
	 * TASK_RUNNING, but do not wake the task because it waits
	 * for a lock wakeup. Also indicate success because from
	 * the regular waker's point of view this has succeeded.
	 *
	 * After acquiring the lock the task will restore p::__state
	 * from p::saved_state which ensures that the regular
	 * wakeup is not lost. The restore will also set
	 * p::saved_state to TASK_RUNNING so any further tests will
	 * not result in false positives vs. @success
	 */
	if (p->saved_state & state) {
		p->saved_state = TASK_RUNNING;
		*success = 1;
	}
#endif
	return false;
}

/*
 * Notes on Program-Order guarantees on SMP systems.
 *
 *  MIGRATION
 *
 * The basic program-order guarantee on SMP systems is that when a task [t]
 * migrates, all its activity on its old CPU [c0] happens-before any subsequent
 * execution on its new CPU [c1].
 *
 * For migration (of runnable tasks) this is provided by the following means:
 *
 *  A) UNLOCK of the rq(c0)->lock scheduling out task t
 *  B) migration for t is required to synchronize *both* rq(c0)->lock and
 *     rq(c1)->lock (if not at the same time, then in that order).
 *  C) LOCK of the rq(c1)->lock scheduling in task
 *
 * Release/acquire chaining guarantees that B happens after A and C after B.
 * Note: the CPU doing B need not be c0 or c1
 *
 * Example:
 *
 *   CPU0            CPU1            CPU2
 *
 *   LOCK rq(0)->lock
 *   sched-out X
 *   sched-in Y
 *   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(0)->lock // orders against CPU0
 *                                   dequeue X
 *                                   UNLOCK rq(0)->lock
 *
 *                                   LOCK rq(1)->lock
 *                                   enqueue X
 *                                   UNLOCK rq(1)->lock
 *
 *                   LOCK rq(1)->lock // orders against CPU2
 *                   sched-out Z
 *                   sched-in X
 *                   UNLOCK rq(1)->lock
 *
 *
 *  BLOCKING -- aka. SLEEP + WAKEUP
 *
 * For blocking we (obviously) need to provide the same guarantee as for
 * migration. However the means are completely different as there is no lock
 * chain to provide order. Instead we do:
 *
 *   1) smp_store_release(X->on_cpu, 0)   -- finish_task()
 *   2) smp_cond_load_acquire(!X->on_cpu) -- try_to_wake_up()
 *
 * Example:
 *
 *   CPU0 (schedule)  CPU1 (try_to_wake_up) CPU2 (schedule)
 *
 *   LOCK rq(0)->lock LOCK X->pi_lock
 *   dequeue X
 *   sched-out X
 *   smp_store_release(X->on_cpu, 0);
 *
 *                    smp_cond_load_acquire(&X->on_cpu, !VAL);
 *                    X->state = WAKING
 *                    set_task_cpu(X,2)
 *
 *                    LOCK rq(2)->lock
 *                    enqueue X
 *                    X->state = RUNNING
 *                    UNLOCK rq(2)->lock
 *
 *                                          LOCK rq(2)->lock // orders against CPU1
 *                                          sched-out Z
 *                                          sched-in X
 *                                          UNLOCK rq(2)->lock
 *
 *                    UNLOCK X->pi_lock
 *   UNLOCK rq(0)->lock
 *
 *
 * However, for wakeups there is a second guarantee we must provide, namely we
 * must ensure that CONDITION=1 done by the caller can not be reordered with
 * accesses to the task state; see try_to_wake_up() and set_current_state().
 */

/**
 * try_to_wake_up - wake up a thread
 * @p: the thread to be awakened
 * @state: the mask of task states that can be woken
 * @wake_flags: wake modifier flags (WF_*)
 *
 * Conceptually does:
 *
 *   If (@state & @p->state) @p->state = TASK_RUNNING.
 *
 * If the task was not queued/runnable, also place it back on a runqueue.
 *
 * This function is atomic against schedule() which would dequeue the task.
 *
 * It issues a full memory barrier before accessing @p->state, see the comment
 * with set_current_state().
 *
 * Uses p->pi_lock to serialize against concurrent wake-ups.
 *
 * Relies on p->pi_lock stabilizing:
 *  - p->sched_class
 *  - p->cpus_ptr
 *  - p->sched_task_group
 * in order to do migration, see its use of select_task_rq()/set_task_cpu().
 *
 * Tries really hard to only take one task_rq(p)->lock for performance.
 * Takes rq->lock in:
 *  - ttwu_runnable()    -- old rq, unavoidable, see comment there;
 *  - ttwu_queue()       -- new rq, for enqueue of the task;
 *  - psi_ttwu_dequeue() -- much sadness :-( accounting will kill us.
 *
 * As a consequence we race really badly with just about everything. See the
 * many memory barriers and their comments for details.
 *
 * Return: %true if @p->state changes (an actual wakeup was done),
 *	   %false otherwise.
 */
static int
try_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	unsigned long flags;
	int cpu, success = 0;

	preempt_disable();
	if (p == current) {
		/*
		 * We're waking current, this means 'p->on_rq' and 'task_cpu(p)
		 * == smp_processor_id()'. Together this means we can special
		 * case the whole 'p->on_rq && ttwu_runnable()' case below
		 * without taking any locks.
		 *
		 * In particular:
		 *  - we rely on Program-Order guarantees for all the ordering,
		 *  - we're serialized against set_special_state() by virtue of
		 *    it disabling IRQs (this allows not taking ->pi_lock).
		 */
		if (!ttwu_state_match(p, state, &success))
			goto out;

		trace_sched_waking(p);
		WRITE_ONCE(p->__state, TASK_RUNNING);
		trace_sched_wakeup(p);
		goto out;
	}

	/*
	 * If we are going to wake up a thread waiting for CONDITION we
	 * need to ensure that CONDITION=1 done by the caller can not be
	 * reordered with p->state check below. This pairs with smp_store_mb()
	 * in set_current_state() that the waiting thread does.
	 */
	raw_spin_lock_irqsave(&p->pi_lock, flags);
	smp_mb__after_spinlock();
	if (!ttwu_state_match(p, state, &success))
		goto unlock;

	trace_sched_waking(p);

	/*
	 * Ensure we load p->on_rq _after_ p->state, otherwise it would
	 * be possible to, falsely, observe p->on_rq == 0 and get stuck
	 * in smp_cond_load_acquire() below.
	 *
	 * sched_ttwu_pending()			try_to_wake_up()
	 *   STORE p->on_rq = 1			  LOAD p->state
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   UNLOCK rq->lock
	 *
	 * [task p]
	 *   STORE p->state = UNINTERRUPTIBLE	  LOAD p->on_rq
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * A similar smb_rmb() lives in try_invoke_on_locked_down_task().
	 */
	smp_rmb();
	if (READ_ONCE(p->on_rq) && ttwu_runnable(p, wake_flags))
		goto unlock;

#ifdef CONFIG_SMP
	/*
	 * Ensure we load p->on_cpu _after_ p->on_rq, otherwise it would be
	 * possible to, falsely, observe p->on_cpu == 0.
	 *
	 * One must be running (->on_cpu == 1) in order to remove oneself
	 * from the runqueue.
	 *
	 * __schedule() (switch to task 'p')	try_to_wake_up()
	 *   STORE p->on_cpu = 1		  LOAD p->on_rq
	 *   UNLOCK rq->lock
	 *
	 * __schedule() (put 'p' to sleep)
	 *   LOCK rq->lock			  smp_rmb();
	 *   smp_mb__after_spinlock();
	 *   STORE p->on_rq = 0			  LOAD p->on_cpu
	 *
	 * Pairs with the LOCK+smp_mb__after_spinlock() on rq->lock in
	 * __schedule().  See the comment for smp_mb__after_spinlock().
	 *
	 * Form a control-dep-acquire with p->on_rq == 0 above, to ensure
	 * schedule()'s deactivate_task() has 'happened' and p will no longer
	 * care about it's own p->state. See the comment in __schedule().
	 */
	smp_acquire__after_ctrl_dep();

	/*
	 * We're doing the wakeup (@success == 1), they did a dequeue (p->on_rq
	 * == 0), which means we need to do an enqueue, change p->state to
	 * TASK_WAKING such that we can unlock p->pi_lock before doing the
	 * enqueue, such as ttwu_queue_wakelist().
	 */
	WRITE_ONCE(p->__state, TASK_WAKING);

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, considering queueing p on the remote CPUs wake_list
	 * which potentially sends an IPI instead of spinning on p->on_cpu to
	 * let the waker make forward progress. This is safe because IRQs are
	 * disabled and the IPI will deliver after on_cpu is cleared.
	 *
	 * Ensure we load task_cpu(p) after p->on_cpu:
	 *
	 * set_task_cpu(p, cpu);
	 *   STORE p->cpu = @cpu
	 * __schedule() (switch to task 'p')
	 *   LOCK rq->lock
	 *   smp_mb__after_spin_lock()		smp_cond_load_acquire(&p->on_cpu)
	 *   STORE p->on_cpu = 1		LOAD p->cpu
	 *
	 * to ensure we observe the correct CPU on which the task is currently
	 * scheduling.
	 */
	if (smp_load_acquire(&p->on_cpu) &&
	    ttwu_queue_wakelist(p, task_cpu(p), wake_flags | WF_ON_CPU))
		goto unlock;

	/*
	 * If the owning (remote) CPU is still in the middle of schedule() with
	 * this task as prev, wait until it's done referencing the task.
	 *
	 * Pairs with the smp_store_release() in finish_task().
	 *
	 * This ensures that tasks getting woken will be fully ordered against
	 * their previous state and preserve Program Order.
	 */
	smp_cond_load_acquire(&p->on_cpu, !VAL);

	cpu = select_task_rq(p, p->wake_cpu, wake_flags | WF_TTWU);
	if (task_cpu(p) != cpu) {
		if (p->in_iowait) {
			delayacct_blkio_end(p);
			atomic_dec(&task_rq(p)->nr_iowait);
		}

		wake_flags |= WF_MIGRATED;
		psi_ttwu_dequeue(p);
		set_task_cpu(p, cpu);
	}
#else
	cpu = task_cpu(p);
#endif /* CONFIG_SMP */

	ttwu_queue(p, cpu, wake_flags);
unlock:
	raw_spin_unlock_irqrestore(&p->pi_lock, flags);
out:
	if (success)
		ttwu_stat(p, task_cpu(p), wake_flags);
	preempt_enable();

	return success;
}

/**
 * task_call_func - Invoke a function on task in fixed state
 * @p: Process for which the function is to be invoked, can be @current.
 * @func: Function to invoke.
 * @arg: Argument to function.
 *
 * Fix the task in it's current state by avoiding wakeups and or rq operations
 * and call @func(@arg) on it.  This function can use ->on_rq and task_curr()
 * to work out what the state is, if required.  Given that @func can be invoked
 * with a runqueue lock held, it had better be quite lightweight.
 *
 * Returns:
 *   Whatever @func returns
 */
int task_call_func(struct task_struct *p, task_call_f func, void *arg)
{
	struct rq *rq = NULL;
	unsigned int state;
	struct rq_flags rf;
	int ret;

	raw_spin_lock_irqsave(&p->pi_lock, rf.flags);

	state = READ_ONCE(p->__state);

	/*
	 * Ensure we load p->on_rq after p->__state, otherwise it would be
	 * possible to, falsely, observe p->on_rq == 0.
	 *
	 * See try_to_wake_up() for a longer comment.
	 */
	smp_rmb();

	/*
	 * Since pi->lock blocks try_to_wake_up(), we don't need rq->lock when
	 * the task is blocked. Make sure to check @state since ttwu() can drop
	 * locks at the end, see ttwu_queue_wakelist().
	 */
	if (state == TASK_RUNNING || state == TASK_WAKING || p->on_rq)
		rq = __task_rq_lock(p, &rf);

	/*
	 * At this point the task is pinned; either:
	 *  - blocked and we're holding off wakeups	 (pi->lock)
	 *  - woken, and we're holding off enqueue	 (rq->lock)
	 *  - queued, and we're holding off schedule	 (rq->lock)
	 *  - running, and we're holding off de-schedule (rq->lock)
	 *
	 * The called function (@func) can use: task_curr(), p->on_rq and
	 * p->__state to differentiate between these states.
	 */
	ret = func(p, arg);

	if (rq)
		rq_unlock(rq, &rf);

	raw_spin_unlock_irqrestore(&p->pi_lock, rf.flags);
	return ret;
}

/**
 * wake_up_process - Wake up a specific process
 * @p: The process to be woken up.
 *
 * Attempt to wake up the nominated process and move it to the set of runnable
 * processes.
 *
 * Return: 1 if the process was woken up, 0 if it was already running.
 *
 * This function executes a full memory barrier before accessing the task state.
 */
int wake_up_process(struct task_struct *p)
{
	return try_to_wake_up(p, TASK_NORMAL, 0);
}
EXPORT_SYMBOL(wake_up_process);

int wake_up_state(struct task_struct *p, unsigned int state)
{
	return try_to_wake_up(p, state, 0);
}

/*
 * Perform scheduler related setup for a newly forked process p.
 * p is forked by current.
 *
 * __sched_fork() is basic setup used by init_idle() too:
 */
static void __sched_fork(unsigned long clone_flags, struct task_struct *p)
{
	p->on_rq			= 0;

	p->se.on_rq			= 0;
	p->se.exec_start		= 0;
	p->se.sum_exec_runtime		= 0;
	p->se.prev_sum_exec_runtime	= 0;
	p->se.nr_migrations		= 0;
	p->se.vruntime			= 0;
	INIT_LIST_HEAD(&p->se.group_node);

#ifdef CONFIG_FAIR_GROUP_SCHED
	p->se.cfs_rq			= NULL;
#endif

#ifdef CONFIG_SCHEDSTATS
	/* Even if schedstat is disabled, there should not be garbage */
	memset(&p->stats, 0, sizeof(p->stats));
#endif

	RB_CLEAR_NODE(&p->dl.rb_node);
	init_dl_task_timer(&p->dl);
	init_dl_inactive_task_timer(&p->dl);
	__dl_clear_params(p);

	INIT_LIST_HEAD(&p->rt.run_list);
	p->rt.timeout		= 0;
	p->rt.time_slice	= sched_rr_timeslice;
	p->rt.on_rq		= 0;
	p->rt.on_list		= 0;

#ifdef CONFIG_PREEMPT_NOTIFIERS
	INIT_HLIST_HEAD(&p->preempt_notifiers);
#endif

#ifdef CONFIG_COMPACTION
	p->capture_control = NULL;
#endif
	init_numa_balancing(clone_flags, p);
#ifdef CONFIG_SMP
	p->wake_entry.u_flags = CSD_TYPE_TTWU;
	p->migration_pending = NULL;
#endq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, rf->flags);
		rq = task_rq(p);
		raw_spin_rq_lock(rq);
		/*
		 *	move_queued_task()		task_rq_lock()
		 *
		 *	ACQUIRE (rq->lock)
		 *	[S] ->on_rq = MIGRATING		[L] rq = task_rq()
		 *	WMB (__set_task_cpu())		ACQUIRE (rq->lock);
		 *	[S] ->cpu = new_cpu		[L] task_rq()
		 *					[L] ->on_rq
		 *	RELEASE (rq->lock)
		 *
		 * If we observe the old CPU in task_rq_lock(), the acquire of
		 * the old rq->lock will fully serialize against the stores.
		 *
		 * If we observe the new CPU in task_rq_lock(), the address
		 * dependency headed by '[L] rq = task_rq()' and the acquire
		 * will pair with the WMB to ensure we then also see migrating.
		 */
		if (likely(rq == task_rq(p) && !task_on_rq_migrating(p))) {
			rq_pin_lock(rq, rf);
			return rq;
		}
		raw_spin_rq_unlock(rq);
		raw_spin_unlock_irqrestore(&p->pi_lock, rf->flags);

		while (unlikely(task_on_rq_migrating(p)))
			cpu_relax();
	}
}

/*
 * RQ-clock updating methods:
 */

static void update_rq_clock_task(struct rq *rq, s64 delta)
{
/*
 * In theory, the compile should just see 0 here, and optimize out the call
 * to sched_rt_avg_update. But I don't trust it...
 */
	s64 __maybe_unused steal = 0, irq_delta = 0;

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	irq_delta = irq_time_read(cpu_of(rq)) - rq->prev_irq_time;

	/*
	 * Since irq_time is only updated on {soft,}irq_exit, we might run into
	 * this case when a previous update_rq_clock() happened inside a
	 * {soft,}irq region.
	 *
	 * When this happens, we stop ->clock_task and only update the
	 * prev_irq_time stamp to account for the part that fit, so that a next
	 * update will consume the rest. This ensures ->clock_task is
	 * monotonic.
	 *
	 * It does however cause some slight miss-attribution of {soft,}irq
	 * time, a more accurate solution would be to update the irq_time using
	 * the current rq->clock timestamp, except that would require using
	 * atomic ops.
	 */
	if (irq_delta > delta)
		irq_delta = delta;

	rq->prev_irq_time += irq_delta;
	delta -= irq_delta;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
	if (static_key_false((&paravirt_steal_rq_enabled))) {
		steal = paravirt_steal_clock(cpu_of(rq));
		steal -= rq->prev_steal_time_rq;

		if (unlikely(steal > delta))
			steal = delta;

		rq->prev_steal_time_rq += steal;
		delta -= steal;
	}
#endif

	rq->clock_task += delta;

#ifdef CONFIG_HAVE_SCHED_AVG_IRQ
	if ((irq_delta + steal) && sched_feat(NONTASK_CAPACITY))
		update_irq_load_avg(rq, irq_delta + steal);
#endif
	update_rq_clock_pelt(rq, delta);
}

void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_rq_held(rq);

	if (rq->clock_update_flags & RQCF_ACT_SKIP)
		return;

#ifdef CONFIG_SCHED_DEBUG
	if (sched_feat(WARN_DOUBLE_CLOCK))
		SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
	rq->clock_update_flags |= RQCF_UPDATED;
#endif

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}

#ifdef CONFIG_SCHED_HRTICK
/*
 * Use HR-timers to deliver accurate preemption points.
 */

static void hrtick_clear(struct rq *rq)
{
	if (hrtimer_active(&rq->hrtick_timer))
		hrtimer_cancel(&rq->hrtick_timer);
}

/*
 * High-resolution timer tick.
 * Runs from hardirq context with interrupts disabled.
 */
static enum hrtimer_restart hrtick(struct hrtimer *timer)
{
	struct rq *rq = container_of(timer, struct rq, hrtick_timer);
	struct rq_flags rf;

	WARN_ON_ONCE(cpu_of(rq) != smp_processor_id());

	rq_lock(rq, &rf);
	update_rq_clock(rq);
	rq->curr->sched_class->task_tick(rq, rq->curr, 1);
	rq_unlock(rq, &rf);

	return HRTIMER_NORESTART;
}

#ifdef CONFIG_SMP

static void __hrtick_restart(struct rq *rq)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	ktime_t time = rq->hrtick_time;

	hrtimer_start(timer, time, HRTIMER_MODE_ABS_PINNED_HARD);
}

/*
 * called from hardirq (IPI) context
 */
static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;
	struct rq_flags rf;

	rq_lock(rq, &rf);
	__hrtick_restart(rq);
	rq_unlock(rq, &rf);
}

/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	struct hrtimer *timer = &rq->hrtick_timer;
	s64 delta;

	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense and can cause timer DoS.
	 */
	delta = max_t(s64, delay, 10000LL);
	rq->hrtick_time = ktime_add_ns(timer->base->get_time(), delta);

	if (rq == this_rq())
		__hrtick_restart(rq);
	else
		smp_call_function_single_async(cpu_of(rq), &rq->hrtick_csd);
}

#else
/*
 * Called to set the hrtick timer state.
 *
 * called with rq->lock held and irqs disabled
 */
void hrtick_start(struct rq *rq, u64 delay)
{
	/*
	 * Don't schedule slices shorter than 10000ns, that just
	 * doesn't make sense. Rely on vruntime for fairness.
	 */
	delay = max_t(u64, delay, 10000LL);
	hrtimer_start(&rq->hrtick_timer, ns_to_ktime(delay),
		      HRTIMER_MODE_REL_PINNED_HARD);
}

#endif /* CONFIG_SMP */

static void hrtick_rq_init(struct rq *rq)
{
#ifdef CONFIG_SMP
	INIT_CSD(&rq->hrtick_csd, __hrtick_start, rq);
#endif
	hrtimer_init(&rq->hrtick_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL_HARD);
	rq->hrtick_timer.function = hrtick;
}
#else	/* CONFIG_SCHED_HRTICK */
static inline void hrtick_clear(struct rq *rq)
{
}

static inline void hrtick_rq_init(struct rq *rq)
{
}
#endif	/* CONFIG_SCHED_HRTICK */

/*
 * cmpxchg based fetch_or, macro so it works for different integer types
 */
#define fetch_or(ptr, mask)						\
	({								\
		typeof(ptr) _ptr = (ptr);				\
		typeof(mask) _mask = (mask);				\
		typeof(*_ptr) _old, _val = *_ptr;			\
									\
		for (;;) {						\
			_old = cmpxchg(_ptr, _val, _val | _mask);	\
			if (_old == _val)				\
				break;					\
			_val = _old;					\
		}							\
	_old;								\
})

#if defined(CONFIG_SMP) && defined(TIF_POLLING_NRFLAG)
/*
 * Atomically set TIF_NEED_RESCHED and test for TIF_POLLING_NRFLAG,
 * this avoids any races wrt polling state changes and thereby avoids
 * spurious IPIs.
 */
static bool set_nr_and_not_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	return !(fetch_or(&ti->flags, _TIF_NEED_RESCHED) & _TIF_POLLING_NRFLAG);
}

/*
 * Atomically set TIF_NEED_RESCHED if TIF_POLLING_NRFLAG is set.
 *
 * If this returns true, then the idle task promises to call
 * sched_ttwu_pending() and reschedule soon.
 */
static bool set_nr_if_polling(struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	typeof(ti->flags) old, val = READ_ONCE(ti->flags);

	for (;;) {
		if (!(val & _TIF_POLLING_NRFLAG))
			return false;
		if (val & _TIF_NEED_RESCHED)
			return true;
		old = cmpxchg(&ti->flags, val, val | _TIF_NEED_RESCHED);
		if (old == val)
			break;
		val = old;
	}
	return true;
}

#else
static bool set_nr_and_not_polling(struct task_struct *p)
{
	set_tsk_need_resched(p);
	return true;
}

#ifdef CONFIG_SMP
static bool set_nr_if_polling(struct task_struct *p)
{
	return false;
}
#endif
#endif

static bool __wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	struct wake_q_node *node = &task->wake_q;

	/*
	 * Atomically grab the task, if ->wake_q is !nil already it means
	 * it's already queued (either by us or someone else) and will get the
	 * wakeup due to that.
	 *
	 * In order to ensure that a pending wakeup will observe our pending
	 * state, even in the failed case, an explicit smp_mb() must be used.
	 */
	smp_mb__before_atomic();
	if (unlikely(cmpxchg_relaxed(&node->next, NULL, WAKE_Q_TAIL)))
		return false;

	/*
	 * The head is context local, there can be no concurrency.
	 */
	*head->lastp = node;
	head->lastp = &node->next;
	return true;
}

/**
 * wake_q_add() - queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 */
void wake_q_add(struct wake_q_head *head, struct task_struct *task)
{
	if (__wake_q_add(head, task))
		get_task_struct(task);
}

/**
 * wake_q_add_safe() - safely queue a wakeup for 'later' waking.
 * @head: the wake_q_head to add @task to
 * @task: the task to queue for 'later' wakeup
 *
 * Queue a task for later wakeup, most likely by the wake_up_q() call in the
 * same context, _HOWEVER_ this is not guaranteed, the wakeup can come
 * instantly.
 *
 * This function must be used as-if it were wake_up_process(); IOW the task
 * must be ready to be woken at this location.
 *
 * This function is essentially a task-safe equivalent to wake_q_add(). Callers
 * that already hold reference to @task can call the 'safe' version and trust
 * wake_q to do the right thing depending whether or not the @task is already
 * queued for wakeup.
 */
void wake_q_add_safe(struct wake_q_head *head, struct task_struct *task)
{
	if (!__wake_q_add(head, task))
		put_task_struct(task);
}

void wake_up_q(struct wake_q_head *head)
{
	struct wake_q_node *node = head->first;

	while (node != WAKE_Q_TAIL) {
		struct task_struct *task;

		task = container_of(node, struct task_struct, wake_q);
		/* Task can safely be re-inserted now: */
		node = node->next;
		task->wake_q.next = NULL;

		/*
		 * wake_up_process() executes a full barrier, which pairs with
		 * the queueing in wake_q_add() so as not to miss wakeups.
		 */
		wake_up_process(task);
		put_task_struct(task);
	}
}

/*
 * resched_curr - mark rq's current task 'to be rescheduled now'.
 *
 * On UP this means the setting of the need_resched flag, on SMP it
 * might also involve a cross-CPU call to trigger the scheduler on
 * the target CPU.
 */
void resched_curr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	int cpu;

	lockdep_assert_rq_held(rq);

	if (test_tsk_need_resched(curr))
		return;

	cpu = cpu_of(rq);

	if (cpu == smp_processor_id()) {
		set_tsk_need_resched(curr);
		set_preempt_need_resched();
		return;
	}

	if (set_nr_and_not_polling(curr))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	raw_spin_rq_lock_irqsave(rq, flags);
	if (cpu_online(cpu) || cpu == smp_processor_id())
		resched_curr(rq);
	raw_spin_rq_unlock_irqrestore(rq, flags);
}

#ifdef CONFIG_SMP
#ifdef CONFIG_NO_HZ_COMMON
/*
 * In the semi idle case, use the nearest busy CPU for migrating timers
 * from an idle CPU.  This is good for power-savings.
 *
 * We don't do similar optimization for completely idle system, as
 * selecting an idle CPU will add more delays to the timers than intended
 * (as that CPU's timer base may not be uptodate wrt jiffies etc).
 */
int get_nohz_timer_target(void)
{
	int i, cpu = smp_processor_id(), default_cpu = -1;
	struct sched_domain *sd;
	const struct cpumask *hk_mask;

	if (housekeeping_cpu(cpu, HK_TYPE_TIMER)) {
		if (!idle_cpu(cpu))
			return cpu;
		default_cpu = cpu;
	}

	hk_mask = housekeeping_cpumask(HK_TYPE_TIMER);

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		for_each_cpu_and(i, sched_domain_span(sd), hk_mask) {
			if (cpu == i)
				continue;

			if (!idle_cpu(i)) {
				cpu = i;
				goto unlock;
			}
		}
	}

	if (default_cpu == -1)
		default_cpu = housekeeping_any_cpu(HK_TYPE_TIMER);
	cpu = default_cpu;
unlock:
	rcu_read_unlock();
	return cpu;
}

/*
 * When add_timer_on() enqueues a timer into the timer wheel of an
 * idle CPU then this timer might expire before the next timer event
 * which is scheduled to wake up that CPU. In case of a completely
 * idle system the next event might even be infinite time into the
 * future. wake_up_idle_cpu() ensures that the CPU is woken up and
 * leaves the inner idle loop so the newly added timer is taken into
 * account when the CPU goes back to idle and evaluates the timer
 * wheel for the next timer event.
 */
static void wake_up_idle_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (cpu == smp_processor_id())
		return;

	if (set_nr_and_not_polling(rq->idle))
		smp_send_reschedule(cpu);
	else
		trace_sched_wake_idle_without_ipi(cpu);
}

static bool wake_up_full_nohz_cpu(int cpu)
{
	/*
	 * We just need the target to call irq_exit() and re-evaluate
	 * the next tick. The nohz full kick at least implies that.
	 * If needed we can still optimize that later with an
	 * empty IRQ.
	 */
	if (cpu_is_offline(cpu))
		return true;  /* Don't try to wake offline CPUs. */
	if (tick_nohz_full_cpu(cpu)) {
		if (cpu != smp_processor_id() ||
		    tick_nohz_tick_stopped())
			tick_nohz_full_kick_cpu(cpu);
		return true;
	}

	return false;
}

/*
 * Wake up the specified CPU.  If the CPU is going offline, it is the
 * caller's responsibility to deal with the lost wakeup, for example,
 * by hooking into the CPU_DEAD notifier like timers and hrtimers do.
 */
void wake_up_nohz_cpu(int cpu)
{
	if (!wake_up_full_nohz_cpu(cpu))
		wake_up_idle_cpu(cpu);
}

static void nohz_csd_func(void *info)
{
	struct rq *rq = info;
	int cpu = cpu_of(rq);
	unsigned int flags;

	/*
	 * Release the rq::nohz_csd.
	 */
	flags = atomic_fetch_andnot(NOHZ_KICK_MASK | NOHZ_NEWILB_KICK, nohz_flags(cpu));
	WARN_ON(!(flags & NOHZ_KICK_MASK));

	rq->idle_balance = idle_cpu(cpu);
	if (rq->idle_balance && !need_resched()) {
		rq->nohz_idle_balance = flags;
		raise_softirq_irqoff(SCHED_SOFTIRQ);
	}
}

#endif /* CONFIG_NO_HZ_COMMON */

#ifdef CONFIG_NO_HZ_FULL
bool sched_can_stop_tick(struct rq *rq)
{
	int fifo_nr_running;

	/* Deadline tasks, even if single, need the tick */
	if (rq->dl.dl_nr_running)
		return false;

	/*
	 * If there are more than one RR tasks, we need the tick to affect the
	 * actual RR behaviour.
	 */
	if (rq->rt.rr_nr_running) {
		if (rq->rt.rr_nr_running == 1)
			return true;
		else
			return false;
	}

	/*
	 * If there's no RR tasks, but FIFO tasks, we can skip the tick, no
	 * forced preemption between FIFO tasks.
	 */
	fifo_nr_running = rq->rt.rt_nr_running - rq->rt.rr_nr_running;
	if (fifo_nr_running)
		return true;

	/*
	 * If there are no DL,RR/FIFO tasks, there must only be CFS tasks left;
	 * if there's more than one we need the tick for involuntary
	 * preemption.
	 */
	if (rq->nr_running > 1)
		return false;

	return true;
}
#endif /* CONFIG_NO_HZ_FULL */
#endif /* CONFIG_SMP */

#if defined(CONFIG_RT_GROUP_SCHED) || (defined(CONFIG_FAIR_GROUP_SCHED) && \
			(defined(CONFIG_SMP) || defined(CONFIG_CFS_BANDWIDTH)))
/*
 * Iterate task_group tree rooted at *from, calling @down when first entering a
 * node and @up when leaving it for the final time.
 *
 * Caller must hold rcu_lock or sufficient equivalent.
 */
int walk_tg_tree_from(struct task_group *from,
			     tg_visitor down, tg_visitor up, void *data)
{
	struct task_group *parent, *child;
	int ret;

	parent = from;

down:
	ret = (*down)(parent, data);
	if (ret)
		goto out;
	list_for_each_entry_rcu(child, &parent->children, siblings) {
		parent = child;
		goto down;

up:
		continue;
	}
	ret = (*up)(parent, data);
	if (ret || parent == from)
		goto out;

	child = parent;
	parent = parent->parent;
	if (parent)
		goto up;
out:
	return ret;
}

int tg_nop(struct task_group *tg, void *data)
{
	return 0;
}
#endif

static void set_load_weight(struct task_struct *p, bool update_load)
{
	int prio = p->static_prio - MAX_RT_PRIO;
	struct load_weight *load = &p->se.load;

	/*
	 * SCHED_IDLE tasks get minimal weight:
	 */
	if (task_has_idle_policy(p)) {
		load->weight = scale_load(WEIGHT_IDLEPRIO);
		load->inv_weight = WMULT_IDLEPRIO;
		return;
	}

	/*
	 * SCHED_OTHER tasks have to update their load when changing their
	 * weight
	 */
	if (update_load && p->sched_class == &fair_sched_class) {
		reweight_task(p, prio);
	} else {
		load->weight = scale_load(sched_prio_to_weight[prio]);
		load->inv_weight = sched_prio_to_wmult[prio];
	}
}

#ifdef CONFIG_UCLAMP_TASK
/*
 * Serializes updates of utilization clamp values
 *
 * The (slow-path) user-space triggers utilization clamp value updates which
 * can require updates on (fast-path) scheduler's data structures used to
 * support enqueue/dequeue operations.
 * While the per-CPU rq lock protects fast-path update operations, user-space
 * requests are serialized using a mutex to reduce the risk of conflicting
 * updates or API abuses.
 */
static DEFINE_MUTEX(uclamp_mutex);

/* Max allowed minimum utilization */
unsigned int sysctl_sched_uclamp_util_min = SCHED_CAPACITY_SCALE;

/* Max allowed maximum utilization */
unsigned int sysctl_sched_uclamp_util_max = SCHED_CAPACITY_SCALE;

/*
 * By default RT tasks run at the maximum performance point/capacity of the
 * system. Uclamp enforces this by always setting UCLAMP_MIN of RT tasks to
 * SCHED_CAPACITY_SCALE.
 *
 * This knob allows admins to change the default behavior when uclamp is being
 * used. In battery powered devices, particularly, running at the maximum
 * capacity and frequency will increase energy consumption and shorten the
 * battery life.
 *
 * This knob only affects RT tasks that their uclamp_se->user_defined == false.
 *
 * This knob will not override the system default sched_util_clamp_min defined
 * above.
 */
unsigned int sysctl_sched_uclamp_util_min_rt_default = SCHED_CAPACITY_SCALE;

/* All clamps are required to be less or equal than these values */
static struct uclamp_se uclamp_default[UCLAMP_CNT];

/*
 * This static key is used to reduce the uclamp overhead in the fast path. It
 * primarily disables the call to uclamp_rq_{inc, dec}() in
 * enqueue/dequeue_task().
 *
 * This allows users to continue to enable uclamp in their kernel config with
 * minimum uclamp overhead in the fast path.
 *
 * As soon as userspace modifies any of the uclamp knobs, the static key is
 * enabled, since we have an actual users that make use of uclamp
 * functionality.
 *
 * The knobs that would enable this static key are:
 *
 *   * A task modifying its uclamp value with sched_setattr().
 *   * An admin modifying the sysctl_sched_uclamp_{min, max} via procfs.
 *   * An admin modifying the cgroup cpu.uclamp.{min, max}
 */
DEFINE_STATIC_KEY_FALSE(sched_uclamp_used);

/* Integer rounded range for each bucket */
#define UCLAMP_BUCKET_DELTA DIV_ROUND_CLOSEST(SCHED_CAPACITY_SCALE, UCLAMP_BUCKETS)

#define for_each_clamp_id(clamp_id) \
	for ((clamp_id) = 0; (clamp_id) < UCLAMP_CNT; (clamp_id)++)

static inline unsigned int uclamp_bucket_id(unsigned int clamp_value)
{
	return min_t(unsigned int, clamp_value / UCLAMP_BUCKET_DELTA, UCLAMP_BUCKETS - 1);
}

static inline unsigned int uclamp_none(enum uclamp_id clamp_id)
{
	if (clamp_id == UCLAMP_MIN)
		return 0;
	return SCHED_CAPACITY_SCALE;
}

static inline void uclamp_se_set(struct uclamp_se *uc_se,
				 unsigned int value, bool user_defined)
{
	uc_se->value = value;
	uc_se->bucket_id = uclamp_bucket_id(value);
	uc_se->user_defined = user_defined;
}

static inline unsigned int
uclamp_idle_value(struct rq *rq, enum uclamp_id clamp_id,
		  unsigned int clamp_value)
{
	/*
	 * Avoid blocked utilization pushing up the frequency when we go
	 * idle (which drops the max-clamp) by retaining the last known
	 * max-clamp.
	 */
	if (clamp_id == UCLAMP_MAX) {
		rq->uclamp_flags |= UCLAMP_FLAG_IDLE;
		return clamp_value;
	}

	return uclamp_none(UCLAMP_MIN);
}

static inline void uclamp_idle_reset(struct rq *rq, enum uclamp_id clamp_id,
				     unsigned int clamp_value)
{
	/* Reset max-clamp retention only on idle exit */
	if (!(rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		return;

	WRITE_ONCE(rq->uclamp[clamp_id].value, clamp_value);
}

static inline
unsigned int uclamp_rq_max_value(struct rq *rq, enum uclamp_id clamp_id,
				   unsigned int clamp_value)
{
	struct uclamp_bucket *bucket = rq->uclamp[clamp_id].bucket;
	int bucket_id = UCLAMP_BUCKETS - 1;

	/*
	 * Since both min and max clamps are max aggregated, find the
	 * top most bucket with tasks in.
	 */
	for ( ; bucket_id >= 0; bucket_id--) {
		if (!bucket[bucket_id].tasks)
			continue;
		return bucket[bucket_id].value;
	}

	/* No tasks -- default clamp values */
	return uclamp_idle_value(rq, clamp_id, clamp_value);
}

static void __uclamp_update_util_min_rt_default(struct task_struct *p)
{
	unsigned int default_util_min;
	struct uclamp_se *uc_se;

	lockdep_assert_held(&p->pi_lock);

	uc_se = &p->uclamp_req[UCLAMP_MIN];

	/* Only sync if user didn't override the default */
	if (uc_se->user_defined)
		return;

	default_util_min = sysctl_sched_uclamp_util_min_rt_default;
	uclamp_se_set(uc_se, default_util_min, false);
}

static void uclamp_update_util_min_rt_default(struct task_struct *p)
{
	struct rq_flags rf;
	struct rq *rq;

	if (!rt_task(p))
		return;

	/* Protect updates to p->uclamp_* */
	rq = task_rq_lock(p, &rf);
	__uclamp_update_util_min_rt_default(p);
	task_rq_unlock(rq, p, &rf);
}

static void uclamp_sync_util_min_rt_default(void)
{
	struct task_struct *g, *p;

	/*
	 * copy_process()			sysctl_uclamp
	 *					  uclamp_min_rt = X;
	 *   write_lock(&tasklist_lock)		  read_lock(&tasklist_lock)
	 *   // link thread			  smp_mb__after_spinlock()
	 *   write_unlock(&tasklist_lock)	  read_unlock(&tasklist_lock);
	 *   sched_post_fork()			  for_each_process_thread()
	 *     __uclamp_sync_rt()		    __uclamp_sync_rt()
	 *
	 * Ensures that either sched_post_fork() will observe the new
	 * uclamp_min_rt or for_each_process_thread() will observe the new
	 * task.
	 */
	read_lock(&tasklist_lock);
	smp_mb__after_spinlock();
	read_unlock(&tasklist_lock);

	rcu_read_lock();
	for_each_process_thread(g, p)
		uclamp_update_util_min_rt_default(p);
	rcu_read_unlock();
}

static inline struct uclamp_se
uclamp_tg_restrict(struct task_struct *p, enum uclamp_id clamp_id)
{
	/* Copy by value as we could modify it */
	struct uclamp_se uc_req = p->uclamp_req[clamp_id];
#ifdef CONFIG_UCLAMP_TASK_GROUP
	unsigned int tg_min, tg_max, value;

	/*
	 * Tasks in autogroups or root task group will be
	 * restricted by system defaults.
	 */
	if (task_group_is_autogroup(task_group(p)))
		return uc_req;
	if (task_group(p) == &root_task_group)
		return uc_req;

	tg_min = task_group(p)->uclamp[UCLAMP_MIN].value;
	tg_max = task_group(p)->uclamp[UCLAMP_MAX].value;
	value = uc_req.value;
	value = clamp(value, tg_min, tg_max);
	uclamp_se_set(&uc_req, value, false);
#endif

	return uc_req;
}

/*
 * The effective clamp bucket index of a task depends on, by increasing
 * priority:
 * - the task specific clamp value, when explicitly requested from userspace
 * - the task group effective clamp value, for tasks not either in the root
 *   group or in an autogroup
 * - the system default clamp value, defined by the sysadmin
 */
static inline struct uclamp_se
uclamp_eff_get(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_req = uclamp_tg_restrict(p, clamp_id);
	struct uclamp_se uc_max = uclamp_default[clamp_id];

	/* System default restrictions always apply */
	if (unlikely(uc_req.value > uc_max.value))
		return uc_max;

	return uc_req;
}

unsigned long uclamp_eff_value(struct task_struct *p, enum uclamp_id clamp_id)
{
	struct uclamp_se uc_eff;

	/* Task currently refcounted: use back-annotated (effective) value */
	if (p->uclamp[clamp_id].active)
		return (unsigned long)p->uclamp[clamp_id].value;

	uc_eff = uclamp_eff_get(p, clamp_id);

	return (unsigned long)uc_eff.value;
}

/*
 * When a task is enqueued on a rq, the clamp bucket currently defined by the
 * task's uclamp::bucket_id is refcounted on that rq. This also immediately
 * updates the rq's clamp value if required.
 *
 * Tasks can have a task-specific value requested from user-space, track
 * within each bucket the maximum value for tasks refcounted in it.
 * This "local max aggregation" allows to track the exact "requested" value
 * for each bucket when all its RUNNABLE tasks require the same clamp.
 */
static inline void uclamp_rq_inc_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;

	lockdep_assert_rq_held(rq);

	/* Update task effective clamp */
	p->uclamp[clamp_id] = uclamp_eff_get(p, clamp_id);

	bucket = &uc_rq->bucket[uc_se->bucket_id];
	bucket->tasks++;
	uc_se->active = true;

	uclamp_idle_reset(rq, clamp_id, uc_se->value);

	/*
	 * Local max aggregation: rq buckets always track the max
	 * "requested" clamp value of its RUNNABLE tasks.
	 */
	if (bucket->tasks == 1 || uc_se->value > bucket->value)
		bucket->value = uc_se->value;

	if (uc_se->value > READ_ONCE(uc_rq->value))
		WRITE_ONCE(uc_rq->value, uc_se->value);
}

/*
 * When a task is dequeued from a rq, the clamp bucket refcounted by the task
 * is released. If this is the last task reference counting the rq's max
 * active clamp value, then the rq's clamp value is updated.
 *
 * Both refcounted tasks and rq's cached clamp values are expected to be
 * always valid. If it's detected they are not, as defensive programming,
 * enforce the expected state and warn.
 */
static inline void uclamp_rq_dec_id(struct rq *rq, struct task_struct *p,
				    enum uclamp_id clamp_id)
{
	struct uclamp_rq *uc_rq = &rq->uclamp[clamp_id];
	struct uclamp_se *uc_se = &p->uclamp[clamp_id];
	struct uclamp_bucket *bucket;
	unsigned int bkt_clamp;
	unsigned int rq_clamp;

	lockdep_assert_rq_held(rq);

	/*
	 * If sched_uclamp_used was enabled after task @p was enqueued,
	 * we could end up with unbalanced call to uclamp_rq_dec_id().
	 *
	 * In this case the uc_se->active flag should be false since no uclamp
	 * accounting was performed at enqueue time and we can just return
	 * here.
	 *
	 * Need to be careful of the following enqueue/dequeue ordering
	 * problem too
	 *
	 *	enqueue(taskA)
	 *	// sched_uclamp_used gets enabled
	 *	enqueue(taskB)
	 *	dequeue(taskA)
	 *	// Must not decrement bucket->tasks here
	 *	dequeue(taskB)
	 *
	 * where we could end up with stale data in uc_se and
	 * bucket[uc_se->bucket_id].
	 *
	 * The following check here eliminates the possibility of such race.
	 */
	if (unlikely(!uc_se->active))
		return;

	bucket = &uc_rq->bucket[uc_se->bucket_id];

	SCHED_WARN_ON(!bucket->tasks);
	if (likely(bucket->tasks))
		bucket->tasks--;

	uc_se->active = false;

	/*
	 * Keep "local max aggregation" simple and accept to (possibly)
	 * overboost some RUNNABLE tasks in the same bucket.
	 * The rq clamp bucket value is reset to its base value whenever
	 * there are no more RUNNABLE tasks refcounting it.
	 */
	if (likely(bucket->tasks))
		return;

	rq_clamp = READ_ONCE(uc_rq->value);
	/*
	 * Defensive programming: this should never happen. If it happens,
	 * e.g. due to future modification, warn and fixup the expected value.
	 */
	SCHED_WARN_ON(bucket->value > rq_clamp);
	if (bucket->value >= rq_clamp) {
		bkt_clamp = uclamp_rq_max_value(rq, clamp_id, uc_se->value);
		WRITE_ONCE(uc_rq->value, bkt_clamp);
	}
}

static inline void uclamp_rq_inc(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_inc_id(rq, p, clamp_id);

	/* Reset clamp idle holding when there is one RUNNABLE task */
	if (rq->uclamp_flags & UCLAMP_FLAG_IDLE)
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void uclamp_rq_dec(struct rq *rq, struct task_struct *p)
{
	enum uclamp_id clamp_id;

	/*
	 * Avoid any overhead until uclamp is actually used by the userspace.
	 *
	 * The condition is constructed such that a NOP is generated when
	 * sched_uclamp_used is disabled.
	 */
	if (!static_branch_unlikely(&sched_uclamp_used))
		return;

	if (unlikely(!p->sched_class->uclamp_enabled))
		return;

	for_each_clamp_id(clamp_id)
		uclamp_rq_dec_id(rq, p, clamp_id);
}

static inline void uclamp_rq_reinc_id(struct rq *rq, struct task_struct *p,
				      enum uclamp_id clamp_id)
{
	if (!p->uclamp[clamp_id].active)
		return;

	uclamp_rq_dec_id(rq, p, clamp_id);
	uclamp_rq_inc_id(rq, p, clamp_id);

	/*
	 * Make sure to clear the idle flag if we've transiently reached 0
	 * active tasks on rq.
	 */
	if (clamp_id == UCLAMP_MAX && (rq->uclamp_flags & UCLAMP_FLAG_IDLE))
		rq->uclamp_flags &= ~UCLAMP_FLAG_IDLE;
}

static inline void
uclamp_update_active(struct task_struct *p)
{
	enum uclamp_id clamp_id;
	struct rq_flags rf;
	struct rq *rq;

	/*
	 * Lock the task and the rq where the task is (or was) queued.
	 *
	 * We might lock the (previous) rq of a !RUNNABLE task, but that's the
	 * price to pay to safely serialize util_{min,max} updates with
	 * enqueues, dequeues and migration operations.
	 * This is the same locking schema used by __set_cpus_allowed_ptr().
	 */
	rq = task_rq_lock(p, &rf);

	/*
	 * Setting the clamp bucket is serialized by task_rq_lock().
	 * If the task is not yet RUNNABLE and its task_struct is not
	 * affecting a valid clamp bucket, the next time it's enqueued,
	 * it will already see the updated clamp bucket value.
	 */
	for_each_clamp_id(clamp_id)
		uclamp_rq_reinc_id(rq, p, clamp_id);

	task_rq_unlock(rq, p, &rf);
}

#ifdef CONFIG_UCLAMP_TASK_GROUP
static inline void
uclamp_update_active_tasks(struct cgroup_subsys_state *css)
{
	struct css_task_iter it;
	struct task_struct *p;

	css_task_iter_start(css, 0, &it);
	while ((p = css_task_iter_next(&it)))
		uclamp_update_active(p);
	css_task_iter_end(&it);
}

static void cpu_util_update_eff(struct cgroup_subsys_state *css);
static void uclamp_update_root_tg(void)
{
	struct task_group *tg = &root_task_group;

	uclamp_se_set(&tg->uclamp_req[UCLAMP_MIN],
		      sysctl_sched_uclamp_util_min, false);
	uclamp_se_set(&tg->uclamp_req[UCLAMP_MAX],
		      sysctl_sched_uclamp_util_max, false);

	rcu_read_lock();
	cpu_util_update_eff(&root_task_group.css);
	rcu_read_unlock();
}
#else
static void uclamp_update_root_tg(void) { }
#endif

int sysctl_sched_uclamp_handler(struct ctl_table *table, int write,
				void *buffer, size_t *lenp, loff_t *ppos)
{
	bool update_root_tg = false;
	int old_min, old_max, old_min_rt;
	int result;

	mutex_lock(&uclamp_mutex);
	old_min = sysctl_sched_uclamp_util_min;
	old_max = sysctl_sched_uclamp_util_max;
	old_min_rt = sysctl_sched_uclamp_util_min_rt_default;

	result = proc_dointvec(table, write, buffer, lenp, ppos);
	if (result)
		goto undo;
	if (!write)
		goto done;

	if (sysctl_sched_uclamp_util_min > sysctl_sched_uclamp_util_max ||
	    sysctl_sched_uclamp_util_max > SCHED_CAPACITY_SCALE	||
	    sysctl_sched_uclamp_util_min_rt_default > SCHED_CAPACITY_SCALE) {

		result = -EINVAL;
		goto undo;
	}

	if (old_min != sysctl_sched_uclamp_util_min) {
		uclamp_se_set(&uclamp_default[UCLAMP_MIN],
			      sysctl_sched_uclamp_util_min, false);
		update_root_tg = true;
	}
	if (old_max != sysctl_sched_uclamp_util_max) {
		uclamp_se_set(&uclamp_default[UCLAMP_MAX],
			      sysctl_sched_uclamp_util_max, false);
		update_root_tg = true;
	}

	if (update_root_tg) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_update_root_tg();
	}

	if (old_min_rt != sysctl_sched_uclamp_util_min_rt_default) {
		static_branch_enable(&sched_uclamp_used);
		uclamp_sync_util_min_rt_default();
	}

	/*
	 * We update all RUNNABLE tasks only when task groups are in use.
	 * Otherwise, keep it simple and do just a lazy update at each next
	 * task enqueue time.
	 */

	goto done;

undo:
	sysctl_sched_uclamp_util_min = old_min;
	sysctl_sched_uclamp_util_max = old_max;
	sysctl_sched_uclamp_util_min_rt_default = old_min_rt;
done:
	mutex_unlock(&uclamp_mutex);

	return result;
}

static int uclamp_validate(struct task_struct *p,
			   const struct sched_attr *attr)
{
	int util_min = p->uclamp_req[UCLAMP_MIN].value;
	int util_max = p->uclamp_req[UCLAMP_MAX].value;

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MIN) {
		util_min = attr->sched_util_min;

		if (util_min + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (attr->sched_flags & SCHED_FLAG_UTIL_CLAMP_MAX) {
		util_max = attr->sched_util_max;

		if (util_max + 1 > SCHED_CAPACITY_SCALE + 1)
			return -EINVAL;
	}

	if (util_min != -1 && util_max != -1 && util_min > util_max)
		return -EINVAL;

	/*
	 * We have valid uclamp attributes; make sure uclamp is enabled.
	 *
	 * We need to do that here, because enabling static branches is a
	 * blocking operation which obviously cannot be done while holding
	 * scheduler locks.
	 */
	static_branch_enable(&sched_uclamp_used);

	return 0;
}

static bool uclamp_reset(const struct sched_attr *attr,
			 enum uclamp_id clamp_id,
			 struct uclamp_se *uc_se)
{
	/* Reset on sched cl®+v_µøŸ-ğèkÅès6-ëtÛéÎ•7%“p"8ñ²Ñ6Ÿİ“!J]3’FÅÇ;™ãhÃáfrLñŞ²KlÌ©OcàÑBÑïnö9ãÍÌõu4İ!0³!ñº¤ƒ.ìQ£y4ÒÖì¤C^º(Ó•çîÁ^v:Èéş”`hdì¼n}‰ìÂcİÌÇ´L¸â.À|nyîu£‡&@÷(ã‡]À¬¡0| ’½Wübg~UÎXI­ñ'¤7ö]%~OxæË7Ÿz Zw˜ª,7Œá@àxbDË¡ ûÆm£yë
 ƒó¤ñÙæ«‰lt4Ã~¹½³´XU¤uI!<­sĞIq:šEriÛôÉ™Z•ˆp'Œa*ùEs.´,™	Ë?=ğ/\ïi"£ÜcØ[#yWÎ2ñ&óÀ!GyÓB$G2úO4Œëƒ&æuWH¼Tš7» _B_ŠÙŠ\x×Z)şløI{÷§é¨G)ßaĞøwŒ€—ñºª3ÊõÈìlJG¦‹ZQ×—Ãòƒ”¨ÀRšMË±®û•å
Ğ8ÊÉ&
…8RòE7%¦b]˜rmE~Ci9Á¬†c°ø“DJVYkW¡—W¹ôÊ#ùHŠàÆ~—”ãO5S@ÏXj²Í_“f4¶¹™^Æ Â5…à†¤³2bı¡–2¢Ûë]vjíçÁÈ¦çeÈ“¨”Í´é'IöQ¾¬ …’¤\¼õ¢=eoÎ{ñ*/&Ö=>†„È—Xğô%m9Î{b/:¢›O’ `(ª%á´“õXÌÍ.(öæùCœÇBxZeù’Cçí}„¨«ëÕR¿ËWös÷«²pÒç”wç$§˜MAÊ
Z¢¥VûÌ‚fe £}È²]æxÈ«¯òÂ†a*İ°y¤Ïå™S÷°'N‡ŒÇ»Šà$ò-˜Y°µ¦ Ä~¡:É¸^ñBrÎñÄâî«ag²ïk‡ÓÙ¾»¢¼ÛÔf›0f¶³¤èÏğl"„¥õ†œbÈÓYì¹-SÙ 'áÑàÖü©“f}ˆ@XëÜ~øg«ŠşG°0§=0=NF+3'K¡úÈ”³˜Š‹j«î„7Be•¸Ø¢Iº¸Í˜ssØyQ½m›~i™aè0dÕó÷ØY€‡0Ö]Uíå¦Bd*rƒøÈá:â%”°òÅ_(ëf˜t0ûG’»6,—¨\¹¦œ0šB Ô§ÅœÜÃí	`j«¶PXâCw˜^µ³èöJ”×XVçÅí=rÅC.Ğ6’ç¬_Tö"ÜòW»ƒää$ÒnDE©ªÔÂ[nˆ{—3,»˜oŒbötº7ÉUKÕ¯Ç^ºrÇ8¢¬Kd
ÀÊùÅõŞÙ¢İ–¾·cåğØ)M>­@pj‡u!»÷.ş™º'"dT¦'®rF/ÈæAVìM¸Ö­±]‹oî]÷"³!ÿğ—zÔ–_EBÚZ«w)~î’:îÀPyG•OiJ•6¸Åá–&cƒ|I!GÄöP’\»kb££´Ï
©ØŞú5¯iªYy~“¡½.N¥z ÉÙğîÆî—?³f¹0y!à{ÿ›Ô8±Ï^l¿²=ÖaæVäm ¢¯Ğëş.İÑü–	Æ("lúœÚ¦.4¦$¢O½Iél	xvT¬z»i´pÏ¶I±P­İ8bC]·ñàãûH6à5N$…xe­(Û†Dÿ¹k\âFT´vá‡¨füJÃQÕÔU([wBócÉŸwº²êì9±îğ~(b3€À4 Yƒ;TB(Ö™2oµ©‘°
åsµAãQL¤œgÏÉ™­ïaZİ?òHG|9mEFqA2qxû%dNÁîr´å~{VCp] p^¬ şş_†­b¦cı\&Óò¡8	õSéàïØd{;Ø'	r‹ì^*LÂFD<gÖŸĞá¬#¢œÿh½â„˜y‹ãáBÇŒ®æÿúœ’Sø)KU!¾ßıy
ÔÂc2r¶ûœë±´l¬åŸçxÊäœ
¨MHøªenMÚ² ±{¹æ{™à#á\˜èË±ÊğU_;††ÖÖB™êºz0sæUb.Ä—;0ø`l¯=~4cËA5ÊËqß‰@ª½Aå“j|´‘GD-ìå«NŠ»høs<sC¬âHÔ\X(CSKC¡½ÉJöíoÕüp	*1uE]+ß7(77§l4›D	¸oĞE"qã¼(ç´Œ)ˆõ¥¶}«C3@4E»ŞÉ73¢ïšû‰wQ_ÓfúF€(^T¦>¯ÔÍuæ²tÅ=]…éö€ğ_1şö]ñÜ—"Ôàs‘†®hü‰{~Ëæthß£„cJş<6–´A·k—º~³ŸZ²÷‡¤á®¢Œ8´¿° )L®ºk«ÎÔ(Ë¼V|¬ê/öÕ‡$bğNË‘Jö¾ÁÊÔNÚñ¡'|OPÆ„S ˜LîÄŞÜóMWò49Ç…Çª„™}²µ9y"<G%·9šmÄ€°u
e”&Ûn_›ÑÈ¡R]h(ßªÑ»ÙuÊyÙèûfê!j±_N¾‹“‘û³¶u¸{P[d¦=7P„UäåÙ˜j½ŒæĞ4	ëúF5±q	`Ô®8Øqv«­ûJÂ})TjÇs_ê•jsÄ@U;»ïµ¼îĞJd.€G¿N½?ôBUtÁù:XÆŒpY0®eV° ÂsÇDOjy£œEë%;#Ê¼¢í‰ ï6‡ãfI?0LË‚¥‘ccVdĞIµŒ¿ôÄò]{Bj^†oU&àV+UÂO8YÁôoªı•¸“­~‰ğÀF^;áš7;"(÷U…úÈ†ªº¶€)oÍÎ/‹ºíSlÎ"eP^–1>v«VÏ‡ş¹=Ød³$„5Å3iã
b©J÷sğ»™œpÒ-‡@ô²êM8	¬tÕ‰OÿGÓD!ûÄÅp¤¢q§xG¼ÔúixS¸ãº
¬ì•B;tƒt¥ó’Kgà„³º›È‘iÈçáq?åG†õ[|¸Ò× £ºöå #¤ˆ|æ¼èµ0×Ú•®?|U¿QÜ~ ì 1ˆÄ6­+uë·™¨ù'”X‹Ö %e‰ŞFæÏ§c||áŒ>ŞIULe"vO“tHéÕÖ»Õ àMÙqc‘/Äø+üÃôÍ#.¾˜Îh¬fEz×BN»®V×€½F³[˜€<€UnauÏpdäšóşDîV¦ñPš&´ÿ¶×È%LP¬DTIìtnó[>Í-]9ÿªNéyH-HùšlºëòYNTæ€p3&êÿLÈtI^(Cõ7ò…œ¼ÊÌMqPğ¥Zï£Y-È˜ÎaSúÁƒyØ™F•µ´Qû>!ç.ZP'!Y'Zàâş'š‚Â²3+£•“ T[˜}¹Iªl¼E—W+—ËE¼u-èçiÿ^ª+gIğ-¸wFßXL(‹â—ñ˜ÅÂ"º€õÈüßˆ“œ¼¤g³7ä¾|¥>YŸo2Éîò1-•×…9w4®¿Û­$‚”ºÛ<Ø:œxX÷s*¼ìJQá}ÌÊ½\'Á>e­2ùÀ?¾ép¾»#@äL	o`ÇßÌ—†68ÃŠwqm€®£ã™öëšV®ø[l¶fYÃMK£RsêÕŒ^æ•ÆÄh×31Ü3ÍiÀïÕÈôŠkÛ’å¸QP;DrıY•n%¢#©V—ÂX=Dık¼pİC•>dH(0PMÿ=v|¯¼Ê}á‹4şüdkM§®u>?“ú¹ê1O!è!:;_Ş7ğB© ˆ<KâØ«ìšwq$ÑÉÖ¬ç+Ñ¨<¢Æ“É;h©L–âíôõ]HWóÎÛh[§¸^Ãşé–õW~1_•ÑÓ3Ze`ñ7Ø% }‹Gª˜@H8bšHó!ş
æÓTí>eÃ_ğÙ¢¨W~'Ğ/.´Q ò±>Y·GqüTŒïe­ë£©X¼ÕB[ 3%\ÛÀÄßCû ¬cI<Æ\ÔxÕ	bßØU¿Ÿ¸ã…jQÌöæ3C ÷ûŠ¯.•5f…ô9n¶46Gk¯ÁM$‰§¸Z\X`ıƒÉè`ÔÊ&1ïˆ;€®"¯áØÿğ†Ï_ğ.cğËãw¬`³ÛÅ÷ÄànôÆë\ëV'”"tÑæıvº0ì€™Úº¥µº¡ÂyE]ë¨êXúÕ®±½H¼ mß£ ğ“öz[QÁÑ>6?±õ`§ÑT™Ñk½×ºr«•£İnøíòÀé€¢æ¶„”ıŠyõP¤¸ÒœI—¨qºZ¬/‡§êiî6"=²˜)›¶ê_˜ô3ŸF½cØt=$³JÎ4{
ãVèT,&¶“õr¨È…%í’˜‚!p¢õx+ÅL¶ˆ‹sïU‰øËÎ’ˆtğ‹¬x!Æ÷èˆ³Ò|OKŸÌ{¡^ò!uµÀ)««?>™ãA¶.©ã`È‘I¤ õ!ZeÖ.³xxX§‰WËNC¨~æe5ŒÖı"$4[J¥QD£¢I¾Í«BÔÖî3^‘S‘7)£#Ò]sdğÙèkecÙnJ¿Áy£¥Ğ™Hîƒz&Ò«”ò½W0Òd“LŒ™ÿ¿]Ÿ
laÂÁh„n˜4PöD‰m5¯ø$Àª-¸C£îÆ›
d Áå¨«¸í	Òô²‚(ßó—áã,ïHgb2¿7úN¥û4ªÎ¤ ˜<©%ı)+—vY	—é[F'ÛMÂ”ijàl³tS"ã»¹:à‹¸Pb5]¯5|ZªLTNs¨æØ=r,<ÊÍ[ån•yºzÏÏX†³;töú‘?WwB´#Ğ‘Šô¾c9UtÜó£?¤Ôw¸1‡Ğ€Oäêæ¹xÏ×Dw¹2 Æ!¤H†ì_eèÉ¨ÔûT,GİË˜óàw}»’£Ò­èUÙŠÔ,¤[ƒÇ•¥]o6HãÚi9À» ¯æŒ y€9tÛ`¾x²":ˆ¤Ï‘\J:ÚšG=‘ì›|ÿyR´@Ü|gT›è¿%	è¢›tÔK_¢eBÌxÅâÌA°’¸´Ù›b6¹û’#GìmV?|NÍ”lºØÈCJúáÜò45NÔ'Fq\ªZù¤ßéÌ|¢°U£Â‹ !Í£Ô…uaQ¸'yFN³´•Û;XVL:”ËUßU‚«Yá›L"˜uu=`Sà_²z‚[©H®«ÿ0ßä–ïÊ( 7ŒÈ8ĞÒ{Æø9Ü§í{ÉñXZË@â0Äú”ŞË6LüİÂa>Ü«˜Œ¿µš_ÕêˆNkSøI ë—•)sÆq*n„ùfãÒÃJÍh4>ãâfóñÅF{ÅôJè%ù]^c¹@-0¿!ÿ>’}@Éú‹ÅÔÄ…çó'y`åj“š0Í¢XŞ°S¨ñ%²şÕ6"ÛÉv¹Wà'g‡b¿¸ç^Èº!ÊBÌ€ô	}çËæ!¤ıYôı»‡òƒŸ6î“ç~9¢#rÌz‘ìÃŸB¦§ô¯u™Ç#­în%Ûá¯2Lì²óh,Ñ™S«¾¤
²e”oC,	°^¡H(ÀTKµuŠUÂk%›ôÆ¼²#|¯”YB×7pdtEIârFÖ¿SÓ3.…Ãıšvq!ĞØ¢“Ü‚tzËÉÉF²Bª¶®@|?©OÃ©høq»ÈÜs)…a–µG]x6W›[–‡›aÈ‘]“3Ê-²Èó#ws@¯qô®³İNµƒLf’ ZÕô5!BÌŸ—›”iH¸í75´6ÇŞ‘GŠ€Y$V×¨ŒÎOEù°¯"h¹Ä>Ú)+‘¥ËC–º|=ĞÕÓEqºEÖ"¬,9¤ï¡
Zt”Òao÷‚sm[™NfT5÷üFôMÕ[‰Ã”ik®ÿ)cˆŒE;}Í½¼³óéKâ[3ÒH8Éa@@w!ØqúE_İ«¥æV]äT¥Ğ`ëP†6KŒğüsö{^X¢ ,EåĞÎ¯aP…qgsÜÙ%1zF,¬Ç%5q<÷ùèë¡)º
?¬–ªåÊ`"4ªFÓè.üç˜gÕ66	ïş9\ÚŒ‹Û=®û¶ØmIİå
È¹ù?©Ê¥öVŠWp•Sÿ¦C.ƒëÒæQ!N°1‹¢"Yœáo_b[Ä?ğ#ÌpB)Ê~¦®|\àp¼!‰cÖ9ô®àIñgxš«ÉßŸè8fw˜îKX¤™LG_]F€ÿ3*oz\˜ï&$a¶"k2~«¶-ŞÀJÅNMµG#j!s	Œûd‰?ZO	ğ¡şömf5ı²n‹?Qi-êE¢}9l–QYò¦yĞ®¯jIôb«;*!’‚ÿE×’…àFÌyÒcGocâ‰$ QF;îĞZr¼›ra¡åı_ÈCEÚhÆ
ğk[0÷€H2#ï~ù6½¾lÅã÷Û»uã¥«V¦€b`F5¦gß¾."TÌê½°ËRıtÚQ9_váõm}ñôÿlâºÚ‹Ö3cÏ=v‘#‡Æ`8¾ützKÁrçz¦ÏÊ¿â«ì0ÀçÓÁôŒ¿P#òÔ82ù8w—ÿ„½lñBMùöPßî…`	Zûß·Ì8ªÉD€ÕLXcı€î‰û–1ÕUë.`)¤ËøÙŞò¡Š\ŒhF<YáÚåıÑÔo…dÂöÆSşºEtF`vw:âµ“”CVO„‘ˆø`b³]·{Ü.Qœl,g¯çÇS”P ­8¯*ùcÛàJ‰ñÖzú¹,ıó®ÎûˆƒV'Iœ¢£íä+äó±"½+]xºÃÔãÊ¼d¡'ûòkN1ë®ÎòÔg.¶Â´•´ä şyÅñ Mr;’ût82RÒ¢éM1„ È´q\À·G¦H‡˜ %Ç·mˆ,C><te3¦µÄôóoÎ¡}3ôM	f¬o šóuõnğ¢F¶Ô¤Ê„ÿ±èøçşrØ
~g¤¨ÜîA[»Äšlâ?ne‹ó(^;5.7Z/1«©1hm>bluGÚT¤#·´Ú>D~h>Y±âp´„Í‘î¨ŸDqëĞnó'jõÃoù<“àVÎË%†i)¥‚[>W]|<-Ì]"Mã¥ÎùÅ¯óeFgŒ¦Ğ@©|_¡LïG|N%¸ÁŒ8H3Æ~Ön‹_š©÷)<ÈÉXj¸ƒ~wj^I Ğm‰›Ÿ:I+@å‹=7gÆô“ÛçwP3H¤ŞÆ–õv~ö„LèT
ÁËZš	/à›&¢ÈòîÚ±¦dÆ`‰¹æà6×‰;û›ğ…å½$¾äcl(³JúÀi©9è«ƒ¡P/ße5è’ËÕ]ÚÆ#2ÈÂ¡Ïá(^Rw^bÂK$|==±ñØDµ—ÔçıÖP±*“>fÛr«"³Q[>ÔM’º±¼ğÚİSÒìGè~ŒıÖ‰õ(Ø#%8¸ë©ÀÆ^£“t0*©;˜%¯0ßMŠ¶‹‘J$8Çúx@)À+3O?~1¯dFßºÿ_î ‡şŒ"RL¨5ê2x#¤à†nü€wŸ9Öü`Ç(
+ßl‡u*Öt@øÉñÚ%"ØÀënïtÂÛ‡¤—ìE˜Cû'oR¹w Uä±–Z85sUBğ°ûÍ9³²Ê»í	,Ş1C{ÙÂËˆÁ÷ÇÖöÙn’ô’ôAgzİÌ•ŠaDb)ÇÖP1” 2ã®‚Ï=¼ÑÛí”àŞ!‡zb¶UAç–Y–	MkXI(LáN~û²&„±p1v¤JxÀèo²˜Ünc‘¼Ş2^û;¹#ûsJ~Úc5Íàõpkæ¡“£§˜Zç	zñ¼c?jÜ¹¡Zîáò	¦*$ø(»üÙÂU5ÅèùqñoÎ°é]×>û-?r¥µ:ûß ÌaA%“œZyİ¤6#¡Ó@o•KrF1c¿1Q¥£²*búÏ Å»t.°™âu^¯,øâÁ×5ª0ÓÁT;ºŞG=°rÜì•:Æ…~Ş¡·„L¡ùèU…éØ«+ë›‘wø<&¨Y±¡Y"ïÙ×ÿPÚ)Õ*‰ÇGãÓ› aZaô}+Déu«!ô°F¸®pš$L\rmóÆòO!¡†¸ƒî>ò<v]¸à­ŸXêVÉö_ Ì·ÑšáãŒĞóZ¢¶ÌJÒ&5PoòAD_²gs¡2M¦ø©P¼>£Ğd)Ó¸%ÑñÏW<‰eœ 	!n)ãáøÔ¬vuğ#9ÔHÕT€ˆVhœr	›À.*õXŞpnÎ-F·¾ÈåtŞu¨uíuÖ3‘v°Å»ÙdRö:-?	ºŞáæÅ›Rœ]$€İÊ+:‚ªÄo1— WDö
c-¡C.|yÓ7¯‰"ÈûêÿĞG§yıÁç<_ã‘QŞhĞì“fÄ16ı†™epàú.ŞôÎ=öÃØä© m €FêÀÂÜJ©0œ¶kLÓZPf:)9ş–#¶¨=q [Ö¼œóµ3°Ñ‰ŸDXŠzJ¨–¸¨·„‹\ìåcÜ)¬…~x¹§—ê ¥+[yÒeàºæ5ÖñxÃ<nsKËö$N–Ê
<[S=Æ€QŞ“R—ÀD­YlğX±ã#>×ÎÜUdEKüç¯ô»J‡¯šV¤½¾EÅá&›>’H6Mh]‡ès	çÇßõ×(–Äl7Ôâ%‹â4M†eÖ«—$D&®a‹
Ø^=ÎKŠ	FoïY]K™§ÑèNÉ:-¤ä¹`n£ÄwZ¨+‡üÙÂ±ÑóşÂÓŞyŞùÔîš'³C“¹.ñªÙ=â¦#ö}gJ¢
^[gÜÄv$w¿Û‰ÅOvlˆ}è?Ÿ”Ã²b°#'útblµCóH½Êª§œJ6ÉaŸ[µ¨—s"–÷öŞJ?Ç+½	r Gwú‡‡¤\Ï€2£
²ı?ù¬eøjW‘†uÊÁ{<‘º"‹î|~°O+¦
ıöé7Õ²!c&€Nâçğ Ir¤Ò‡}Ö$˜¹A±¼VD8}:£ĞT?1T¨9ğ‡|î'i`pñş~õà´@	ğÁu4=<\´×ğşOüx'xeCµ¿¢X£Š,&wF—áİ›<¥Û0Kãë„¾Ü†Œ‚<I$¶Ù|ÊÎjØ„¯A–[[çHoÊj!èüIè
É¢úòŠöë