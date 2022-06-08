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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   Eþ˜‹Åœ²¹áõ¶¥ä5wDf¸à÷>ÌÌ{i—ûÁ§Ô¨_`¢ê¯x³ÔXôk¯¿
?XÍa-@ÎŽµ‹oOË÷5j‡ažÃ&D@,0¡:-¹Mîú±Y#•"TV…ïžÓÛùoYYfJ½-£°ó–æ›@ , cr°~T¬&£ÔmðC—ïÌ&#j³PÎxƒ¯9MõµC´ùpš·ÁVEC¥{zÙk2hÒË[wKŽò‘Paí?Èröð4*×µ§>4§°„HGXß«7\‚¬ëÁ\Íª­âmE•: åí¾¹ˆÈTQêtt¤n™rµóäEü¹ßÅ›¯ok#ùkëÛÊbòEYˆ
Ù°T…³°oô|©ƒ†æÔ’Ìµ7²È‹öÙˆ¡dþ_ÕÅÈš¯¿6ÓË&h^šÑn×IÍ ®÷ª…kœˆç¾‘tÎ{¦²2Ó¾ƒ@Å\ HW{ùå+ÌÙÒúZŠ$°r¸"Æ’{¶å“(sÈ5<ÇŠÕÎò/4÷U`ô™ý^¿ËÒLÁfêwø…Ù¶$ûÊÑ¼0¦§&2®\ “h(d‹ÜHÆÞlÐvMáoTègßÓÃðV2Áxê4°–{ÛÔU”ÖŒÑ|4p{üéJ`û×N½tTŸŽûœ®G JxÂ=@ cú@rh{Âj	-Þv{±Ú«þ«ßOX‹‹e“e¼½0Ñ¹lå›ê5!”èËÖI ÎÚŒØé æaõqz1%Œò­%y»ÛCwïL±6Â©­ÉÎœ÷LÔÊ»¿Õ[ðÈc£žóé~.Ôþ”*ØMï‹z+Õ±°¥•U¬:>ù©ËÎû*tPhi›½Q™(á–› z¸/³DÉ@Þ·/¿¥}ÎÓäI~rmW é/ù†D@S@:Qp…•Í<CJuÄ[§¬Fä]DÿþR@yé&ºß&
çf‡Üå¤T ´gJƒj@3EÕv^)CÞXöÛ/ñËlÊ¨TŸKN3jfáÍª”û”aÕ?\iñgÃë6,ù=|YüˆÐÅS£g½æâ³ÛÍpIÂÚWnoÝ¬«e]1òá,k)»°]%‰å›pé´:]Høµ•¢ÄªbMœ ç©×ZÖµÐ—ÿÊyPZ°¹Ý+zg@tKïÙ˜1“®³Bü¢ã`„
 Ä/ççP-žÌlŠçIüH<ÙÛ’1³ì\æójEØÍk‚žÒå÷‚8©Þ…ôùŠ¯ì­¢U¤ÝcÙE’Ù²?b,ñ—Š±Â˜Œ®,n¹YŽìJ°¾$>Mî“‘«2rW"áÇ3ÿô_`Ÿ¡¨È8Y+åT6-wÑ œìØB@ZäzqÑù)€¥­°û0 ¹…³Î}Ñ'‡¶­×‚Åz¿hµÔa±1"¯W7m.Øù›hÁqS9­’AçtÜÇ_èNÛR£2‹û£ïS·7ëŸp“ŸèÒ:Õú™iÜZCHía¡†ƒ¹yz0ø^m Çpëu#X$g4î"UèÐ‡ˆïCuä!xcÙ”CÍ^ïË>UH€©øü†RÝ€–/Ã/öiã/öá„ÀLTõDÛKÑìØ”ÔÿU5UCâäj‰¾p‹±-ªl«Á`ÿßÕæ6Í?³r#úµˆóöchQ[æjLõø"j2~ÿÃûç>H1)qJ$­Uð¶ËQ~Q’Ù.àBeÉË?Ÿ7‚JÙûGÕyÏÂëé.r€U»0Ã)±èÙl«’)S†ÿÞ;Ú@±Í$ÝàŽýù¯¾íç'ÓcR1o#_®Ž9^Çz22¨*êÓPk`l‹˜Ü*’‚­Ð¼#|'ÅYJ«SV‚ûí˜L8+íB	Óž¹+ðõ;0‚š©®m†áwÁÌÀMöâLª8u•ÝK´ê>S%(S7kÓ.‡ "ÔwîgY‚7ƒÝ'Ž0!yÄÃ¥÷LëŠþ™"Š/ÕÓæzÏ¡š¤áÊMUÔDÒòç¢$ù€Þ"åQ©á³íÃ+59²é£Ç-’ ¥‹ŸéêéF«ž³óäy yêlšcjgµÔ¹y¨vÏ"Ó&„DêË3¯TÃ2èPº€L7ÿpøå5ªÍq)mÿ©yS¿ïÊ¬l•¡!°/OÖ,NÆÁZp—ýðPÒ‹c•^Q~ËÓMšZÍ¬m5À›nãÚâ’›äÎ©äË¡«Z€‘P‹\¿*²tÉ£ûXÏD¸ƒ(iµG^êÑ•"“Ö§Ê›N¥ÔŠøŠJþ¯Kþ6}ÓR#:‹uèé¨’q–ÒQmã¨Ì©¾¥¡@FMg›‹ðpM„Ò7P*e‘2*aîmAk~Oj;y\q‰¨¹;µ’IÚN]Ä‚‰©[o^—FÎý\b3"œ½¶W•ƒ°Ðr	ö‘¼ÒÓ‘K9“Çkgtå±0ÔJx‘´õ$ßÕ­‡™Â& CqPÌ%öØ7¨Üi”€ãJ·ox­ë1‘¢¨¼*ÛsA0çú©žîž<	vwÈ¥t ·Sþ|Šý¬­‚Aã‡˜u‚2sÑ{¬žÂzJÈâ½ˆÉ¹6f?sgò6³õàwQ¦RÆÙƒ…‚ µ¦Ã
Ø~ÿÌÅ@((%OmÇF®8°5Ðc¶`À2=ùKîíhMTž¤lŽ5l92AŸèLÕ-‹©fòÒƒv¨ßýÙ(ß4ò°íŒÙC>_xÏABŒÙjå áö~O@äÂä|ç¿bÈB”²¼fvqx%#òËT÷¼WÕÁN¶øºeˆäÔØô–´Œ}¥Qsöýg í§ø†ÓtÙ³Å¦ª£˜þQŽ+å_±'«i;oåÎœå}âJò=Ö*­a-±ö(K‡	ñò"Þ6ÿƒgÝÃ«•Õ_¨ #,í»9'mJ“ùÉÙ1û¹ãdC©›=JfIàtöbV'°¢ÿã›ëáhhµ¡(Sö¤oª
ó]¹¦HD¡â9&çlx”Gr1RŽ$¡š«“yý¶>3è­\Ÿ¸(î ì­B/6Ÿ³ºÈÎ›• Œ'aØL5ž‹(†¨rÆjG‰­MõDjîœ1¬Æÿe¼’í6B/ùªF ‰™˜_‚–Ã1ÓÄ°ï%%iU
Dß15$en´àŸ˜×‹êê¶ïêï¢ùÙ¿ƒfÐBôOŽÌóÎa[É!5oÌ‹hÜ„ÏvIòMÏeoÃûö÷<VY`5“ïw —*Vm}%æVƒ\7sLÏxfß½«^LfhBÁ51ní(2ø´8u¯‡¿äÛ¨åÍP’.y‰Æ™VVMáð¶[øA²M¹V@ýËc«³sÓ¸áwj6ˆÑê¤ç¢úJíÞøX‹ºw"û;Åz”~ "”»(ËÂˆÌíªû­ü@›£€ˆIún‡šÊ˜ÑBâQ‰ÙkTgz3štç•nP!)–|Ñ8œª4]A`j:Ö(‹¸üŽ×d´×
¬šŸÌVêµHò¼mZ0ò©ª‹€¸0CZÌHðiûÄBŠzÍu;ŸR——6éò×R^ê$CvÛ?C{¤"ü¢Ç|6bQt\v>‹¼Î]!õeþÕ@ŠN»™‡$³ž©½Êwñ>t‹»PÎLÎîÄÇ¡Ð9
Ô¡éL|S[¢¶bZ¾ŠšŠ‚v†ïÑ‰ˆl¬:¼†º²m„)ü( Ý°ýSŒ à<c9·û±ìþö‹ý¢·¦q3ÿ”:+ò!¼ÂP)ioºÑ«‹"t1)Õ€èþ.	^Ô%¹¼œÑÛƒGÊb+ÌãÚm”d?Çsó;Ô;,³S¸ÏNµ{qD÷T‚J}WXuÚ„!ŒeÅ˜mm1èâAMh9ô7=…~“2›gu•ÈòG€P4‡'é SÏïÖ9YÒ€…›ò÷??&6B†Ó p›í=·:ßáµüƒfƒBv ç­Úm<eê€1^[ÚçVßÝ¡~ôO3íÀòøÊgyvÁ³KïÞJëŠNYfHyGÿÌâ©¥šè¤e†§ûÓjgžnò×î‰ÕÜæë¾òhˆ1¶öøl©xÿ¢²’õ¯SË;UÏ?©B£û%{ï×™ØµóË»© 9[NŠ>uÏóT½ Œ\y ïípttfÂW@Ø±‹7‘tÛ7™4³·$½	1Èc»Ìr¶¦™>FmþåAð¥^Îžš±cP9LÞ£rÃl¹žý/üh4˜+)3`æFXÏyUÅdœ¯*Z¦ÎêHœ¾ó€0KÓ.jÌD“TWÿÚz7T8ap©ÿ¹†QžH‹àTh}¬AO&‹õRyæŠs'Xó%-	Š¤|j	"}ÀFÝy‘Òý=ÃòÏ &vùÿ<'ÿ#ÚÄç–K}¢oÙ™Z7°“UJÚ·,Yiuß±þÝÇY¦ÄeÝdãÖÁ'Óç[%c“èk¦z¾L„º0ýuw½Ór—ì÷½Åe™ßúŠˆž7ò|¡‚¿Ó'Ä­`Pü¡ñÝœq*h¥f $¹½?åB'LàO×º~¥ŽÉiUKÁk3óÙYF‚e+âsNŸeÝô 7j?)8¤l90€†C™v†Æ×ãºƒc>ò¡%Ï»ý¥œ±Þ´Î¶Ð?Ñûæ"ÆÚG#¬ÈöUõ@ûû!Yõ2ý¹ˆayE¿A.nŸã0î®Ùª²7bLá£"’ah]éõÌÀ±DMñwe÷šû"*þG³U¤#ÝÁ‡ÔÂ vÜíŒÀîÁb¨ÚùËIÝ­•8ñ÷7$ˆ®ÎJ)ˆÇ(•‹tM-v*:Gï|úWxÃ"ŸÖÓKÃjï`¶Æ[‰Ö·Øn"U’9]³ÏÈ®v0¨æ6Â\EàXýRâ(\GÅ‹~èžÞöËõ†`7%÷Ø£ÞDŠ§Î™…U1vKÿ]GUñyXÎÉ†++/A5QSÊy)
¢Ïn:C¨¢°ñfîÐ5=§Nú`ë?â~²ú
=/ÿbŠLÇëÊÿmv¿Y§ñk L`ò0—Æ/•¢3H[Ñ¹Ý¹0­³b´çM5V0¦ìÇÈÌÏjÑç|²'tÒò;©›@å÷Ò>Ë«q
NÏI4 ñ}º…£áaAD½FpÇªµ6^·^rg,m„í“ÐÉœëGª®Mmé®ÖØåä…µ°pfÊBäöcùk‚6·Xé¡mƒÒ6y\Oé£¡Q‰¢ôúª¶ˆ>‹ŒÁä>,n“Zˆªîc¦_¾/íˆ5¶!6.¦vO¸mÎ?Ipí¿9=«†K®Ÿb2Ø–*êâ¯¯²´$áÑ /Wœ››çf»~LC| ÚY¶h³L¥337G979/Óé³Ž¾†Hˆ“uŽþÆãêÉ*{<A˜cYL@¶«mÉS‚e‰&?¾ìÒ¥Òý‰·þ'SªáËŠWÛ6´[€kõÀåªÇ3Ñ÷&Ûâ}®0““Ž1¢ê…ÖV'6Ï}·ƒ-Øˆ™ž}Xiü=[´{ùá÷·øöe{-/Ž—ößLûŒ™‘µàÐÿÌcéþ³ÔU¤œ	&u¹4K8ˆ.Ón.ˆ‰Ïçe6F-0,íyP9y~ÿš<R4¼\”"C-€á]Åt@é¾)9óŠÆú,ß×­¾É´:.fM„cë˜1GˆÅ<²%Ø/ U"gÀÕQ?ðfà¨;õ¡“#¼Ÿ5VWC}ý††É^ï+†N;ÕÔ¶Ü]b î¡Õ±èiSˆ„g=Ï=°ß+6eóÉxÌ\òwp0#“¹ÉÒEãÅÆ8éNŸÂJ[V§(çùyyØ0Gj2Çn^\Ú¢GÄ ó.ñ×ºW†šTÎ4ò-·•¿jÆX²Âj;¨€êŸ#ŒLÏJÈ—|ŒîNd5#Ï—©BÜãZêyo³ökÒÉÄK/«öá
	3™3fAgwëÙÆèáo†ýÒÖýqTJb¾Ôã|èR9tRž˜0Ôìpæµ¯×—ÝÞ-Õ®—Ð9V"Žvç´ ã¯›õãØ4×ûrÎ—Íçwü‰°,ªQ6Ö¸Ç¯Õ6Q£fÊÎB#LÕŒÝÒ7<¯½ÂL¾ýôá»	°èo©éÏ¤Ÿb»"Wü¦{rø,TãëI9ÜÎ?™AQ¦wÒVedŸÂo=×ˆ,¾iâÃ*Ï«â³”üõ
6~»^Š~NkIk×iÅ£™Xká%À%)ò+³Êžëô¸¯Jr¬6Ò}áQ'Þˆ²H„ÿ¾Û‚\ÓH,)¦îSïgPŽCÖÒe÷LËæ–oE¯ßsV eÎGùHÄT¼äÏ»õY½G”&C³Å6@Ûeï23ÒQ¸é/IæíëhöGçC ½QaøñI (zPªÞ¼òJ´ÍkÒÉ›¡Â´ô¹m?°Sƒ!«[è£™Ï-M§EÊ¯rõcàö‹eª_þäXÑ	G æš'ZIÚ²z«]ÿ^›ñ÷z"AÁEa¶Lçg7"Æy Á¶ËvÄ'12–
Ú”ÿj[/=V“•Œ,ù©Ò1l[²gÙaCðA§µØ)i°³vÀV¶¬—yêg³†öQˆÙ(‘øØ6î¯<ái²1M¦CQÎ–=áP³+µKÌá=]Rê¿|Ä(TÏ:*Ù¤$mn,ÆgZ0Õ±Ãüï,ãP53.¾'~¸ï5„þÞðMþÜây¶}™¬ä¸¡0ZÜ~Ê¯g*øÐ'xÚ£På	åÚÈÙÀ"ÇÂ~UÆ Ž‰fâs½””qqæîu×š³è:~%®Á˜ÿ}q'ü¤¡ º*Ù5=º<”YþÁ`…h“‰3¼à	ôï=¼‘^t
1ë]dysSÑ ú"ÂøW?×÷Y˜CÎx‰L4ÿëŸ6“Ä —Že…ñ
ˆ­X¸:¡Kš¢AäÌKó(i {.zSC&(ž›ÚlûMU #Á‘-1+áØáëÇâ³EÊ9F?Æi+YEBELüíÉô*v,Æ§:äÛã…qXJ§<ˆÎ€ZNdªHHcpù^ïîÙÿQ|Á«mì@LxãË¾h {L)ÊÉÑë½-ñÔ>1¥(6ìô|Ž(_§ê!íeIHz±¿›`…þ“Hìû¾µ‹¹|nëj\¾‹m9è´Ô×öÐÌ|;˜È¥ƒ¾¨­Ãí'MY´#ÏGb¡ý*:ßœ'ós$uk‘xM;€–Ü·°ïÖNÁê1ÈdÞ½œ¸åx†®Òh1ú‹*à½©¢»ÞÄ-k¦Ä)AÅi‰Ü¿DÍ :¶q¶»ƒµ}w‘ÆxbÆÑS]»ír$ðáZxÆÈÇÜþéK¶O-Ô“Ø …({Æ£;(’È¾z|;Y¼ývôFÞ"øõò Ÿ¤@8ÏSÀ’ ñê`Õ›¬É»!ÞA‡­4]¿_à™È
ÆçËÅ÷‘5?úá ìÊWsãê˜qX4Cw›ö+é˜Àå´F˜¹oaÍÙÞ+Ö#Sž²K•ÈÀzçÕeÖî¨A›!—*‰¹É¤®V'ÌJ=ÌÍ¬ˆ6VF½h[ž¯
s¦¡¯÷F> 7ðu…ûþtaüµ,M”ø—½Á‰Øµ
ýyá±÷md³¦†ÙÆ¬ÓÛß–7ŽMV9%–¸ÑÑv«Fa4Ÿ¦&û‹„¦¥Î³ÞÞ8¾²˜’3bk™ë-·Óõywýæ£é¢iÅálþA“§ÐÞ™.fzgEŽö?K©	Ñ}ˆÜÅ¹¾°¢W¢šo-@³ÎñªG»ƒ¼OåU0h~Ôzî„-ÖEæu¦*²=ÍNA ßZx~§zÌmè+ûY0	oèWŽ;óáÀØªÀ˜–'¨•q×÷ŒÅéÉ=¢NÔ&­EüÃª(£±~G»®¬?­¤ò5“œL´åu³
§3a€ ¾ÈC}A¥ïN'BUq°ÂMâ"¬ áŽ?ˆ£Ó°©’pã×Ü?jMR»+al–Çæ$PÄg]t¶RÜ¨‚ °Xœ!¤ß¥l`X¥¿pý‘J…¤šT[-æ¯„‘ï®	Àºº‚Âò5ëì”¢}"À5î§³¦Ü0
Tš³Ô÷óµZÍ‹y–€q¸\=y_MÄ|´†hgý([mlÒB+Ûä¸àüÜ”>¿,q®ÕßÎH€/°ç…ó¾•H‹E”uÕøÝ¨Ú+°Ã˜ž|ç+8ZJz±ü´<éTfëhYüMaùê8¹³¥rL.2ëãiçlS$šÑU¨¢i¤÷›ŸêE{·wÑ	ÜÁþƒ€KÝV¢4JqsÆ“;êÉ0@QVšÕ¡¥ï˜G ÛB-©~qêXò=>Õ2%HEÆËR}Ï­›Ó=.Ó¢öÝ“7ÛzŠÎ ÿ=
½˜R|—ð¥‘mXw¸…²7©ŽEý»oÆ],\¾µÎ­ç˜ Ô&÷[˜j¶	.R¢
Lº^‡ ¯žÜ–Kù-åË
ÍÇ²iÐO	S,´%FM¨Cf¬ bg|¯úgL™L¹­bÉ½7oñCÅx¼eÂÇÉ«TH¥jô·¦Ÿ^§Š÷O‡¬óÃfn·€fÓSP¦ëpºl`w)¾`Îd½ECÏEfÚFdÀõ™…Â§ónÑ××XãÓì“{T$-æ÷Ÿ^ÔùJSÝ+å™==”Æ? 4`W½ÚÝZú`[´ÝÄë¿”53ÈsR‘î{VW_C•…-ðzK°åÓßã¬ÂQ_áXƒŒüÕö$ãÉR ;>‚`Ù§ç¸®@ ù#Ç-…ÊWfÃ oÒfj $ÄY‘Ô\µBúé”bYöPË«\ÍÙ˜×îUi0¸>k¥ì§ãÂÚH¹œý‚¨"U^ñýËˆ?0>‚?Ãô>¿§ýá"5¨D„ƒk~°Úz–ëÐJ§±Î` tôÂåÐÇOI|¨0‡’ei-Z·æÁ‰Ä%;$g}U¡5q)‘ñC"ÑäŠãf(’ðÛ5­µÔll^ÙÏ^<±÷iË%ýd°O`,5HúœÆXÿG¿|DRQ€æ;óMÒÝD„:¿Æ vB“uV|–¯ò™¡2„ä*š¹­¥^Rí–[t|òèd7Gù)©™õûÄ9xåÊ‹¦Ëk¹†dì&;G¨'õ!ÒŸýR½3ã9*\´Ð~oðÛ¾ÙÁ5–M¬|º´…Qõe&Y{þ<^Ëãg 'à»>ãq[„ÆeH\aØ,yÒïÞÆVÐ|BCÃÉ?ˆír¡·] ­uK_˜8ð»x… X°!ƒLü6¢QS”«ÿÒ9¨N‰ød·ÇZEŒ‹¯\9¸×ë9¨4KHXI([?ÆPþ‡jeâú PæÃþŠ#Ì!Ñ36Ô·çºò´íñmÊ”x#0P­W‡š%Ÿä.'ñ/K>=L˜¦eHL$$‚R;ËR 5ÃlKI¨$|ª8ÿéCwD7uz»ix6·Q[d‚¡ß½¬Û9«<ðº¿­n:m›£¹&U× 'EÍnþÞ°8v/=)U3¡ÄÛþ’
“ÆX÷(½´p¯Ê¢¯5guÌ`bB.Í7Ý™],9Ë†€¢£”Z%¦[‹k}¼]ßî”0Á›nrÄ¼–SLd]û ˆ¨÷bñ¥á±‚oÇLíuª¦A¢,[œv³=ûz]Ù':ç°å
áîŽUETðm ð0e`› °"GRžýŒ&Cþ~Çþ±ëæÙNÖ›Ðü¢y9\r)ƒ@ÀM[ŠÑÙ@±¥™ÙP¦w³\8@Ð[Ý®\7 á>~Šä•	¦·ÚV¦kš¾Ù%Ç»ªžÓ^¬W|dÖ¯Ê—“
ß-ý$ûçBR5sEp1Ý¶Ó¦ÏøË·@h°ùŸRÁècshä1O´¢Ð>ÛÙGÎÎäáø$<ÙÀ‡ 'Aúø.ºÆ­G©gˆêÓÆì"q\ã—‡ØZ{K ‡Üð9‘KìUD°.²[eØÝ}4Wšãô((‚û¹[]_Íe,Ð_ÙŠèjÃ\øaÜé4”;ÃÂÚ!Eò ó­Ø¦Sò„×x‡<§mpŠä@~AÆ½<uÐ¾=(‡0Iƒ3aµßss£±Y0ðªûô)†´¹~^6[ûû%_‰@É¢%³Z€M5Ø§HùöaÓ=³äÒSpÌ3Å…#4"…
~ýwzkEis¬Ò©r´ìWÂÝÓ
ª;ü´(V«‰1	'í#gN»t,5Ùq¿^ÙàÝ~evh%kQ¾zÝo‡œÎ¤t‡ŒZ	ej%[âå(k„¨³õÌ¯åÀ
B…Œk/ g„ÈïÁ_šwj…Ñ±3øÃæe•ÒzíjFýYŸ"Š¨j9Ûg<C÷^GF?Z¸›ð¢ÛÍ›•ÓtL#âí²7;öÄàçv!!ŒÓj$ãôù­Ž©Šçm~8§ru‚óþ‰yqŽ'O ÆP ¢’DŽ$k–&ìxXZd.ìŽ´}½¨'w/í+…±ã~vÜ2¯Û‹³‹M,_ó›äƒ-›oç‘ý:B»ŽSSã2¬2ÞU_`S‚¸ç,EX\tý«À£±”3Bµ	L*lâËÓN$æÏÇ
£™½ÏŠ2<y)?"fÍ·½6vè®­	a/ûßÄÔ`‡°õ3}¥&à8!,h-DÙGÄ«÷–Bòy~»çO˜âs
äJbÚ„ÔÞ°NÁã»Udý:ø®£Ú8(o“•—O+Å“™@û‡—wáÍ×ÖjaÌ^mÅšI¥SÔ¼L|}’/÷4îÚôlqtcßOd’¨ßÂj“ýR!!Ÿ}Šz«¸Y|a‰™míWÇûLÍl*ß2ÿÙ+4åø0ÿüi­_ü­}‰óa÷ê7\˜+Šˆï<'u«’ë„çÎ+.¥;4_>ÏLUc?Q ÕíL;ÊÖƒÉ½áŠ![+û5úø ê”q –Ýëðÿ+Äš¯¬1s”jÜl¤‰ÒS ´VFR*"m‘¸LÍ`Œ˜ýÃzíÚ¾ñæÒÁ™Ã•(u?¨‘ºOÝÍ~G±3Üúª“1gÄâTc;P¶ÑÒãEQÜÐÌþ¶Ë±¢ñq\žv\=†ßM _sŽƒ—	÷I[è 5õ­D¿_Òv¨Uki„ùzl÷ý3ˆâûÃB+~ÒêU¾0p¾ëŸÖàñßý`¢÷vÉEòÂ*á›†×!Òvãq°aMŒ>eÓLh	«#&£ÏlOiÆß}YÆ œ|(Þ’‘N-]œðz¯œ‚;ÉÂ6´á’”	(S)i8Ag­@Ì:ÿ§Õ.xŽ²ÕB„>!À‹Ýœÿt§øßªkÁð/ŸçðóÖeñ|tS!Ê/‚ÊÙÉmMùºóm×½¾Äq*è§fíÛ[i%ßbjLŽ*}†“zòƒªGŒ¼ÊÞ'	·Îä—lŒ°¥=4Ñ¢ùR3rwF·pJ³ËÁn¤åTÎ¢/pÀ“[x€¹Ï
$9nò“0š‚ËKµ.^º tùÐ=ÖýŒÉL Í	¤£o¶æ}ŸEK!Jšr½%žì=…5ê^ÏÝgPDatÛžMÞíµ{ÿŒ ” ÆÌ±Úm»é|ƒh&šÍá¯UJÐû¶V6®d‰Ò¡›Em_wšøõ‹tD«,¬È¿®¡4ÊÐÁ_:–áv I«'Ï¯ïÈÒw$äñkmÔø÷&bÌ'ùNü2U÷€öïU;Ht	o‰D[žv åÓþŽ÷ßÃgZ-¼“@N¼´s¸ƒÀ¤À$.}ç}§°³¨å×€CØöCäã:h0*27YÞN³®¦ÌØÞXö¨áøÁ|óÛ[35#»ž_¶zÑ˜fÂšÄ¨³$°J.xzfY‡	—]‘>ùå¤ó¿ûö°‚²˜²gŠ9¢4±‘AÓ‹¸(:~èæ>2’|Eìg¢)ì«ª¥ÓÙœ-ôKn†PQE®Ÿ‡+pw4~ìÊ÷¶í’_ôËq!~›ßûqØ‚óøvŸoé¶Š€´ÎÕGDïzseÌxâêKCøW†y›¹¸¾«¸jzîò÷¿‹dzØ‹‰X
ýoÀ³(2Ë¥ˆ˜{AùvŒãéÐ…¤/¹ñs+H(iìók)ïBhdaH¦eq˜P]žâ·u/ÍjÝŠþ·|@AÛc)zhžh…SõW©]ìäš˜›$ÂÛãoÝ¥t7SO%¸¡ -`PŸë½N’HCšlÞ’# å!sCñ	ÝäL‘œl0,ûD‰lÒ–Ú$¶€{ÿ7¤zEÛ­˜ÀÔÎrÉqjkÏ{÷^È‡ ªtGÌ(Ö=v$[žiŽVé>5*…’–UãXž`õÊ<W¯¦Pdìšùyå°(ÜM^ê¹ñ!®'€¶¼ WùMºpX&Šêœ2åøñ´äÃ#ÆÏAÒ•»¸DzùÁ0·)J½(}ïªÎò-“ðc¨ùEËüÄêxNì'þu÷^UÃ4°¾šRÃ¬œæ„{•¼•ÌyO°Íž&¥I1ƒj[Àÿc;/=Ä¶>$kÓ¦íY¸[þœ:!3ƒóTTÁ) 4+–ï •ÝQŒËÔgÀw(öyUã·îj.hj*Vœöúõ¦ôß>àý–M#·³ã.jÖpÚw[~/JÆ–¥	Oç54B»§ó=ütA˜kû¢æ`ŠÃ'(¤VÙenÏ.ìú†öDB˜(îgV•lž#—Æ¢]+aGiíà^g˜¤hÞlB©Ùã´	UãŸ“·WœÅkóì0ß«ËÐ+á®†ÛÌvüýcß	huÛ É!ÿÖKJºzoÛ£iiÈQ¢Äœ'mhý²ŒÜKB$dÝÑêGŸªAáo™‚¥~qú€öL*n8”Âù.šü÷¥a|Ð¾\¨*´ö2ÊOÄa+5Àñ'10êùÆ'¶Ë>€×tò¦rã­öçßŒ|®rˆVxYONIF^$! Q‹[ûÁñþ¸Ê^9¼ýGÌ0äO ÎýGÚ†Â?ê$5óe"Bd©Ð†¼m¬ûnÆušÌ¿¦Sd%@tLïEÊ‡rà.‘”Ä)¬§!1¥MäD~<šMÑ%(î`<Ä)Ëå[â¬½£Ôç:}„!ã9Î)+†{þ„©ùÅ¸ë·q5»·õ•“÷ßÿuÒe§þ!^FêkZIROÝ'0q$	;Ó,Ï¼üc27’@*1¼†öYÄ#åü³	3ëÀ‘­¶Œ(KÑÛdP¸ ´êðà0Ž¬‰n¹nYœy¶­Ÿ³vI‹sòKÊ1É‰2É°ÖòŽÿ±,¢ÀYÞ—’åá0E1…¢ŸØ@Îg›¨-Zlò§.ÿÑ'å¨þXŽÖMõ¬}3•€¾+¸dÞ‹F†dÈ‡¾öÌÆ^xG£û-ÙÉ?åU¨Ø´‹Î¹€6íRqyÅšëméLŸ‚ ùš^íêÏ¬?¾îB=)ªŸ"1u£è—R•-‚%ƒ'“+€q0s¦‰Q$1ŸŠ_ð¿éÝ­¨€æY(¹âkv$½2ó;Œ'>œ›èP üvHžtÐ	LâÅWDxÉ?È‘âºóÞV+¦k‚@mè°R<Ì#ÃkdÜ{´ ÒK¯Ü¿tpZPr7¦J±5|ÌÙ|)èÙmTdÃN³Ä¹eCó“£Ç*M¤õ&¾¸¤ŸhX‹%îÍÛ’Ûô8Ã÷)!L~ýÍ.›ùŒá%‹Áe6HáHX‘Rm'…„Y†k¨ð"­Ž$Â¶›«Ûƒ0/ã–F“LÜßN†é~ PL]è-v¦÷†íöòïÏ¾ÿ[É+q(Pç´ñ|7ëGß1Ì[tgxhîS”6ê‹þÃœ­Ç+£¿Eÿ<‡S¡äÿµš³ïÙÑÝˆZ7Œ†À9Ôp&>¿*9ÌÖÌ½tª~ãH‡GŸ`ÎpóË·Ãá³4úi*H‡¼_8‡mžZuÔ6ô ³)ˆ~´ÅKØròW´zìv}nOz:{));ëßÎÌþ¶ó:5^¤X×™¦ÏD4Ýÿê=l˜¾Ù@<åaHèÐØö­©ÈPø1Ð `¼+¬ü/Ç÷ÛÆLò êt®îÆ-žä7RæðCtæ½§—aí•¨é«A‚_PBc1{ŽI÷8C9‰¿"TF}ƒåqgfŒ”Š‡˜xÝµƒSøR¢{QãyÕæXÄ0žÓéæ.Ÿ³ý!ã—/ô¦+¾WKe"¨»¼!Ü|åW,;«¨ƒ™1-+¦;^ÃÜÓÈÛSÌˆ>X•£\Ô}ßšÜßuòÄƒQò„	·ðŽ­’œ¿žgÃEI¿Œ/¡;aR äÛÞëiNyÂQñ@§}uWCUË¨íe†g®*(™C¦<8"ø¨&(j?+GÎÆ’„ÔS~esÍQ ²t»SºYð§ã5	â¾a‰:î½ÌÅ`™uÆÖîþSüž¸âI?›	EÐ^â”hów*‚ö8ß"èÆÅri#ÀêÉ÷æÞí‡íßL‚€‰Y	žàÓùàÔ!Ö¨CÕî Î¼G[ÇŒc¬öo-ÛÒÕ¯ügtç)ý·ýçë ²ŒìöS-8ÓÜøù¯¥…Ùøos>âb	híâl’o˜ð¶¦lò¬±Œy=×šÅ°ÛÑûé"RUÓ±——{‰@Ÿ¬lÕ$_W;NB¿ü7[}¯¢d2{º	ôÊ~yÌ«Ð°ÚH¹€1à4	² 
HóÅÀ¶±#>O—…>ÏÿÖS¯=Óµó‰Ý¨újÍã–Ù3€Ü¨áv°"‹«øô­Â„q1xiÂº™øÇã¶µ³Ýmk>„T³õòß'„Ai4°hç£¯J^/Æ‘X j€~'Ù¸¸¶ü¥@¥ªP\×_gO_Š‹6ü5epH_o‰ƒ1°híüösœž7Á ]Vvê±&Æ;xRa¥ìJâãžèŸuØú^GO ›^Öw½t-w‡Zÿr—äåðÇ–±3;g›Ò¡þøÇb÷gˆuïö?;¹R¿fsz¨ÿ·¡Æí¢•œï™:ÙÌ˜<¸É`bû'rÔúë‘“´¡0ÁLñûj
s^Ý “‡‡3ú‹[§Á‚ÀŸ¨³Ô@]£ºã´CÚ]•XËÓ¬ÿ¬.ª¸¦·"‰5T`4JÝï–µñÑ û+9ÎåˆÑ`“6deæõ3QMø: ^¥Ìç Sï[çpõºi¦!|PPí:çø”ÂÄbêœ6Ö]Ç½­Ì‡ÌA†ï/[G«ÚAô+ò|³Ä ™¸¹ÆIn<ÍÄûøÆºÛ”*•2`l.‘_\[wöàJ\0L‘€ƒ/]	ž+—Q.ê<gK¿,Î²dÆCùK“zm¤¸è³²÷èœJÊµ»ßˆ¹Þ)À¬éä`?ò™¬oQ}³Þ²ä© áx<rghê±è‰¥E]*ß«ïx¼È%iûÿ¶Ü=	ðæ4.)Ì²¦B¾ip¹”‚ë}Ÿ0·ûX4j©€%9êí¡{S#ë93
%ëzmþ7ŠÄ£^Ó9,ïØ.È™1y5Ü¬4]Qòdæ•wŽ/\[³¸ðè$ *µÍ¢×·Öô¬–LžaÝõò.î;UÞ!‚˜”qè²õß‡DªÌ«ä8ëÜÍSËCNñBÆ¹ö‡Â’ãô0¡_Ô!š«µÙŽLOI‰uëÞ÷O½ÐéY<Õ›³Ôb8áðQµ¯êªM'®tPcÍ^ü
ÜýI«á ö¨ØiÀ™©°løþmGéÇ1ÌXªipÕØƒÞpAe±—aŸØ£Qƒ/—¾$ú]ú	eî7®xòî<d/|w#ãÕ:Á[ÕQ„·ëŽ‰SëÓîÏ‹ÒéØRkœöQ×—@ÜP„Éðnê†Œõ]™b„Ÿ‚££LP"±²`Ò-[p¢1õ!Jü½×Žo(V“Ú§Rï‹¹¿¦=£,Ê çK<ëpÐjÊÄk5%:Út¼jâ&®E«Ãf§/æåy¼»Ý‚Ùø»= ç0£9ÝéV°*&£	TO2c'²,ð¯b:d‡•ô£Ýí61ô}4­1¿F†§nßö"^,(s\*7Û%^Î_ÁAù÷ó ^AtR¢<_Ú ‘Ñ¿‡âHÅmºÞA5FæóB±ˆw ]Ÿ jõÍ?AÖ¼ÎpPg[(`ÿwuÎL-ZÜdÀ@Ö015:?°ò,¯ÎfMöqJ€Ù—5èü£(†ŽÉ	*§”ÜçûÅ<É=ýqVË¡i®†rµRoÇ‡þ8Í”¿%­z¸çyØ³´Z6¾§Yá/_à .¦¢PãQñ·ðÞø¬\±GÌ÷öÞ}µ¥sÖ”´azT:m©QEÔ(ùÆÙ²rê…?%ùH¸o¿cÌ«{p×9OL Ð4“ƒQüFZ×OúÌ‹x
íls"5öûþì„¼“üP´×t%ÆÉ˜jô_½–-.ºÿ6žÚœÙÛRˆX5Íea›²ôSÛÏ`Ú.&Ð£NX5æŽôø…÷wtç€›ñ%‡û×Áï×¸Ò‚qvÞ·Pd!hGÆŒr†6]+Õ´”ÕmË­n	ãäøš^ù;(­s€þƒñ"ôùOÉ¶•ÉMW>mYõì”rT`váZ¤ÄuVÿ~¢NHúÑÚ¡”½¨š!>7v[n%¡¹Tû³€üÑêsöø´ã‡¶:Íp¯ƒá Sa<Mó(/>‰þb_%Ìþ—¤íéÎæ†@¦†<°”_©À«ì$ƒ¨uR…¤éi™/8úKV5_Àdf%×1î¯Áv†ÍQ¼>Â+c6òpSé¬S²•zÜˆ¤ôI>¢E¬Øx-ÌÂÿ{2vhµãqÅ	Õ=&rÑEzjòòY`Î£.á‰dŸmûÃª§ÅÀçÙ+¶l­¸ò©Á˜si‡pu ¤…ïnsCQ#¤Æ_ëOØ¿e”nqŒMÄcïßìLUK ¶AQpn‚”4»Zï°düb4w`”È§YËˆ=å”ô=õEbŽH^Ýk_ù^¼	:È2‰3á?ð(¦‡]v¾Ðà¸cÓÑÍãû!‚è¶oøë¿ªnäñÈÓ¶iÜ fÐ:[¬z]ìï„s¯qšÍÐžÃøb”_`Î°,pIy?¯zS›ÿAe*×b« [†#Mé=@Û5p²JëPM0\¿¡»³7Öhé(ks«E×ÐÑÝÄ%ñ¥}3J¡rµ‡+­y‹Czk(É{ä÷#tjúÞ–>Ð1ðZlóO¾¯ml8–4‡Ø³HØF#\E®áD*‘°z,Û”š#kŽ•Ô!*^;E™äñò…A7pY¶Ö¼Ì^ýE(Úûø^7‹Ýó`øöR¹ˆ8CÀm¶[wÑç”$“ÉÂ€¡‡:ü¢ŠºÍã«	Q–ft»–@1L¶ŽªWZ%¡guâaç`r«KNÎª÷$Ÿ(ƒN®;Hi†©Ñ<”]ùìe†œŸÏ†Ù8o4“„ªe–cÚñ0õöôÉGvœjFå•9GÒ[Òqï¤ž6XÿñŸEÍjœB¹¶CëÞ—±==TAÍ_Y;§ÎVü5–iL­PiÏ¤_‹Ê˜å±#E,–Ëq}¢ýÜWµÓê«¦œæW–ÂcsÑ¿³ñØ#O}ÍÛ‚DåP»Š'•gÿ[°ÔºÎåè˜ï·Z™ûaï¹@"á?wÃy•¤¦÷87Ôšw ×ÚñâKÒQb„‡é½èyø…c£!£Å`ÈŠÉ .¹Mk6TÂq†0Ÿ¶²&ÏæTpZéÝ’+Båùûkò=Pµþ‰ë…m64¹À?¹º™)/ËXýô§ X†u)Écì]ïâ|·b qH“Í(êÒÇ]ÖF=PÚÛ°b”›bD4ËÈ÷‰:#…ÊëðÁ2bÿ’t2p6Ôb+Æsü	Ów¢*$C‘Á:¶±*î·ÉrÍ×ôðÎmñ-Nú<å7–·¢§_»+maÊjžy¹`ùŽ%Ô_·hÂ+/‡ÚmªZÜÐ`Jëƒ^L$‹7;XSêbŒïn ;N3{•:a) ¿uÇçíÐäB€Á`T¤œE!ÓšR^¼z…‹µ×kêB.Rì„¹kRQ|ÛlJŽ$Ú•$“&ò*äÀÓ,ÐáìJÌ8aÏŸÈ,}O[ýg3Xæ£w M¹S²&!\¨™´¿‰1ÙyIªïeªs¤y7\ýGüå›é*ShOÅ&G©·³óøA3ÀEûÑ‹ÃSeY¯÷ÒL>ÀxI@ø·…»XÒ³ÞGb MhÓk†ÿ)wî¸éñŒ°Â™›^ƒ`¯é¹™ç€ÂyYÿv¯ã#¹Ôcu á´Â°¾»	÷ÈiÁ·‹êo±=Ân+ââ²ä˜}»‡0”ÑüJö©EP‹\³µò”ØÜn(N<îµí¸~ÈáyI4-ûº„í•üú
$%Ý
€F…ÙpÛB~hj´†NöÁd“Â)çä}¼gLÞ õ«¦Ì"º¬L),ñFŒçðv~„)!éÈ`®ÙÑ—:ûz??Sÿ¯&ˆj`3ÐÓ%òÀYå\þs¾±Þývü"7Ô¿@Êð™x¶ŒJ5.äðÌð§ä6œ-²!0ÃÃÔe5ˆýøÌì¬åÕHðàoL»ÝM”³p5b@ÊÓ½'pàŠOÄÓ'dþ°´]qW&œÒQUæÝ~P…0]MÄŠ$JM™_8…1Î¦56ÔfCÖ|;Ç¸¯ð¼¶óî¹k
Ö#iÈ”_‘,¬w»ÐQx©ºA…†ï/xWè¥Xm6íÚ€e9¤‹$OïO+¨ìÑvã]B©oÓ³€÷ÍÔ®¦ «:“K3çÛíW`ŽkÒ&NòÉâTaØÆâéõ"_O`dø9ô´Rn7ÖƒùÀ£¿‹›’ò¶2çU.G{‹¨õì+äxl¦õÊüEät±B§ .3ä–Q"å@*ëÐµÞF¬œƒ“\—–•àšAf“èÓµrIh×OžÚ¶TOŽ¼?ë¡&êº¬æÆÔïÌ„>IúTyùù,%Ã/¨w‘§oÑÀG ¦\¹büÐ~§Ü©ãbßì1µ ¨óÄ½(ôÁtÊx)¬µ-KE2HÁ[úfŠ°°±Ô·\žÀjB¿ëÏúiø‘’ÛÚ,Í4…Ð†ž€41¿=vl÷4¯|ÐÕ³»µ)^¯“]¶£ºY°v×Ð5qZÆÅnÇÔÜï·ú¹‰ÐéÑMYJ¢šk<oÞßçIBÝe_¹a.åÀPÙ‰Î:W÷?¿ÂKá»èâ¤Þ®Dáª`Ã4ž}"sI­è/vGÅEÔ:l"4	¨=!Ó˜BP]ì¥¬kv;¬8së€:è]¥¨´c½ê;)-æWðux{;5±E|z˜âo”ýëÃ?øNÆºòæ ¿ñŽkÕJwãÔ]x"¯{¦œµR…§Aäl,CÔ½Ú ÝìÂÞhSèèoºÏ+Yˆ!WfGÕs!¨ïmã ‰8~ã©ÅÊ ÖÈ¢¦cíÍ“Lú˜ ŽŠjgk}JYOÑ¹^~mÄ_ª>LÀHkßnEˆydúÚÚÙ¥² 3nÃ§)äƒ6ªð”7lÙVØýYÖ!OEÏvZCÊr•Žæåv²á)Å–O3Y¼ó[z6L?= ~:+'"Fc$£jÿ³]?ö¨ ZáIÚãA82^¸¹¦¡Ál`dƒÙJÛÕíJæ§Pbðyï`É¶Qœ5°AÐ©`J£-¨Ê8O#Æa_Gƒuˆ¿¸½ÙR‡°RoBÒ¨·xÝxîÔø…Ø½s¥­
…@ÑmÐú•íjû}Î`6¬î¿?Šçõ­AÚ.ŸÁxGbØÃ óâ}.Ø,Á"[„×öƒøl%ÉVé~Q¿e‚æŽZ—> Á‰²TaÙEC"áùÇšpÚ¼äèäÒú®°×Ø—eoÒNqQâ\°Ù¤dN´üUÐ7›çÓwOèõO”ÔÕ~ž<u÷óÕ¢íÅ<L¬ôiRšÈ…´€ —¦bJÁ2'f³ªpQï®;ä£”õœ×yt•E]KÉzæÛ¿ÆÎ£"ü»DUª2"IU?Îv!qÌ°…öIûÐ)ë€µpº ìF DÈ¿œVß6Nw ã~B½6•ÃKý¢ê¤¤"%öë”EÒ49ì 4´ É¼‹Q‡Ãî(K_hy#ŸÄ¬ÈÎæá¨¨Ãî>ÿB,´DD}†t¥ªæ¯Y,ò€¬‰g˜–Ê*!( ï®£Í–ÒHä9>€?k‹Á­më…ÇÖïÇXBšÆ8ø´ŸG7”a	‚CHšÛèBëkøÜö‹l“¨ÓOa•þ¾!ä¤yQCý•%÷Û^Ô‡ë~Â¤ËðàKÄSCÎ¥÷úoQ¶Œ<NJ9tÂŒ…Î<66-~
lå…té–Í­€R¤a…j„xÃ‘t?¹b'l.ËŸ#Þ°Ú¦‚=vt†1ì-ž*÷í!rQ@±´ÁÉÎŠCÈ©/—Ï4öAk"'&dˆ›	Ù¼3½ÎÝ*g1·#ßœ|µç)XšI ²iÿ‹…ŸÎå/ùòdÊë<Ì7çm çÐõ°Ç[C–N´ø=	ÍO@Iª|r­±8ØL'`²si£tK¼³W'zõ8,%ÁÍÈ’¹Æ|Mçi
éüŠkø‹	ÕÅÛgŠŽyÛŒa_^I¸YkvX0ˆ5P6ÌãŸþ£6¼ƒ„5<0&•Î¥¶R_]Ó¸öä¶Lå‡-Ò,Îž`öãøn‹‰'Oœä¾óO,~·Â3’T¢ý){!¶yDÞeH9Â/9ÒÆ'4oÑ3è;œÝŒäÂ‹Â¹ð}x¾Ñã‚’»6O9d H7-Å™•[]îà®Œ=šÕÎÌý­ï\ºÚ–×(»vP¿†¨4ÈÇ?žLîn\Ku"qŽŽnømÏ|.}2Ÿè¶$í1î°hŽaíü.qÍ½kl=''Ø
]'WÆlÄ¿zY?€tr%g0Â¸”å-~dÇöÃgb+·t‹–„ô8´MQCÄy­«Xd‘2J—Öø–ô·Q°ëï5Ž·Ól6‘Ü„Üùo(SQè­á²ºšo,fã®gÞãáŸ¶OÜÙžCõhª]o^Ô2Ÿ^8ýÔO®7Æ—¿+qBq”ÈŠƒ½S¤w&4Ä_kÆH­4±ÞZ5¯–:Kõ×9
ëÔ¥Î7cÉi9Ð^€‰Ñýð÷-VkQÚ;IHxëüÏÁ+Ûµè…jSo…N¡Êa:ŠÆÞpË›*yY–Ñæ\z¡¡Æ2ŠùSÕž¥hIS.pú3UÖ¥»þ3¤@·£ÍC*(A$cžEƒ›>ÏD²ýƒç‚k·¯dŽ.Ñœ¦S÷öB6L¡5	ë#dØ?$ØÕ!næ3ÞˆÝ´uk_÷oq¯úT­*ÄÍ·@’¢ÖË€/¼‘Ò§yv·uÂýÅ§òÀ1­`bÁdÙ¿þþöÒ2C³¶Ë]Õ ü]Ê"›2ÔŸ1Ùþ^™	Øæ~KeOîwIËL1mÃNÉ&œZþ&„:%ùÌ|€ß"°E¡x	lÄégq¸àÊ\¥ˆ_	 ÿÆ²åžÑµdr‚w`ü±úrÏS( Ñâ_ÜËZ	²ãvF™ªãÎ
JîTvxObºµÊÐ¬ÉQC³:UÛJ0Ö?ÆqSí¯2ORRUÅíþ¦*g›‘.AÂ•X+éVW'×xyø	a˜hŸy¬1Nó ¦Ìg_»¥ö¯yˆðf²Ò˜€ºKwRs°ùë ÒÍ©a£‰ýØ¢`^íôìÇÖæ¿ ¶?@˜®z*+Ck8\.qšÜê7L"Szò¾ñQÞd¸¬ÜX Î[c{™OL8j±ÒÝBÒõ'q.C·G‡ø¨â‰2X•³"lZ]¨ˆ†¯Shn$ñ‚¬+‚}hÁh,	öäü-‡ƒ¤rè½ËûPN{t¼„_ˆå½4¿žXÔC
›5*½!¼ì;AV:Æ µrA‘w¹ßÔÅèº—g½U23™w«H+ÊêO™@À`ÃŽ7³wÒ1 ¶Çû$yß^¤½öÂ”û&”eÔH»"4Í2žm„!îå©§âÝ›¢¡ÔñãOB$—ý eÞW•]	æ~C‹bwX[5ƒ2˜˜™¤ôÙ‹×Þœ‹pàÂ€˜U±væÆ÷É·@·QjáàÆÎ+¨	²Ìm{`ršÂ«Ùš	¿* À‡'¢„„ˆÌðöaÓòdy èk—Î!‡±©{;Ú—À;¬;=W+ÑpN5]U¹k£ÛeÂüh?ºýÀ'(±Ú$±¨ŒÙo$æNÚ¨»÷ ÷×AO›Í¼¬^ ü®OhÂ³KÖÍn
:¥êJ­Þ{]!(
HmÝÝQù‘Þ‚µ‰uŽKºÙŽ*ê.îee}ùñ(©úPC×háÝlŸ~1gwé›‰<¢`‰…5!Fiõ`î9W²ŽÈË¯Œ®b5•æŽüÑ’Ö¯ëit‚ »KÍyë·Ýù>•*Éc–hN¦@µ¿ñBï÷`Kô´Ÿ©…ö"3ö8’GàæÏ¾ýï3q13RaºãÈºÂñm’8¥?Bw†þz*cüæI”µ¤KÝ“Ü&ÿ©ë°=<j2«8_œHÏ«Y–­ø"˜ý·±‡ë¥×§E[†7´bþŒ(V<{ÚÚ€ølÿ‘InÌÛ×Å–1(ðI÷ò¯dxÃÄ“¥’££ü°¢.hŸ;ÇiU}3ã;Ê:¦«2tÞA@¬Bx™±> ¦ª6™áãƒÒn@Î ÐHEûS“¸~¢ ð®M·¿*½ñåõýšpÄàÔT¯S,Ÿr0kíÒ±|ƒ›¬íõÏ!L,6ýGbðË™9Ycß7&Ý¾ŠIoÝ0†á)1ã¢/UÔÚËŸÌ2CÁiê3CàhjdÑ*F¡‘Œ„ëèO,}G'eÊs˜ÂP'%,ÂcûeJ0´61WÇNöVõ€¥÷ÕÀFXAN?Þ¬UÙëoœ$Úß¼uK§¸'’	¦)¯ˆÓ€Aô–^x´»ò`’e÷‹?ê±nü\¾y
Û`I	h*¼P&ï’+¸í›ûý_ˆ~÷ê”}ÿ “ïßô=£GˆdÂ8æËe(‡±"V®Nƒv¾É¬šúòóÎ,ñF»—Tp6½²Åa©ŠqîÆ‡QŠX¶‘öÑ¨óî™—êãâËEGöâçÒÔ®“`~ÁoŠšû“bS‡ÀÄ3Å¾÷c÷”q“%¥PÝ{Ù£yê;GOÒEˆŽ¸×Ì­ù²o]po|~§C#ðµ‡&ž¹k×Zæ‡P8ÿa!—½ß(pÊéˆæŸ$bvøèèd°¸°VÉ[d›c$‘B²ëeÅ*‡KÂÿõ‡ëïú¹ä"Ø°IÄq¦(D@Ù7ßqV`õ×
ÚÍ0t€á|	ã‹8‚z~ÝÃÌC¹Š[ðˆŸ._10³`ºä!‘ˆÂù¾ì[d¾¯:‡/o
ˆ,7“ÙEpïÅB“vlò/ÕðW#ˆÿpáÜËmNp\Žkþ VèÑ¸ì¸ hEg÷'»wã*²¨%òNÏ	ÂI–9dñZ§¼¢µƒL€Äž¿ƒû’—ÍRq¹loîªÈ6{ÌKaþp}Ç¬X¼« {5áæƒÔ#`Ù™°oüÉ€¥zPÔäÌV_íŒ
WušÍÇ£Í¹¼ššgQíì¥»Š9ïŠ©fÓøžq²yÇ{°/–u+"Âß’É îÎº‚c8ÉSú	kqêM6£ÜÎ÷P:ëqµÀ9:0dþÚÛì%i4U’³:œÈa³À®n»‹ºsåÆì,¡.H$6MÏº'mº&?Ð¯èœÝÐî[ŸÉÆ»+™µ:ªfÿ}&EÔ‘á¾œÜhÇ€†íONïó}J†Žá:Dgµ/¹’ LA¡=ã{­è)þÓ¿wòüæôphÎÄy¥!D Æ†$^‚uVO !<Nw€5öè¨20Š“S¶¡Õ•™7ÐÔ¹Cè!šd×já<#dÀ[SO¡–ZOæZ±lÑÌé;"’{ë„ÇQdÌBz%ŠyÞ§P„¤Ú†|É?n¢qÈ¢'®ñöD&	sMû{CöjýÏÎÊ³[FûÇ.&Î¯š›‚Y¼+«Q§²9Ž,wí¥Y÷ÆY@*C±[â¬üßw«¦ŸQV/ªÌ•ëv–[(îïht-æm
>ày˜xï!º~Ør9ïb‘Ñ0;Eé&ÕO('ßooŒïW‰á¼D€cô<»lwÊi8ž­@·ÁMI„,¯ä˜ï‚IðþÂªŒ0þw
êI	:€—ž9Š«!-­1¬†ÓhOæÅŸÌøÔ©~iØž=b-V™OdWKg,œ°ÌOÕ¢ã"{dPº€†1DÒ½œQ_1@ä^,°¶JhIJ-u€
%ÍÁ‘Xº\Æ¹¾Ú‰ß+k¡ƒ6Ž<7ÉÕI&s ZSùâ=vYmuèµ©ÍD sï•u*ä¢Ù†
7Ú´2Ã³|sÅh!¾ÃU¿£u]¦% üð^éDGãà ÅÈa1Áµ L^¥‰_ˆ›Í\ìðS{p@Hk:xoâî|É+«;âùxŠÝ‚½ä„Œè#®¡ãñî0Óƒs@d¥ž¿ ôÈaÆóàx­[ïÁÌÉ £?dM¸ÊÁ¹ny-<Cd&ÁÝM<}¼†ÇªmXq3Wëù´æ^jˆg„„à±~¹–žºz˜0ÏÁø1Ê‡ª@G|<ëL€¼Vƒc49/D"ö‹7Â(ç]Dò¶Áš9H•­ZîRÁáÅ5·Ÿº™	­%^ êâ¿LŒÒLJ›fCïæt)Ã.ð)´ÔA‘=¾ÝiwÿÀ£ê!J9rr9X±óêÿ73ÃŽqy½ÛÇÜý¨K!¼}šŸ|\6¦˜kYãñ`¶j0|caÿ¿qŠ“J2ávÕPÛ4äý(èç´äFšˆ³Áj4bˆ¿-Órøå;?ŒÄ o"Ëö×à÷åq‘¾ÜŠt”È1dðc—¬²šz‘°ò¢ÌÇz`[$›žô‚(ÕfAUòÀÕí““È]ð¯Ü8k„"20ýÿ[ä@ ³‚`–¸ÚoxÅîMÂ¸n‘ž	8]V›Òƒxé	ÉzR†îo¯cÇ^áÜÃÛ5yñÛ©È¨Š¨ ‡'Ç©µ­mËSx§'<¢\_#ËIRá¨äPàÞZªêö[Õ0½„#Ê¸;Í÷6¼Jü~ÅspßØ&lžë@"g×7Á+jŒLE:•+žÄdt-ÄÚòÄôÌb'ö0±YCÙ4ÔA¶FÅ‚6DÐêe5Ih&ÝŠÉÓt<¶Tœ¹š9’6„TÍÙ}úCÉG‘ÄudÔk²ƒËµ…°[*åfQÉÉä^˜ERì§e{ò=oÑzš•ˆ+{gN"^é“]Ýnê¹¤a·Äò‚sÖ¿x²»õ‰›H.ØCÄª.ÑÀ6‹"‚ÌžÁ!ªOFŠÅÂß·ã[ÛsHÂk1Á4¥'[nª×cíù[ç“²­iK	±úrü†à,q;‘ÂKƒã]Ð%¾üV^8è…†«-îP³î*Í—eé¨×ùÿyWäÍ¾GJÇ’Ø\8Å¼­š”‚ÉÂâGÁßÁLÛ"©\ôÒ[ÑM©ñ‰)›1|
ÌMtkí/ÚE9÷|}#‘FÁÿ÷·X:Šr€ÂÆÇVÌ»G öÖ$+ œíÚÒ2Àw$¿Ø¾!–Ûqò7#"`'Ž”h£†!ïQ¯Sh¨®rhÅ¥°³³µM×ÄX!ÉSUwC¬'î–ßExùƒ&§6QMý0„ÏŽÖÚD˜/s\aÓv@;¯}‰÷«ŒË™6zº}	¢·@`¯ˆ`UdwZý\ ŠTE&Ãã³Ê³ÇþÄ™ñÅ¾(J„¦¯N…RÉÇ&7ÂŠ^­¥¡§÷qbÒ×lNZÈ$,»”'ÿaÙ8¿&±ãy>^[®Ê8ì„´tîÖ†µí³Âu;ã0|ÞÈ2"¯4à²d¶WdŒ2õ*Nþ	ÜÅÏºzØÎ>®MO"TýŒ‡ «ö·M"›Ñ;5@›â¦T&y$hêú— `ª÷¼>Ÿëq1Zâlƒ×‘^V,Ãêf$JwGÖK2~.XÇŒÙx«G÷íNÅÂ%UÍJS6Œ¸5¥ØÀ°OÝÈðáˆgUÞ—}=ç˜¥gÝ,è2IÈ•ÕžwüëXÐo²ùK<ðTÌºìÅdóü¾e SSñ¨™Ô•	(Ç¾=‘»ã–xbÉ¶Žk.U£ ”Þ˜J?Ð%™W@·Þ»iÊ™³Çª·QŠž‚±ƒ)6Z$±§Z‹àû£cð
'×E¹÷@ñc¦éË$ Gå‚ð'ç™u>!úþ5-©Þlð”Ó~Àš¸7¯sµŸDUpä‰o¯kk¨ùí¾†Ç#F9¿±íÿ¤$ÈæuðŒ6&Ý£aSã;³p”\Gï|g ¶'¿-UT™?‰™E¶tžÊ¢i‚ÕÛbq£I@§’EŽÀ›žáz™”OÁ¯›—ÉSÍ÷ûZ‡x ‰@Z5R>MÎÃê?!~´»ÜaôXMºÙG,fñs³<}ó!¾À½.•jï÷ŒV’ÖÅFóÃ[Iœ(•in”·q,*´@á¹øKZ%K*Ìÿ›$ØZš¸¨M}.ÀRkšÎÂ=3;Ev¼@QFOuÜ®PG–2yÜ<qþ×E5‹IÔ‘Ï[oìcÁ¾JŒ\72Œ’Æ¹üÜl±±±Úõ^³Ö	‡È·;Sì“F+úæM‘aIrBÑl¾äËÒCÀ[\£™
\µ$³ÂºË3'¡	-½tG·*kÆŒé„pFÜ•°c2\M§“yÞ¬¸"Ü“ŸX®f–›êÒšwò+ç÷¡7Ê:Ô÷ŸwDè1¾†$¢`ª~‰g0bEšÞùíÜÿnÝñ ƒ|@>+roŒ0îŒ§=ä6Â»î E:XY~wÜZÁê‰-V7Ž×e-ûz‰™_œ­B$+‡˜§çgd›äCvý¬UÀçwKø’ãÜo
9dd'j›†®º¼Êæ"ò1àŒÞœïâSCJ.Õr¶‘84·.¨‰>;q³+á{ÿWº‰³„Ñ=ë¤XøyHü*:XÞŽJ˜/ÑËuWÂIç‘+uÂÕF+Éù?ÓÒ‰€Ëï£õáä„·¨
…øÄÃ¶ZO?ÃV¿¶_€I3ÜµŸ1×~ŠÁH†Å`e§ùÃÌzäá$$t«ÖæoâÛëŽ‚œ±/ðÌ®þT:Ùœ\™Ç%@—g£áföçâÿóšÛ)I¼!(%NøÞµ´*f]Ò7DÊ*"È‹pÒ.RÚ„“_QJ\b®”€o‚*´(j~ÑÀŽ‡5¹íïÎùfvn0%ˆ„Å¡˜XL¦jß[$ßhL³]=Œ­¿4h´nïma µ¤ï.û€ÑIAƒ¬t:k˜ )…EZîÌT›Ü¶¹ÁÄCKœ¬‘Ž"IbH2í
Æ¼<ÇÄ{Ã  ×—>Ë,%ÿ3çôõkà#nú8—sñrIwš+ª¤|¯Åì™eÍZTÏE4œàÁôF5<w$¨éa)þ£õÞ­ïÉéË½ç×LU(WÇÆºP>Cû^áJðÚQýB2âkâ¶$êkaeËlšÛ«^`ƒ÷‘¡P››rfAØø§oÜŽ‰ BÈ¢¶Ï·dá~‡:ÍÃ	§Ž·" Ü„B§´Õ?¯@ó]—þ}‚@Óå€g*Ù¡”Þ¾?./«S^Õ)ÿY-Vp|Êq`´‹;¶ƒ ]ÙèWÃ+™êÝþ)~8±²Í§Ý¢Àè£'$j¦®ë'*áfÉ°šû
_ï4O™ÑSÀ°ï± ûV‘éÝoX4åpåjÉ¾¡¬ßõçQÎâó8D'æHeÞ¸áêí0m;ÿJÛ£E­ˆr`Œ£ðÃ­%pÒ$ÎTâêíõo6=Š9ž¡jVËdñêÌ„„º½Dtï…DóúágúºÚýö¡ÞGh»v&ÌNÚÂWºÞ)^¼iæ»Ï'¦håÑH‘ÎîF—ÉÃü”¦ˆ­C<¢£à\Òî›†%²Ht¹?µ^FìE,š÷~qùÆÖ¶Â÷v¾ëˆœŒ sá;
ëÿkTù=ÇCí<J^‹iUµ(ºnjY[[þË_íZO‰jð;ôUëÌ­Zß¿%ÏÀOY¤½Û[C3a†Tƒ5›À<!Ø¦ 5ýH•ˆàü°bñH–+¦­PÙ Cr÷ýÑToˆõ:–DèB…ý‡e»©±Ú¥r•ÊZZVêÆÈdNFŽnÆ:m%FcMÜwá“
s¾Nõ“Æÿ®æÊ	ÕÙ¬ ^“~”þfºƒù0Èà@áÎjÊÇ»Û‰C×)I¢JLô¯Y5;T³:SÍ‘¹û1	„ôÀqd1‚™Ý$ØÐ,pû»u	qAÕùÙ¶Q”B…ìß¹›´A†#¤!žôëqÀe€pÓ‚@ÇQžÍšÕòˆ±è¢“/)h8âXÀ¦1¸
EFƒÛeKÄ|ZÕE=¶CB8õ¨Ÿì4š½Ñˆ*äuÏ3‹˜òž¾žZª`¸›1Œ¼±<.„˜Þ|SiÙëœª¸ÄÍc>H,ÆìiÔ‘È¸ÂÄ¢F«>¶"Uå»,–#,ó„"c€ˆ;•Î1–’WùÈÍ±žÀø%®÷4Ç:°;ô[n¢V
EK@ìà¢q>“¨ÑÿËZ-7üÏBõG/ßö&4âöKcØ®ë^–ùˆøE­Âó£ìè[š·*"=ú‹nÕ4Œ)T©ãŒ]úgd¨Rà(F¤þÄQŠ³À9¯PJ]X¨Tí?N*»rf-»â+_-E7T5Mï22„QHM
5œÕ ûV?+v÷
5CoÐÇÞ:g‚Bøá’T#§Æú£De³KEâow‡c‚v[§þòD0Ù÷šv‚•åï¸5xu­³.þÐï­3|ÍbÐyÉgT.·S~Cø¨Ømì^¬)«;Vkô¹»Q¦ÜÎÒ9÷ÝänA?oQ„EüMi»îá?Ü$ÈÔŽyÙ{jFšÍÙÐë2»Á¼t&ÊD³~ÈÃšõÇÖ	·ëƒ—Og”6Fi9Wã}"rÐEŸCÇ:‘™c  ¨v`H„[Qÿj°u$¥=¾Ó.-V…î[bÆðj]•N[Hþoê&`H]qÙÜfÀðç¯ø*£UÚæÍ~‡ ýþm¼Â°×¨,ÛGRX{påMƒ#ä„5{Á¢Ãy¿m®óR#]tÌªØ³mª.îsOTYg­:dæ'ë<|¹|e‡?+¦r~“ô6þI@'Kgà:äÐcC#¬>VZA)€Ø7?ðrdÕ‡Üq#Wa;!gB“ßûÓ3K:3zu)†"VûšŠ¨‹ÚÑRÉØq/²]ÒIÏ11b'|¯¯ºX¸ô.dã{ûjâ"›+øƒFfþr~ÊUë)?0!ú0%«³×ë˜—•ˆ@ºhµ·|ë@Drhƒ‰yy›‡¡°ðrê&ÿEÒ( q‘zqlÂpêZ¥FÍã/5‹Ïíïx›¬õw^çÛÜÝŸìgÐ<VOJk¸€î6Ý³õšfGÿ’5ö6_!G8iTP·‘@wQIŽ£ÎÿŠ},l³(¥X1GF]jŒaA¼G³7XKëÁSŸ ]O5? ÍøúÁ–Hp .—R*QS*²:„»ÕœÎ_Ú¿?a2wQˆ?.)â r…ô‰$oŸW²Ò‡ÒâWÎa"æ7„#õ-S-Êl–„VäDÂ"Lµî™­¥`‡uzÏcYds3¾Bê¦¾G×Ï-½P™W*z $¥iŒf¡BRÛRi¯îã;|©Á)-/©7jb…,¥NËc2Vj¹`½zùmè}'õvÀlÒ­•ªŸì¢€«`©aÐÎCRå’%ú#˜i×…qÅþ”6›‘ŽPû bhR3yU“Û}t¾v‡ÿ¶þ*)Ûý£%oøub7ü–?~’oçwùeóE@DÆK|uohøÀ;’ë‘–Øp]ÈÒæã‰0ÄãØ;Y{€ðmßiùßœž;¾.îCèi£x=d¤6Úêg¶cu9˜k ÅF$&tÊ%ì:x–ø†3Ó¨«‡Žú
e
æ„'«ú*sOÂ@c¾£ÃûŒnTÕšüÛŸš’/‡S?«Èá»!ËÃ)Î£ä—2¦}¾Ý&cÖl-T·%¢ÆðÌëQsDª	ÁVYhÎ¡+ËÌ¬V0'’eè1i÷n„Ëú:`Ô»3Þò qx®QTh/ÂÕ2J s´¢Õ½{5ðJ9ýÙ#î–1j²Ü?Œß/C6¸PwÞ#Äguý•¿B:~oLï •¿$÷YxÍšK•œ–5¨Þ%?…åaxÇÒKXÿ7Êy¥ØÅ>\oû[Ã·;8ßXxLzæO'Ê\îï7¸õ°›Œ„¸îš©’ÙVy[qP@òË¡êË…uhøíté…|‘zÐÏöôŽ€ãN—DÁ"¤4¨|Ö¯%dÆìÖ
 I½ ½*,|Ø—¢aÏØ³’òûÙ6³Í³:Dvlýäêåøð”àËæƒú“|Æ¸¢¹ü&æbþÄI$·õ_!™ÿ¡V>…6ˆ¶~Ú“’œòÿ€£©Ð{‡¿Ion/ªm8þµº´á11˜Ñè-DËwˆU/ØäYšK &áK	ËÑz\5åèDšô	¾A™=ÙOÐL®…X²o¥S?¹O°öÕÂ0"×åöA±†çgSm)Ú©®0s›Î¯´X!Íïýà:d†8D>k·K²(Qª­ÃÕ&z%Ã³U9„0>?ƒàü¬xh‚ovzê–'_Êšœxl;ù%×Taç8º:øF&b0æe9BG‘7rh-­¹§_ à@YæÈÑÊ¶@à€Sƒy‚^¹v¼”Ù2‘Lä(E”P0¯j¯.ôaþæþï(™Iö•â¦ú(©JeG4£&&©™éáÑÐÙìÏC¿ytbò³ÌëÓÂgpˆ_¶Õ4Ãhµåÿz¸uTE§œ:e=juµ£#b$ví]´oÆÝOH–S)ìyŠwtFOtÔyŒ¾é„3#K¢ºÇÉ«·eñ™JR™¶\ö©ÌÇ«¥ißHõ>I0bY-õ(…å¹Z5Úñ½{Øù%Ïá0®Ô’¼ŽÑNÔåfJÐFˆðÓÀ¾ðÖI §«íç€Ï“/ jªñþ	@b¿ÃcoLàe€ä™:@Þ#ãÃÕîÀã?úTÉ®v µ®Q¦‘•ËõeŒŠþ^!èKåöu€ÊZPbç=t@Ý$b¿ÌY‡UWÁåàs¾ìñ;Ðdà÷rãð.WüCWóY K
áÐê²}fev0Y+Ú å«)‘1D‚©`Ë„æv±¶ý„WÙ,¡s3ð´ék9e¤QBÎè± y5Óû¤öøªÓÐ_ Qø™yÚI¬r,½ø÷G›k.S1?„VˆúoÃF•øÅ6S]²G-MÕ
 °ÊoõxdbvvôË[€ñ@`DðÊ?üiÍ“?ýÇÑz¥]áv¸¿¹W0Õ-‚9ã™ÌÏRè`l>1}¿ÓBK§’šDã’‘6MR>°¬óàgiÄ„²¤3S8Á|–W&z«ÙmW<ÚP8p½>°Ä@›Bô¸_Ñé!»J‹éµŸ±±—z[°o¯]fT~#¸] 0b¸ïÛ2¤GF)DU>ÞÐ"Ôí ßñæU`'©ù‚éˆFfu²5Iö"™~Ë¢]$šråê}œIE<ia‘ ¿#þ±ò¸vsøHÕSpk0m°„†á@¸ï¿^¿’wh6‹•ÁÈÉm<öÈbìzÃhæå^î/î$€½¸!Í†ªCÞˆÃ(†¸EsxP‚¦´f'žª°˜ÑåOšç×cÃêBTréê­Ó'bûû˜Íà°n’¶BË!{E±%µ¶ˆr!Ž²þæRUPê‡§ÀªZ*,zŒAÙqò“qáŠG¼.ÂI9bHÐ¯„~5Ý‚vØF„TÆ3-¼UB*eKéß4xù>ýê,¾ÔF1µ5ÍõÁÄÍÔòqÅ=o[ôU0ÀsF²áz?²Çû±`ÊˆÐrkz(6Â™ö³Ü@àÊ// SPDX-License-Identifier: GPL-2.0-or-later
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
			   edge_types