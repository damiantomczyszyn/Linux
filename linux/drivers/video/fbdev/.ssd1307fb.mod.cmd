D_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		break;
	default:
		if (dev->tuner_type == TUNER_ABSENT)
			return -EINVAL;
