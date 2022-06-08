C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		*/
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for baseband compatible with both RC-5 and RC-6A */
		params.modulation = false;
		/* RC-5:  2,222,222 ns = 1/36 kHz * 32 cycles * 2 marks * 1.25*/
		/* RC-6A: 3,333,333 ns = 1/36 kHz * 16 cycles * 6 marks * 1.25*/
		params.max_pulse_width = 3333333; /* ns */
		/* RC-5:    666,667 ns = 1/36 kHz * 32 cycles * 1 mark * 0.75 */
		/* RC-6A:   333,333 ns = 1/36 kHz * 16 cycles * 1 mark * 0.75 */
		params.noise_filter_min_width = 333333; /* ns */
		/*
		 * This board has inverted receive sense:
		 * mark is received as low logic level;
		 * falling edges are detected as rising edges; etc.
		 */
		params.invert_level = true;
		break;
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		/*
		 * The IR controller on this board only returns pulse widths.
		 * Any other mode setting will fail to set up the device.
		 */
		params.mode = V4L2_SUBDEV_IR_MODE_PULSE_WIDTH;
		params.enable = true;
		params.interrupt_enable = true;
		params.shutdown = false;

		/* Setup for a standard NEC protocol */
		params.carrier_freq = 37917; /* Hz, 455 kHz/12 for NEC */
		params.carrier_range_lower = 33000; /* Hz */
		params.carrier_range_upper = 43000; /* Hz */
		params.duty_cycle = 33; /* percent, 33 percent for NEC */

		/*
		 * NEC max pulse width: (64/3)/(455 kHz/12) * 16 nec_units
		 * (64/3)/(455 kHz/12) * 16 nec_units * 1.375 = 12378022 ns
		 */
		params.max_pulse_width = 12378022; /* ns */

		/*
		 * NEC noise filter min width: (64/3)/(455 kHz/12) * 1 nec_unit
		 * (64/3)/(455 kHz/12) * 1 nec_units * 0.625 = 351648 ns
		 */
		params.noise_filter_min_width = 351648; /* ns */

		params.modulation = false;
		params.invert_level = true;
		break;
	}
	v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
	return 0;
}

static int cx23885_input_ir_open(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx == NULL)
		return -ENODEV;

	return cx23885_input_ir_start(kernel_ir->cx);
}

static void cx23885_input_ir_stop(struct cx23885_dev *dev)
{
	struct v4l2_subdev_ir_parameters params;

	if (dev->sd_ir == NULL)
		return;

	/*
	 * Stop the sd_ir subdevice from generating notifications and
	 * scheduling work.
	 * It is shutdown this way in order to mitigate a race with
	 * cx23885_input_rx_work_handler() in the overrun case, which could
	 * re-enable the subdevice.
	 */
	atomic_set(&dev->ir_input_stopping, 1);
	v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	while (params.shutdown == false) {
		params.enable = false;
		params.interrupt_enable = false;
		params.shutdown = true;
		v4l2_subdev_call(dev->sd_ir, ir, rx_s_parameters, &params);
		v4l2_subdev_call(dev->sd_ir, ir, rx_g_parameters, &params);
	}
	flush_work(&dev->cx25840_work);
	flush_work(&dev->ir_rx_work);
	flush_work(&dev->ir_tx_work);
}

static void cx23885_input_ir_close(struct rc_dev *rc)
{
	struct cx23885_kernel_ir *kernel_ir = rc->priv;

	if (kernel_ir->cx != NULL)
		cx23885_input_ir_stop(kernel_ir->cx);
}

int cx23885_input_init(struct cx23885_dev *dev)
{
	struct cx23885_kernel_ir *kernel_ir;
	struct rc_dev *rc;
	char *rc_map;
	u64 allowed_protos;

	int ret;

	/*
	 * If the IR device (hardware registers, chip, GPIO lines, etc.) isn't
	 * encapsulated in a v4l2_subdev, then I'm not going to deal with it.
	 */
	if (dev->sd_ir == NULL)
		return -ENODEV;

	switch (dev->board) {
	case CX23885_BOARD_HAUPPAUGE_HVR1270:
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1290:
	case CX23885_BOARD_HAUPPAUGE_HVR1250:
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
		/* Integrated CX2388[58NIC_KWQ16_DATA_SIZE];
};

struct cnic_iscsi {
	struct cnic_dma		task_array_info;
	struct cnic_dma		r2tq_info;
	struct cnic_dma		hq_info;
};

struct cnic_context {
	u32			cid;
	struct kwqe_16_data	*kwqe_data;
	dma_addr_t		kwqe_data_mapping;
	wait_queue_head_t	waitq;
	int			wait_cond;
	unsigned long		timestamp;
	unsigned long		ctx_flags;
#define	CTX_FL_OFFLD_START	0
#define	CTX_FL_DELETE_WAIT	1
#define	CTX_FL_CID_ERROR	2
	u8			ulp_proto_id;
	union {
		struct cnic_iscsi	*iscsi;
	} proto;
};

struct kcq_info {
	struct cnic_dma	dma;
	struct kcqe	**kcq;

	u16		*hw_prod_idx_ptr;
	u16		sw_prod_idx;
	u16		*status_idx_ptr;
	u32		io_addr;

	u16		(*next_idx)(u16);
	u16		(*hw_idx)(u16);
};

#define UIO_USE_TX_DOORBELL 0x017855DB

struct cnic_uio_dev {
	struct uio_info		cnic_uinfo;
	u32			uio_dev;

	int			l2_ring_size;
	void			*l2_ring;
	dma_addr_t		l2_ring_map;

	int			l2_buf_size;
	void			*l2_buf;
	dma_addr_t		l2_buf_map;

	struct cnic_dev		*dev;
	struct pci_dev		*pdev;
	struct list_head	list;
};

struct cnic_local {

	spinlock_t cnic_ulp_lock;
	void *ulp_handle[MAX_CNIC_ULP_TYPE];
	unsigned long ulp_flags[MAX_CNIC_ULP_TYPE];
#define ULP_F_INIT	0
#define ULP_F_START	1
#define ULP_F_CALL_PENDING	2
	struct cnic_ulp_ops __rcu *ulp_ops[MAX_CNIC_ULP_TYPE];

	unsigned long cnic_local_flags;
#define	CNIC_LCL_FL_KWQ_INIT		0x0
#define	CNIC_LCL_FL_L2_WAIT		0x1
#define	CNIC_LCL_FL_RINGS_INITED	0x2
#define	CNIC_LCL_FL_STOP_ISCSI		0x4

	struct cnic_dev *dev;

	struct cnic_eth_dev *ethdev;

	struct cnic_uio_dev *udev;

	int		l2_rx_ring_size;
	int		l2_single_buf_size;

	u16		*rx_cons_ptr;
	u16		*tx_cons_ptr;
	u16		rx_cons;
	u16		tx_cons;

	struct cnic_dma		kwq_info;
	struct kwqe		**kwq;

	struct cnic_dma		kwq_16_data_info;

	u16		max_kwq_idx;

	u16		kwq_prod_idx;
	u32		kwq_io_addr;

	u16		*kwq_con_idx_ptr;
	u16		kwq_con_idx;

	struct kcq_info	kcq1;
	struct kcq_info	kcq2;

	union {
		void				*gen;
		struct status_block_msix	*bnx2;
		struct host_hc_status_block_e1x	*bnx2x_e1x;
		/* index values - which counter to update */
		#define SM_RX_ID		0
		#define SM_TX_ID		1
	} status_blk;

	struct host_sp_status_block	*bnx2x_def_status_blk;

	u32				status_blk_num;
	u32				bnx2x_igu_sb_id;
	u32				int_num;
	u32				last_status_idx;
	struct tasklet_struct		cnic_irq_task;

	struct kcqe		*completed_kcq[MAX_COMPLETED_KCQE];

	struct cnic_sock	*csk_tbl;
	struct cnic_id_tbl	csk_port_tbl;

	struct cnic_dma		gbl_buf_info;

	struct cnic_iscsi	*iscsi_tbl;
	struct cnic_context	*ctx_tbl;
	struct cnic_id_tbl	cid_tbl;
	atomic_t		iscsi_conn;
	u32			iscsi_start_cid;

	u32			fcoe_init_cid;
	u32			fcoe_start_cid;
	struct cnic_id_tbl	fcoe_cid_tbl;

	u32			max_cid_space;

	/* per connection parameters */
	int			num_iscsi_tasks;
	int			num_ccells;
	int			task_array_size;
	int			r2tq_size;
	int			hq_size;
	int			num_cqs;

	struct delayed_work	delete_task;

	struct cnic_ctx		*ctx_arr;
	int			ctx_blks;
	int			ctx_blk_size;
	unsigned long		ctx_align;
	int			cids_per_blk;

	u32			chip_id;
	int			func;

	u32			shmem_base;

	struct cnic_ops		*cnic_ops;
	int			(*start_hw)(struct cnic_dev *);
	void			(*stop_hw)(struct cnic_dev *);
	void			(*setup_pgtbl)(struct cnic_dev *,
					       struct cnic_dma *);
	int			(*alloc_resc)(struct cnic_dev *);
	void			(*free_resc)(struct cnic_dev *);
	int			(*start_cm)(struct cnic_dev *);
	void			(*stop_cm)(struct cnic_dev *);
	void			(*enable_int)(struct cnic_dev *);
	void			(*disable_int_sync)(struct cnic_dev *);
	void			(*ack_int)(struct cnic_dev *);
	void			(*arm_int)(struct cnic_dev *, u32 index);
	void			(*close_conn)(struct cnic_sock *, u32 opcode);
};

struct bnx2x_bd_chain_next {
	u32	addr_lo;
	u32	addr_hi;
	u8	reserved[8];
};

#define ISCSI_DEFAULT_MAX_OUTSTANDING_R2T 	(1)

#define ISCSI_RAMROD_CMD_ID_UPDATE_CONN		(ISCSI_KCQE_OPCODE_UPDATE_CONN)
#define ISCSI_RAMROD_CMD_ID_INIT		(ISCSI_KCQE_OPCODE_INIT)

#define CDU_REGION_NUMBER_XCM_AG 2
#define CDU_REGION_NUMBER_UCM_AG 4

#define CDU_VALID_DATA(_cid, _region, _type)	\
	(((_cid) << 8) | (((_region)&0xf)<<4) | (((_type)&0xf)))

#define CDU_CRC8(_cid, _region, _type)	\
	(calc_crc8(CDU_VALID_DATA(_cid, _region, _type), 0xff))

#define CDU_RSRVD_VALUE_TYPE_A(_cid, _region, _type)	\
	(0x80 | ((CDU_CRC8(_cid, _region, _type)) & 0x7f))

#define BNX2X_CONTEXT_MEM_SIZE		1024
#define BNX2X_FCOE_CID			16

#define BNX2X_ISCSI_START_CID		18
#define BNX2X_ISCSI_NUM_CONNECTIONS	128
#define BNX2X_ISCSI_TASK_CONTEXT_SIZE	128
#define BNX2X_ISCSI_MAX_PENDING_R2TS	4
#define BNX2X_ISCSI_R2TQE_SIZE		8
#define BNX2X_ISCSI_HQ_BD_SIZE		64
#define BNX2X_ISCSI_GLB_BUF_SIZE	64
#define BNX2X_ISCSI_PBL_NOT_CACHED	0xff
#define BNX2X_ISCSI_PDU_HEADER_NOT_CACHED	0xff

#define BNX2X_FCOE_NUM_CONNECTIONS	1024

#define BNX2X_FCOE_L5_CID_BASE		MAX_ISCSI_TBL_SZ

#define BNX2X_CHIP_IS_E2_PLUS(bp) (CHIP_IS_E2(bp) || CHIP_IS_E3(bp))

#define BNX2X_RX_DESC_CNT		(BNX2_PAGE_SIZE / \
					 sizeof(struct eth_rx_bd))
#define BNX2X_MAX_RX_DESC_CNT		(BNX2X_RX_DESC_CNT - 2)
#define BNX2X_RCQ_DESC_CNT		(BNX2_PAGE_SIZE / \
					 sizeof(union eth_rx_cqe))
#define BNX2X_MAX_RCQ_DESC_CNT		(BNX2X_RCQ_DESC_CNT - 1)

#define BNX2X_NEXT_RCQE(x) (((x) & BNX2X_MAX_RCQ_DESC_CNT) ==		\
		(BNX2X_MAX_RCQ_DESC_CNT - 1)) ?				\
		((x) + 2) : ((x) + 1)

#define BNX2X_DEF_SB_ID			HC_SP_SB_ID

#define BNX2X_SHMEM_MF_BLK_OFFSET	0x7e4

#define BNX2X_SHMEM_ADDR(base, field)	(base + \
					 offsetof(struct shmem_region, field))

#define BNX2X_SHMEM2_ADDR(base, field)	(base + \
					 offsetof(struct shmem2_region, field))

#define BNX2X_SHMEM2_HAS(base, field)				\
		((base) &&					\
		 (CNIC_RD(dev, BNX2X_SHMEM2_ADDR(base, size)) >	\
		  offsetof(struct shmem2_region, field)))

#define BNX2X_MF_CFG_ADDR(base, field)				\
			((base) + offsetof(struct mf_cfg, field))

#ifndef ETH_MAX_RX_CLIENTS_E2
#define ETH_MAX_RX_CLIENTS_E2 		ETH_MAX_RX_CLIENTS_E1H
#endif

#define CNIC_FUNC(cp)			((cp)->func)

#define BNX2X_HW_CID(bp, x)		((BP_PORT(bp) << 23) | \
					 (BP_VN(bp) << 17) | (x))

#define BNX2X_SW_CID(x)			(x & 0x1ffff)

#define BNX2X_CL_QZONE_ID(bp, cli)					\
		(BNX2X_CHIP_IS_E2_PLUS(bp) ? cli :			\
		 cli + (BP_PORT(bp) * ETH_MAX_RX_CLIENTS_E1H))

#ifndef MAX_STAT_COUNTER_ID
#define MAX_STAT_COUNTER_ID						\
	(CHIP_IS_E1H(bp) ? MAX_STAT_COUNTER_ID_E1H :			\
	 ((BNX2X_CHIP_IS_E2_PLUS(bp)) ? MAX_STAT_COUNTER_ID_E2 :	\
	  MAX_STAT_COUNTER_ID_E1))
#endif

#define CNIC_SUPPORTS_FCOE(cp)						\
	(BNX2X_CHIP_IS_E2_PLUS(bp) && !NO_FCOE(bp))

#define CNIC_RAMROD_TMO			(HZ / 4)

#endif

