->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_DVICO_FUSIONHDTV_7_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP:
	case CX23885_BOARD_DVICO_FUSIONHDTV_DVB_T_DUAL_EXP2:
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		fallthrough;
	case CX23885_BOARD_DVICO_FUSIONHDTV_5_EXP:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_HAUPPAUGE_HVR1850:
	case CX23885_BOARD_HAUPPAUGE_HVR1800:
		/* Defaults for VID B - Analog encoder */
		/* DREQ_POL, SMODE, PUNC_CLK, MCLK_POL Serial bus + punc clk */
		ts1->gen_ctrl_val    = 0x10e;
		ts1->ts_clk_en_val   = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val     = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;

		/* APB_TSVALERR_POL (active low)*/
		ts1->vld_misc_val    = 0x2000;
		ts1->hw_sop_ctrl_val = (0x47 << 16 | 188 << 4 | 0xc);
		cx_write(0x130184, 0xc);

		/* Defaults for VID C */
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6920:
		ts1->gen_ctrl_val  = 0x4; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TEVII_S470:
	case CX23885_BOARD_TEVII_S471:
	case CX23885_BOARD_DVBWORLD_2005:
	case CX23885_BOARD_PROF_8000:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_TT_CT2_4500_CI:
	case CX23885_BOARD_DVBSKY_S950:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_NETUP_DUAL_DVBS2_CI:
	case CX23885_BOARD_NETUP_DUAL_DVB_T_C_CI_RF:
	case CX23885_BOARD_TERRATEC_CINERGY_T_PCIE_DUAL:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_TBS_6980:
	case CX23885_BOARD_TBS_6981:
		ts1->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		tbs_card_init(dev);
		break;
	case CX23885_BOARD_MYGICA_X8506:
	case CX23885_BOARD_MAGICPRO_PROHDTVE2:
	case CX23885_BOARD_MYGICA_X8507:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	case CX23885_BOARD_MYGICA_X8558PRO:
		ts1->gen_ctrl_val  = 0x5; /* Parallel */
		ts1->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts1->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		ts2->gen_ctrl_val  = 0xc; /* Serial bus + punctured clock */
		ts2->ts_clk_en_val = 0x1; /* Enable TS_CLK */
		ts2->src_sel_val   = CX23885_SRC_SEL_PARALLEL_MPEG_VIDEO;
		break;
	cas