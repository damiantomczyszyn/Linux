q;
	dev->video_dev->device_caps = V4L2_CAP_READWRITE | V4L2_CAP_STREAMING |
				      V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	switch (dev->board) { /* i2c device tuners */
	case CX23885_BOARD_HAUPPAUGE_HVR1265_K4:
	case CX23885_BOARD_HAUPPAUGE_HVR5525:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_DVB:
	case CX23885_BOARD_HAUPPAUGE_QUADHD_ATSC:
		dev->video_dev->device_caps |= V4L2_CAP_TUNER;
		br